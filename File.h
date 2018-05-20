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
	*	��������	SetPath
	*	����˵����	�����ļ�����·��
	*	����������	const CStringUtil & prmPath
	*	����ֵ��	void
	*
	**************************************/
	void		SetPath(const CStringUtil &prmPath);

	/*************************************
	*
	*	��������	GetPath
	*	����˵����	��ȡ�ļ�����·��
	*	����ֵ��	const CStringUtil
	*
	**************************************/
	const CStringUtil	&GetPath();

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

	void		Destroy();
private:
	CStringUtil		m_path;//�ļ�����·��
	CStringUtil		m_fileName;//�ļ����ƣ��ļ�����·�� = m_path+m_fileName
	UINT64			m_startSector;//�ļ��ڷ����е���ʼ����
	UINT64			m_fileSize;//�ļ���С
	FILE_OBJECT_TYPE	m_objectType;//�ļ����ͣ��ļ���Ŀ¼����Ŀ¼
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