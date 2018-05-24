#include "ntfs.h"
#include "commutil.h"
#include <shlwapi.h>

#pragma comment(lib,"shlwapi.lib")

CNtfsFileSystem::CNtfsFileSystem(IBaseReader *prmReader /* = NULL */)
	:CBaseFileSystem(prmReader)
{

}

CNtfsFileSystem::~CNtfsFileSystem()
{
	this->FreeRunList(m_mftRunList);
}

void CNtfsFileSystem::Init()
{
	UCHAR	szBuf[512] = { 0 };
	this->ReadBuf(szBuf, 0, 512);
	//this->m_vm->ReadSector(this->m_startSecotr, 512, szBuf);
	UINT16 *bytesPerSector = (UINT16*)&szBuf[0xB];
	this->SetBytesPerSector(*bytesPerSector);
	UINT8 *sectorsPerCluster = (UINT8*)&szBuf[0xD];
	this->SetSectorsPerCluster(*sectorsPerCluster);

	m_mftStartCluster = *((UINT64*)&szBuf[0x30]);
	m_clustersPerIndex = *((UINT32*)&szBuf[0x44]);
	//��ȡ$MFT�ļ�0x80����������
	this->GetMFTRunList();

	//��ȡntfs������Ŀ¼��Ϣ
	m_rootDirectory = new CBaseFileObject;
	if(m_rootDirectory==NULL)
	{
		return;
	}
	m_rootDirectory->SetFileStartSector(m_mftStartCluster * m_sectorsPerCluster + 10);
	m_rootDirectory->SetFileName(TEXT(""));
	m_rootDirectory->SetFileSize(0);
	m_rootDirectory->SetFileType(FILE_OBJECT_TYPE_ROOT);
}

UINT64	CNtfsFileSystem::ReadFileContent(CBaseFileObject *prmFileObject, UCHAR prmDstBuf[], UINT64 prmByteOff, UINT64 prmByteToRead)
{
	UINT64		tmpResult = this->ReadFileContent(prmDstBuf,prmByteOff,prmByteToRead,
		prmFileObject->GetFileSize(), prmFileObject->GetFileExtent());
	return tmpResult;
}

void CNtfsFileSystem::GetDeletedFiles(vector<CBaseFileObject*> &fileArray)
{
	//this->FreeArray(fileArray);
	UINT32	clusterSize = m_sectorsPerCluster * m_bytesPerSector;
	TCHAR	szFileName[MAX_PATH * 2] = { 0 };
	TCHAR	szAnsiName[MAX_PATH * 2] = { 0 };
	TCHAR	szAttrValue[1024] = { 0 };
	UINT32	nameLenOffset = 0;
	UINT32	nameLen = 0;
	UINT16  usnOffset = 0;
	//FileInfo	fileInfo;
	//����һ���صĴ�С
	UCHAR	*szBuf = (UCHAR*)malloc(sizeof(UCHAR)*clusterSize);
	if (szBuf == NULL)
	{
		return;
	}
	Ntfs_Data_Run	*p = m_mftRunList;
	//����mft 0x80���������������ݣ�����������������Ϊmft��¼ͷ��ÿ��mft��¼ͷ1024�ֽ�
	while (p != NULL)
	{
		for (int i = 0; i < p->length; i++)
		{
			UINT64	clusterOffset = p->lcn*m_sectorsPerCluster + i*m_sectorsPerCluster;
			this->ReadBuf(szBuf, clusterOffset, clusterSize);
			usnOffset = *(UINT16*)&szBuf[4];
			for (UINT32 j = 0; j < m_sectorsPerCluster; j++)
			{
				memcpy(szBuf + 0x1FE + j*m_bytesPerSector, szBuf + usnOffset + 2 + j * 2, 2);//�ָ�ÿ��������������ֽ�����
			}
			for (UINT32 j = 0; j < clusterSize ; j += 1024)
			{
				if (szBuf[j + 0x16] == 0)
				{
#ifdef _DEBUG
					UINT64 seqNo = p->vcn*m_sectorsPerCluster / 2;
					seqNo += i*m_sectorsPerCluster / 2 + j / 1024;
#endif
					//fileInfo.seqNo = p->vcn*m_ntfsType->sectorsPerCluster / 2;
					//fileInfo.seqNo += i*m_ntfsType->sectorsPerCluster / 2 + j / 1024;
					if (!this->GetAttrValue(ATTR_FILE_NAME, szBuf + j, (UCHAR*)szAttrValue))
					{
						continue;
					}
					nameLenOffset = 0x58;
					nameLen = *(UINT8*)(szAttrValue + nameLenOffset);
					//��ȡ�ļ���
					memset(szFileName, 0, sizeof(szFileName));
					memcpy(szFileName, szAttrValue + nameLenOffset + 2, nameLen << 1);
					::WideCharToMultiByte(CP_THREAD_ACP, 0, (LPCWSTR)szFileName, -1, szAnsiName, MAX_PATH * 2, 0, 0);
					
					//���������ļ�����ռ������Щ������Ϣ
					File_Content_Extent_s	*fileExtents = NULL;

					this->GetFileExtent(szBuf + j, clusterOffset + j / 512, &fileExtents);

					CBaseFileObject	*fileObject = new CBaseFileObject;
					fileObject->SetFileName(szAnsiName);
					fileObject->SetFileSize(this->GetFileSize(szBuf+j));
					//�����ļ�����ռ��������Ϣ
					fileObject->SetFileExtent(fileExtents);
					fileObject->SetAccessTime(this->GetAccessTime(szBuf+j));
					fileObject->SetCreateTime(this->GetCreateTime(szBuf+j));
					fileObject->SetModifyTime(this->GetModifyTime(szBuf+j));
					fileArray.push_back(fileObject);
				}
			}
		}
		p = p->next;
	}
	free(szBuf);
}

