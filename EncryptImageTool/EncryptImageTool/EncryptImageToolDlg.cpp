
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
#include "Decrypt.h"

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
	ON_EN_KILLFOCUS(IDC_EDITMAXBOX, &CEncryptImageToolDlg::OnEnKillfocusEditmaxbox)
	ON_EN_KILLFOCUS(IDC_EDITMINBOX, &CEncryptImageToolDlg::OnEnKillfocusEditminbox)
	ON_EN_KILLFOCUS(IDC_EDIT, &CEncryptImageToolDlg::OnEnKillfocusEdit)
	ON_EN_KILLFOCUS(IDC_EDITOUT, &CEncryptImageToolDlg::OnEnKillfocusEditout)
	ON_BN_CLICKED(IDC_BUTCOPY, &CEncryptImageToolDlg::OnBnClickedButcopy)
	ON_BN_CLICKED(IDC_CHECKLOCK, &CEncryptImageToolDlg::OnBnClickedCheckLock)
	ON_BN_CLICKED(IDC_BUTDE, &CEncryptImageToolDlg::OnBnClickedButde)
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
	m_zipFilePath = "";
	m_enFileOutPath = "";
	m_exePath = Tool::curdir();

	// 设置key
	m_key = Tool::SECRET_KEY;

	char c;
	int count = 0;
	std::string strKey ="秘钥为：";
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
	std::fstream file(logPath, std::ios::out);		// 清空文件

	GetDlgItem(IDC_BUTENSTART)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTZIP)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTCOPY)->EnableWindow(FALSE);


	// 解密
#if ENDE
	GetDlgItem(IDC_BUTDE)->ShowWindow(TRUE);
#else
	GetDlgItem(IDC_BUTDE)->ShowWindow(FALSE);
