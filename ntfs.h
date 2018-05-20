/***************************************************
*
*	@Version:	1.0
*	@Author:	Mengxl
*	@Date:		2017-11-1 22:30
*	@File:		ntfs.h
*
****************************************************/
#ifndef NTFS_INCLUDE_H
#define NTFS_INCLUDE_H
#include "FileSystem.h"
#include "commutil.h"



class CNtfsFileSystem:public CBaseFileSystem
{
public:
	CNtfsFileSystem(IBaseReader *prmReader = NULL);

	~CNtfsFileSystem();
	/*************************************
	*
	*	��������	Init
	*	����˵����	�ļ������ĳ�ʼ������fat32ϵͳ����
	*				����������¼��Ϣ�������Ӳ�ͬ���ļ�ϵͳ
	*				ʵ�ֲ�ͬ������ļ�ϵͳ����Ҫ���г�ʼ����
	*				�ú�����ʵ�������Ϊ�ա�
	*	����ֵ��	void
	*
	**************************************/
	virtual		void	Init();

	/*************************************
	*
	*	��������	ReadFileContent
	*	����˵����	��ȡ�ļ�����
	*	����������	@param prmFileObject[�������]����ȡ�ļ��Ķ���
	*	����������	@param prmDstBuf[�������]��ȡ�����ݴ��뵽�û�������
	*	����������	@param prmByteOff[�������]��ʾ���ļ��Ķ��ٽ���ƫ�ƴ���ʼ��ȡ
	*	����������	@param prmByteToRead[�������]��ʾ��ȡ�����ֽ�
	*	����ֵ��	���سɹ�д�뵽prmDstBuf�е��ֽ���
	*
	**************************************/
	virtual UINT64	ReadFileContent(CBaseFileObject *prmFileObject,
		UCHAR prmDstBuf[], UINT64 prmByteOff, UINT64 prmByteToRead);

	/*************************************
	*
	*	��������	GetFileObjectByPath
	*	����˵����	�����ļ���·����ȡ�ļ�����
	*	����������	@param prmFileName[�������]�ļ����ַ���
	*	����ֵ��	������ļ�ϵͳ�гɹ��ҵ�prmFileName�ļ�����CBaseFileObject����
	*				���򷵻�NULL
	*
	**************************************/
	virtual	CBaseFileObject	*GetFileObjectByPath(LPCTSTR prmFileName);

	/*************************************
	*
	*	��������	GetChildren
	*	����˵����	��ȡĿ¼�е��������ļ�/�ļ��ж���
	*	����������	@param prmParentDirectory[�������]��Ŀ¼�Ķ���
	*	����ֵ��	vector<CBaseFileObject*> *
	*
	**************************************/
	virtual	vector<CBaseFileObject*> *GetChildren(CBaseFileObject *prmParentDirectory);

private:

	/*************************************
	*
	*	��������	GetMFTRunList
	*	����˵����	��ȡ$MFT�ļ���0x80����ֵ�����ݸ���0x80����������������
	*				����ʵ�ָ��ݲο��Ų����ļ��غ�
	*	����ֵ��	void
	*
	**************************************/
	void	GetMFTRunList();

	/*************************************
	*
	*	��������	GetAttrValue
	*	����˵����	��ȡ�ļ�MFTͷ��ָ�����Ե�ֵ
	*	����������	@param prmAttrTitle[�������]������ȡ������ֵ��0x30,0x80��
	*	����������	@param prmBuf[�������]�ļ���MFT��ʶֵ
	*	����������	@ prmAttrValue[�������]��ȡ������Ӧ���Զ�Ӧ�����ݷ���û�����
	*	����ֵ��	�ɹ��ҵ���Ӧ���Է���1�����򷵻�0
	*
	**************************************/
	UINT32	GetAttrValue(NTFS_ATTRDEF prmAttrTitle,UCHAR prmBuf[],UCHAR *prmAttrValue);

