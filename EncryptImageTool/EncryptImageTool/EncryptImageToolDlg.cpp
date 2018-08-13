
// EncryptImageToolDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "EncryptImageTool.h"
#include "EncryptImageToolDlg.h"
#include "afxdialogex.h"
#include "SelectFolderDlg.h"
#include <afxwin.h>
#include "Tools.h"
#include "EncryptImage.h"
#include "Decrypt.h"

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


// CEncryptImageToolDlg �Ի���



CEncryptImageToolDlg::CEncryptImageToolDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CEncryptImageToolDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CEncryptImageToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CEncryptImageToolDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTFILESEL, &CEncryptImageToolDlg::OnBnClickedButfilesel)
	ON_BN_CLICKED(IDC_BUTFILEOUT, &CEncryptImageToolDlg::OnBnClickedButfileout)
	ON_BN_CLICKED(IDC_BUTENSTART, &CEncryptImageToolDlg::OnBnClickedButenstart)
	ON_BN_CLICKED(IDC_BUTTONKEY, &CEncryptImageToolDlg::OnBnClickedButtonReadKey)
	ON_BN_CLICKED(IDC_BUTZIP, &CEncryptImageToolDlg::OnBnClickedButzip)
	ON_EN_KILLFOCUS(IDC_EDITMAXBOX, &CEncryptImageToolDlg::OnEnKillfocusEditmaxbox)
	ON_EN_KILLFOCUS(IDC_EDITMINBOX, &CEncryptImageToolDlg::OnEnKillfocusEditminbox)
	ON_EN_KILLFOCUS(IDC_EDIT, &CEncryptImageToolDlg::OnEnKillfocusEdit)
	ON_EN_KILLFOCUS(IDC_EDITOUT, &CEncryptImageToolDlg::OnEnKillfocusEditout)
	ON_BN_CLICKED(IDC_BUTCOPY, &CEncryptImageToolDlg::OnBnClickedButcopy)
	ON_BN_CLICKED(IDC_CHECKLOCK, &CEncryptImageToolDlg::OnBnClickedCheckLock)
	ON_BN_CLICKED(IDC_BUTDE, &CEncryptImageToolDlg::OnBnClickedButde)
	ON_BN_CLICKED(IDC_CHECKZIP, &CEncryptImageToolDlg::OnBnClickedCheckzip)
END_MESSAGE_MAP()


// CEncryptImageToolDlg ��Ϣ�������

BOOL CEncryptImageToolDlg::OnInitDialog()
{
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
	m_selFilePath = "";
	m_selFileOutPath = "";
	m_zipFilePath = "";
	m_enFileOutPath = "";
	m_exePath = Tool::curdir();

	// ����key
	m_key = Tool::SECRET_KEY;

	char c;
	int count = 0;
	std::string strKey ="��ԿΪ��";
	while (m_key[count])
	{
		//infile >> c;
		c= m_key[count];
		strKey += c;
		count++;
	}

	GetDlgItem(IDC_KEYTEXT)->SetWindowTextW(CString(strKey.c_str()));

	m_vecPngFiles.clear();
	m_vecZipPngFiles.clear();
	m_vecAllFiles.clear();

	m_pMinEdit = (CEdit*)GetDlgItem(IDC_EDITMINBOX);
	m_pMinEdit->SetWindowTextW(_T("65"));
	m_pMinEdit->SetSel(0, 2);

	m_pMaxEdit = (CEdit*)GetDlgItem(IDC_EDITMAXBOX);
	m_pMaxEdit->SetWindowTextW(_T("70"));
	m_pMaxEdit->SetSel(0, 2);


	m_pSelFileEdit = (CEdit*)GetDlgItem(IDC_EDIT);

	m_pOutFileEdit = (CEdit*)GetDlgItem(IDC_EDITOUT);

	std::string logPath = Tool::curdir() + "\\log.txt";
	std::fstream file(logPath, std::ios::out);		// ����ļ�

	GetDlgItem(IDC_BUTENSTART)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTZIP)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTCOPY)->EnableWindow(FALSE);


	// ����
#if ENDE
	GetDlgItem(IDC_BUTDE)->ShowWindow(TRUE);
