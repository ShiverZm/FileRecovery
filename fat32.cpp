#include "fat32.h"
#include <shlwapi.h>

#include "commutil.h"

#pragma comment(lib,"shlwapi.lib")

CFat32FileSystem::CFat32FileSystem(IBaseReader	*prmReader)
	:CBaseFileSystem(prmReader)
{
	m_fatTable = NULL;
}

CFat32FileSystem::~CFat32FileSystem()
{
	free(m_fatTable);
}

void CFat32FileSystem::Init()
{
	//��ȡfat32ͷ������
	this->ReadBuf((UCHAR*)&m_fatSector,0,512);

	m_fatNum = m_fatSector.BPB_FATSz32 * m_fatSector.BPB_BytsPerSec / 4;
	m_fatTable = (UINT32*)malloc(sizeof(UINT32)*m_fatNum);
	memset(m_fatTable, 0, sizeof(UINT32)*m_fatNum);
	if (m_fatTable == NULL)
	{
		return;
	}
	//��ȡ�ļ������
	this->ReadBuf((UCHAR*)m_fatTable, m_fatSector.BPB_ResvdSecCnt, sizeof(UINT32)*m_fatNum);
	m_bytesPerSector = m_fatSector.BPB_BytsPerSec;
	m_sectorsPerCluster = m_fatSector.BPB_SecPerClus;

	//��ȡfat32��Ŀ¼��Ϣ
	m_rootDirectory = new CBaseFileObject;
	m_rootDirectory->SetFileType(FILE_OBJECT_TYPE_ROOT);
	m_rootDirectory->SetFileSize(0);
	m_rootDirectory->SetFileName(TEXT(""));
	m_rootDirectory->SetPath(TEXT(""));
	UINT64	tmpRootStartSector = m_fatSector.BPB_FATSz32 * m_fatSector.BPB_NumFATs;
	tmpRootStartSector += m_fatSector.BPB_ResvdSecCnt;
	tmpRootStartSector += (2-m_fatSector.BPB_RootClus)*m_fatSector.BPB_SecPerClus;
	m_rootDirectory->SetFileStartSector(tmpRootStartSector);
}