void CNtfsFileSystem::GetMFTRunList()
{
	UCHAR	tmpBuf[1024] = { 0 };
	UCHAR	tmpAttrValue[1024] = { 0 };
	this->ReadBuf(tmpBuf, m_mftStartCluster*m_sectorsPerCluster,1024);
	UINT16	usnOffset = *(UINT16*)&tmpBuf[4];//�������кŵ�ƫ��ֵ
	memcpy(tmpBuf + 0x1FE, tmpBuf + usnOffset + 2, 2);//�ָ���һ������������ֽ���ʵ����
	memcpy(tmpBuf + 0x3FE, tmpBuf + usnOffset + 4, 2);//�ָ��ڶ�������������ֽ���ʵ����
	if (this->GetAttrValue(ATTR_DATA, tmpBuf, tmpAttrValue))
	{
		UINT16	dataRunOffset = *(UINT16*)&tmpAttrValue[0x20];
		this->GetDataRunList(tmpAttrValue, dataRunOffset, &m_mftRunList);
	}
}

UINT32 CNtfsFileSystem::GetAttrValue(NTFS_ATTRDEF prmAttrTitle,UCHAR prmBuf[],UCHAR *prmAttrValue)
{
	UINT32	fileSign = *(UINT32*)prmBuf;
	if (fileSign != 0x454c4946)
	{
		return 0;
	}
	UINT16 flag = *(UINT16*)&prmBuf[0x16];
	/*if (flag == 0 || flag == 2)
	{
		//�ļ���Ŀ¼��ɾ��
		return 0;
	}*/
	//mft��¼ͷ��ʼƫ��20���������ֽ�Ϊmft��һ�����Ե�ƫ��ֵ
	UINT32	attrOffset = *(UINT16*)&prmBuf[20];//��һ�����Ե�ƫ��ֵ

	UINT32	attrLen = 0;//���Եĳ���
	while (attrOffset < 1024)
	{
		if (prmBuf[attrOffset] == 0xff)
		{
			return 0;
		}
		attrLen = *(UINT16*)&prmBuf[attrOffset + 4];
		//�ҵ���Ӧ�����ԣ���������ֵ����szAttrValue��
		if (prmBuf[attrOffset] == prmAttrTitle)
		{
			memcpy(prmAttrValue, prmBuf + attrOffset, attrLen);
			return 1;
		}
		attrOffset += attrLen;
	}
	return 0;
}

UINT32 CNtfsFileSystem::GetAttrFromAttributeList(NTFS_ATTRDEF prmAttrType,UINT32 prmOffset,UCHAR *prmAttrList,UCHAR *prmAttrValue)
{
	UINT32	*tmpAttrLen = (UINT32*)(prmAttrList+4);
	while(prmOffset+4<*tmpAttrLen)
	{
		UINT16	*tmpLen = (UINT16*)(prmAttrList+prmOffset+4); 
		if(prmAttrList[prmOffset]==prmAttrType)
		{
			memcpy(prmAttrValue,prmAttrList+prmOffset,*tmpLen);
			return prmOffset;
		}
		prmOffset += *tmpLen;
	}
	return 0;
}

void CNtfsFileSystem::GetDataRunList(UCHAR *prmBuf,UINT16 prmRunListOffset,Ntfs_Data_Run **prmList)
{
	UINT64 index_alloc_size = 0;
	UINT64 lcn = 0;
	int j;
	UINT16	bufOffset = prmRunListOffset;
	UINT32	temp = 0;
	*prmList = NULL;
	Ntfs_Data_Run *p = NULL;

	//buff[off]  ����λ��¼���漸���ֽ�Ϊ���� , ��4λ��¼�����ֽ�Ϊ��ʼ�غ�
	//�ȼ��㳤��
	for (;;)
	{
		index_alloc_size = 0;
		for (j = 0; j < prmBuf[bufOffset] % 16; j++)
		{
			index_alloc_size = index_alloc_size + prmBuf[bufOffset + 1 + j] * (UINT64)pow((long double)256, j);
		}

		//, �ټ�����ʼ�غ�
		if (prmBuf[bufOffset + prmBuf[bufOffset] % 16 + prmBuf[bufOffset] / 16] > 127)  //������������
		{
			for (j = 0; j < prmBuf[bufOffset] / 16; j++)
			{

				temp = ~prmBuf[bufOffset + prmBuf[bufOffset] % 16 + 1 + j];
				temp = temp & 255;
				lcn = lcn - temp * (UINT64)pow((long double)256, j);
			}
			lcn = lcn - 1;
		}
		else
		{
			for (j = 0; j < prmBuf[bufOffset] / 16; j++)
			{
				lcn = lcn + prmBuf[bufOffset + prmBuf[bufOffset] % 16 + 1 + j] * (UINT64)pow((long double)256, j);
			}
		}
		Ntfs_Data_Run *datarun = new Ntfs_Data_Run;
		if (*prmList == NULL)
		{
			*prmList = datarun;
		}
		datarun->lcn = lcn;
		datarun->vcn = 0;
		datarun->length = index_alloc_size;//��ʾ��������ռ�ö��ٴ�

		if (p != NULL)
		{
			datarun->vcn += p->length;
			p->next = datarun;
		}
		p = datarun;

		//��ȡ��һ��run list ��ƫ��λ��
		bufOffset = bufOffset + prmBuf[bufOffset] / 16 + prmBuf[bufOffset] % 16 + 1;
		if (0 == prmBuf[bufOffset])
		{
			// run list ����
			break;
		}
	}
}

void CNtfsFileSystem::FreeRunList(Ntfs_Data_Run *prmList)
{
	Ntfs_Data_Run	*p = prmList;
	Ntfs_Data_Run	*q;
	while (p)
	{
		q = p->next;
		delete p;
		p = q;
	}
}

