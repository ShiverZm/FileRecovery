#ifndef FAT32_INCLUDE_H
#define FAT32_INCLUDE_H

#include "FileSystem.h"
#include "commutil.h"

class CFat32FileSystem:public CBaseFileSystem
{
public:
	CFat32FileSystem(IBaseReader  *prmReader=NULL);
	virtual ~CFat32FileSystem();
	/*************************************
	*
	*	��������	Init
	*	����˵����	fat32������ʽ��ʼ��
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

	/***
	*
	*��ȡ�ļ�����һ����
	*
	***/
	UINT32    GetNextCluster(UINT32	prmCurCluster);

	//�������ļ���
	void	  ParseShortFileName(DIR_ENTRY_s *prmDirEntry, TCHAR	prmFileName[], size_t prmFileNameLength);

	//�������ļ���
	void	  ParseLongFileName(TCHAR	prmLongFileName[], size_t prmFileNameLength, DIR_ENTRY_s *prmFirstEntry, DIR_ENTRY_s *prmLastEntry);
	
	//��DIR_ENTRY_sĿ¼�ṹ�н������ļ����ļ��еĿ�ʼ�غ�
	UINT32	 ParseStartCluster(DIR_ENTRY_s *prmDirEntry);

	/*************************************
	*
	*	��������	ParseFileObject
	*	����˵����	���ݻ�ȡ��fat32Ŀ¼��������ļ���Ϣ
	*	����������	DIR_ENTRY_s * prmFirstEntry
	*	����������	DIR_ENTRY_s * prmLastEntry
	*	����ֵ��	CBaseFileObject	*
	*
	**************************************/
	CBaseFileObject	*ParseFileObject(DIR_ENTRY_s *prmFirstEntry,DIR_ENTRY_s *prmLastEntry);
private:
	FAT32_s m_fatSector;
	UINT32	*m_fatTable;
	UINT32	m_fatNum;
};

#endif