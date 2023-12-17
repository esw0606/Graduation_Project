
// ImageProcessingView.cpp: CImageProcessingView 클래스의 구현
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "ImageProcessing.h"
#endif

#include "ImageProcessingDoc.h"
#include "ImageProcessingView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CImageProcessingView

IMPLEMENT_DYNCREATE(CImageProcessingView, CView)

BEGIN_MESSAGE_MAP(CImageProcessingView, CView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_COMMAND(IDM_EQUAL_IMAGE, &CImageProcessingView::OnEqualImage)
	ON_COMMAND(IDM_BW_127_IMAGE, &CImageProcessingView::OnBw127Image)
	ON_COMMAND(IDM_BW_INPUT_IMAGE, &CImageProcessingView::OnBwInputImage)
	ON_COMMAND(IDM_GRAY_IMAGE, &CImageProcessingView::OnGrayImage)
	ON_COMMAND(IDM_NGTV_IMAGE, &CImageProcessingView::OnNgtvImage)
	ON_COMMAND(IDM_LIGHT_PM_IMAGE, &CImageProcessingView::OnLightPmImage)
	ON_COMMAND(IDM_LIGHT_MD_IMAGE, &CImageProcessingView::OnLightMdImage)
	ON_COMMAND(IDM_PARA_CAP_IMAGE, &CImageProcessingView::OnParaCapImage)
	ON_COMMAND(IDM_PARA_CUP_IMAGE, &CImageProcessingView::OnParaCupImage)
	ON_COMMAND(IDM_GAMMA_IMAGE, &CImageProcessingView::OnGammaImage)
	ON_COMMAND(IDM_ZMIN_NRML_IMAGE, &CImageProcessingView::OnZminNrmlImage)
	ON_COMMAND(IDM_ZMIN_NH_IMAGE, &CImageProcessingView::OnZminNhImage)
	ON_COMMAND(IDM_ZMIN_YS_IMAGE, &CImageProcessingView::OnZminYsImage)
	ON_COMMAND(IDM_ZOUT_IMAGE, &CImageProcessingView::OnZoutImage)
	ON_COMMAND(IDM_TURN_IMAGE, &CImageProcessingView::OnTurnImage)
	ON_COMMAND(IDM_MOVE_IMAGE, &CImageProcessingView::OnMoveImage)
	ON_COMMAND(IDM_MIRROR_UD_IMAGE, &CImageProcessingView::OnMirrorUdImage)
	ON_COMMAND(IDM_MIRROR_LR_IMAGE, &CImageProcessingView::OnMirrorLrImage)
	ON_COMMAND(IDM_EMBOS_IMAGE, &CImageProcessingView::OnEmbosImage)
	ON_COMMAND(IDM_BLR_AVG_IMAGE, &CImageProcessingView::OnBlrAvgImage)
	ON_COMMAND(IDM_BLR_GAUS_IMAGE, &CImageProcessingView::OnBlrGausImage)
	ON_COMMAND(IDM_HPF_IMAGE, &CImageProcessingView::OnHpfImage)
	ON_COMMAND(IDM_LPF_IMAGE, &CImageProcessingView::OnLpfImage)
	ON_COMMAND(IDM_CALC_USA_IMAGE, &CImageProcessingView::OnCalcUsaImage)
	ON_COMMAND(IDM_CALC_MNS_IMAGE, &CImageProcessingView::OnCalcMnsImage)
	ON_COMMAND(IDM_RBRTS_IMAGE, &CImageProcessingView::OnRbrtsImage)
	ON_COMMAND(IDM_PRW_IMAGE, &CImageProcessingView::OnPrwImage)
	ON_COMMAND(IDM_SOBL_IMAGE, &CImageProcessingView::OnSoblImage)
	ON_COMMAND(IDM_LAPLA_IMAGE, &CImageProcessingView::OnLaplaImage)
	ON_COMMAND(IDM_LOG_IMAGE, &CImageProcessingView::OnLogImage)
	ON_COMMAND(IDM_DOG_IMAGE, &CImageProcessingView::OnDogImage)
	ON_COMMAND(IDM_HIST_CHART_IMAGE, &CImageProcessingView::OnHistChartImage)
	ON_COMMAND(IDM_HIST_STCH_IMAGE, &CImageProcessingView::OnHistStchImage)
	ON_COMMAND(IDM_HIST_EQUAL_IMAGE, &CImageProcessingView::OnHistEqualImage)
	ON_COMMAND(IDM_BW_AVG_IMAGE, &CImageProcessingView::OnBwAvgImage)

END_MESSAGE_MAP()

// CImageProcessingView 생성/소멸

CImageProcessingView::CImageProcessingView() noexcept
{
	// TODO: 여기에 생성 코드를 추가합니다.

}

CImageProcessingView::~CImageProcessingView()
{
}

BOOL CImageProcessingView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// CImageProcessingView 그리기

void CImageProcessingView::OnDraw(CDC* pDC)
{
	CImageProcessingDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	////빠른 OnDraw
	int i, k;
	unsigned char RR, GG, BB;

	// 메모리 DC 선언 (DC : 출력)
	CDC memDC; // CDC 클래스 가져오기
	CBitmap* pOldBitmap, bitmap;

	// * Input 출력 (원본)
	// pDC와 연동가능한 메모리 DC 선언
	memDC.CreateCompatibleDC(pDC);

	// pDC와 호환되는 bitmap 생성 (BMP)
	bitmap.CreateCompatibleBitmap(pDC, pDoc->m_inW, pDoc->m_inH);
	pOldBitmap = memDC.SelectObject(&bitmap);
	// 패턴방식 초기화 
	memDC.PatBlt(0, 0, pDoc->m_inW, pDoc->m_inH, SRCCOPY); 
	// WHITENESS : 흰색으로 초기화 , SRCCOPY : 원래 비트맵 그대로 출력
	
	// 메모리에 DC 가져오기
	for (i = 0; i < pDoc->m_inH; i++) {
		for (k = 0; k < pDoc->m_inW; k++) {
			RR = pDoc->m_InputImageR[i][k];
			GG = pDoc->m_InputImageG[i][k];
			BB = pDoc->m_InputImageB[i][k];
			memDC.SetPixel(k, i, RGB(RR, GG, BB));
		}
	}
	// 출력 : 메모리 DC를 화면 DC에 고속 복사
	/// BitBlt : 메모리 DC비트맵을 현재 DC로 고속복사.
	/// 두 화면다 호환되거나, 한쪽이 흑백이여야한다.
	pDC->BitBlt(5, 5, pDoc->m_inW, pDoc->m_inH, &memDC, 0, 0, SRCCOPY);
	// WHITENESS : 흰색으로 초기화 , SRCCOPY : 원래 비트맵 그대로 출력
	// -> 이 영역에서 	Invalidate() 부분과 연관이 있다.
	///	에러가 뜬다면 : Invalidate(FALSE)를 TRUE로 변경
	//// (어차피 "SRCCOPY" 때문에Invalidate(FALSE)처리와 같다)
	
	// 메모리 Free
	memDC.SelectObject(pOldBitmap);
	memDC.DeleteDC();
	bitmap.DeleteObject();

	//// * Output 출력
	//// pDC와 호환되는 메모리DC 생성
	//memDC.CreateCompatibleDC(pDC);
	///// pDC와 호환되는 bitmap 생성
	//bitmap.CreateCompatibleBitmap(pDC, pDoc->m_outW, pDoc->m_outH);
	//pOldBitmap = memDC.SelectObject(&bitmap);
	//// 패턴방식 초기화 
	//memDC.PatBlt(pDoc->m_outW, pDoc->m_outH, 0, 0, SRCCOPY);
	//
	////메모리 DC에 그리기
	//for (i = 0; i < pDoc->m_outH; i++) {
	//	for (k = 0; k < pDoc->m_outW; k++) {
	//		RR = pDoc->m_OutputImageR[i][k];
	//		GG = pDoc->m_OutputImageG[i][k];
	//		BB = pDoc->m_OutputImageB[i][k];
	//		memDC.SetPixel(k, i, RGB(RR, GG, BB));
	//	}
	//}
	//// 출력 : 메모리 DC를 화면 DC에 고속 복사 
	//pDC->BitBlt(pDoc->m_inW + 10, 5, pDoc->m_outW, pDoc->m_outH, &memDC, 0, 0, SRCCOPY);

	//// Free
	//memDC.SelectObject(pOldBitmap);
	//memDC.DeleteDC();
	//bitmap.DeleteObject();
}


// CImageProcessingView 인쇄

BOOL CImageProcessingView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CImageProcessingView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CImageProcessingView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}


