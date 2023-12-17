// CModelessDlg.cpp: 구현 파일
//

#include "pch.h"
#include "ImageProcessing.h"
#include "CModelessDlg.h"
#include "afxdialogex.h"

#include "ImageProcessingView.h"
#include "ImageProcessingDoc.h"


// CModelessDlg 대화 상자

IMPLEMENT_DYNAMIC(CModelessDlg, CDialogEx)

CModelessDlg::CModelessDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG1, pParent)
{

}

CModelessDlg::~CModelessDlg()
{
}

void CModelessDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CModelessDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
END_MESSAGE_MAP()


// CModelessDlg 메시지 처리기


BOOL CModelessDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	CRect rect;    GetClientRect(&rect);
	CPoint pos;    
	pos.x = GetSystemMetrics(SM_CXSCREEN) / 2.0f - rect.Width() / 2.0f;    
	pos.y = GetSystemMetrics(SM_CYSCREEN) / 2.0f - rect.Height() / 2.0f;    
	SetWindowPos(NULL, pos.x, pos.y, 0, 0, SWP_NOSIZE);



	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CModelessDlg::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	DestroyWindow();
	CDialogEx::OnClose();
}

void CModelessDlg::CallViewDraw()
{


}

void CModelessDlg::SetRGB(unsigned char** R, unsigned char** G, unsigned char** B)
{
	m_R = R;
	m_G = G;
	m_B = B;
}

void CModelessDlg::SetHeightWidth(int i_height, int i_width, int o_height, int o_width)
{
	m_inH = i_height;
	m_inW = i_width;
	m_outH = o_height;
	m_outW = o_width;
}


void CModelessDlg::OnDraw(CDC* pDC)
{
	// 이미지를 그리는 코드는 그대로 유지
	int i, k;
	unsigned char RR, GG, BB;

	MoveWindow(800,500, m_outW + 30, m_outH + 50); 

	// 메모리 DC 선언 (DC : 출력)
	CDC memDC;
	CBitmap* pOldBitmap, bitmap;

	// * Output 출력
	memDC.CreateCompatibleDC(pDC);
	bitmap.CreateCompatibleBitmap(pDC, m_outW, m_outH);
	pOldBitmap = memDC.SelectObject(&bitmap);
	memDC.PatBlt(m_outW, m_outH, 0, 0, SRCCOPY);

	// 메모리 DC에 그리기
	for (i = 0; i < m_outH; i++) {
		for (k = 0; k < m_outW; k++) {
			RR = m_R[i][k];
			GG = m_G[i][k];
			BB = m_B[i][k];
			memDC.SetPixel(k, i, RGB(RR, GG, BB));
		}
	}

	// 출력: 메모리 DC를 화면 DC에 고속 복사
	pDC->BitBlt(5, 5, m_outW, m_outH, &memDC, 0, 0, SRCCOPY);

	// Free
	memDC.SelectObject(pOldBitmap);
	memDC.DeleteDC();
	bitmap.DeleteObject();
}


void CModelessDlg::OnPaint()
{
	CPaintDC dc(this);

	OnDraw(&dc);

	CDialog::OnPaint();
	

}

void CModelessDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

}