#endif

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
	CString filePath = CSelectFolderDlg::Show();

	GetDlgItem(IDC_SEL_FILETXT)->SetWindowText(filePath);

	m_pSelFileEdit->SetWindowText(filePath);
	
	m_selFilePath = CT2CA(filePath.GetBuffer(0));

	if (getLock())		// 锁定不能操作
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
	// TODO:  选择加密输出文件
	CString outFile = CSelectFolderDlg::Show();

	GetDlgItem(IDC_FILE_OUT)->SetWindowTextW(outFile);
	m_selFileOutPath = CT2CA(outFile.GetBuffer(0));

	m_pOutFileEdit->SetWindowText(outFile);

	if (getLock())		// 锁定不能操作
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
	// 
	int count = 0;	// 当前操作文件数量
	int enCount = 0;	// 可加密数量
	int enCounted = 0; // 已经加密数量
	int maxCount = m_vecZipPngFiles.size();// 需要加密数量

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
	if ( m_vecZipPngFiles.empty() )
	{
		Tool::EnToolLog("[error] 无加密文件");
		return;
	}
	
	Tool::EnToolLog("========加密输出====================");
	Tool::EnToolLog("========加密输出 : " + m_enFileOutPath);


	// 开始加密文件
	for (auto &filename : m_vecZipPngFiles)
	{
		Tool::EnToolLog("[encryFile] 待加密文件： " + filename);
		fileState = "";
		int state = CEncryptImage::EncryptPNG(filename, m_key, m_zipFilePath, m_enFileOutPath);

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
		else{
			fileState += "-->加密失败";
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
	//GetDlgItem(IDC_BUTZIP)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTENSTART)->EnableWindow(FALSE);

	//MessageBoxA(NULL, LPCSTR("加密完成，欢迎使用!"), NULL, MB_OK);
	MessageBox(_T("加密完成，欢迎使用!"), _T("加密完成"), MB_OK | MB_ICONINFORMATION);
}

void CEncryptImageToolDlg::SetBtnState(bool enable)
{
	GetDlgItem(IDC_BUTFILESEL)->EnableWindow(enable);
	GetDlgItem(IDC_BUTFILEOUT)->EnableWindow(enable)
		;
	//GetDlgItem(IDC_BUTENSTART)->EnableWindow(enable);
	GetDlgItem(IDC_BUTTONKEY)->EnableWindow(enable);

	GetDlgItem(IDC_CHECKLOCK)->EnableWindow(enable);

	m_pSelFileEdit->SetReadOnly(!enable);
	m_pOutFileEdit->SetReadOnly(!enable);
	m_pMinEdit->SetReadOnly(!enable);
	m_pMaxEdit->SetReadOnly(!enable);
}

void CEncryptImageToolDlg::ReadImageKey()
{
	// TODO:  读取秘钥
	//auto key_path = CSelectFolderDlg::BootOpenDialog();

	//std::ifstream infile;
	//infile.open(key_path);   //将文件流对象与文件连接起来 
	//if (!infile.is_open())   //若失败,则输出错误消息,并终止程序运行 
	//{
	//	std::cout << "==== 秘钥读取失败" << std::endl;
	//	GetDlgItem(IDC_KEYTEXT)->SetWindowTextW(_T("读取秘钥失败"));
	//	return;
	//}

	//char c;
	//int count = 0;
	//std::string strKey ="秘钥为：";
	//while (infile >> c)
	//{
	//	//infile >> c;
	//	m_key[count] = c;
	//	strKey += c;
	//	count++;

	//}
	////关闭文件输入流 
	//infile.close();

	//Tool::EnToolLog(strKey);

	//GetDlgItem(IDC_KEYTEXT)->SetWindowTextW(CString(strKey.c_str()));

}

/**
 * 调用成功 返回 0
 * 失败 返回 -1
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
		MessageBox(_T("请选择待加密文件夹"), _T("提示"), MB_OK | MB_ICONERROR);
		//MessageBoxA(NULL, LPCSTR("请选择待加密文件夹!"), NULL, MB_OK);
	}
	else if ( m_selFileOutPath.empty())
	{
		bSet = false;
		MessageBox(_T("请选择输出文件夹"), _T("提示"), MB_OK | MB_ICONERROR);
		//MessageBoxA(NULL, LPCSTR("请选择输出文件夹!"), NULL, MB_OK);
	}
	else if ( m_key.empty())
	{
		bSet = false;
		MessageBox(_T("秘钥未设置"), _T("提示"), MB_OK | MB_ICONERROR);
		//MessageBoxA(NULL, LPCSTR("秘钥未设置!"), NULL, MB_OK);

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
	// TODO:  读取秘钥按钮
	ReadImageKey();
}


void CEncryptImageToolDlg::OnBnClickedButzip()
{
	if (!getLock())
	{
		return;
	}

	// TODO:  压缩PNG文件
	int maxCount = m_vecPngFiles.size();

	CProgressCtrl* pProg = (CProgressCtrl*)GetDlgItem(IDC_PROGRESS);
	pProg->SetRange(0, 100);
	pProg->SetPos(0);

	int zPos = 0;

	// 加密详细情况列表
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

	// 如果最小品质比最大品质大，则提高最大品质 +5
	if ( minQua > maxQua)
	{
		maxQua = minQua + 5;
	}

	// 图片品质设定范围检查
	if ( minQua < 0 || minQua > 95 || maxQua < 5 || maxQua > 100)
	{
		//MessageBoxA(NULL, LPCSTR("请重新设置压缩图片品质!"), NULL, MB_OK);
		MessageBox(_T("请重新设置压缩图片品质"), _T("提示"), MB_OK | MB_ICONERROR);
		return;
	}

	// 开始压缩图片
	if (m_vecPngFiles.empty())
	{
		MessageBox(_T("没有可压缩文件"), _T("提示"), MB_OK | MB_ICONERROR);
		List->AddString(_T("======没有文件可压缩======"));
	}
	else
	{
		List->AddString(_T("======开始压缩PNG======"));
	}

	int count = 1;
	for (auto &filename : m_vecPngFiles)
	{
		std::string zipFile = "";
		int zipState = ImageZipPng(filename, minQua, maxQua);

		char buff[256];
		sprintf_s(buff, "压缩 (%d/%d) ", count, maxCount);

		zipFile += buff;

		zipFile += filename;
		
		if ( zipState == 0)
		{
			zipFile += "==> 压缩完成";
		}
		else{
			zipFile += "==> 压缩失败";
		}
		zPos = count / maxCount * 100;
		++count;
		pProg->SetPos(zPos);

		List->AddString(CString(zipFile.c_str()));

	}

	List->AddString(_T("======压缩PNG完成======"));

	GetDlgItem(IDC_BUTZIP)->EnableWindow(FALSE);		// 压缩按钮不可点击


	// 读取待加密文件
	m_vecZipPngFiles.clear();
	std::string zipFilePath = m_zipFilePath;
	auto allFiles = Tool::walk(zipFilePath);
	for (auto filename : allFiles)
	{
		if (Tool::splitext(filename)[1] == ".png")
		{
			m_vecZipPngFiles.push_back(filename);
		}
	}
	//MessageBoxA(NULL, LPCSTR("文件压缩完成可进行加密操作"), NULL, MB_OK);
	MessageBox(_T("文件压缩完成可进行加密操作"), _T("压缩完成"), MB_OK | MB_ICONINFORMATION);

	GetDlgItem(IDC_BUTENSTART)->EnableWindow(TRUE);	// 压缩完成可以加密
}

void CEncryptImageToolDlg::OnEnKillfocusEditmaxbox()
{
	// TODO:  设定控件edit可输入数字范围  最大品质
	UpdateData(true);

	std::string eQua = "";
	CString eStr;

	m_pMaxEdit->GetWindowTextW(eStr);
	if (eStr.IsEmpty())
	{
		MessageBox(_T("品质数值超过范围! (5-100)"), _T("提示"), MB_OK | MB_ICONERROR);
		m_pMaxEdit->SetWindowText(_T("70"));
		return;
	}
	eQua = CT2CA(eStr.GetBuffer(0));
	int maxQua = atoi(eQua.c_str());

	if ( maxQua <5 || maxQua > 100)
	{
		//MessageBoxA(NULL, LPCSTR("品质数值超过范围! (5-100)"), NULL, MB_OK);
		MessageBox(_T("品质数值超过范围! (5-100)"), _T("提示"), MB_OK | MB_ICONERROR);
		m_pMaxEdit->SetWindowText(_T("70"));
		eQua = "70";
		UpdateData(false);
	}
	
}


void CEncryptImageToolDlg::OnEnKillfocusEditminbox()
{
	// TODO:  设定控件edit可输入数字范围  最小品质
	UpdateData(true);

	std::string eQua = "";
	CString eStr;

	m_pMinEdit->GetWindowTextW(eStr);
	if (eStr.IsEmpty())
	{
		MessageBox(_T("品质数值超过范围! (0-95)"), _T("提示"), MB_OK | MB_ICONERROR);
		m_pMinEdit->SetWindowText(_T("65"));
		return;
	}

	eQua = CT2CA(eStr.GetBuffer(0));
	int minQua = atoi(eQua.c_str());

	if (minQua < 0 || minQua > 95)
	{
		MessageBox(_T("品质数值超过范围! (0-95)"), _T("提示"), MB_OK | MB_ICONERROR);
		//MessageBoxA(NULL, LPCSTR("品质数值超过范围! (0-95)"), NULL, MB_OK);
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
		//MessageBoxA(NULL, LPCSTR("所输入正确加密路径!"), NULL, MB_OK);
		return;
	}

}

// 输出加密文件 输入框
void CEncryptImageToolDlg::OnEnKillfocusEditout()
{
	CString eStr;
	m_pOutFileEdit->GetWindowText(eStr);
	std::string outFiles = CT2CA(eStr.GetBuffer(0));

	m_selFileOutPath = outFiles;

	if (!PathIsDirectory(eStr))
	{
		//MessageBoxA(NULL, LPCSTR("所输入正确加密输出路径!"), NULL, MB_OK);
		return;
	}

	if (eStr.IsEmpty())
	{
		return;
	}

	UpdateOutFile();

}

/*复制目录中的所有内容*/
BOOL CEncryptImageToolDlg::CopyFolder(LPCTSTR pstrSrcFolder, LPCTSTR pstrDstFolder)
{
	if ((NULL == pstrSrcFolder) || (NULL == pstrSrcFolder))
	{
		return FALSE;
	}

	/*检查源目录是否是合法目录*/
	if (!IsDirectory(pstrSrcFolder))
	{
		return FALSE;
	}

	/*把文件名称转换为CString类型，并确认目录的路径末尾为反斜杠*/
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

	/*如果是目录自身复制，直接返回复制成功*/
	if (0 == _tcscmp(strSrcFolder, strDstFolder))
	{
		return TRUE;
	}

	/*如果目的目录不存在,则创建目录*/
	if (!IsDirectory(strDstFolder))
	{
		if (!CreateDirectory(strDstFolder, NULL))
		{
			/*创建目的目录失败*/
			return FALSE;
		}
	}

	/*创建源目录中查找文件的通配符*/
	CString strWildcard(strSrcFolder);
	strWildcard += _T("*.*");

	/*打开文件查找，查看源目录中是否存在匹配的文件*/
	/*调用FindFile后，必须调用FindNextFile才能获得查找文件的信息*/
	CFileFind finder;
	BOOL bWorking = finder.FindFile(strWildcard);

	while (bWorking)
	{
		/*查找下一个文件*/
		bWorking = finder.FindNextFile();

		/*跳过当前目录“.”和上一级目录“..”*/
		if (finder.IsDots())
		{
			continue;
		}

		/*得到当前要复制的源文件的路径*/
		CString strSrcFile = finder.GetFilePath();

		/*得到要复制的目标文件的路径*/
		CString strDstFile(strDstFolder);
		strDstFile += finder.GetFileName();

		/*判断当前文件是否是目录,*/
		/*如果是目录，递归调用复制目录,*/
		/*否则，直接复制文件*/
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

	/*关闭文件查找*/
	finder.Close();

	return TRUE;

}

/*判断一个路径是否是已存在的目录*/
BOOL CEncryptImageToolDlg::IsDirectory(LPCTSTR pstrPath)
{
	if (NULL == pstrPath)
	{
		return FALSE;
	}

	/*去除路径末尾的反斜杠*/
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

	/*判断该路径是否是目录*/
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
	// TODO:  开始加密
	SetBtnState(false);

	// 先设置选择文件
	UpdateSelFile();

	// 复制文件

	CString selPath;
	selPath = (CString)m_selFilePath.c_str();

	CString outPath;
	outPath = (CString)m_enFileOutPath.c_str();

	if (CopyFolder( (LPCTSTR)selPath, (LPCTSTR)outPath) )		// 拷贝成功
	{
		//MessageBoxA(NULL, LPCSTR("复制成功，可进行压缩、加密！"), NULL, MB_OK);
		MessageBox(_T("复制成功，可进行压缩、加密！"), _T("复制完成"), MB_OK | MB_ICONINFORMATION);
		GetDlgItem(IDC_BUTZIP)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTCOPY)->EnableWindow(FALSE);
	}
	else{		// 拷贝失败
		//MessageBoxA(NULL, LPCSTR("文件复制失败"), NULL, MB_OK);
		MessageBox(_T("文件复制失败!"), _T("提示"), MB_OK | MB_ICONERROR);
	}
}


void CEncryptImageToolDlg::OnBnClickedCheckLock()
{
	bool bCheck = (this->IsDlgButtonChecked(IDC_CHECKLOCK) == 1);
	setLock(bCheck);
	CheckFilePath();

}

void CEncryptImageToolDlg::setLock(bool bLock)
{
	m_bLock = bLock;

	this->CheckDlgButton(IDC_CHECKLOCK, bLock);
	GetDlgItem(IDC_BUTCOPY)->EnableWindow(bLock);
}


// 解密
void CEncryptImageToolDlg::OnBnClickedButde()
{
	if (m_deFileOutPath.empty())
	{
		MessageBox(_T("解密路径不存在"), _T("提示"), MB_OK | MB_ICONERROR);
		return;
	}
	Tool::EnToolLog("============================================");
	Tool::EnToolLog("开始解密文件");

	// 读取加密文件
	/*CString enPath;
	enPath = (CString)m_enFileOutPath.c_str();

	CString dePath;
	dePath = (CString)m_deFileOutPath.c_str();*/

	// 读取待加密文件
	std::vector<std::string> files;
	files.clear();

	Tool::EnToolLog("读取加密文件");
	auto all_files = Tool::walk(m_enFileOutPath);

	for (auto filename : all_files)
	{
		if (Tool::splitext(filename)[1] == ".png")
		{
			files.push_back(filename);
		}
	}
	Tool::EnToolLog("读取加密文件 ------> 完成");

	// 开始解密
	BOOL bFinish = TRUE;

	for (auto enFilename : files)
	{
		Tool::EnToolLog("开始解密文件 ------> "+enFilename);
		int state =	CDecrypt::DecryptPNG(enFilename, m_key, m_enFileOutPath, m_deFileOutPath);
		std::string errStr = "==> [decrypt]解密文件 code " + state;
		Tool::EnToolLog(errStr);
		if (state != 1 && state != 0 )  // 0 已经解密 1 解密成功
		{
			bFinish = FALSE;
			Tool::EnToolLog("解密失败 退出解密！！！！！");
			break;
		}
	}
	
	// 解密完成
	if (bFinish)
	{
		Tool::EnToolLog("解密完成！！！！！");
		MessageBox(_T("恭喜！解密成功！"), _T("解密完成"), MB_OK | MB_ICONINFORMATION);
	} 
	else
	{
		Tool::EnToolLog("解密失败！！！！！");
		MessageBox(_T("解密失败！请查看log文件"), _T("解密失败"), MB_OK | MB_ICONERROR);
	}
}