	/*************************************
	*
	*	��������	GetAttrFromAttributeList
	*	����˵����	��$0x20���Ա��ж�ȡָ���������͵�ֵ
	*	����������	@param prmAttrType[�������]��������������
	*	����������	@param prmOffset[�������]��prmAttrList��prmOffset����ʼ����
	*					��Ϊһ�����Ա��п��ܴ��ڶ����ͬ���͵�����
	*	����������	@param prmAttrList[�������]���Ա����ݻ�����
	*	����������	@param prmAttrValue[�������]���ҵ����������ݷ���û�����
	*	����ֵ��	�ҵ�ָ���������Է��ظ�����������prmAttrList�е�ƫ��ֵ�����򷵻�0
	*
	**************************************/
	UINT32	GetAttrFromAttributeList(NTFS_ATTRDEF prmAttrType,UINT32 prmOffset,UCHAR *prmAttrList,UCHAR *prmAttrValue);

	/*************************************
	*
	*	��������	GetDataRunList
	*	����˵����	��ȡ����������
	*	����������	@param prmBuf[�������]���mft��¼ͷ��1024�ֽ�
	*	����������	@param prmRunListOffset[�������]������������prmBuf�е�ƫ��ֵ
	*	����������	@param prmList[�������]��ȡ�õ����������뵽��������
	*	����ֵ��	void
	*
	**************************************/
	void	GetDataRunList(UCHAR *prmBuf,UINT16 prmRunListOffset,Ntfs_Data_Run **prmList);

	/*************************************
	*
	*	��������	FreeRunList
	*	����˵����	�ͷ�����������ռ�õ��ڴ�ռ�
	*	����������	@param prmList[�������]����������
	*	����ֵ��	void
	*
	**************************************/
	void	FreeRunList(Ntfs_Data_Run	*prmList);

	/*************************************
	*
	*	��������	GetExtendMFTAttrValue
	*	����˵����	����չmft��¼ͷ�л�ȡָ������
	*	����������	@param prmSeqNum[�������]��չmft�ο���
	*	����������	@param prmAttrType[�������]����ȡ��������
	*	����������	@param prmAttrValue[�������]��ȡ�������Է���û�������
	*	����ֵ��	����ҵ�prmAttrType��mft��¼ͷ��ֵ����1�����򷵻�0
	*
	**************************************/
	UINT32	GetExtendMFTAttrValue(UINT64 prmSeqNum,NTFS_ATTRDEF prmAttrType,UCHAR *prmAttrValue);

	/*************************************
	*
	*	��������	GetOffsetByMFTRef
	*	����˵����	����mft�ο��ż������ļ������е�ƫ��ֵ
	*	����������	@param prmSeqNo[�������]mft�ο���
	*	����ֵ��	����mft�ο��Ŷ�Ӧ��mft��¼ͷ�ڷ����е�ƫ���ֽ�
	*
	**************************************/
	UINT64	GetOffsetByMFTRef(UINT64 prmSeqNo);

	/*************************************
	*
	*	��������	GetFileFromIndexRoot
	*	����˵����	��MFT��¼ͷ�ڲ�����������н��������ļ�/�ļ�����
					��MFT�����л�ȡ��0x90����ֵ
	*	����������	@param prmAttrValue[�������]0x90�ڲ�������������ֵ
	*	����������	@param * prmFileArray[�������]�ӷ����н��������ļ����뵽�ñ�����
	*	����ֵ��	void
	*
	**************************************/
	void	GetFileFromIndexRoot(UCHAR *prmAttrValue,vector<CBaseFileObject*> *prmFileArray);