UINT64	CFat32FileSystem::ReadFileContent(CBaseFileObject *prmFileObject, UCHAR prmDstBuf[], UINT64 prmByteOff, UINT64 prmByteToRead)
{
	if(prmFileObject->GetFileType()!=FILE_OBJECT_TYPE_FILE)
	{
		return 0;
	}
	UINT64		tmpFileSize = prmFileObject->GetFileSize();
	if(tmpFileSize>=0xFFFFFFFF)
	{
		return 0;
	}
	if(prmByteOff>=tmpFileSize)
	{
		return 0;
	}
	UINT32		tmpRemainSize = (UINT32)tmpFileSize-(UINT32)prmByteOff;
	if(tmpRemainSize<prmByteToRead)
	{
		prmByteToRead = tmpRemainSize;
	}
	UINT32		tmpClusterSize = m_fatSector.BPB_SecPerClus*m_fatSector.BPB_BytsPerSec;
	UINT32		tmpClusterNum = (UINT32)prmFileObject->GetFileStartSector();
	//���ݷ����������ż���غ�
	tmpClusterNum -= m_fatSector.BPB_FATSz32*m_fatSector.BPB_NumFATs;
	tmpClusterNum -= m_fatSector.BPB_ResvdSecCnt;
	tmpClusterNum = tmpClusterNum/m_fatSector.BPB_SecPerClus+2;

	while(prmByteOff>=tmpClusterSize && tmpClusterNum)
	{
		prmByteOff -= tmpClusterSize;
		tmpClusterNum = this->GetNextCluster(tmpClusterNum);
	}
	if(tmpClusterNum==0)
	{
		//�Ƿ�����
		return 0;
	}
	char	*tmpBuf = (char*)malloc(tmpClusterSize);
	if(tmpBuf==NULL)
	{
		return 0;
	}
	UINT32 tmpOffset = m_fatSector.BPB_FATSz32*m_fatSector.BPB_NumFATs;
	tmpOffset += m_fatSector.BPB_ResvdSecCnt;
	tmpOffset += (tmpClusterNum-m_fatSector.BPB_RootClus)*m_fatSector.BPB_SecPerClus;
	//��ȡprmByteOff����fat32�����еĴ�����
	UINT64 tmpRet = this->ReadBuf((UCHAR*)tmpBuf,tmpOffset,tmpClusterSize);
	UINT64		tmpResult = 0;
	if(prmByteToRead<=tmpClusterSize-prmByteOff)
	{
		//��ȡ������С��һ�ش�С
		memcpy(prmDstBuf,tmpBuf+(UINT32)prmByteOff,(UINT32)prmByteToRead);
		tmpResult = prmByteToRead;
		goto SUCCESS;
	}
	else
	{
		//��һ�����������ݷ��뻺����
		memcpy(prmDstBuf,tmpBuf+(UINT32)prmByteOff,tmpClusterSize-(UINT32)prmByteOff);
		tmpResult = tmpClusterSize-prmByteOff;
		tmpClusterNum = this->GetNextCluster(tmpClusterNum);
		//���ٴ���ȡ���ݴ�С
		prmByteToRead = prmByteToRead - tmpResult;
	}
	//ʣ�µ��������ض�ȡ
	for(int i=0;i<prmByteToRead/tmpClusterSize;i++)
	{
		tmpOffset = m_fatSector.BPB_FATSz32*m_fatSector.BPB_NumFATs;
		tmpOffset += m_fatSector.BPB_ResvdSecCnt;
		tmpOffset += (tmpClusterNum-m_fatSector.BPB_RootClus)*m_fatSector.BPB_SecPerClus;
		this->ReadBuf(prmDstBuf+tmpResult,tmpOffset,tmpClusterSize);
		tmpResult += tmpClusterSize;
		prmByteToRead = prmByteToRead - tmpClusterSize;
		tmpClusterNum = this->GetNextCluster(tmpClusterNum);
	}
	//���ǿ��ܻ���������Ҫ��ȡ��������һ�ش�С
	if(prmByteToRead>0)
	{
		tmpOffset = m_fatSector.BPB_FATSz32*m_fatSector.BPB_NumFATs;
		tmpOffset += m_fatSector.BPB_ResvdSecCnt;
		tmpOffset += (tmpClusterNum-m_fatSector.BPB_RootClus)*m_fatSector.BPB_SecPerClus;
		this->ReadBuf(prmDstBuf+tmpResult,tmpOffset,prmByteToRead);
		tmpResult += prmByteToRead;
	}
SUCCESS:
	free(tmpBuf);
	return tmpResult;
}