#else
	GetDlgItem(IDC_BUTDE)->ShowWindow(FALSE);
#endif

	setLock(false);
	setZip(true);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CEncryptImageToolDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CEncryptImageToolDlg::OnPaint()
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
HCURSOR CEncryptImageToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CEncryptImageToolDlg::OnBnClickedButfilesel()
{
	// TODO:  ѡ������ļ�
	CString filePath = CSelectFolderDlg::Show();

	GetDlgItem(IDC_SEL_FILETXT)->SetWindowText(filePath);

	m_pSelFileEdit->SetWindowText(filePath);
	
	m_selFilePath = CT2CA(filePath.GetBuffer(0));

	if (getLock())		// �������ܲ���
	{
		return;
	}

	if (filePath.IsEmpty())
	{
		return;
	}

}


void CEncryptImageToolDlg::OnBnClickedButfileout()
{
	// TODO:  ѡ���������ļ�
	CString outFile = CSelectFolderDlg::Show();

	GetDlgItem(IDC_FILE_OUT)->SetWindowTextW(outFile);
	m_selFileOutPath = CT2CA(outFile.GetBuffer(0));

	m_pOutFileEdit->SetWindowText(outFile);

	if (getLock())		// �������ܲ���
	{
		return;
	}

	if (outFile.IsEmpty())
	{
		return;
	}

	UpdateOutFile();
}


void CEncryptImageToolDlg::OnBnClickedButenstart()
{
	if (!getLock())
	{
		return;
	}

	// ��ȡ�������ļ�
	m_vecZipPngFiles.clear();
	std::string zipFilePath = m_zipFilePath;
	if (getZip() == false)		// ����Ҫѹ��  ��ȡԭʼ�ļ���ʼ����
	{
		zipFilePath = m_selFilePath;
	}
	auto allFiles = Tool::walk(zipFilePath);
	for (auto filename : allFiles)
	{
		if (Tool::splitext(filename)[1] == ".png")
		{
			m_vecZipPngFiles.push_back(filename);
		}
	}

	// 
	int count = 0;	// ��ǰ�����ļ�����
	int enCount = 0;	// �ɼ�������
	int enCounted = 0; // �Ѿ���������
	int maxCount = m_vecZipPngFiles.size();// ��Ҫ��������

	// ������
	int proPos = 0;		// ���ܽ���
	CProgressCtrl* pProg = (CProgressCtrl*)GetDlgItem(IDC_PROGRESS);
	pProg->SetRange(0, 100);
	pProg->SetPos(0);

	// ������ϸ����б�
	std::string fileState = "";
	CListBox *List;
	List = (CListBox*)GetDlgItem(IDC_LISTCONTROL);
	List->ResetContent();
	if ( m_vecZipPngFiles.empty() )
	{
		Tool::EnToolLog("[error] �޼����ļ�");
		return;
	}
	
	Tool::EnToolLog("========�������====================");
	Tool::EnToolLog("========������� : " + m_enFileOutPath);


	// ��ʼ�����ļ�
	for (auto &filename : m_vecZipPngFiles)
	{
		Tool::EnToolLog("[encryFile] �������ļ��� " + filename);
		fileState = "";
		int state = CEncryptImage::EncryptPNG(filename, m_key, zipFilePath, m_enFileOutPath);

		fileState += filename;
		if ( state == 0)
		{
			fileState += "-->�Ѿ�����";
			enCounted++;
		}
		else if ( state == 1)
		{
			fileState += "-->���ܳɹ�";
			enCount++;
		}
		else{
			fileState += "-->����ʧ��";
		}
		count++;

		proPos = count * 100 / maxCount;
		pProg->SetPos(proPos);

		List->AddString(CString(fileState.c_str()));

	}

	List->AddString(_T("!!!!!!�������!!!!!!"));


	// ��ʾ���ܲ������
	char chBuff[128];
	sprintf_s(chBuff, "һ����%d���ļ�����,���ܳɹ��ļ�:%d�����Ѿ������ļ�:%d,", maxCount, enCount, enCounted);
	fileState = chBuff;
	List->AddString(CString(fileState.c_str()));

	// �������
	SetBtnState(true);
	GetDlgItem(IDC_CHECKLOCK)->EnableWindow(TRUE);

	// ������ɻָ�״̬
	//GetDlgItem(IDC_BUTZIP)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTENSTART)->EnableWindow(FALSE);

	//MessageBoxA(NULL, LPCSTR("������ɣ���ӭʹ��!"), NULL, MB_OK);
	MessageBox(_T("������ɣ���ӭʹ��!"), _T("�������"), MB_OK | MB_ICONINFORMATION);
}