UINT32	CNtfsFileSystem::GetExtendMFTAttrValue(UINT64 prmSeqNum,NTFS_ATTRDEF prmAttrType,UCHAR *prmAttrValue)
{
	UCHAR	tmpBuf[1024] = {0};
	prmSeqNum = prmSeqNum & MFTREFMASK;
	UINT64	tmpOffset = this->GetOffsetByMFTRef(prmSeqNum);
	this->ReadBuf(tmpBuf,tmpOffset/512,1024);
	UINT16	tmpUsnOffset = *(UINT16*)&tmpBuf[4];
	memcpy(tmpBuf+0x1FE,tmpBuf+tmpUsnOffset+2,2);
	memcpy(tmpBuf+0x3FE,tmpBuf+tmpUsnOffset+4,2);
	if(this->GetAttrValue(prmAttrType,tmpBuf,prmAttrValue))
	{
		return 1;
	}
	return 0;
}

UINT64	CNtfsFileSystem::GetOffsetByMFTRef(UINT64 prmSeqNo)
{
	prmSeqNo = prmSeqNo & MFTREFMASK;
	UINT64	tmpOffset = prmSeqNo<<1;
	tmpOffset = tmpOffset * m_bytesPerSector;
	UINT64	tmpVCN = tmpOffset/m_sectorsPerCluster/m_bytesPerSector;
	Ntfs_Data_Run	*p = m_mftRunList;
	while(p!=NULL)
	{
		if(tmpVCN > p->vcn && tmpVCN < p->vcn+p->length)
		{
			break;
		}
		p = p->next;
	}
	if(p==NULL)
	{
		return 0;
	}
	UINT64 tmpOfs = tmpOffset-p->vcn*m_sectorsPerCluster*m_bytesPerSector;
	return tmpOfs + p->lcn*m_sectorsPerCluster*m_bytesPerSector;
}

void CNtfsFileSystem::GetFileFromIndexRoot(UCHAR *prmAttrValue,vector<CBaseFileObject*> *prmFileArray)
{
	if(prmAttrValue[0] != ATTR_INDEX_ROOT)
	{
		return;
	}
	//0x90���Գ���
	UINT32   tmpAttrLen = *(UINT32*)(prmAttrValue+4);
	//��һ��������ƫ��ֵ
	UINT32	 tmpAttrOffset = *(UINT32*)(prmAttrValue+0x30);
	//�ҵ���0x30�����һ��������β��ƫ��ֵ
	UINT32	tmpTotalSize = *(UINT32*)(prmAttrValue+0x34);
	//��ʵ�������С��Ҫ��ȥ��0x30����һ������ƫ����ֵ
	tmpTotalSize -= tmpAttrOffset;
	tmpAttrOffset += 0x30;
	UINT8	tmpFlags = *(UINT8*)(prmAttrValue+0x3C);
	if(tmpFlags==1)
	{
		//��Ҫ�ⲿ�������С�����Ͳ���Ҫ�ٽ�����
		return;
	}
	if(tmpAttrOffset>= tmpAttrLen || tmpAttrOffset+tmpTotalSize > tmpAttrLen)
	{
		return;
	}
	this->ParseFileFromIndex(prmAttrValue,tmpAttrOffset,tmpTotalSize,prmFileArray);
}

void CNtfsFileSystem::GetFileFromAllocIndex(UCHAR *prmAttrValue,vector<CBaseFileObject*> *prmFileArray)
{
	if(prmAttrValue[0]!=ATTR_INDEX_ALLOCATION)
	{
		return;
	}
	UINT32	tmpAttrLen = *(UINT32*)(prmAttrValue+4);
	UINT16	tmpIndexOffset = *(UINT16*)(prmAttrValue+0x20);
	if(tmpIndexOffset>=tmpAttrLen)
	{
		return;
	}
	UINT32		tmpClusterSize = m_sectorsPerCluster * m_bytesPerSector;
	UCHAR		*tmpClusterBuf = (UCHAR*)malloc(tmpClusterSize);
	Ntfs_Data_Run	*tmpDataRunList = NULL;
	this->GetDataRunList(prmAttrValue,tmpIndexOffset,&tmpDataRunList);
	Ntfs_Data_Run *p = tmpDataRunList;
	
	while(p!=NULL)
	{
		//ÿ��ֻ����һ���صĴ�С
		for(int i=0;i<p->length;i++)
		{
			this->ReadBuf(tmpClusterBuf,p->lcn*m_sectorsPerCluster + i*m_sectorsPerCluster,tmpClusterSize);
			//�������кŸ���
			UINT16 usnOffset = *(UINT16*)&tmpClusterBuf[4];
			for (UINT32 j = 0; j < m_sectorsPerCluster; j++)
			{
				memcpy(tmpClusterBuf + 0x1FE + j*m_bytesPerSector, tmpClusterBuf + usnOffset + 2 + j * 2, 2);//�ָ�ÿ��������������ֽ�����
			}
			UINT32	tmpFirstIndex = *(UINT32*)(tmpClusterBuf+0x18);
			tmpFirstIndex += 0x18;
			UINT32	tmpLastIndex = *(UINT32*)(tmpClusterBuf+0x1C);
			this->ParseFileFromIndex(tmpClusterBuf,tmpFirstIndex,tmpLastIndex,prmFileArray);
		}
		p = p->next;
	}
	this->FreeRunList(tmpDataRunList);
	free(tmpClusterBuf);
}