CBaseFileObject *CFat32FileSystem::GetFileObjectByPath(LPCTSTR prmFileName)
{
	if(_tcslen(prmFileName)>MAX_PATH*4)
	{
		return NULL;
	}
	TCHAR		tmpFullFileName[MAX_PATH*4] = {0};
	memcpy(tmpFullFileName,prmFileName,_tcslen(prmFileName)*sizeof(TCHAR));
	CStringUtil		tmpString(prmFileName);
	tmpString.ReplaceStr(_T("/"),_T("\\"));
	vector<CStringUtil>		tmpStrArray;
	//���ļ�����"\"���зָ�
	tmpString.SplitString(tmpStrArray,_T("\\"));
	if(tmpStrArray.size()==0)
	{
		return NULL;
	}
	TCHAR		tmpFileName[MAX_PATH] = {0};

	//�߼��ش�С
	UINT32		tmpClusterSize = m_fatSector.BPB_BytsPerSec*m_fatSector.BPB_SecPerClus;
	UCHAR		*tmpBuf = (UCHAR*)malloc(sizeof(UCHAR)*tmpClusterSize);

	DIR_ENTRY_s		*tmpDirEntry;
	//
	UINT32			tmpClusterNum = 2;
	UINT32			tmpOffset = 0;
	UINT32			i = 0;
	for(i=0;i<tmpStrArray.size();i++)
	{
		while(tmpClusterNum)
		{
			tmpOffset = m_fatSector.BPB_FATSz32*m_fatSector.BPB_NumFATs;
			tmpOffset += m_fatSector.BPB_ResvdSecCnt;
			tmpOffset += (tmpClusterNum-m_fatSector.BPB_RootClus)*m_fatSector.BPB_SecPerClus;
			//ÿ�ζ�ȡһ���߼��ش�С������
			this->ReadBuf(tmpBuf,tmpOffset,tmpClusterSize);
			UINT32	 j = 0;
			for(j=0;j<tmpClusterSize;j+=32)
			{
				tmpDirEntry = (DIR_ENTRY_s*)(tmpBuf+j);
				if(tmpDirEntry->name[0]==0)
				{
					break;
				}
				//��¼��ǰ�ļ���
				DIR_ENTRY_s	*tmpFirstEntry = tmpDirEntry;
				//�ҵ���һ���ǳ��ļ�����ʶ���ļ���
				while(tmpDirEntry->attr==0x0F)
				{
					j+=32;
					tmpDirEntry = (DIR_ENTRY_s*)(tmpBuf+j);
				}
				if(tmpDirEntry->name[0]==0xE5)//������ɾ�����ļ�
				{
					continue;
				}
				if(tmpFirstEntry==tmpDirEntry)
				{
					this->ParseShortFileName(tmpDirEntry,tmpFileName,MAX_PATH);
				}
				else
				{
					this->ParseLongFileName(tmpFileName,MAX_PATH,tmpFirstEntry,tmpDirEntry);
				}
				if(tmpStrArray[i].CompareNoCase(tmpFileName)==0)
				{
					break;
				}
			}
			if(j>=tmpClusterSize)
			{
				tmpClusterNum = this->GetNextCluster(tmpClusterNum);
			}
			else
			{
				tmpClusterNum = this->ParseStartCluster(tmpDirEntry);
				break;
			}
		}
	}
	
	if(i>=tmpStrArray.size())
	{
		CBaseFileObject	*tmpFileObject = new CBaseFileObject;
		if(tmpDirEntry->attr & 0x10)
		{
			tmpFileObject->SetFileSize(0);
			tmpFileObject->SetFileType(FILE_OBJECT_TYPE_DIRECTORY);
		}
		else
		{
			tmpFileObject->SetFileSize(tmpDirEntry->size);
			tmpFileObject->SetFileType(FILE_OBJECT_TYPE_FILE);
		}
		UINT32		tmpClusterNum = this->ParseStartCluster(tmpDirEntry);
		UINT32		tmpSectors = m_fatSector.BPB_FATSz32*m_fatSector.BPB_NumFATs;
		tmpSectors += m_fatSector.BPB_ResvdSecCnt;
		tmpSectors += (tmpClusterNum-m_fatSector.BPB_RootClus)*m_fatSector.BPB_SecPerClus;
		tmpFileObject->SetFileStartSector(tmpSectors);
		tmpFileObject->SetFileName(tmpStrArray[tmpStrArray.size()-1]);
		::PathRemoveFileSpec(tmpFullFileName);
		tmpFileObject->SetPath(tmpFullFileName);
		free(tmpBuf);
		return tmpFileObject;
	}
	free(tmpBuf);
	return 0;
}