// CImageProcessingView 진단

#ifdef _DEBUG
void CImageProcessingView::AssertValid() const
{
	CView::AssertValid();
}

void CImageProcessingView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CImageProcessingDoc* CImageProcessingView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CImageProcessingDoc)));
	return (CImageProcessingDoc*)m_pDocument;
}
#endif //_DEBUG


// CImageProcessingView 메시지 처리기


void CImageProcessingView::OnEqualImage()
{
	CImageProcessingDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);

    if (!pDoc)
        return;

    pDoc->OnEqualImage();
    Invalidate(FALSE);
}


void CImageProcessingView::OnBw127Image()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CImageProcessingDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	pDoc->OnBw127Image();
	Invalidate(FALSE);
}


void CImageProcessingView::OnBwInputImage()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CImageProcessingDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	pDoc->OnBwInputImage();
	Invalidate(FALSE);
}

void CImageProcessingView::OnBwAvgImage()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CImageProcessingDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	pDoc->OnBwAvgImage();
	Invalidate(FALSE);
}

void CImageProcessingView::OnGrayImage()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CImageProcessingDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	pDoc->OnGrayImage();
	Invalidate(FALSE);
}


void CImageProcessingView::OnNgtvImage()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CImageProcessingDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	pDoc->OnNgtvImage();
	Invalidate(FALSE);
}


