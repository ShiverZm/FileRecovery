/***************************************************
*
*	@Version:	1.0
*	@Author:	Mengxl
*	@Date:		2017-10-28 22:29
*	@File:		File.h
*
****************************************************/
#ifndef FILE_MXL_INCLUDE_H
#define FILE_MXL_INCLUDE_H

#include "StringUtil.h"
#include "commutil.h"

using namespace commutil;

enum	FILE_OBJECT_TYPE
{
	FILE_OBJECT_TYPE_FILE,
	FILE_OBJECT_TYPE_DIRECTORY,
	FILE_OBJECT_TYPE_ROOT,
	FILE_OBJECT_TYPE_UNKNOWN
};

class CBaseFileObject
{
public:
	/*************************************
	*
	*	��������	CBaseFileObject
	*	����˵����	
	*	����ֵ��	
	*
	**************************************/
	CBaseFileObject();



	/*************************************
	*
	*	��������	SetFileStartSector
	*	����˵����	�����ļ�����ʼ����
	*	����������	
	*	����ֵ��	void
	*
	**************************************/
	void		SetFileStartSector(UINT64  prmSector);

	/*************************************
	*
	*	��������	GetFileStartSector
	*	����˵����	��ȡ�ļ�����ʼ����
	*	����ֵ��	UINT64
	*
	**************************************/
	UINT64		GetFileStartSector();

	/*************************************
	*
	*	��������	SetFileName
	*	����˵����	�����ļ�����
	*	����������	const CStringUtil & prmFileName
	*	����ֵ��	void
	*
	**************************************/
	void		SetFileName(const CStringUtil &prmFileName);

	/*************************************
	*
	*	��������	GetFileName
	*	����˵����	��ȡ�ļ�����
	*	����ֵ��	CStringUtil	&
	*
	**************************************/
	const CStringUtil	&GetFileName();

	/*************************************
	*
	*	��������	SetFileType
	*	����˵����	�����ļ�����
	*	����������	FILE_OBJECT_TYPE prmType
	*	����ֵ��	void
	*
	**************************************/
	void		SetFileType(FILE_OBJECT_TYPE prmType);

	/*************************************
	*
	*	��������	GetFileType
	*	����˵����	��ȡ�ļ�����
	*	����ֵ��	����ö������ļ�����FILE_OBJECT_TYPE_FILE��
					������ļ��з���FILE_OBJECT_TYPE_DIRECTORY
	*
	**************************************/
	FILE_OBJECT_TYPE  GetFileType();

	/*************************************
	*
	*	��������	SetFileSize
	*	����˵����	�����ļ��Ĵ�С�������Ŀ¼���ļ���СΪ0
	*	����������	UINT64 prmFileSize
	*	����ֵ��	void
	*
	**************************************/
	void		SetFileSize(UINT64 prmFileSize);

	/*************************************
	*
	*	��������	GetFileSize
	*	����˵����	��ȡ�ļ��Ĵ�С
	*	����ֵ��	UINT64
	*
	**************************************/
	UINT64		GetFileSize();

	void		SetAccessTime(const CStringUtil &accessTime){ m_accessTime = accessTime; }

	CStringUtil	&GetAccessTime(){ return m_accessTime; }

	void		SetModifyTime(const CStringUtil &modifyTime){ m_modifyTime = modifyTime; }

	CStringUtil	&GetModifyTime(){ return m_modifyTime; }

	void		SetCreateTime(const CStringUtil &createTime){ m_createTime = createTime; }

	CStringUtil &GetCreateTime(){ return m_createTime; }

	void		SetFileExtent(File_Content_Extent_s *fileExtent){ m_fileExtent = fileExtent; }

	File_Content_Extent_s *GetFileExtent(){ return m_fileExtent; }

	void		Destroy();
private:
	CStringUtil		m_fileName;//�ļ����ƣ��ļ�����·�� = m_path+m_fileName
	CStringUtil		m_accessTime;//������ʱ��
	CStringUtil		m_modifyTime;//�޸�ʱ��
	CStringUtil		m_createTime;//����ʱ��
	UINT64			m_startSector;//�ļ��ڷ����е���ʼ����
	UINT64			m_fileSize;//�ļ���С
	FILE_OBJECT_TYPE	m_objectType;//�ļ����ͣ��ļ���Ŀ¼����Ŀ¼
	File_Content_Extent_s	*m_fileExtent;//�ļ�����ռ������Щ������Ϣ
private:

	/*************************************
	*
	*	��������	~CBaseFileObject
	*	����˵����	˽��������������������ջ�Ϸ������
					������ʹ��delete ɾ���ļ�����
	*	����ֵ��	
	*
	**************************************/
	virtual ~CBaseFileObject();
};

#endif