void CEncryptImageToolDlg::SetBtnState(bool enable)
{
	GetDlgItem(IDC_BUTFILESEL)->EnableWindow(enable);
	GetDlgItem(IDC_BUTFILEOUT)->EnableWindow(enable);
	//GetDlgItem(IDC_BUTENSTART)->EnableWindow(enable);
	GetDlgItem(IDC_BUTTONKEY)->EnableWindow(enable);
	GetDlgItem(IDC_CHECKZIP)->EnableWindow(enable);

	m_pSelFileEdit->SetReadOnly(!enable);
	m_pOutFileEdit->SetReadOnly(!enable);
	m_pMinEdit->SetReadOnly(!enable);
	m_pMaxEdit->SetReadOnly(!enable);
}

void CEncryptImageToolDlg::ReadImageKey()
{
	// TODO:  ��ȡ��Կ
	//auto key_path = CSelectFolderDlg::BootOpenDialog();

	//std::ifstream infile;
	//infile.open(key_path);   //���ļ����������ļ��������� 
	//if (!infile.is_open())   //��ʧ��,�����������Ϣ,����ֹ�������� 
	//{
	//	std::cout << "==== ��Կ��ȡʧ��" << std::endl;
	//	GetDlgItem(IDC_KEYTEXT)->SetWindowTextW(_T("��ȡ��Կʧ��"));
	//	return;
	//}

	//char c;
	//int count = 0;
	//std::string strKey ="��ԿΪ��";
	//while (infile >> c)
	//{
	//	//infile >> c;
	//	m_key[count] = c;
	//	strKey += c;
	//	count++;

	//}
	////�ر��ļ������� 
	//infile.close();

	//Tool::EnToolLog(strKey);

	//GetDlgItem(IDC_KEYTEXT)->SetWindowTextW(CString(strKey.c_str()));

}

/**
 * ���óɹ� ���� 0
 * ʧ�� ���� -1
 */
int CEncryptImageToolDlg::ImageZipPng(const std::string filename, int minQua, int maxQua)
{
	std::string exePath = m_exePath + "\\pngquant.exe";
	std::string cmdStr = m_exePath + "\\pngZip.bat";
	
	Tool::EnToolLog("[zip file]" + filename);

	std::string outFile = filename.substr(m_selFilePath.size(), filename.size());
	outFile = m_zipFilePath + outFile;

	if (Tool::filedir(outFile) == -1)
	{
		Tool::EnToolLog("[zipout not file]" + outFile);
		return -2;
	}

	Tool::EnToolLog("[zipout png]" + outFile);

	// ִ��·��
	cmdStr.append(" ");
	cmdStr.append(exePath);

	// ��СƷ��
	char chBuff[128];
	sprintf_s(chBuff, " %d", minQua);
	cmdStr.append(chBuff);

	// ���Ʒ��
	sprintf_s(chBuff, " %d", maxQua);
	cmdStr.append(chBuff);

	// ��ѹ���ļ���
	cmdStr.append(" ");
	cmdStr.append(filename);

	// ѹ�����ļ���
	cmdStr.append(" ");
	cmdStr.append(outFile);
	Tool::EnToolLog("[zipCmd]" + cmdStr);

	WinExec(cmdStr.c_str(), SW_HIDE);
 	int state = system(cmdStr.c_str() );
	return state;

}

