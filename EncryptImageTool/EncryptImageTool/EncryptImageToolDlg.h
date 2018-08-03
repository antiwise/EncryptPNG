
// EncryptImageToolDlg.h : ͷ�ļ�
//

#pragma once
#include "AES.h"
#include <vector>

// CEncryptImageToolDlg �Ի���
class CEncryptImageToolDlg : public CDialogEx
{
// ����
public:
	CEncryptImageToolDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_ENCRYPTIMAGETOOL_DIALOG };

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
public:
	afx_msg void OnBnClickedButfilesel();	// ѡ������ļ�
	afx_msg void OnBnClickedButfileout();	// ѡ������ļ�
	afx_msg void OnBnClickedButenstart();	// ��ʼ����
	afx_msg void OnBnClickedButtonReadKey();// ��ȡ��Կ
	afx_msg void OnBnClickedButzip();		// ��PNGͼƬѹ��
public:

	// ���ܹ����в��ܲ���
	void SetBtnState(bool enabel);

	// ��ȡ��Կ
	void ReadImageKey();

	// ѹ���ļ�
	int ImageZipPng(const std::string filename, int minQua, int maxQua);

	// ����ļ�·���Ƿ�����
	void CheckFilePath();
	void CopyAllFile();

private:
	std::string m_selFilePath;		// ԭʼ�ļ�·��
	std::string m_zipFilePath;		// ѹ���ļ�·��
	std::string m_selFileOutPath;	// ���ܺ�����ļ�·��
	std::string m_exePath;			// ��ǰ����·��
	aes_key m_key;				// ������Կ

	std::vector< std::string > m_vecPngFiles;		// ԭʼͼƬ�б�δ����
	std::vector< std::string > m_vecAllFiles;		// ԭʼͼƬ�б�δ����
	std::vector< std::string > m_vecZipPngFiles;		// ѹ����ͼƬ�б�������

	CEdit*      m_pMinEdit;
	CEdit*      m_pMaxEdit;
public:
	
	
	afx_msg void OnEnKillfocusEditmaxbox();
	afx_msg void OnEnKillfocusEditminbox();
};