void CNtfsFileSystem::ParseFileFromIndex(UCHAR *prmBuf,UINT16 prmOffset,UINT32 prmBufLen,vector<CBaseFileObject*> *prmFileArray)
{
	wchar_t		tmpUnicodeFileName[MAX_PATH] = {0};
	char		tmpAnsiName[MAX_PATH*2] = {0};
	
	UINT8		tmpNameSpace = 0;
	UINT8		tmpFileNameLen = 0;
	UINT32	tmpAttrOffset = prmOffset;
	UINT64	tmpMFTNum = 0;//mft�ο���

	while(tmpAttrOffset<prmBufLen)
	{
		UINT16 tmpIndexItemLen = *(UINT16*)(prmBuf+tmpAttrOffset+8);
		if(prmBuf[tmpAttrOffset+6]==0 && prmBuf[tmpAttrOffset+7]==0)
		{
			tmpAttrOffset += tmpIndexItemLen;
			continue;
		}
		//�ļ������ռ�
		tmpNameSpace = *(UINT8*)(prmBuf+tmpAttrOffset+0x51);
		tmpFileNameLen = *(UINT8*)(prmBuf+tmpAttrOffset + 0x50);
		tmpMFTNum = *(UINT64*)(prmBuf+tmpAttrOffset);
		tmpMFTNum = tmpMFTNum & MFTREFMASK;
		if(tmpMFTNum<=25)
		{
			//mft�ο���С��25�Ķ����Բ��ÿ���
			tmpAttrOffset += tmpIndexItemLen;
			continue;
		}
		UINT64	tmpFileOffset = this->GetOffsetByMFTRef(tmpMFTNum);
		//�ļ�û�д�ŵ����ս�������У��ٽ��µ��ļ����뵽������
		if(!IsFileExists(tmpFileOffset/m_bytesPerSector,prmFileArray))
		{
			CBaseFileObject	*tmpFileObject = new CBaseFileObject();
			//ֻ�����ļ������ռ�Ϊdos��ʱ���ȥmftͷ������win32�ļ������ӿ�����ٶȡ�
			//������windows������Ŀ¼���м�ǧ���ļ�����������Ҫ��ǧ�εĴ��̶�����Ч�ʼ���
			if(tmpNameSpace==2)
			{
				//if(!this->SetFileWin32Name(tmpFileOffset,tmpFileObject))
				
				CStringUtil	tmpFileNameStr =  this->GetFileWin32Name(tmpFileOffset);
				tmpFileObject->SetFileName(tmpFileNameStr);
				if(tmpFileNameStr.GetLength()==0)
				{
					//���index����dos�����ռ䣬��mftͷ��û���ҵ�win32�����ռ���ļ������������ļ���Ҫ
					tmpFileObject->Destroy();
					tmpAttrOffset += tmpIndexItemLen;
					continue;
				}
			}
			else
			{
				memset(tmpUnicodeFileName,0,sizeof(tmpUnicodeFileName));
				memcpy(tmpUnicodeFileName,prmBuf+tmpAttrOffset+0x52,tmpFileNameLen<<1);
#ifdef _UNICODE
				tmpFileObject->SetFileName(tmpUnicodeFileName);
#else
				memset(tmpAnsiName,0,sizeof(tmpAnsiName));
				::WideCharToMultiByte(CP_ACP,0,tmpUnicodeFileName,-1,tmpAnsiName,MAX_PATH*2,0,0);
				tmpFileObject->SetFileName(tmpAnsiName);
#endif
			}
			tmpFileObject->SetFileStartSector(tmpFileOffset/m_bytesPerSector);
			UINT32	tmpFileFlags = *(UINT32*)(prmBuf+tmpAttrOffset+0x48);
			if(tmpFileFlags & 0x10000000)
			{
				tmpFileObject->SetFileType(FILE_OBJECT_TYPE_DIRECTORY);
			}
			else
			{
				tmpFileObject->SetFileType(FILE_OBJECT_TYPE_FILE);
			}
			UINT64	tmpFileSize = *(UINT64*)(prmBuf+tmpAttrOffset+0x40);
			tmpFileObject->SetFileSize(tmpFileSize);
			prmFileArray->push_back(tmpFileObject);
			
		}
		tmpAttrOffset += tmpIndexItemLen;
	}
}

CStringUtil CNtfsFileSystem::GetFileWin32Name(UINT64 prmOffset)
{
	UCHAR		tmpBuf[1024] = {0};
	wchar_t		tmpFileName[MAX_PATH] = {0};
	char		tmpAnsiName[MAX_PATH*2] = {0};
	this->ReadBuf(tmpBuf,prmOffset/m_bytesPerSector,1024);
	UINT16	tmpUsnOffset = *(UINT16*)&tmpBuf[4];
	memcpy(tmpBuf+0x1FE,tmpBuf+tmpUsnOffset+2,2);
	memcpy(tmpBuf+0x3FE,tmpBuf+tmpUsnOffset+4,2);
	UINT32	fileSign = *(UINT32*)tmpBuf;
	if (fileSign != 0x454c4946)
	{
		return FALSE;
	}
	UINT16 flag = *(UINT16*)&tmpBuf[0x16];
	if (flag == 0 || flag == 2)
	{
		//�ļ���Ŀ¼��ɾ��
		return FALSE;
	}
	//mft��¼ͷ��ʼƫ��20���������ֽ�Ϊmft��һ�����Ե�ƫ��ֵ
	UINT32	attrOffset = *(UINT16*)&tmpBuf[20];//��һ�����Ե�ƫ��ֵ

	UINT32	attrLen = 0;//���Եĳ���
	while (attrOffset < 1024)
	{
		if (tmpBuf[attrOffset] == 0xff)
		{
			return 0;
		}
		attrLen = *(UINT16*)&tmpBuf[attrOffset + 4];
		//�ҵ���Ӧ�����ԣ���������ֵ����szAttrValue��
		if (tmpBuf[attrOffset] == ATTR_FILE_NAME && tmpBuf[attrOffset+0x59]!=2)
		{
			UINT8	tmpFileLen = tmpBuf[attrOffset + 0x58];
			memset(tmpFileName,0,sizeof(tmpFileName));
			memcpy(tmpFileName,tmpBuf+attrOffset+0x5A,tmpFileLen<<1);
#ifdef _UNICODE
			return tmpFileName;
#else
			::WideCharToMultiByte(CP_ACP,0,tmpFileName,-1,tmpAnsiName,MAX_PATH*2,0,0);
			return tmpAnsiName;
#endif
			
		}
		attrOffset += attrLen;
	}
	return TEXT("");
}