void CEncryptImageToolDlg::CheckFilePath()
{
	bool bSet = true;
	if (m_selFilePath.empty())
	{
		bSet = false;
		MessageBox(_T("��ѡ��������ļ���"), _T("��ʾ"), MB_OK | MB_ICONERROR);
		//MessageBoxA(NULL, LPCSTR("��ѡ��������ļ���!"), NULL, MB_OK);
	}
	else if ( m_selFileOutPath.empty())
	{
		bSet = false;
		MessageBox(_T("��ѡ������ļ���"), _T("��ʾ"), MB_OK | MB_ICONERROR);
		//MessageBoxA(NULL, LPCSTR("��ѡ������ļ���!"), NULL, MB_OK);
	}
	else if ( m_key.empty())
	{
		bSet = false;
		MessageBox(_T("��Կδ����"), _T("��ʾ"), MB_OK | MB_ICONERROR);
		//MessageBoxA(NULL, LPCSTR("��Կδ����!"), NULL, MB_OK);

	}

	setLock(bSet);

}

void CEncryptImageToolDlg::UpdateSelFile()
{
	m_vecPngFiles.clear();

	auto allFiles = Tool::walk(m_selFilePath);
	for (auto filename : allFiles)
	{
		if (Tool::splitext(filename)[1] == ".png")
		{
			m_vecPngFiles.push_back(filename);
		}
	}

	m_vecAllFiles.clear();
	m_vecAllFiles = Tool::getAllFiles(m_selFilePath);

}

void CEncryptImageToolDlg::UpdateOutFile()
{
	m_zipFilePath = m_selFileOutPath + "\\PNGZipFiles";
	m_enFileOutPath = m_selFileOutPath + "\\EnFiles";
	m_deFileOutPath = m_selFileOutPath + "\\DeFiles";
}

void CEncryptImageToolDlg::OnBnClickedButtonReadKey()
{
	// TODO:  ��ȡ��Կ��ť
	ReadImageKey();
}


void CEncryptImageToolDlg::OnBnClickedButzip()
{
	if (!getLock())
	{
		return;
	}

	// TODO:  ѹ��PNG�ļ�
	int maxCount = m_vecPngFiles.size();

	CProgressCtrl* pProg = (CProgressCtrl*)GetDlgItem(IDC_PROGRESS);
	pProg->SetRange(0, 100);
	pProg->SetPos(0);

	int zPos = 0;

	// ������ϸ����б�
	CListBox *List;
	List = (CListBox*)GetDlgItem(IDC_LISTCONTROL);
	List->ResetContent();

	std::string eQua = "";
	CString eStr;

	m_pMinEdit->GetWindowTextW(eStr);
	eQua = CT2CA(eStr.GetBuffer(0));
	int minQua = atoi(eQua.c_str());

	m_pMaxEdit->GetWindowTextW(eStr);
	eQua = CT2CA(eStr.GetBuffer(0));
	int maxQua = atoi(eQua.c_str());

	// �����СƷ�ʱ����Ʒ�ʴ���������Ʒ�� +5
	if ( minQua > maxQua)
	{
		maxQua = minQua + 5;
	}

	// ͼƬƷ���趨��Χ���
	if ( minQua < 0 || minQua > 95 || maxQua < 5 || maxQua > 100)
	{
		//MessageBoxA(NULL, LPCSTR("����������ѹ��ͼƬƷ��!"), NULL, MB_OK);
		MessageBox(_T("����������ѹ��ͼƬƷ��"), _T("��ʾ"), MB_OK | MB_ICONERROR);
		return;
	}

	// ��ʼѹ��ͼƬ
	if (m_vecPngFiles.empty())
	{
		MessageBox(_T("û�п�ѹ���ļ�"), _T("��ʾ"), MB_OK | MB_ICONERROR);
		List->AddString(_T("======û���ļ���ѹ��======"));
	}
	else
	{
		List->AddString(_T("======��ʼѹ��PNG======"));
	}

	int count = 1;
	for (auto &filename : m_vecPngFiles)
	{
		std::string zipFile = "";
		int zipState = ImageZipPng(filename, minQua, maxQua);

		char buff[256];
		sprintf_s(buff, "ѹ�� (%d/%d) ", count, maxCount);

		zipFile += buff;

		zipFile += filename;
		
		if ( zipState == 0)
		{
			zipFile += "==> ѹ�����";
		}
		else{
			zipFile += "==> ѹ��ʧ��";
		}
		zPos = count / maxCount * 100;
		++count;
		pProg->SetPos(zPos);

		List->AddString(CString(zipFile.c_str()));

	}

	List->AddString(_T("======ѹ��PNG���======"));

	GetDlgItem(IDC_BUTZIP)->EnableWindow(FALSE);		// ѹ����ť���ɵ��

	//MessageBoxA(NULL, LPCSTR("�ļ�ѹ����ɿɽ��м��ܲ���"), NULL, MB_OK);
	MessageBox(_T("�ļ�ѹ����ɿɽ��м��ܲ���"), _T("ѹ�����"), MB_OK | MB_ICONINFORMATION);

	GetDlgItem(IDC_BUTENSTART)->EnableWindow(TRUE);	// ѹ����ɿ��Լ���
}