	/*************************************
	*
	*	��������	GetFileFromAllocIndex
	*	����˵����	���ⲿ������������������ļ�/�ļ���
	*	����������	@param prmAttrValue[in]���������0xA0����ֵ
	*	����������	@param prmFileArray[�������]��ȡ���ļ����뵽�ñ�����
	*	����ֵ��	void
	*
	**************************************/
	void	GetFileFromAllocIndex(UCHAR *prmAttrValue,vector<CBaseFileObject*> *prmFileArray);
	
	/*************************************
	*
	*	��������	ParseFileFromIndex
	*	����˵����	�����������н��������е��ļ���
	*	����������	@param prmBuf[�������]�ļ�����������ֵ
	*	����������	@param prmOffset[�������]��һ��������ƫ��ֵ
	*	����������	@param prmBufLen[�������]���һ����β��ƫ��ֵ
	*	����������	@param prmFileArray[�������]��ȡ���ļ�����ñ�����
	*	����ֵ��	void
	*
	**************************************/
	void	ParseFileFromIndex(UCHAR *prmBuf,UINT16 prmOffset,UINT32 prmBufLen,vector<CBaseFileObject*> *prmFileArray);

	/*************************************
	*
	*	��������	GetFileWin32Name
	*	����˵����	��ȡ�ļ���win32���ƣ���mft�н����������п�����dos�����������������Ҫ��
	*	����������	@param prmOffset[�������]mft�ڷ�����ƫ��ֵ
	*	����ֵ��	�����ļ���win32�����ռ�����
	*
	**************************************/
	CStringUtil	GetFileWin32Name(UINT64 prmOffset);

	/*************************************
	*
	*	��������	IsFileExists
	*	����˵����	�ļ��Ƿ��Ѿ�����
	*	����������	@prmStartSector[�������] �ļ��Ƿ��Ѿ�����
	*	����������	@param prmFileArray[�������]�ѽ������ļ�����
	*	����ֵ��	����ļ��Ѿ�����prmFileArray�����з���TRUE,���򷵻�FALSE
	*
	**************************************/
	BOOL	IsFileExists(UINT64 prmStartSector,vector<CBaseFileObject*> *prmFileArray);
	
	/*************************************
	*
	*	��������	GetFileExtent
	*	����˵����	��ȡ�ļ�ռ�õĴ���Ϣ
	*	����������	@param prmBuf[�������]�ļ���mft��¼ͷ��Ϣ
	*	����������	@param prmMftSector[�������]�ļ���mft��ʼ����
	*	����������	@param prmFileExtent[�������]��ȡ���ļ����ݴ���Ϣ��������ʽ����ñ���
	*	����ֵ��	void
	*
	**************************************/
	void	GetFileExtent(UCHAR *prmBuf,UINT64 prmMftSector,File_Content_Extent_s **prmFileExtent);

	/*************************************
	*
	*	��������	ReadFileContent
	*	����˵����	��ȡntfs�������ļ����ݵ�˽�к���
	*	����������	@param prmDstBuf[�������]����ȡ���ļ����ݷ���û�������
	*	����������	@param prmByteOff[�������]���ļ����ݵ�prmByteOffƫ�ƴ���ʼ��ȡ����
	*	����������	@param prmByteToRead[�������]���ļ��ж�ȡ�����ֽڵ�����
	*	����������	@param prmFileSize[�������]�ļ���С
	*	����������	@param prmFilExtent[�������]�ļ�������ռ������
	*	����ֵ��	���سɹ�д�뵽prmDstBuf���������ֽ���
	*
	**************************************/
	UINT64	ReadFileContent(UCHAR prmDstBuf[],UINT64 prmByteOff,UINT64 prmByteToRead,
		UINT64 prmFileSize,File_Content_Extent_s *prmFileExtent);

	/*************************************
	*
	*	��������	FreeFileExtent
	*	����˵����	�ͷ��ļ���ռ�õĴ�����
	*	����������	@param prmFileExtent[�������]�ļ�������ռ�õĴ�����
	*	����ֵ��	void
	*
	**************************************/
	void  FreeFileExtent(File_Content_Extent_s *prmFileExtent);