BOOL CNtfsFileSystem::IsFileExists(UINT64 prmStartSector,vector<CBaseFileObject*> *prmFileArray)
{
	for(size_t i=0;i<prmFileArray->size();i++)
	{
		if(prmStartSector == prmFileArray->at(i)->GetFileStartSector())
		{
			return TRUE;
		}
	}
	return FALSE;
}

void CNtfsFileSystem::GetFileExtent(UCHAR *prmBuf,UINT64 prmMftSector,File_Content_Extent_s **prmFileExtent)
{
	UCHAR	szAttrValue[1024] = { 0 };
	UCHAR	szAttrList[1024] = {0};
	UCHAR	szExtentAttrValue[1024] = {0};
	UINT32 readBytes = 0;
	Ntfs_Data_Run		*runList = NULL;
	Ntfs_Data_Run		*q = NULL;
	UINT32		attrLen = 0;
	UINT32		attrOff = 0;
	UINT32		result = 0;

	//��ȡ0x20�����б�
	if(this->GetAttrValue(ATTR_ATTRIBUTE_LIST,prmBuf,szAttrList))
	{
		UINT32	tmpOffset = 0x18;
		while(tmpOffset=this->GetAttrFromAttributeList(ATTR_DATA,tmpOffset,szAttrList,szAttrValue))
		{
			UINT16	tmpLen = *(UINT16*)(szAttrValue+4);
			UINT64		seqNum =   *(UINT64*)(szAttrValue + 0x10);
			if(this->GetExtendMFTAttrValue(seqNum,ATTR_DATA,szExtentAttrValue))
			{
				UINT16	runlistOffset = *(UINT16*)&szExtentAttrValue[0x20];
				Ntfs_Data_Run *p = NULL;
				this->GetDataRunList(szExtentAttrValue,runlistOffset,&p);
				if (runList == NULL)
				{
					runList = p;
					q = p;
				}
				else
				{//0x20���ж��0x80�������ԣ���Ҫ�ϲ����run list
					while (q->next)
					{
						q = q->next;
					}
					q->next = p;
				}
			}
			tmpOffset += tmpLen;
		}
	}
	if(runList!=NULL)
	{
		goto END;
	}
	//��ȡ0x80��������
	if (this->GetAttrValue(ATTR_DATA, prmBuf, szAttrValue))
	{
		attrLen = *(UINT32*)&szAttrValue[4];
		if (szAttrValue[8] == 0)
		{//��פ���ԣ�˵���ļ�������0x80������
			UINT32 fileLength = *(UINT32*)&szAttrValue[0x10];
			*prmFileExtent = new File_Content_Extent_s;
			(*prmFileExtent)->startSector = prmMftSector;
			(*prmFileExtent)->totalSector = 2;
			(*prmFileExtent)->isPersist = 1;
		}
		else //�ļ�����Ϊ�ǳ�פ
		{
			UINT16	runlistOffset = *(UINT16*)&szAttrValue[0x20];
			this->GetDataRunList(szAttrValue, runlistOffset, &runList);
			goto END;
		}
	}
END:
	Ntfs_Data_Run *p = runList;
	File_Content_Extent_s *tmpExtent = *prmFileExtent;
	while (p != NULL && p->lcn != 0 && p->length != 0 && p->length < m_totalSector / m_sectorsPerCluster)
	{
		File_Content_Extent_s *t = new File_Content_Extent_s;
		t->totalSector = p->length*m_sectorsPerCluster;
		t->startSector = p->lcn*m_sectorsPerCluster;
		//������Щ������ɾ���ļ�
		if (tmpExtent == NULL)
		{
			tmpExtent = t;
			*prmFileExtent = t;
		}
		else
		{
			tmpExtent->next = t;
			tmpExtent = t;
		}
		p = p->next;
	}
	this->FreeRunList(runList);
}