void CImageProcessingView::OnLightPmImage()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CImageProcessingDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	pDoc->OnLightPmImage();
	Invalidate(FALSE);
}


void CImageProcessingView::OnLightMdImage()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CImageProcessingDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	pDoc->OnLightMdImage();
	Invalidate(FALSE);
}


void CImageProcessingView::OnParaCapImage()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CImageProcessingDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	pDoc->OnParaCapImage();
	Invalidate(FALSE);
}


void CImageProcessingView::OnParaCupImage()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CImageProcessingDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	pDoc->OnParaCupImage();
	Invalidate(FALSE);
}


void CImageProcessingView::OnGammaImage()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CImageProcessingDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	pDoc->OnGammaImage();
	Invalidate(FALSE);
}


void CImageProcessingView::OnZminNrmlImage()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CImageProcessingDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	pDoc->OnZminNrmlImage();
	Invalidate(FALSE);
}


void CImageProcessingView::OnZminNhImage()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CImageProcessingDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	pDoc->OnZminNhImage();
	Invalidate(FALSE);
}


void CImageProcessingView::OnZminYsImage()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CImageProcessingDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	pDoc->OnZminYsImage();
	Invalidate(FALSE);
}


void CImageProcessingView::OnZoutImage()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CImageProcessingDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	pDoc->OnZoutImage();
	Invalidate(FALSE);
}


void CImageProcessingView::OnTurnImage()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CImageProcessingDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	pDoc->OnTurnImage();
	Invalidate(TRUE); 
}


void CImageProcessingView::OnMoveImage()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CImageProcessingDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	pDoc->OnMoveImage();
	Invalidate(FALSE);
}


void CImageProcessingView::OnMirrorUdImage()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CImageProcessingDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	pDoc->OnMirrorUdImage();
	Invalidate(FALSE);
}


void CImageProcessingView::OnMirrorLrImage()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CImageProcessingDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	pDoc->OnMirrorLrImage();
	Invalidate(FALSE);
}


void CImageProcessingView::OnEmbosImage()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CImageProcessingDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	pDoc->OnEmbosImage();
	Invalidate(FALSE);
}


void CImageProcessingView::OnBlrAvgImage()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CImageProcessingDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	pDoc->OnBlrAvgImage();
	Invalidate(FALSE);
}


void CImageProcessingView::OnBlrGausImage()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CImageProcessingDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	pDoc->OnBlrGausImage();
	Invalidate(FALSE);
}


void CImageProcessingView::OnHpfImage()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CImageProcessingDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	pDoc->OnHpfImage();
	Invalidate(FALSE);
}


void CImageProcessingView::OnLpfImage()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CImageProcessingDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	pDoc->OnLpfImage();
	Invalidate(FALSE);
}


void CImageProcessingView::OnCalcUsaImage()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CImageProcessingDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	pDoc->OnCalcUsaImage();
	Invalidate(FALSE);
}


void CImageProcessingView::OnCalcMnsImage()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CImageProcessingDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	pDoc->OnCalcMnsImage();
	Invalidate(FALSE);
}


void CImageProcessingView::OnRbrtsImage()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CImageProcessingDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	pDoc->OnRbrtsImage();
	Invalidate(FALSE);
}


void CImageProcessingView::OnPrwImage()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CImageProcessingDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	pDoc->OnPrwImage();
	Invalidate(FALSE);
}


void CImageProcessingView::OnSoblImage()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CImageProcessingDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	pDoc->OnSoblImage();
	Invalidate(FALSE);
}


void CImageProcessingView::OnLaplaImage()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CImageProcessingDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	pDoc->OnLaplaImage();
	Invalidate(FALSE);
}


void CImageProcessingView::OnLogImage()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CImageProcessingDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	pDoc->OnLogImage();
	Invalidate(FALSE);
}


void CImageProcessingView::OnDogImage()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CImageProcessingDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	pDoc->OnDogImage();
	Invalidate(FALSE);
}


void CImageProcessingView::OnHistChartImage()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CImageProcessingDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	pDoc->OnHistChartImage();
	Invalidate(FALSE);
}


void CImageProcessingView::OnHistStchImage()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CImageProcessingDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	pDoc->OnHistStchImage();
	Invalidate(FALSE);
}


void CImageProcessingView::OnHistEqualImage()
{
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	CImageProcessingDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	pDoc->OnHistEqualImage();
	Invalidate(FALSE);
}