void CEncryptImageToolDlg::OnEnKillfocusEditmaxbox()
{
	// TODO:  �趨�ؼ�edit���������ַ�Χ  ���Ʒ��
	UpdateData(true);

	std::string eQua = "";
	CString eStr;

	m_pMaxEdit->GetWindowTextW(eStr);
	if (eStr.IsEmpty())
	{
		MessageBox(_T("Ʒ����ֵ������Χ! (5-100)"), _T("��ʾ"), MB_OK | MB_ICONERROR);
		m_pMaxEdit->SetWindowText(_T("70"));
		return;
	}
	eQua = CT2CA(eStr.GetBuffer(0));
	int maxQua = atoi(eQua.c_str());

	if ( maxQua <5 || maxQua > 100)
	{
		//MessageBoxA(NULL, LPCSTR("Ʒ����ֵ������Χ! (5-100)"), NULL, MB_OK);
		MessageBox(_T("Ʒ����ֵ������Χ! (5-100)"), _T("��ʾ"), MB_OK | MB_ICONERROR);
		m_pMaxEdit->SetWindowText(_T("70"));
		eQua = "70";
		UpdateData(false);
	}
	
}


void CEncryptImageToolDlg::OnEnKillfocusEditminbox()
{
	// TODO:  �趨�ؼ�edit���������ַ�Χ  ��СƷ��
	UpdateData(true);

	std::string eQua = "";
	CString eStr;

	m_pMinEdit->GetWindowTextW(eStr);
	if (eStr.IsEmpty())
	{
		MessageBox(_T("Ʒ����ֵ������Χ! (0-95)"), _T("��ʾ"), MB_OK | MB_ICONERROR);
		m_pMinEdit->SetWindowText(_T("65"));
		return;
	}

	eQua = CT2CA(eStr.GetBuffer(0));
	int minQua = atoi(eQua.c_str());

	if (minQua < 0 || minQua > 95)
	{
		MessageBox(_T("Ʒ����ֵ������Χ! (0-95)"), _T("��ʾ"), MB_OK | MB_ICONERROR);
		//MessageBoxA(NULL, LPCSTR("Ʒ����ֵ������Χ! (0-95)"), NULL, MB_OK);
		m_pMinEdit->SetWindowText(_T("65"));
		eQua = "65";

		UpdateData(false);
	}

}


void CEncryptImageToolDlg::OnEnKillfocusEdit()
{
	CString eStr;
	m_pSelFileEdit->GetWindowTextW(eStr);
	std::string selFiles = CT2CA( eStr.GetBuffer(0) );

	m_selFileOutPath = selFiles;

	if (!PathIsDirectory(eStr))
	{
		//MessageBoxA(NULL, LPCSTR("��������ȷ����·��!"), NULL, MB_OK);
		return;
	}

}

// ��������ļ� �����
void CEncryptImageToolDlg::OnEnKillfocusEditout()
{
	CString eStr;
	m_pOutFileEdit->GetWindowText(eStr);
	std::string outFiles = CT2CA(eStr.GetBuffer(0));

	m_selFileOutPath = outFiles;

	if (!PathIsDirectory(eStr))
	{
		//MessageBoxA(NULL, LPCSTR("��������ȷ�������·��!"), NULL, MB_OK);
		return;
	}

	if (eStr.IsEmpty())
	{
		return;
	}

	UpdateOutFile();

}

