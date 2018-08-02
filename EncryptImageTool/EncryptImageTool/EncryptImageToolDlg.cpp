
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
	m_exePath = Tool::curdir();

	clear(m_key);
	m_vecPngFiles.clear();
	m_vecZipPngFiles.clear();
	GetDlgItem(IDC_BUTENSTART)->EnableWindow(0);

	CheckFilePath();

	m_pMinEdit = (CEdit*)GetDlgItem(IDC_EDITMINBOX);
	m_pMinEdit->SetWindowTextW(_T("65"));
	m_pMinEdit->SetSel(0, 2);

	m_pMaxEdit = (CEdit*)GetDlgItem(IDC_EDITMAXBOX);
	m_pMaxEdit->SetWindowTextW(_T("70"));
	m_pMaxEdit->SetSel(0, 2);



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
	auto filePath = CSelectFolderDlg::Show();

	GetDlgItem(IDC_SEL_FILETXT)->SetWindowTextW(filePath);
	
	m_selFilePath = CT2CA(filePath.GetBuffer(0));

	m_vecPngFiles.clear();

	auto allFiles = Tool::walk(m_selFilePath);
	for ( auto filename : allFiles)
	{
		if ( Tool::splitext(filename)[1] == ".png")
		{
			m_vecPngFiles.push_back(filename);
		}
	}

	CheckFilePath();
}


void CEncryptImageToolDlg::OnBnClickedButfileout()
{
	// TODO:  ѡ���������ļ�
	auto outFile = CSelectFolderDlg::Show();

	GetDlgItem(IDC_FILE_OUT)->SetWindowTextW(outFile);
	m_selFileOutPath = CT2CA(outFile.GetBuffer(0));

	CheckFilePath();
}


