
// DemoDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "FileRecovery/FileSystemFactory.h"


// CDemoDlg �Ի���
class CDemoDlg : public CDialogEx
{
// ����
public:
	CDemoDlg(CWnd* pParent = NULL);	// ��׼���캯��
	~CDemoDlg();
// �Ի�������
	enum { IDD = IDD_DEMO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
private:
	CComboBox m_diskList;
	CListCtrl m_deletedFileList;
	CBaseFileSystem	*m_fileSystem;
	vector<CBaseFileObject*>	m_deleteFileArray;
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedOk();
private:
	
	/*************************************
	*
	*	��������	RestoreFile
	*	����˵����	���ļ��ָ���ָ��Ŀ¼
	*	����������	prmFileSystem[�������]ɾ���ļ������ļ�ϵͳ����
	*	����������	prmFileObject[�������]ɾ���ļ�����
	*	����������	restorFileName[�������]�ָ��ļ�����·��
	*	����ֵ��	void
	*
	**************************************/
	void		RestoreFile(CBaseFileSystem *prmFileSystem, CBaseFileObject *prmFileObject,
		CString &restorFileName);
};