/*����Ŀ¼�е���������*/
BOOL CEncryptImageToolDlg::CopyFolder(LPCTSTR pstrSrcFolder, LPCTSTR pstrDstFolder)
{
	if ((NULL == pstrSrcFolder) || (NULL == pstrSrcFolder))
	{
		return FALSE;
	}

	/*���ԴĿ¼�Ƿ��ǺϷ�Ŀ¼*/
	if (!IsDirectory(pstrSrcFolder))
	{
		return FALSE;
	}

	/*���ļ�����ת��ΪCString���ͣ���ȷ��Ŀ¼��·��ĩβΪ��б��*/
	CString strSrcFolder(pstrSrcFolder);
	if (strSrcFolder.Right(1) != _T('\\'))
	{
		strSrcFolder += _T("\\");
	}
	CString strDstFolder(pstrDstFolder);
	if (strDstFolder.Right(1) != _T("\\"))
	{
		strDstFolder += _T("\\");
	}

	/*�����Ŀ¼�����ƣ�ֱ�ӷ��ظ��Ƴɹ�*/
	if (0 == _tcscmp(strSrcFolder, strDstFolder))
	{
		return TRUE;
	}

	/*���Ŀ��Ŀ¼������,�򴴽�Ŀ¼*/
	if (!IsDirectory(strDstFolder))
	{
		if (!CreateDirectory(strDstFolder, NULL))
		{
			/*����Ŀ��Ŀ¼ʧ��*/
			return FALSE;
		}
	}

	/*����ԴĿ¼�в����ļ���ͨ���*/
	CString strWildcard(strSrcFolder);
	strWildcard += _T("*.*");

	/*���ļ����ң��鿴ԴĿ¼���Ƿ����ƥ����ļ�*/
	/*����FindFile�󣬱������FindNextFile���ܻ�ò����ļ�����Ϣ*/
	CFileFind finder;
	BOOL bWorking = finder.FindFile(strWildcard);

	while (bWorking)
	{
		/*������һ���ļ�*/
		bWorking = finder.FindNextFile();

		/*������ǰĿ¼��.������һ��Ŀ¼��..��*/
		if (finder.IsDots())
		{
			continue;
		}

		/*�õ���ǰҪ���Ƶ�Դ�ļ���·��*/
		CString strSrcFile = finder.GetFilePath();

		/*�õ�Ҫ���Ƶ�Ŀ���ļ���·��*/
		CString strDstFile(strDstFolder);
		strDstFile += finder.GetFileName();

		/*�жϵ�ǰ�ļ��Ƿ���Ŀ¼,*/
		/*�����Ŀ¼���ݹ���ø���Ŀ¼,*/
		/*����ֱ�Ӹ����ļ�*/
		if (finder.IsDirectory())
		{
			if (!CopyFolder(strSrcFile, strDstFile))
			{
				finder.Close();
				return FALSE;
			}
		}
		else
		{
			if (!CopyFile(strSrcFile, strDstFile, FALSE))
			{
				finder.Close();
				return FALSE;
			}
		}

	} /*while (bWorking)*/

	/*�ر��ļ�����*/
	finder.Close();

	return TRUE;

}

/*�ж�һ��·���Ƿ����Ѵ��ڵ�Ŀ¼*/
BOOL CEncryptImageToolDlg::IsDirectory(LPCTSTR pstrPath)
{
	if (NULL == pstrPath)
	{
		return FALSE;
	}

	/*ȥ��·��ĩβ�ķ�б��*/
	CString strPath = pstrPath;
	if (strPath.Right(1) == _T('\\'))
	{
		strPath.Delete(strPath.GetLength() - 1);
	}

	CFileFind finder;
	BOOL bRet = finder.FindFile(strPath);
	if (!bRet)
	{
		return FALSE;
	}

	/*�жϸ�·���Ƿ���Ŀ¼*/
	finder.FindNextFile();
	bRet = finder.IsDirectory();
	finder.Close();
	return bRet;
}


