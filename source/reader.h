/***************************************************
*
*	@Version:	1.0
*	@Author:	Mengxl
*	@Date:		2018-5-20 22:00
*	@File:		reader.h
*	���ڶ�����������ȡ���ݵ���
*
****************************************************/

#ifndef READER_INCLUDE_H
#define READER_INCLUDE_H
#include <windows.h>

class IBaseReader
{
public:
	virtual	~IBaseReader(){}
	
	/*************************************
	*
	*	��������	OpenDevice
	*	����˵����	��һ����������̣������ṩ��ȡ������Դ
	*	����������	@param prmDevice[�������]��ʾ�豸�ı�ʶ��
	*	����ֵ��	�򿪳ɹ�����true�����򷵻�false
	*
	**************************************/
	virtual	bool	OpenDevice(const TCHAR *prmDevice) = 0;

	/*************************************
	*
	*	��������	ReadSector
	*	����˵����	��ȡ��������е�����
	*	����������	@param prmStartSector[�������]��������еĴ���ȡ��ʼ������
	*	����������	@param prmBytesToRead[�������]����ȡ���ֽ�����
	*	����������	@param prmBuf[�������]���ɹ���ȡ������д�뵽�û�������
	*	����ֵ��	���سɹ�д�뵽prmBuf�е��ֽ���
	*
	**************************************/
	virtual	UINT64	ReadSector(UINT64 prmStartSector, UINT64 prmBytesToRead, UCHAR *prmBuf) = 0;
};

//����ΪIBaseReader��һ�����࣬������windows�¶Է�����ť�������ж�ȡ
class CSectorReader:public IBaseReader
{
public:
	CSectorReader();

	~CSectorReader();
	/*************************************
	*
	*	��������	OpenDevice
	*	����˵����	��һ����������̣������ṩ��ȡ������Դ
	*	����������	@param prmDevice[�������]��ʾ�豸�ı�ʶ��
	*	����ֵ��	�򿪳ɹ�����true�����򷵻�false
	*
	**************************************/
	virtual	bool OpenDevice(const TCHAR *prmDevice);

	/*************************************
	*
	*	��������	ReadSector
	*	����˵����	��ȡ��������е�����
	*	����������	@param prmStartSector[�������]��������еĴ���ȡ��ʼ������
	*	����������	@param prmBytesToRead[�������]����ȡ���ֽ�����
	*	����������	@param prmBuf[�������]���ɹ���ȡ������д�뵽�û�������
	*	����ֵ��	���سɹ�д�뵽prmBuf�е��ֽ���
	*
	**************************************/
	virtual	UINT64	ReadSector(UINT64 prmStartSector, UINT64 prmBytesToRead, UCHAR *prmBuf);
private:
	HANDLE	m_diskHandle;//�򿪷����ľ��
};

#endif