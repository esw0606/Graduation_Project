#pragma once

#include "ImageProcessingDoc.h"

// CModelessDlg 대화 상자
CImageProcessingDoc;
class CModelessDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CModelessDlg)

public:
	CModelessDlg(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CModelessDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

private:
	int m_prevWidth;
	int m_prevHeight;

public:


	virtual BOOL OnInitDialog();
	afx_msg void OnClose();
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);

	unsigned char** m_R = NULL;
	unsigned char** m_G = NULL;
	unsigned char** m_B = NULL;
	int m_inH = 0;
	int m_inW = 0;
	int m_outH = 0;
	int m_outW = 0;

	void CallViewDraw();
	void SetRGB(unsigned char** R, unsigned char** G, unsigned char** B);
	void SetHeightWidth(int i_height, int i_width, int o_height, int o_width);

	void OnDraw(CDC* pDC);

};
