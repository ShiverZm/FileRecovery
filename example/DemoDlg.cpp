
// DemoDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Demo.h"
#include "DemoDlg.h"
#include "afxdialogex.h"
#include "FileRecovery/FileSystemFactory.h"
#include <ShlObj.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CDemoDlg �Ի���



CDemoDlg::CDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDemoDlg::IDD, pParent)
{
	m_fileSystem = NULL;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CDemoDlg::~CDemoDlg()
{
	for (size_t i = 0; i < m_deleteFileArray.size(); i++)
	{
		m_deleteFileArray[i]->Destroy();
	}
	delete m_fileSystem;
}

void CDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_diskList);
	DDX_Control(pDX, IDC_LIST1, m_deletedFileList);
}

BEGIN_MESSAGE_MAP(CDemoDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CDemoDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDOK, &CDemoDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CDemoDlg ��Ϣ�������

BOOL CDemoDlg::OnInitDialog()
{
	TCHAR		szBuf[MAX_PATH + 2] = { 0 };
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������
	::GetLogicalDriveStrings(MAX_PATH, szBuf);
	int i = 0;
	while (i < MAX_PATH)
	{
		if (szBuf[i] == 0)
		{
			break;
		}
		m_diskList.AddString(szBuf + i);
		i += 4;
	}
	m_diskList.SetCurSel(0);
	DWORD	dwExtendStyle = m_deletedFileList.GetExtendedStyle();
	dwExtendStyle |= LVS_EX_CHECKBOXES|LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES;
	m_deletedFileList.SetExtendedStyle(dwExtendStyle);
	m_deletedFileList.InsertColumn(0, TEXT(""), LVCFMT_LEFT, 50);
	m_deletedFileList.InsertColumn(1, TEXT("�ļ���"), LVCFMT_LEFT, 120);
	m_deletedFileList.InsertColumn(2, TEXT("�ļ���С"), LVCFMT_LEFT, 120);
	m_deletedFileList.InsertColumn(3, TEXT("�޸�ʱ��"), LVCFMT_LEFT, 120);
	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CDemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CDemoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CDemoDlg::OnBnClickedButton1()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CString	tmpStr;
	m_diskList.GetLBText(m_diskList.GetCurSel(), tmpStr);
	m_fileSystem = CFileSystemFactory::GetFileSystem(tmpStr.GetString());
	if (m_fileSystem == NULL)
	{
		return;
	}
	m_fileSystem->GetDeletedFiles(m_deleteFileArray);
	for (size_t i = 0; i < m_deleteFileArray.size(); i++)
	{
		int	tmpRow = m_deletedFileList.InsertItem(0,TEXT(""));
		m_deletedFileList.SetItemText(tmpRow, 1, m_deleteFileArray[i]->GetFileName().GetString());
		CString	tmpFileSize;
		tmpFileSize.Format(TEXT("%I64d"), m_deleteFileArray[i]->GetFileSize());
		m_deletedFileList.SetItemText(tmpRow, 2, tmpFileSize);
		m_deletedFileList.SetItemText(tmpRow, 3, m_deleteFileArray[i]->GetModifyTime().GetString());
	}
}


void CDemoDlg::OnBnClickedOk()
{
	TCHAR		tmpDir[MAX_PATH + 2] = { 0 };
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	BROWSEINFO		bInfo = { 0 };
	bInfo.hwndOwner = this->m_hWnd;
	bInfo.lpszTitle = TEXT("ѡ��ָ��ļ�����·��");
	bInfo.ulFlags = BIF_BROWSEFORCOMPUTER | BIF_DONTGOBELOWDOMAIN|BIF_RETURNONLYFSDIRS;
	LPITEMIDLIST	pItemList = ::SHBrowseForFolder(&bInfo);
	CString	tmpFileName;
	if (pItemList == NULL)
	{
		return;
	}
	::SHGetPathFromIDList(pItemList, tmpDir);
	for (int i = 0; i < m_deletedFileList.GetItemCount(); i++)
	{
		BOOL tmpSelected = m_deletedFileList.GetCheck(i);
		if (tmpSelected)
		{
			tmpFileName = tmpDir;
			tmpFileName = tmpFileName + "\\" + m_deletedFileList.GetItemText(i, 1);
			this->RestoreFile(m_fileSystem, m_deleteFileArray[i], tmpFileName);
		}
		//if (m_deletedFileList.GetItem())
	}
	//CDialogEx::OnOK();
}

void CDemoDlg::RestoreFile(CBaseFileSystem *prmFileSystem, CBaseFileObject *prmFileObject, CString &restorFileName)
{
	char	*tmpBuf = (char*)malloc(1024 * 1024);
	memset(tmpBuf, 0, 1024 * 1024);
	UINT64	tmpFileSize = prmFileObject->GetFileSize();
	UINT64	tmpBytesRead = 0;
	HANDLE	tmpFileHandle = ::CreateFile(restorFileName.GetBuffer(), GENERIC_WRITE,
		FILE_SHARE_READ, NULL, CREATE_NEW , FILE_ATTRIBUTE_NORMAL, NULL);
	if (tmpFileHandle == INVALID_HANDLE_VALUE)
	{
		free(tmpBuf);
		return;
	}
	while (tmpBytesRead < tmpFileSize)
	{
		UINT64  tmpVal = prmFileSystem->ReadFileContent(prmFileObject, (UCHAR*)tmpBuf, tmpBytesRead, 1024 * 1024);
		if (tmpVal == 0)
		{
			break;
		}
		tmpBytesRead += tmpVal;
		DWORD	tmpBytesWrite = 0;
		::WriteFile(tmpFileHandle, tmpBuf, (DWORD)tmpVal, &tmpBytesWrite, NULL);
	}
	::CloseHandle(tmpFileHandle);
	free(tmpBuf);
}