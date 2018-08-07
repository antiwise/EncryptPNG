
// EncryptImageToolDlg.h : 头文件
//

#pragma once
#include "AES.h"
#include <vector>

// CEncryptImageToolDlg 对话框
class CEncryptImageToolDlg : public CDialogEx
{
// 构造
public:
	CEncryptImageToolDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_ENCRYPTIMAGETOOL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButfilesel();	// 选择加密文件
	afx_msg void OnBnClickedButfileout();	// 选择输出文件
	afx_msg void OnBnClickedButenstart();	// 开始加密
	afx_msg void OnBnClickedButtonReadKey();// 读取秘钥
	afx_msg void OnBnClickedButzip();		// 对PNG图片压缩
public:

	// 加密过程中不能操作
	void SetBtnState(bool enabel);

	// 读取秘钥
	void ReadImageKey();

	// 压缩文件
	int ImageZipPng(const std::string filename, int minQua, int maxQua);

	// 检查文件路径是否都设置
	void CheckFilePath();
	void CopyAllFile();

	// 获得选择文件
	void UpdateSelFile();

private:
	std::string m_selFilePath;		// 原始文件路径
	std::string m_zipFilePath;		// 压缩文件路径
	std::string m_selFileOutPath;	// 加密后输出文件路径
	std::string m_exePath;			// 当前程序路径
	aes_key m_key;				// 加密秘钥

	std::vector< std::string > m_vecPngFiles;		// 原始图片列表，未加密
	std::vector< std::string > m_vecAllFiles;		// 原始图片列表，未加密
	std::vector< std::string > m_vecZipPngFiles;		// 压缩后图片列表，待加密

	CEdit*      m_pMinEdit;
	CEdit*      m_pMaxEdit;
	CEdit*      m_pSelFileEdit;
public:
	afx_msg void OnEnKillfocusEditmaxbox();
	afx_msg void OnEnKillfocusEditminbox();
	afx_msg void OnEnKillfocusEdit();
};
