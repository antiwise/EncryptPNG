
// EncryptImageToolDlg.cpp : 实现文件
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


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CEncryptImageToolDlg 对话框



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
END_MESSAGE_MAP()


// CEncryptImageToolDlg 消息处理程序

BOOL CEncryptImageToolDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码
	m_selFilePath = "";
	m_selFileOutPath = "";
	m_exePath = Tool::curdir();

	clear(m_key);
	m_vecPngFiles.clear();
	m_vecZipPngFiles.clear();
	GetDlgItem(IDC_BUTENSTART)->EnableWindow(0);

	CheckFilePath();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CEncryptImageToolDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CEncryptImageToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CEncryptImageToolDlg::OnBnClickedButfilesel()
{
	// TODO:  选择加密文件
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
	// TODO:  选择加密输出文件
	auto outFile = CSelectFolderDlg::Show();

	GetDlgItem(IDC_FILE_OUT)->SetWindowTextW(outFile);
	m_selFileOutPath = CT2CA(outFile.GetBuffer(0));

	CheckFilePath();
}


void CEncryptImageToolDlg::OnBnClickedButenstart()
{
	// TODO:  开始加密
	SetBtnState(false); 

	// 
	int count = 0;	// 当前操作文件数量
	int enCount = 0;	// 可加密数量
	int enCounted = 0; // 已经加密数量
	int maxCount = m_vecPngFiles.size();// 需要加密数量

	// 进度条
	int proPos = 0;		// 加密进度
	CProgressCtrl* pProg = (CProgressCtrl*)GetDlgItem(IDC_PROGRESS);
	pProg->SetRange(0, 100);
	pProg->SetPos(0);

	// 加密详细情况列表
	std::string fileState = "";
	CListBox *List;
	List = (CListBox*)GetDlgItem(IDC_LISTCONTROL);
	List->ResetContent();


	// 开始加密文件
	for (auto &filename : m_vecZipPngFiles)
	{
		fileState = "";
		int state = CEncryptImage::EncryptPNG(filename, m_key, m_selFilePath+"\\PNGZipFiles", m_selFileOutPath);

		fileState += filename;
		if ( state == 0)
		{
			fileState += "-->已经加密";
			enCounted++;
		}
		else if ( state == 1)
		{
			fileState += "-->加密成功";
			enCount++;
		}
		count++;

		proPos = count * 100 / maxCount;
		pProg->SetPos(proPos);

		List->AddString(CString(fileState.c_str()));

	}

	List->AddString(_T("!!!!!!加密完成!!!!!!"));


	// 提示加密操作情况
	char chBuff[128];
	sprintf_s(chBuff, "一共有%d个文件加密,加密成功文件:%d个，已经加密文件:%d,", maxCount, enCount, enCounted);
	fileState = chBuff;
	List->AddString(CString(fileState.c_str()));

	// 加密完成
	SetBtnState(true);

	// 加密完成恢复状态
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
	// TODO:  读取秘钥
	auto key_path = CSelectFolderDlg::BootOpenDialog();

	std::ifstream infile;
	infile.open(key_path);   //将文件流对象与文件连接起来 
	if (!infile.is_open())   //若失败,则输出错误消息,并终止程序运行 
	{
		std::cout << "==== 秘钥读取失败" << std::endl;
		GetDlgItem(IDC_KEYTEXT)->SetWindowTextW(_T("读取秘钥失败"));
		return;
	}

	char c;
	int count = 0;
	std::string strKey ="秘钥为：";
	while (infile >> c)
	{
		//infile >> c;
		m_key[count] = c;
		strKey += c;
		count++;

	}
	//关闭文件输入流 
	infile.close();

	GetDlgItem(IDC_KEYTEXT)->SetWindowTextW(CString(strKey.c_str()));

}

int CEncryptImageToolDlg::ImageZipPng(const std::string filename, int minQua, int maxQua)
{
	std::string exePath = m_exePath + "\\pngquant.exe";
	std::string cmdStr = m_exePath + "\\pngZip.bat";
	
	std::string zipFile = m_selFilePath+"\\PNGZipFiles";

	std::string outFile = filename.substr(m_selFilePath.size(), filename.size());
	outFile = zipFile + outFile;

	if (Tool::filedir(outFile) == -1)
	{
		std::cerr << outFile << " 文件夹不存在" << std::endl;
		return 0;
	}

	// 执行路径
	cmdStr.append(" ");
	cmdStr.append(exePath);

	// 最小品质
	char chBuff[128];
	sprintf_s(chBuff, " %d", minQua);
	cmdStr.append(chBuff);

	// 最大品质
	sprintf_s(chBuff, " %d", maxQua);
	cmdStr.append(chBuff);

	// 待压缩文件名
	cmdStr.append(" ");
	cmdStr.append(filename);

	// 压缩后文件名
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
		GetDlgItem(IDC_BUTZIP)->EnableWindow(1);		// 压缩按钮不可点击
	}
	else
	{
		GetDlgItem(IDC_BUTZIP)->EnableWindow(0);		// 压缩按钮不可点击
	}

}

void CEncryptImageToolDlg::OnBnClickedButtonReadKey()
{
	// TODO:  读取秘钥按钮
	ReadImageKey();
	CheckFilePath();
}


void CEncryptImageToolDlg::OnBnClickedButzip()
{
	// TODO:  压缩PNG文件

	int count = 0;
	int maxCount = m_vecPngFiles.size();

	CProgressCtrl* pProg = (CProgressCtrl*)GetDlgItem(IDC_PROGRESS);
	pProg->SetRange(0, 100);
	pProg->SetPos(0);

	int zPos = 0;

	// 加密详细情况列表
	std::string zipFile = "";
	CListBox *List;
	List = (CListBox*)GetDlgItem(IDC_LISTCONTROL);
	List->ResetContent();
	List->AddString(_T("======开始压缩PNG======"));

	for (auto &filename : m_vecPngFiles)
	{
		zipFile = "压缩：";
		zipFile += filename;

		int zipState = ImageZipPng(filename, 50, 60);
		
		if ( zipState == 1)
		{
			zipFile += "==> 压缩完成";

			List->AddString(CString(zipFile.c_str()));


			zPos = count / maxCount * 100;
			pProg->SetPos(zPos);
		}
		
	}

	List->AddString(_T("======压缩PNG完成======"));
	if ( m_vecPngFiles.size() <= 0)
	{
		List->AddString(_T("======没有文件可压缩======"));
	}
// 	List->AddString(_T("======================="));
// 	List->AddString(_T("======请对PNG图片加密======"));

	GetDlgItem(IDC_BUTZIP)->EnableWindow(0);		// 压缩按钮不可点击
	GetDlgItem(IDC_BUTENSTART)->EnableWindow(1);	// 压缩完成可以加密


	// 读取待加密文件
	m_vecZipPngFiles.clear();
	std::string zipFilePath = m_selFilePath + "\\PNGZipFiles";
	auto allFiles = Tool::walk(zipFilePath);
	for (auto filename : allFiles)
	{
		if (Tool::splitext(filename)[1] == ".png")
		{
			m_vecZipPngFiles.push_back(filename);
		}
	}
}