UINT64	CNtfsFileSystem::ReadFileContent(UCHAR prmDstBuf[],UINT64 prmByteOff,UINT64 prmByteToRead, 
	UINT64 prmFileSize,File_Content_Extent_s *prmFileExtent)
{
	UINT64		tmpResult = 0;
	UCHAR		tmpBuf[512] = {0};
	UINT64		tmpByteRead = 0;
	if(prmFileExtent==NULL)
	{
		return 0;
	}
	if(prmByteOff>prmFileSize)
	{
		return 0;
	}
	if(prmByteOff+prmByteToRead>prmFileSize)
	{
		prmByteToRead = prmFileSize-prmByteOff;
	}
	File_Content_Extent_s *p = prmFileExtent;

	if (p->isPersist)
	{
		UCHAR	szBuf[1024] = { 0 };
		UCHAR	szAttr[1024] = { 0 };
		this->ReadBuf(szBuf, p->startSector, 1024);
		UINT16	usnOffset = *(UINT16*)&szBuf[4];//�������кŵ�ƫ��ֵ
		memcpy(szBuf + 0x1FE, szBuf + usnOffset + 2, 2);//�ָ���һ������������ֽ���ʵ����
		memcpy(szBuf + 0x3FE, szBuf + usnOffset + 4, 2);//�ָ��ڶ�������������ֽ���ʵ����
		if (this->GetAttrValue(ATTR_DATA, szBuf, szAttr))
		{
			memcpy(prmDstBuf, szAttr + 0x18 + (UINT32)prmByteOff, (UINT32)prmByteToRead);
			return (UINT32)prmByteToRead;
		}
	}

	while (p && prmByteOff >= p->totalSector*m_bytesPerSector)
	{//�ҵ�offӦ�����ĸ�run list��
		prmByteOff -= p->totalSector*m_bytesPerSector;
		p = p->next;
	}
	//ƫ��ֵ�Ѿ��������ļ���С�����ܶ�
	if (p == NULL)
	{
		goto END;
	}

	//�����ļ�ƫ�Ʋ���512�����������
	//UINT32	tmpOffset = prmByteOff&(~511);
	//�ȶ�ȡ����
	this->ReadBuf(tmpBuf,p->startSector+prmByteOff/512,512);
	//Ҫ��ȡ��ƫ��ΪprmByteOff%512����ȡ���ֽ���Ϊ512-prmByte%512
	memcpy(prmDstBuf,tmpBuf+(prmByteOff&(511)),512-(prmByteOff&511));
	tmpResult = 512-(prmByteOff&511);
	prmByteOff +=tmpResult;
	if(p->startSector + prmByteOff/512 == p->totalSector)
	{
		p = p->next;
		prmByteOff = 0;
	}

	//��ǰrun list���ж����ֽڿɶ�
	UINT64 runListRemainSize = p->totalSector*m_bytesPerSector - prmByteOff;
	UINT64	fileOffset = p->startSector*m_bytesPerSector+prmByteOff;
	while (p && (runListRemainSize < (prmByteToRead - tmpResult)))
	{
		tmpByteRead = this->ReadBuf(prmDstBuf + tmpResult, fileOffset / m_bytesPerSector, (UINT32)runListRemainSize);
		tmpResult += tmpByteRead;
		if (tmpByteRead != runListRemainSize)
		{
			goto END;
		}
		p = p->next;
		if(p==NULL)
		{
			goto END;
		}
		runListRemainSize = p->totalSector*m_bytesPerSector;
		fileOffset = p->startSector*m_bytesPerSector;

		//result = fread(szBuf+ byteReaded,)
	}
	//�����ļ���СΪ2k���������ȡ3k���ݣ���ʱ���ֻ�ܶ�ȡ2k���ݣ��ͻ����p=NULL�����
	if (p == NULL)
	{
		goto END;
	}

	//�������ʣ��һЩ�ֽ�
	if(prmByteToRead!=tmpResult)
	{
		tmpByteRead  = this->ReadBuf(prmDstBuf + tmpResult, fileOffset  / m_bytesPerSector, prmByteToRead-tmpResult);
		tmpResult += tmpByteRead;
	}
END:
	return tmpResult;
}

void CNtfsFileSystem::FreeFileExtent(File_Content_Extent_s *prmFileExtent)
{
	File_Content_Extent_s *p = prmFileExtent;
	File_Content_Extent_s *q = NULL;
	while (p != NULL)
	{
		q = p;
		p = p->next;
		delete q;
	}
}

UINT64	CNtfsFileSystem::GetOffsetByFileName(UINT64 prmParentFileOffset,const CStringUtil &prmFileName)
{
	UCHAR		tmpBuf[1024] = {0};
	UCHAR		tmpAttrValue[1024] = {0};
	UCHAR		tmpAttrList[1024] = {0};
	UCHAR		tmpExtentMFTValue[1024] = {0};
	UINT64		tmpFileOffset = 0;
	if(prmParentFileOffset==0)
	{
		return NULL;
	}
	this->ReadBuf(tmpBuf,prmParentFileOffset/m_bytesPerSector,1024);
	UINT16	tmpUsnOffset = *(UINT16*)&tmpBuf[4];//�������кŵ�ƫ��ֵ
	memcpy(tmpBuf+0x1FE,tmpBuf+tmpUsnOffset+2,2);
	memcpy(tmpBuf+0x3FE,tmpBuf+tmpUsnOffset+4,2);
	//��ȡATTR_LIST $0x20�б�����
	if(this->GetAttrValue(ATTR_ATTRIBUTE_LIST,tmpBuf,tmpAttrList))
	{
		UINT32	tmpOffset = 0x18;
		while(tmpOffset=this->GetAttrFromAttributeList(ATTR_INDEX_ROOT,tmpOffset,tmpAttrList,tmpAttrValue))
		{
			UINT16	*tmpLen = (UINT16*)(tmpAttrValue+4);
			tmpOffset += *tmpLen;
			UINT64	seqNum = *(UINT64*)&tmpAttrValue[0x10];
			seqNum = seqNum & MFTREFMASK;
			this->GetExtendMFTAttrValue(seqNum,ATTR_INDEX_ROOT,tmpExtentMFTValue);
			tmpFileOffset = this->GetOffsetFromRootByFileName(tmpExtentMFTValue,prmFileName);
			if(tmpFileOffset!=0)
			{
				return tmpFileOffset;
			}
		}

		tmpOffset = 0x18;
		while(tmpOffset=this->GetAttrFromAttributeList(ATTR_INDEX_ALLOCATION,tmpOffset,tmpAttrList,tmpAttrValue))
		{
			UINT16	*tmpLen = (UINT16*)(tmpAttrValue+4);
			tmpOffset += *tmpLen;
			UINT64	seqNum = *(UINT64*)&tmpAttrValue[0x10];
			seqNum = seqNum & MFTREFMASK;
			this->GetExtendMFTAttrValue(seqNum,ATTR_INDEX_ALLOCATION,tmpExtentMFTValue);
			tmpFileOffset = this->GetOffsetFromAllocByFileName(tmpExtentMFTValue,prmFileName);
			if(tmpFileOffset!=0)
			{
				return tmpFileOffset;
			}
		}
	}
	if(this->GetAttrValue(ATTR_INDEX_ROOT,tmpBuf,tmpAttrValue))
	{
		tmpFileOffset = this->GetOffsetFromRootByFileName(tmpAttrValue,prmFileName);
		if(tmpFileOffset!=0)
		{
			return tmpFileOffset;
		}
	}
	if(this->GetAttrValue(ATTR_INDEX_ALLOCATION,tmpBuf,tmpAttrValue))
	{
		tmpFileOffset = this->GetOffsetFromAllocByFileName(tmpAttrValue,prmFileName);
	}
	return tmpFileOffset;
}