void CEncryptImageToolDlg::OnBnClickedButcopy()
{

	if (!getLock())
	{
		return;
	}
	// TODO:  ��ʼ����

	GetDlgItem(IDC_CHECKLOCK)->EnableWindow(FALSE);

	// ������ѡ���ļ�
	UpdateSelFile();

	// �����ļ�

	CString selPath;
	selPath = (CString)m_selFilePath.c_str();

	CString outPath;
	outPath = (CString)m_enFileOutPath.c_str();

	if (CopyFolder( (LPCTSTR)selPath, (LPCTSTR)outPath) )		// �����ɹ�
	{
		//MessageBoxA(NULL, LPCSTR("���Ƴɹ����ɽ���ѹ�������ܣ�"), NULL, MB_OK);
		MessageBox(_T("���Ƴɹ����ɽ���ѹ�������ܣ�"), _T("�������"), MB_OK | MB_ICONINFORMATION);
		if (getZip())
		{
			GetDlgItem(IDC_BUTZIP)->EnableWindow(TRUE);
		}
		else{
			GetDlgItem(IDC_BUTENSTART)->EnableWindow(TRUE);
		}
		GetDlgItem(IDC_BUTCOPY)->EnableWindow(FALSE);
	}
	else{		// ����ʧ��
		//MessageBoxA(NULL, LPCSTR("�ļ�����ʧ��"), NULL, MB_OK);
		MessageBox(_T("�ļ�����ʧ��!"), _T("��ʾ"), MB_OK | MB_ICONERROR);
	}
}


void CEncryptImageToolDlg::OnBnClickedCheckLock()
{
	bool bCheck = (this->IsDlgButtonChecked(IDC_CHECKLOCK) == 1);
	setLock(bCheck);
	if (bCheck == true)
	{
		CheckFilePath();
	}

}

void CEncryptImageToolDlg::setLock(bool bLock)
{
	m_bLock = bLock;

	this->CheckDlgButton(IDC_CHECKLOCK, bLock);
	GetDlgItem(IDC_BUTCOPY)->EnableWindow(bLock);
	SetBtnState(!bLock);
}


void CEncryptImageToolDlg::setZip(bool bZip)
{
	m_bZip = bZip;
	this->CheckDlgButton(IDC_CHECKZIP, bZip);
}

// ����
void CEncryptImageToolDlg::OnBnClickedButde()
{
	if (m_deFileOutPath.empty())
	{
		MessageBox(_T("����·��������"), _T("��ʾ"), MB_OK | MB_ICONERROR);
		return;
	}
	Tool::EnToolLog("============================================");
	Tool::EnToolLog("��ʼ�����ļ�");

	// ��ȡ�����ļ�
	/*CString enPath;
	enPath = (CString)m_enFileOutPath.c_str();

	CString dePath;
	dePath = (CString)m_deFileOutPath.c_str();*/

	// ��ȡ�������ļ�
	std::vector<std::string> files;
	files.clear();

	Tool::EnToolLog("��ȡ�����ļ�");
	auto all_files = Tool::walk(m_enFileOutPath);

	for (auto filename : all_files)
	{
		if (Tool::splitext(filename)[1] == ".png")
		{
			files.push_back(filename);
		}
	}
	Tool::EnToolLog("��ȡ�����ļ� ------> ���");

	// ��ʼ����
	BOOL bFinish = TRUE;

	for (auto enFilename : files)
	{
		Tool::EnToolLog("��ʼ�����ļ� ------> "+enFilename);
		int state =	CDecrypt::DecryptPNG(enFilename, m_key, m_enFileOutPath, m_deFileOutPath);
		std::string errStr = "==> [decrypt]�����ļ� code " + state;
		Tool::EnToolLog(errStr);
		if (state != 1 && state != 0 )  // 0 �Ѿ����� 1 ���ܳɹ�
		{
			bFinish = FALSE;
			Tool::EnToolLog("����ʧ�� �˳����ܣ���������");
			break;
		}
	}
	
	// �������
	if (bFinish)
	{
		Tool::EnToolLog("������ɣ���������");
		MessageBox(_T("��ϲ�����ܳɹ���"), _T("�������"), MB_OK | MB_ICONINFORMATION);
	} 
	else
	{
		Tool::EnToolLog("����ʧ�ܣ���������");
		MessageBox(_T("����ʧ�ܣ���鿴log�ļ�"), _T("����ʧ��"), MB_OK | MB_ICONERROR);
	}
}



void CEncryptImageToolDlg::OnBnClickedCheckzip()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������

	bool bCheck = (this->IsDlgButtonChecked(IDC_CHECKZIP) == 1);
	setZip(bCheck);
}