void CEncryptImageToolDlg::OnBnClickedButenstart()
{
	// TODO:  ��ʼ����
	SetBtnState(false); 

	// 
	int count = 0;	// ��ǰ�����ļ�����
	int enCount = 0;	// �ɼ�������
	int enCounted = 0; // �Ѿ���������
	int maxCount = m_vecPngFiles.size();// ��Ҫ��������

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


	// ��ʼ�����ļ�
	for (auto &filename : m_vecZipPngFiles)
	{
		fileState = "";
		int state = CEncryptImage::EncryptPNG(filename, m_key, m_zipFilePath+"\\PNGZipFiles", m_selFileOutPath);

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

	// ������ɻָ�״̬
	GetDlgItem(IDC_BUTZIP)->EnableWindow(1);
	GetDlgItem(IDC_BUTENSTART)->EnableWindow(0);
}

void CEncryptImageToolDlg::SetBtnState(bool enable)
{
	GetDlgItem(IDC_BUTFILESEL)->EnableWindow(enable);
	GetDlgItem(IDC_BUTFILEOUT)->EnableWindow(enable);
	GetDlgItem(IDC_BUTENSTART)->EnableWindow(enable);
	GetDlgItem(IDC_BUTTONKEY)->EnableWindow(enable);
}

void CEncryptImageToolDlg::ReadImageKey()
{
	// TODO:  ��ȡ��Կ
	auto key_path = CSelectFolderDlg::BootOpenDialog();

	std::ifstream infile;
	infile.open(key_path);   //���ļ����������ļ��������� 
	if (!infile.is_open())   //��ʧ��,�����������Ϣ,����ֹ�������� 
	{
		std::cout << "==== ��Կ��ȡʧ��" << std::endl;
		GetDlgItem(IDC_KEYTEXT)->SetWindowTextW(_T("��ȡ��Կʧ��"));
		return;
	}

	char c;
	int count = 0;
	std::string strKey ="��ԿΪ��";
	while (infile >> c)
	{
		//infile >> c;
		m_key[count] = c;
		strKey += c;
		count++;

	}
	//�ر��ļ������� 
	infile.close();

	Tool::EnToolLog(strKey);

	GetDlgItem(IDC_KEYTEXT)->SetWindowTextW(CString(strKey.c_str()));

}

int CEncryptImageToolDlg::ImageZipPng(const std::string filename, int minQua, int maxQua)
{
	std::string exePath = m_exePath + "\\pngquant.exe";
	std::string cmdStr = m_exePath + "\\pngZip.bat";
	
	int lastpos = m_selFilePath.find_last_of("\\");
	m_zipFilePath = m_selFilePath.substr(0, lastpos);

	m_zipFilePath = m_zipFilePath + "\\PNGZipFiles";
	Tool::EnToolLog("[zip file]" + filename);

	std::string outFile = filename.substr(m_selFilePath.size(), filename.size());
	outFile = m_zipFilePath + outFile;

	if (Tool::filedir(outFile) == -1)
	{
		Tool::EnToolLog("[zipout not file]" + outFile);
		return 0;
	}

	Tool::EnToolLog("[zipout file]" + outFile);

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

 	system(cmdStr.c_str() );
	return 1;

}

void CEncryptImageToolDlg::CheckFilePath()
{
	bool bSet = true;
	if (m_selFilePath.empty())
	{
		bSet = false;
	}
	if ( m_selFileOutPath.empty())
	{
		bSet = false;
	}

	if ( m_key.empty())
	{
		bSet = false;
	}

	if ( bSet )
	{
		GetDlgItem(IDC_BUTZIP)->EnableWindow(1);		// ѹ����ť���ɵ��
	}
	else
	{
		GetDlgItem(IDC_BUTZIP)->EnableWindow(0);		// ѹ����ť���ɵ��
	}

}

void CEncryptImageToolDlg::OnBnClickedButtonReadKey()
{
	// TODO:  ��ȡ��Կ��ť
	ReadImageKey();
	CheckFilePath();
}


void CEncryptImageToolDlg::OnBnClickedButzip()
{
	// TODO:  ѹ��PNG�ļ�

	int count = 0;
	int maxCount = m_vecPngFiles.size();

	CProgressCtrl* pProg = (CProgressCtrl*)GetDlgItem(IDC_PROGRESS);
	pProg->SetRange(0, 100);
	pProg->SetPos(0);

	int zPos = 0;

	// ������ϸ����б�
	std::string zipFile = "";
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
		MessageBoxA(NULL, LPCSTR("����������ѹ��ͼƬƷ��!"), NULL, MB_OK);
		return;
	}

	// ��ʼѹ��ͼƬ
	List->AddString(_T("======��ʼѹ��PNG======"));
	for (auto &filename : m_vecPngFiles)
	{
		zipFile = "ѹ����";
		zipFile += filename;

		int zipState = ImageZipPng(filename, minQua, maxQua);
		
		if ( zipState == 1)
		{
			zipFile += "==> ѹ�����";

			List->AddString(CString(zipFile.c_str()));


			zPos = count / maxCount * 100;
			pProg->SetPos(zPos);
		}
		
	}

	List->AddString(_T("======ѹ��PNG���======"));
	if ( m_vecPngFiles.size() <= 0)
	{
		List->AddString(_T("======û���ļ���ѹ��======"));
	}
// 	List->AddString(_T("======================="));
// 	List->AddString(_T("======���PNGͼƬ����======"));

	GetDlgItem(IDC_BUTZIP)->EnableWindow(0);		// ѹ����ť���ɵ��
	GetDlgItem(IDC_BUTENSTART)->EnableWindow(1);	// ѹ����ɿ��Լ���


	// ��ȡ�������ļ�
	m_vecZipPngFiles.clear();
	std::string zipFilePath = m_zipFilePath + "\\PNGZipFiles";
	auto allFiles = Tool::walk(zipFilePath);
	for (auto filename : allFiles)
	{
		if (Tool::splitext(filename)[1] == ".png")
		{
			m_vecZipPngFiles.push_back(filename);
		}
	}
}

void CEncryptImageToolDlg::OnEnKillfocusEditmaxbox()
{
	// TODO:  �趨�ؼ�edit���������ַ�Χ  ���Ʒ��
	UpdateData(true);

	std::string eQua = "";
	CString eStr;

	m_pMaxEdit->GetWindowTextW(eStr);
	eQua = CT2CA(eStr.GetBuffer(0));
	int maxQua = atoi(eQua.c_str());

	if ( maxQua <5 || maxQua > 100)
	{
		MessageBoxA(NULL, LPCSTR("Ʒ����ֵ������Χ! (5-100)"), NULL, MB_OK);
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
	eQua = CT2CA(eStr.GetBuffer(0));
	int minQua = atoi(eQua.c_str());

	if (minQua < 0 || minQua > 95)
	{
		MessageBoxA(NULL, LPCSTR("Ʒ����ֵ������Χ! (0-95)"), NULL, MB_OK);
		UpdateData(false);
	}

}