vector<CBaseFileObject*> *CFat32FileSystem::GetChildren(CBaseFileObject *prmParentDirectory)
{
	if(prmParentDirectory->GetFileType()==FILE_OBJECT_TYPE_FILE)
	{
		return NULL;
	}
	vector<CBaseFileObject*> *tmpArray = new vector<CBaseFileObject*>;
	DIR_ENTRY_s		*tmpDirEntry = { 0 };
	//int		tmpIndex = prmParentDirectory->GetFileType()==FILE_OBJECT_TYPE_ROOT?32:64;
	UINT32	tmpClusterNum = (UINT32)prmParentDirectory->GetFileStartSector();
	//fat32�ļ�ϵͳ�����ļ��������ż����ļ��Ĵغ�
	tmpClusterNum -= m_fatSector.BPB_FATSz32*m_fatSector.BPB_NumFATs;
	tmpClusterNum -= m_fatSector.BPB_ResvdSecCnt;
	tmpClusterNum = tmpClusterNum/m_fatSector.BPB_SecPerClus+2;
	//ÿ�ض����ֽ�
	UINT32		tmpClusterSize = m_fatSector.BPB_SecPerClus*m_fatSector.BPB_BytsPerSec;
	//���ÿ�ص�����
	char		*tmpClusterBuf = (char*)malloc(tmpClusterSize+32);
	if(tmpClusterBuf==NULL)
	{
		return tmpArray;
	}
	memset(tmpClusterBuf,0,tmpClusterSize+32);
	CStringUtil		tmpPath = prmParentDirectory->GetPath();
	while(tmpClusterNum)
	{
		UINT32	tmpOffset = m_fatSector.BPB_FATSz32 * m_fatSector.BPB_NumFATs;
		tmpOffset += m_fatSector.BPB_ResvdSecCnt;
		tmpOffset += (tmpClusterNum-m_fatSector.BPB_RootClus)*m_fatSector.BPB_SecPerClus;
		//��ȡһ�ص��ڴ��У�Ȼ��������е��ļ���Ϣ
		this->ReadBuf((UCHAR*)tmpClusterBuf,tmpOffset,tmpClusterSize);
		for(UINT32 i=0;i<tmpClusterSize;i+=32)
		{
			tmpDirEntry = (DIR_ENTRY_s *)(tmpClusterBuf+i);
			if(tmpDirEntry->name[0]==0)
			{
				break;
			}
			//�����ǰ�Ǿ��Ŀ¼�򶼵�ǰĿ¼(.)��Ŀ¼(..)������������������
			if( (tmpDirEntry->attr&0x80) || tmpDirEntry->name[0]=='.' )
			{
				continue;
			}
			DIR_ENTRY_s	*tmpFirstEntry = tmpDirEntry;
			//�ҵ���һ���ǳ��ļ�����ʶ��Dir_Entry��
			while(tmpDirEntry->attr==0xF)
			{
				i+=32;
				tmpDirEntry = (DIR_ENTRY_s *)(tmpClusterBuf+i);
			}
			if(tmpDirEntry->name[0]==0xE5)
			{
				//ɾ���ļ���ʶ����Ҫɾ�����ļ�
				continue;
			}
			CBaseFileObject	*tmpFileObject = this->ParseFileObject(tmpFirstEntry,tmpDirEntry);
			
			if(prmParentDirectory->GetFileType()!=FILE_OBJECT_TYPE_ROOT)
			{
				tmpFileObject->SetPath(tmpPath+"\\"+prmParentDirectory->GetFileName());
			}
			tmpArray->push_back(tmpFileObject);
		}
		if(tmpDirEntry->name[0]==0)
		{
			break;
		}
		tmpClusterNum = this->GetNextCluster(tmpClusterNum);
	}
	free(tmpClusterBuf);
	return tmpArray;
}

UINT32	CFat32FileSystem::GetNextCluster(UINT32 prmCurCluster)
{
	if (prmCurCluster >= m_fatNum)
	{
		return 0;
	}
	if (m_fatTable[prmCurCluster] == EOC || m_fatTable[prmCurCluster] == 0x0FFFFFF8)
	{
		return 0;
	}
	return m_fatTable[prmCurCluster];
}

void CFat32FileSystem::ParseShortFileName(DIR_ENTRY_s *prmDirEntry, TCHAR prmFileName[], size_t prmFileNameLength)
{
	char	tmpFileName[20] = {0};
	memset(prmFileName, 0, prmFileNameLength);
	int i = 0;
	for (i = 0; i < 8; i++)
	{
		if (prmDirEntry->name[i] == ' ')
		{
			break;
		}
		tmpFileName[i] = prmDirEntry->name[i];
	}
	if (prmDirEntry->name[8] != ' ')
	{
		tmpFileName[i++] = '.';
	}
	for (int j = 8; j < 11; j++)
	{
		if (prmDirEntry->name[j] == ' ')
		{
			break;
		}
		tmpFileName[i++] = prmDirEntry->name[j];
	}
#ifdef _UNICODE
	::MultiByteToWideChar(CP_THREAD_ACP,0,tmpFileName,-1,prmFileName,prmFileNameLength);
#else
	memcpy(prmFileName,tmpFileName,prmFileNameLength);
#endif
}