UINT64  CNtfsFileSystem::GetOffsetFromRootByFileName(UCHAR *prmAttrValue,const CStringUtil &prmFileName)
{
	if(prmAttrValue[0] != ATTR_INDEX_ROOT)
	{
		return 0;
	}
	//0x90���Գ���
	UINT32   tmpAttrLen = *(UINT32*)(prmAttrValue+4);
	//��һ��������ƫ��ֵ
	UINT32	 tmpAttrOffset = *(UINT32*)(prmAttrValue+0x30);
	//�ҵ���0x30�����һ��������β��ƫ��ֵ
	UINT32	tmpTotalSize = *(UINT32*)(prmAttrValue+0x34);
	//��ʵ�������С��Ҫ��ȥ��0x30����һ������ƫ����ֵ
	tmpTotalSize -= tmpAttrOffset;
	tmpAttrOffset += 0x30;
	UINT8	tmpFlags = *(UINT8*)(prmAttrValue+0x3C);
	if(tmpFlags==1)
	{
		//��Ҫ�ⲿ�������С�����Ͳ���Ҫ�ٽ�����
		return 0;
	}
	if(tmpAttrOffset>= tmpAttrLen || tmpAttrOffset+tmpTotalSize > tmpAttrLen)
	{
		return 0;
	}
	return this->GetOffsetByFileNameInIndex(prmAttrValue,tmpAttrOffset,tmpTotalSize,prmFileName);
}

UINT64	CNtfsFileSystem::GetOffsetFromAllocByFileName(UCHAR *prmAttrValue,const CStringUtil &prmFileName)
{
	UINT64	tmpFileOffset = 0;
	if(prmAttrValue[0]!=ATTR_INDEX_ALLOCATION)
	{
		return 0;
	}
	UINT32	tmpAttrLen = *(UINT32*)(prmAttrValue+4);
	UINT16	tmpIndexOffset = *(UINT16*)(prmAttrValue+0x20);
	if(tmpIndexOffset>=tmpAttrLen)
	{
		return 0;
	}
	UINT32		tmpClusterSize = m_sectorsPerCluster * m_bytesPerSector;
	UCHAR		*tmpClusterBuf = (UCHAR*)malloc(tmpClusterSize);
	Ntfs_Data_Run	*tmpDataRunList = NULL;
	this->GetDataRunList(prmAttrValue,tmpIndexOffset,&tmpDataRunList);
	Ntfs_Data_Run *p = tmpDataRunList;

	while(p!=NULL)
	{
		//ÿ��ֻ����һ���صĴ�С
		for(int i=0;i<p->length;i++)
		{
			this->ReadBuf(tmpClusterBuf,p->lcn*m_sectorsPerCluster + i*m_sectorsPerCluster,tmpClusterSize);
			//�������кŸ���
			UINT16 usnOffset = *(UINT16*)&tmpClusterBuf[4];
			for (UINT32 j = 0; j < m_sectorsPerCluster; j++)
			{
				memcpy(tmpClusterBuf + 0x1FE + j*m_bytesPerSector, tmpClusterBuf + usnOffset + 2 + j * 2, 2);//�ָ�ÿ��������������ֽ�����
			}
			UINT32	tmpFirstIndex = *(UINT32*)(tmpClusterBuf+0x18);
			tmpFirstIndex += 0x18;
			UINT32	tmpLastIndex = *(UINT32*)(tmpClusterBuf+0x1C);
			tmpFileOffset = this->GetOffsetByFileNameInIndex(tmpClusterBuf,tmpFirstIndex,tmpLastIndex,prmFileName);
			if(tmpFileOffset!=0)
			{
				break;
			}
		}
		if(tmpFileOffset!=0)
		{
			break;
		}
		p = p->next;
	}
	this->FreeRunList(tmpDataRunList);
	free(tmpClusterBuf);
	return tmpFileOffset;
}

UINT64	CNtfsFileSystem::GetOffsetByFileNameInIndex(UCHAR *prmBuf,UINT16 prmOffset,UINT32 prmBufLen,const CStringUtil &prmFileName)
{
	UINT64		tmpResult = 0;
	wchar_t		tmpUnicodeFileName[MAX_PATH] = {0};
	char		tmpAnsiName[MAX_PATH*2] = {0};
	CStringUtil	tmpFileName;

	UINT8		tmpNameSpace = 0;
	UINT8		tmpFileNameLen = 0;
	UINT32	tmpAttrOffset = prmOffset;
	UINT64	tmpMFTNum = 0;//mft�ο���

	while(tmpAttrOffset<prmBufLen)
	{
		UINT16 tmpIndexItemLen = *(UINT16*)(prmBuf+tmpAttrOffset+8);
		if(prmBuf[tmpAttrOffset+6]==0 && prmBuf[tmpAttrOffset+7]==0)
		{
			tmpAttrOffset += tmpIndexItemLen;
			continue;
		}
		//�ļ������ռ�
		tmpNameSpace = *(UINT8*)(prmBuf+tmpAttrOffset+0x51);
		tmpFileNameLen = *(UINT8*)(prmBuf+tmpAttrOffset + 0x50);
		tmpMFTNum = *(UINT64*)(prmBuf+tmpAttrOffset);
		tmpMFTNum = tmpMFTNum & MFTREFMASK;
		if(tmpMFTNum<=25)
		{
			//mft�ο���С��25�Ķ����Բ��ÿ���
			tmpAttrOffset += tmpIndexItemLen;
			continue;
		}
		UINT64	tmpFileOffset = this->GetOffsetByMFTRef(tmpMFTNum);
		//�ļ�û�д�ŵ����ս�������У��ٽ��µ��ļ����뵽������
		if(tmpNameSpace==2)
		{
			tmpFileName = this->GetFileWin32Name(tmpFileOffset);
		}
		else
		{
			memset(tmpUnicodeFileName,0,sizeof(tmpUnicodeFileName));
			memcpy(tmpUnicodeFileName,prmBuf+tmpAttrOffset+0x52,tmpFileNameLen<<1);
#ifdef _UNICODE
			//tmpFileObject->SetFileName(tmpUnicodeFileName);
			tmpFileName = tmpUnicodeFileName;
#else
			memset(tmpAnsiName,0,sizeof(tmpAnsiName));
			::WideCharToMultiByte(CP_ACP,0,tmpUnicodeFileName,-1,tmpAnsiName,MAX_PATH*2,0,0);
			tmpFileName = tmpAnsiName;
#endif
		}
		if(tmpFileName.CompareNoCase(prmFileName)==0)
		{
			tmpResult = tmpFileOffset;
			break;
		}
		tmpAttrOffset += tmpIndexItemLen;
	}
	return tmpResult; 
}

