/***************************************************
*
*	@Version:	1.0
*	@Author:	Mengxl
*	@Date:		2017-10-28 22:17
*	@File:		FileSystem.h
*
****************************************************/


#ifndef FILE_SYSTEM_INCLUDE_H
#define FILE_SYSTEM_INCLUDE_H

#include <windows.h>
#include <vector>
#include "File.h"
#include "reader.h"

using namespace std;

class CBaseFileSystem
{
public:
	/*************************************
	*
	*	��������	CBaseFileSystem
	*	����˵����	���캯��
	*	����������	prmReader[�������]���ڱ�ʾ�ļ�ϵͳ��ȡ���ݵ���Դ
	*	����ֵ��	
	*
	**************************************/
	CBaseFileSystem(IBaseReader	*prmReader=NULL);

	/*************************************
	*
	*	��������	~CBaseFileSystem
	*	����˵����	��������
	*	����ֵ��	
	*
	**************************************/
	virtual ~CBaseFileSystem();
	
	/*************************************
	*
	*	��������	ReadBuf
	*	����˵����	��ȡ��ƫ��startSector������ʼ��ȡbyteToRead�ֽ�����
	*	����������	@param prmBuf[in/out]����ȡ��byteToRead�ֽ����ݴ�ŵ�szBuf��
	*	����������	@param prmStartSector[in]��ȡ���������ļ��е�ƫ��ֵ
	*	����������	@param prmByteToRead[in]���ļ��ж�ȡbyteToRead�ֽ�
	*	����ֵ��	���سɹ�д�뵽prmBuf�������е��ֽ���
	*
	**************************************/
	virtual		UINT64	ReadBuf(UCHAR prmBuf[],UINT64 prmStartSector,UINT64 prmByteToRead);

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
	virtual		void	Init() = 0;

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
		UCHAR prmDstBuf[], UINT64 prmByteOff, UINT64 prmByteToRead) = 0;

	/*************************************
	*
	*	��������	GetFileObjectByPath
	*	����˵����	�����ļ���·����ȡ�ļ�����
	*	����������	@param prmFileName[�������]�ļ����ַ���
	*	����ֵ��	������ļ�ϵͳ�гɹ��ҵ�prmFileName�ļ�����CBaseFileObject����
	*				���򷵻�NULL
	*
	**************************************/
	virtual	CBaseFileObject	*GetFileObjectByPath(LPCTSTR prmFileName) = 0;

	/*************************************
	*
	*	��������	GetChildren
	*	����˵����	��ȡĿ¼�е��������ļ�/�ļ��ж���
	*	����������	@param prmParentDirectory[�������]��Ŀ¼�Ķ���
	*	����ֵ��	vector<CBaseFileObject*> *
	*
	**************************************/
	virtual	vector<CBaseFileObject*> *GetChildren(CBaseFileObject *prmParentDirectory) = 0;

	/*************************************
	*
	*	��������	SetBytesPerSector
	*	����˵����	����ÿ�����ֽ���
	*	����������	UINT16 prmBytesPerSector
	*	����ֵ��	void
	*
	**************************************/
	void		SetBytesPerSector(UINT16 prmBytesPerSector){m_bytesPerSector = prmBytesPerSector;}

	/*************************************
	*
	*	��������	SetSectorsPerCluster
	*	����˵����	����ÿ��������
	*	����������	UINT8 prmSectorsPerCluster
	*	����ֵ��	void
	*
	**************************************/
	void		SetSectorsPerCluster(UINT8 prmSectorsPerCluster){m_sectorsPerCluster = prmSectorsPerCluster;}

	/*************************************
	*
	*	��������	SetStartSector
	*	����˵����	���õ�ǰ�ļ����������������е���ʼ����
	*	����������	UINT64 prmStartSector
	*	����ֵ��	void
	*
	**************************************/
	void		SetStartSector(UINT64 prmStartSector){m_startSector = prmStartSector;}

	/*************************************
	*
	*	��������	SetTotalSector
	*	����˵����	��������������������
	*	����������	UINT64 prmTotalSector
	*	����ֵ��	void
	*
	**************************************/
	void		SetTotalSector(UINT64 prmTotalSector){m_totalSector = prmTotalSector;}

	/*************************************
	*
	*	��������	GetRootDirectory
	*	����˵����	��ȡ�����ĸ�Ŀ¼
	*	����ֵ��	CBaseFileObject	*
	*
	**************************************/
	CBaseFileObject	*GetRootDirectory(){return m_rootDirectory;}

protected:
	UINT16		m_bytesPerSector;//ÿ�����ֽ���
	UINT8		m_sectorsPerCluster;//ÿ��������
	UINT64		m_startSector;//�÷����ڴ����е���ʼ����
	UINT64		m_totalSector;//�÷����ܹ�ռ�õ�������
	CBaseFileObject	*m_rootDirectory;
	IBaseReader	*m_reader;//��ȡ�������ݵĶ���
};

#endif