	/*************************************
	*
	*	��������	GetOffsetByFileName
	*	����˵����	�����ļ����ҵ�mft��¼ͷ��ƫ��ֵ
	*	����������	@param prmParentFileOffset[�������]���ļ���mft��¼ͷ
					��ƫ��ֵ
	*	����������	@param prmFileName[�������]�ļ���
	*	����ֵ��	�����ļ��ڷ�����ƫ��ֵ�����û���ҵ�����0
	*
	**************************************/
	UINT64	GetOffsetByFileName(UINT64 prmParentFileOffset,const CStringUtil	&prmFileName);

	/*************************************
	*
	*	��������	GetOffsetFromRootByFileName
	*	����˵����	��0x90�������и����ļ��������ļ�mftƫ��ֵ
	*	����������	@param prmAttrValue[�������]��Ŀ¼mft��¼ͷ�е�0x90����ֵ
	*	����������	@param prmFileName[�������]�������ļ���
	*	����ֵ��	�����ļ���mft��¼ͷƫ��ֵ�����û���ҵ��ļ�����0
	*
	**************************************/
	UINT64	GetOffsetFromRootByFileName(UCHAR *prmAttrValue,const CStringUtil &prmFileName);

	/*************************************
	*
	*	��������	GetOffsetFromAllocByFileName
	*	����˵����	���ⲿ���������ļ�������mftƫ��ֵ
	*	����������	@param prmAttrValue[�������]���ļ���0xA0����ֵ
	*	����������	@param prmFileName[�������]�������ļ���
	*	����ֵ��	�����ļ�mft��¼ͷƫ��ֵ�����û���ҵ��ļ�����0
	*
	**************************************/
	UINT64	GetOffsetFromAllocByFileName(UCHAR *prmAttrValue,const CStringUtil &prmFileName);

	/*************************************
	*
	*	��������	GetOffsetByFileNameInIndex
	*	����˵����	���������и����ļ�����ȡ�ļ���mftƫ��ֵ
	*	����������	@param prmBuf[�������]������������
	*	����������	@param prmOffset[�������]��һ���������ƫ��ֵ
	*	����������	@param prmBufLen[�������]���һ��������β��ƫ��ֵ
	*	����������	@param prmFileName[�������]�������ļ���
	*	����ֵ��	�����ļ���mft��¼ͷƫ��ֵ�����û���ҵ��ļ�����0
	*
	**************************************/
	UINT64	GetOffsetByFileNameInIndex(UCHAR *prmBuf,UINT16 prmOffset,UINT32 prmBufLen,const CStringUtil &prmFileName);

	/*************************************
	*
	*	��������	GetFileSize
	*	����˵����	��ȡ�ļ��Ĵ�С��
	*	����������	@param prmMFTRecord[�������]�ļ�mft��¼ͷ����
	*	����ֵ��	�����ļ��Ĵ�С
	*
	**************************************/
	UINT64	GetFileSize(UCHAR *prmMFTRecord);

	/*************************************
	*
	*	��������	GetFileType
	*	����˵����	��mft��¼ͷ���ҵ��ļ������ͣ��ļ�/�ļ��У�
	*	����������	@param prmMFTRecord[�������]�ļ���mft��¼ͷ��Ϣ
	*	����ֵ��	����FILE_OBJECT_TYPE_FILE(�ļ�)��FILE_OBJECT_TYPE_DIRECTORY(�ļ���)
	*
	**************************************/
	FILE_OBJECT_TYPE	GetFileType(UCHAR *prmMFTRecord);
private:
	UINT64		m_mftStartCluster;//$MFT��ʼ�غ�
	UINT64		m_clustersPerIndex;//ÿ������ռ�ô���
	Ntfs_Data_Run	*m_mftRunList;//MFT���������������ʵ�ָ��ݲο��Ž����ļ�mft����
};
#endif