UINT64	CNtfsFileSystem::GetFileSize(UCHAR *prmMFTRecord)
{
	UCHAR	szAttrValue[1024] = { 0 };
	UCHAR	szAttrList[1024] = {0};
	UCHAR	szExtendMFTAttrValue[1024] = {0};
	UINT64	tmpResult = 0;

	//����0x20����
	if(this->GetAttrValue(ATTR_ATTRIBUTE_LIST,prmMFTRecord,szAttrList))
	{
		UINT32	tmpOffset = 0x18;
		while(tmpOffset=this->GetAttrFromAttributeList(ATTR_DATA,tmpOffset,szAttrList,szAttrValue))
		{
			UINT16	*tmpLen = (UINT16*)(szAttrValue+4);
			UINT64		seqNum =   *(UINT64*)(szAttrValue + 0x10);
			if(this->GetExtendMFTAttrValue(seqNum,ATTR_DATA,szExtendMFTAttrValue))
			{
				tmpResult += *(UINT64*)&szExtendMFTAttrValue[0x30];
			}
		}
	}
	if(tmpResult != 0)
	{
		return tmpResult;
	}

	if (this->GetAttrValue(ATTR_DATA, prmMFTRecord, szAttrValue))
	{//�ҵ�0x80��������
		if (szAttrValue[8] == 0)
		{//��פ���ԣ����ļ���СΪ szBuf[attrIndex+0x10]�������ֽ�
			tmpResult = *(UINT32*)&szAttrValue[0x10];
		}
		else
		{
			tmpResult = *(UINT64*)&szAttrValue[0x30];
		}
		return tmpResult;
	}
	return 0;
}

CStringUtil	CNtfsFileSystem::GetAccessTime(UCHAR szBuf[])
{
	UCHAR	szAttrValue[1024] = { 0 };
	if (this->GetAttrValue(ATTR_STANDARD, szBuf, szAttrValue))
	{
		UINT64 accessTime = *(UINT64*)(szAttrValue + 0x30);
		return this->FileTimeToString(accessTime);
	}
	return "";
}

CStringUtil CNtfsFileSystem::GetModifyTime(UCHAR szBuf[])
{
	UCHAR	szAttrValue[1024] = { 0 };
	if (this->GetAttrValue(ATTR_STANDARD, szBuf, szAttrValue))
	{
		UINT64 modifyTime = *(UINT64*)(szAttrValue + 0x20);
		return this->FileTimeToString(modifyTime);
	}
	return "";
}

CStringUtil CNtfsFileSystem::GetCreateTime(UCHAR szBuf[])
{
	UCHAR	szAttrValue[1024] = { 0 };
	if (this->GetAttrValue(ATTR_STANDARD, szBuf, szAttrValue))
	{
		//UINT16 offset = *(UINT16*)(szAttrValue + 0x0A);
		UINT64 createTime = *(UINT64*)(szAttrValue + 0x18);
		return this->FileTimeToString(createTime);
	}
	return "";
}

CStringUtil	CNtfsFileSystem::FileTimeToString(UINT64 prmFileTime)
{
	TCHAR	szBuf[128] = { 0 };
	FILETIME fileTime;
	SYSTEMTIME	systemTime;
	TIME_ZONE_INFORMATION tz;
	fileTime.dwLowDateTime = prmFileTime & 0xFFFFFFFF;
	fileTime.dwHighDateTime = (prmFileTime & 0xFFFFFFFF00000000) >> 32;
	::FileTimeToSystemTime(&fileTime, &systemTime);
	::GetTimeZoneInformation(&tz);
	long lTime = systemTime.wHour * 60 + systemTime.wMinute;
	lTime -= tz.Bias;
	systemTime.wHour = (WORD)lTime / 60;
	systemTime.wMinute = lTime % 60;

	sprintf_s(szBuf, 128, _T("%4d-%02d-%02d %02d:%02d:%02d"), systemTime.wYear, systemTime.wMonth, systemTime.wDay,
		systemTime.wHour, systemTime.wMinute, systemTime.wSecond);
	return szBuf;
}

FILE_OBJECT_TYPE CNtfsFileSystem::GetFileType(UCHAR *prmMFTRecord)
{
	UINT16	tmpFlag = *(UINT16*)(prmMFTRecord+0x16);
	if(tmpFlag==1)
	{
		return FILE_OBJECT_TYPE_FILE;
	}
	else if(tmpFlag==2)
	{
		return FILE_OBJECT_TYPE_DIRECTORY;
	}
	return FILE_OBJECT_TYPE_UNKNOWN;
}