
// EncryptImageTool.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CEncryptImageToolApp: 
// �йش����ʵ�֣������ EncryptImageTool.cpp
//

class CEncryptImageToolApp : public CWinApp
{
public:
	CEncryptImageToolApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CEncryptImageToolApp theApp;