void CFat32FileSystem::ParseLongFileName(TCHAR prmLongFileName[], size_t prmFileNameLength, DIR_ENTRY_s *prmFirstEntry, DIR_ENTRY_s *prmLastEntry)
{
	TCHAR	tmpFileName[1024] = { 0 };
	memset(prmLongFileName, 0, prmFileNameLength);
	UINT32	index = 0;
	//���ļ����Ļ�ȡ��ʽΪ���������ҵ�˳������
	DIR_LONG_ENTRY_s *startEntry = (DIR_LONG_ENTRY_s*)(prmFirstEntry);
	DIR_LONG_ENTRY_s *dirLongEntry = (DIR_LONG_ENTRY_s*)(prmLastEntry);
	dirLongEntry--;
	while (startEntry != dirLongEntry)
	{
		memcpy((char*)tmpFileName + index, dirLongEntry->name0_4, 10);
		index += 10;
		memcpy((char*)tmpFileName + index, dirLongEntry->name5_10, 12);
		index += 12;
		memcpy((char*)tmpFileName + index, dirLongEntry->name11_12, 4);
		index += 4;
		dirLongEntry--;
	}
	memcpy((char*)tmpFileName + index, startEntry->name0_4, 10);
	index += 10;
	memcpy((char*)tmpFileName + index, startEntry->name5_10, 12);
	index += 12;
	memcpy((char*)tmpFileName + index, startEntry->name11_12, 4);
	index += 4;
#ifndef _UNICODE
	DWORD tmpLength = ::WideCharToMultiByte(CP_THREAD_ACP, 0, (WCHAR*)tmpFileName, -1, NULL, 0, 0, 0);
	if (tmpLength > prmFileNameLength)
	{
		return;
	}
	::WideCharToMultiByte(CP_THREAD_ACP, 0, (WCHAR*)tmpFileName, -1, prmLongFileName, prmFileNameLength, 0, 0);
#else
	memcpy(prmLongFileName,tmpFileName,_tcslen(tmpFileName)*sizeof(TCHAR));
#endif
}

UINT32	CFat32FileSystem::ParseStartCluster(DIR_ENTRY_s *prmDirEntry)
{
	UINT32 result = (prmDirEntry->starthi << 16)&(0xFFFF0000);
	result = result | (prmDirEntry->start & 0xFFFF);
	if (result == EOC || result == 0x0FFFFFF8)
	{
		return 0;
	}
	return result;
}

CBaseFileObject *CFat32FileSystem::ParseFileObject(DIR_ENTRY_s *prmFirstEntry,DIR_ENTRY_s *prmLastEntry)
{
	TCHAR		tmpFileName[1024] = {0};
	CBaseFileObject		*tmpFileObject = new CBaseFileObject;
	UINT32		tmpSectors = m_fatSector.BPB_FATSz32*m_fatSector.BPB_NumFATs;
	tmpSectors += m_fatSector.BPB_ResvdSecCnt;
	if(tmpFileObject==NULL)
	{
		return tmpFileObject;
	}
	if(prmFirstEntry == prmLastEntry)
	{
		this->ParseShortFileName(prmFirstEntry,tmpFileName,sizeof(tmpFileName));
	}
	else
	{
		this->ParseLongFileName(tmpFileName,sizeof(tmpFileName),prmFirstEntry,prmLastEntry);
	}
	tmpFileObject->SetFileName(tmpFileName);
	//���������ļ���ʼ�Ĵغţ�ת��Ϊ����ڷ���ƫ�Ƶ����������б���
	UINT32		tmpClusterNum = this->ParseStartCluster(prmLastEntry);
	tmpSectors += (tmpClusterNum-m_fatSector.BPB_RootClus)*m_fatSector.BPB_SecPerClus;
	tmpFileObject->SetFileStartSector(tmpSectors);
	if(prmLastEntry->attr & 0x10)
	{
		//Ŀ¼����
		tmpFileObject->SetFileSize(0);
		tmpFileObject->SetFileType(FILE_OBJECT_TYPE_DIRECTORY);
	}
	else
	{
		tmpFileObject->SetFileSize(prmLastEntry->size);
		tmpFileObject->SetFileType(FILE_OBJECT_TYPE_FILE);
	}
	return tmpFileObject;
}