﻿
// ImageProcessingDoc.cpp: CImageProcessingDoc 클래스의 구현
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "ImageProcessing.h"
#endif

#include "ImageProcessingDoc.h"
#include "CInputDlg.h"
#include "CModelessDlg.h"

#include <propkey.h>
#include <vector>
#include <iostream>

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CImageProcessingDoc

IMPLEMENT_DYNCREATE(CImageProcessingDoc, CDocument)

BEGIN_MESSAGE_MAP(CImageProcessingDoc, CDocument)
END_MESSAGE_MAP()


// CImageProcessingDoc 생성/소멸

CImageProcessingDoc::CImageProcessingDoc() noexcept
{
	// TODO: 여기에 일회성 생성 코드를 추가합니다.

}

CImageProcessingDoc::~CImageProcessingDoc()
{
}

BOOL CImageProcessingDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 여기에 재초기화 코드를 추가합니다.
	// SDI 문서는 이 문서를 다시 사용합니다.

	return TRUE;
}

// CImageProcessingDoc serialization

void CImageProcessingDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 여기에 저장 코드를 추가합니다.
	}
	else
	{
		// TODO: 여기에 로딩 코드를 추가합니다.
	}
}

#ifdef SHARED_HANDLERS

// 축소판 그림을 지원합니다.
void CImageProcessingDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 문서의 데이터를 그리려면 이 코드를 수정하십시오.
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// 검색 처리기를 지원합니다.
void CImageProcessingDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 문서의 데이터에서 검색 콘텐츠를 설정합니다.
	// 콘텐츠 부분은 ";"로 구분되어야 합니다.

	// 예: strSearchContent = _T("point;rectangle;circle;ole object;");
	SetSearchContent(strSearchContent);
}

void CImageProcessingDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = nullptr;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != nullptr)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CImageProcessingDoc 진단

#ifdef _DEBUG
void CImageProcessingDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CImageProcessingDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CImageProcessingDoc 명령


BOOL CImageProcessingDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	// TODO:  여기에 특수화된 작성 코드를 추가합니다.
	// 입력 메모리 해제 (R G B)
	free2D(m_InputImageR, m_inH); 
	free2D(m_InputImageG, m_inH); 
	free2D(m_InputImageB, m_inH);

	// 컬러파일을 읽는 CImage 라이브러리 활용

	bitmap.Load(lpszPathName); // 입력 데이터 Load

	// 중요! 이미지의 높이, 폭 계산
	m_inH = bitmap.GetHeight();
	m_inW = bitmap.GetWidth();

	//메모리 할당
	m_InputImageR = malloc2D(m_inH, m_inW);
	m_InputImageG = malloc2D(m_inH, m_inW);
	m_InputImageB = malloc2D(m_inH, m_inW);

	COLORREF px; // 한점(R G B) _ (컬러레퍼런스) _ MFC라이브러리

	for (int i = 0; i < m_inH; i++) {
		for (int k = 0; k < m_inW; k++) {
			px = bitmap.GetPixel(k, i); // 픽셀 받아오기 (행, 열)
			m_InputImageR[i][k] = (unsigned char)GetRValue(px);
			m_InputImageG[i][k] = (unsigned char)GetGValue(px);
			m_InputImageB[i][k] = (unsigned char)GetBValue(px);
		}
	}

	return TRUE;
}


BOOL CImageProcessingDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (m_OutputImageR == NULL) {
		MessageBox(NULL, L"파일이 열리지 않았습니다.", L"", 0);
		return FALSE;
	}
	// .PNG 로 저장하는 관례적인 코드
	static TCHAR BASED_CODE szFilter[] = _T("PNG파일(*.PNG) | *.png | 모든파일(*.*) | *.* ||");
	CFileDialog SaveDlg(TRUE, _T("*.png"), _T(""), OFN_HIDEREADONLY, szFilter);
	if (SaveDlg.DoModal() != IDOK)
		return FALSE;
	CImage bitmap;
	bitmap.Create(m_outW, m_outH, 32); // 빈 파일 만들기(행, 열, 32); // CImage는 i랑 k가 반대임.
	COLORREF px;
	unsigned char RR, GG, BB;
	for (int i = 0; i < m_outW; i++) {
		for (int k = 0; k < m_outH; k++) {
			RR = m_OutputImageR[k][i];
			GG = m_OutputImageG[k][i];
			BB = m_OutputImageB[k][i];
			px = RGB(RR, GG, BB);
			bitmap.SetPixel(i, k, px);
		}
	}
	bitmap.Save(SaveDlg.GetPathName(), Gdiplus::ImageFormatPNG); // MFC에서는 PNG포멧을 지원한다.
	return TRUE; // 리턴 True해줘야 정상저장됨
}


void CImageProcessingDoc::OnCloseDocument()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	free2D(m_InputImageR, m_inH); // 임의로 만들어둔 2차원 free();
	free2D(m_OutputImageR, m_outH);
	free2D(m_InputImageG, m_inH); // 임의로 만들어둔 2차원 free();
	free2D(m_OutputImageG, m_outH);
	free2D(m_InputImageB, m_inH); // 임의로 만들어둔 2차원 free();
	free2D(m_OutputImageB, m_outH);
	CDocument::OnCloseDocument();
}

unsigned char** CImageProcessingDoc::malloc2D(int h, int w)
{
	// TODO: 여기에 구현 코드 추가.
	unsigned char** memory;
	memory = (unsigned char**)malloc(h * sizeof(unsigned char*));
	for (int i = 0; i < h; i++)
		memory[i] = (unsigned char*)malloc(w * sizeof(unsigned char));
	// 초기화(클리어)
	for (int i = 0; i < h; i++)
		for (int k = 0; k < w; k++)
			memory[i][k] = 255;
	return memory;
}

void CImageProcessingDoc::free2D(unsigned char** memory, int h)
{
	// TODO: 여기에 구현 코드 추가.
	if (memory != NULL) {
		for (int i = 0; i < h; i++)
			free(memory[i]);
		free(memory);
	}
}

double** CImageProcessingDoc::malloc2D_double(int h, int w)
{
	// TODO: 여기에 구현 코드 추가.
	double** mem;
	mem = (double**)malloc(h * sizeof(double*));
	for (int i = 0; i < h; i++)
		mem[i] = (double*)malloc(w * sizeof(double));

	// 초기화(Clear)
	for (int i = 0; i < h; i++) {
		for (int k = 0; k < w; k++) {
			mem[i][k] = 0;
		}
	}
	return mem;
}

void CImageProcessingDoc::free2D_double(double** memory, int h)
{
	// TODO: 여기에 구현 코드 추가.
	if (memory != NULL) {
		for (int i = 0; i < h; i++)
			free(memory[i]);
		free(memory);
	}
}

double CImageProcessingDoc::doubleABS(double x)
{
	// TODO: 여기에 구현 코드 추가.

	if (x >= 0) return x;
	else        return -x;
}

void CImageProcessingDoc::OnEqualImage()
{
	// TODO: 여기에 구현 코드 추가.
	if (m_InputImageR == NULL) {
		MessageBox(NULL, L"파일이 열리지 않았습니다.", L"", 0);
		return;
	}
	// 기존 출력 메모리 해제
	free2D(m_OutputImageR, m_outH);
	free2D(m_OutputImageG, m_outH);
	free2D(m_OutputImageB, m_outH);

	//(중요!) 출력 영상의 높이와 폭을 결정 --> 알고리즘에 따름
	m_outH = m_inH;
	m_outW = m_inW;

	// 출력 영상 메모리 할당
	m_OutputImageR = malloc2D(m_outH, m_outW);
	m_OutputImageG = malloc2D(m_outH, m_outW);
	m_OutputImageB = malloc2D(m_outH, m_outW);

	// 진짜 영상 처리 알고리즘
	for (int i = 0; i < m_inH; i++) {
		for (int k = 0; k < m_inW; k++) {
			m_OutputImageR[i][k] = m_InputImageR[i][k];
			m_OutputImageG[i][k] = m_InputImageG[i][k];
			m_OutputImageB[i][k] = m_InputImageB[i][k];
		}
	}
	CallDialog();
}



void CImageProcessingDoc::OnBw127Image()
{
	// TODO: 여기에 구현 코드 추가.
	if (m_InputImageR == NULL) {
		MessageBox(NULL, L"파일이 열리지 않았습니다.", L"", 0);
		return;
	}
	// 기존 출력 메모리 해제
	free2D(m_OutputImageR, m_outH);
	free2D(m_OutputImageG, m_outH);
	free2D(m_OutputImageB, m_outH);
	//(중요!) 출력 영상의 높이와 폭을 결정 --> 알고리즘에 따름
	m_outH = m_inH;
	m_outW = m_inW;
	// 출력 영상 메모리 할당
	m_OutputImageR = malloc2D(m_outH, m_outW);
	m_OutputImageG = malloc2D(m_outH, m_outW);
	m_OutputImageB = malloc2D(m_outH, m_outW);

	// 진짜 영상 처리 알고리즘
	for (int i = 0; i < m_inH; i++) {
		for (int k = 0; k < m_inW; k++) {
			int px = (m_InputImageR[i][k] + m_InputImageG[i][k] + m_InputImageB[i][k]) / 3;
			if (px > 127) { m_OutputImageR[i][k] = 255; m_OutputImageG[i][k] = 255; m_OutputImageB[i][k] = 255; }
			else { m_OutputImageR[i][k] = 0; m_OutputImageG[i][k] = 0; m_OutputImageB[i][k] = 0; }
		}
	}
	// //AddPoint("OnBw127Image", 0); // (String)함수이름, (dobule)입력값
	CallDialog();
}


void CImageProcessingDoc::OnBwInputImage()
{
	// TODO: 여기에 구현 코드 추가.
	if (m_InputImageR == NULL) {
		MessageBox(NULL, L"파일이 열리지 않았습니다.", L"", 0);
		return;
	}
	// 기존 출력 메모리 해제
	free2D(m_OutputImageR, m_outH);
	free2D(m_OutputImageG, m_outH);
	free2D(m_OutputImageB, m_outH);
	//(중요!) 출력 영상의 높이와 폭을 결정 --> 알고리즘에 따름
	m_outH = m_inH;
	m_outW = m_inW;
	// 출력 영상 메모리 할당
	m_OutputImageR = malloc2D(m_outH, m_outW);
	m_OutputImageG = malloc2D(m_outH, m_outW);
	m_OutputImageB = malloc2D(m_outH, m_outW);

	//Input1 으로 입력받기
	CInputDlg dlg;					// 인스턴스
	if (dlg.DoModal() != IDOK)		// 화면띄우기
		return;
	int value = (int)dlg.m_input1;	// 대화상자 내에 Value 받음	

	// 진짜 영상 처리 알고리즘
	for (int i = 0; i < m_inH; i++) {
		for (int k = 0; k < m_inW; k++) {
			int px = (m_InputImageR[i][k] + m_InputImageG[i][k] + m_InputImageB[i][k]) / 3;
			if (px > value) { m_OutputImageR[i][k] = 255; m_OutputImageG[i][k] = 255; m_OutputImageB[i][k] = 255; }
			else { m_OutputImageR[i][k] = 0; m_OutputImageG[i][k] = 0; m_OutputImageB[i][k] = 0; }
		}
	}
	// //AddPoint("OnBwInputImage", (double)value); // (String)함수이름, (dobule)입력값
	CallDialog();
}


void CImageProcessingDoc::OnBwAvgImage()
{
	// TODO: 여기에 구현 코드 추가.
	if (m_InputImageR == NULL) {
		MessageBox(NULL, L"파일이 열리지 않았습니다.", L"", 0);
		return;
	}
	// 기존 출력 메모리 해제
	free2D(m_OutputImageR, m_outH);
	free2D(m_OutputImageG, m_outH);
	free2D(m_OutputImageB, m_outH);
	//(중요!) 출력 영상의 높이와 폭을 결정 --> 알고리즘에 따름
	m_outH = m_inH;
	m_outW = m_inW;
	// 출력 영상 메모리 할당
	m_OutputImageR = malloc2D(m_outH, m_outW);
	m_OutputImageG = malloc2D(m_outH, m_outW);
	m_OutputImageB = malloc2D(m_outH, m_outW);

	long avg = 0;
	// 진짜 영상 처리 알고리즘
	for (int i = 0; i < m_inH; i++) {
		for (int k = 0; k < m_inW; k++) {
			avg += (long)(m_InputImageR[i][k] + m_InputImageG[i][k] + m_InputImageB[i][k]/3);
		}
	}
	avg /= (long)(m_inH * m_inW);
	for (int i = 0; i < m_inH; i++) {
		for (int k = 0; k < m_inW; k++) {
			int gray = m_InputImageR[i][k] + m_InputImageG[i][k] + m_InputImageB[i][k] / 3;
			if(avg < (long)gray) 
			{ m_OutputImageR[i][k] = m_OutputImageG[i][k] = m_OutputImageB[i][k] = 255; }
			else { m_OutputImageR[i][k] = m_OutputImageG[i][k] = m_OutputImageB[i][k] = 0; }
		}
	}
	CallDialog();
}


void CImageProcessingDoc::OnGrayImage()
{
	// TODO: 여기에 구현 코드 추가.
	if (m_InputImageR == NULL) {
		MessageBox(NULL, L"파일이 열리지 않았습니다.", L"", 0);
		return;
	}
	// 기존 출력 메모리 해제
	free2D(m_OutputImageR, m_outH);
	free2D(m_OutputImageG, m_outH);
	free2D(m_OutputImageB, m_outH);
	//(중요!) 출력 영상의 높이와 폭을 결정 --> 알고리즘에 따름
	m_outH = m_inH;
	m_outW = m_inW;
	// 출력 영상 메모리 할당
	m_OutputImageR = malloc2D(m_outH, m_outW);
	m_OutputImageG = malloc2D(m_outH, m_outW);
	m_OutputImageB = malloc2D(m_outH, m_outW);
	int avg = 0;
	// 진짜 영상 처리 알고리즘
	for (int i = 0; i < m_inH; i++) {
		for (int k = 0; k < m_inW; k++) {
			avg = (int)((m_InputImageR[i][k] + m_InputImageG[i][k] + m_InputImageB[i][k]) / 3);
			m_OutputImageR[i][k] = avg;
			m_OutputImageG[i][k] = avg;
			m_OutputImageB[i][k] = avg;
		}
	}
	////AddPoint("OnGrayImage", 0); // (String)함수이름, (dobule)입력값
	CallDialog();
}


void CImageProcessingDoc::OnNgtvImage()
{
	// TODO: 여기에 구현 코드 추가.
	if (m_InputImageR == NULL) {
		MessageBox(NULL, L"파일이 열리지 않았습니다.", L"", 0);
		return;
	}
	// 기존 출력 메모리 해제
	free2D(m_OutputImageR, m_outH);
	free2D(m_OutputImageG, m_outH);
	free2D(m_OutputImageB, m_outH);
	//(중요!) 출력 영상의 높이와 폭을 결정 --> 알고리즘에 따름
	m_outH = m_inH;
	m_outW = m_inW;
	// 출력 영상 메모리 할당
	m_OutputImageR = malloc2D(m_outH, m_outW);
	m_OutputImageG = malloc2D(m_outH, m_outW);
	m_OutputImageB = malloc2D(m_outH, m_outW);

	// 진짜 영상 처리 알고리즘
	for (int i = 0; i < m_inH; i++) {
		for (int k = 0; k < m_inW; k++) {
			m_OutputImageR[i][k] = 255 - m_InputImageR[i][k];
			m_OutputImageG[i][k] = 255 - m_InputImageR[i][k];
			m_OutputImageB[i][k] = 255 - m_InputImageR[i][k];
		}
	}
	CallDialog();
}


void CImageProcessingDoc::OnLightPmImage()
{
	// TODO: 여기에 구현 코드 추가.
	if (m_InputImageR == NULL) {
		MessageBox(NULL, L"파일이 열리지 않았습니다.", L"", 0);
		return;
	}
	// 기존 출력 메모리 해제
	free2D(m_OutputImageR, m_outH);
	free2D(m_OutputImageG, m_outH);
	free2D(m_OutputImageB, m_outH);
	//(중요!) 출력 영상의 높이와 폭을 결정 --> 알고리즘에 따름
	m_outH = m_inH;
	m_outW = m_inW;
	// 출력 영상 메모리 할당
	m_OutputImageR = malloc2D(m_outH, m_outW);
	m_OutputImageG = malloc2D(m_outH, m_outW);
	m_OutputImageB = malloc2D(m_outH, m_outW);

	//Input1 으로 입력받기
	CInputDlg dlg;					// 인스턴스
	if (dlg.DoModal() != IDOK)		// 화면띄우기
		return;
	int value = (int)dlg.m_input1;	// 대화상자 내에 Value 받음	

	// 진짜 영상 처리 알고리즘
	for (int i = 0; i < m_inH; i++) {
		for (int k = 0; k < m_inW; k++) {
			if (m_InputImageR[i][k] + value > 255) m_OutputImageR[i][k] = 255;
			else if (m_InputImageR[i][k] + value < 0) m_OutputImageR[i][k] = 0;
			else m_OutputImageR[i][k] = m_InputImageR[i][k] + value;
			if (m_InputImageG[i][k] + value > 255) m_OutputImageG[i][k] = 255;
			else if (m_InputImageG[i][k] + value < 0) m_OutputImageG[i][k] = 0;
			else m_OutputImageG[i][k] = m_InputImageG[i][k] + value;
			if (m_InputImageB[i][k] + value > 255) m_OutputImageB[i][k] = 255;
			else if (m_InputImageB[i][k] + value < 0) m_OutputImageB[i][k] = 0;
			else m_OutputImageB[i][k] = m_InputImageB[i][k] + value;
		}
	}
	CallDialog();
}


void CImageProcessingDoc::OnLightMdImage()
{
	// TODO: 여기에 구현 코드 추가.
	if (m_InputImageR == NULL) {
		MessageBox(NULL, L"파일이 열리지 않았습니다.", L"", 0);
		return;
	}
	// 기존 출력 메모리 해제
	free2D(m_OutputImageR, m_outH);
	free2D(m_OutputImageG, m_outH);
	free2D(m_OutputImageB, m_outH);
	//(중요!) 출력 영상의 높이와 폭을 결정 --> 알고리즘에 따름
	m_outH = m_inH;
	m_outW = m_inW;
	// 출력 영상 메모리 할당
	m_OutputImageR = malloc2D(m_outH, m_outW);
	m_OutputImageG = malloc2D(m_outH, m_outW);
	m_OutputImageB = malloc2D(m_outH, m_outW);

	//Input1 으로 입력받기
	CInputDlg dlg;					// 인스턴스
	if (dlg.DoModal() != IDOK)		// 화면띄우기
		return;
	int value = (int)dlg.m_input1;	// 대화상자 내에 Value 받음	

	// 진짜 영상 처리 알고리즘
	for (int i = 0; i < m_inH; i++) {
		for (int k = 0; k < m_inW; k++) {
			if (m_InputImageR[i][k] * value > 255) m_OutputImageR[i][k] = 255;
			else if (m_InputImageR[i][k] * value < 0) m_OutputImageR[i][k] = 0;
			else m_OutputImageR[i][k] = m_InputImageR[i][k] * value;
			if (m_InputImageG[i][k] * value > 255) m_OutputImageG[i][k] = 255;
			else if (m_InputImageG[i][k] * value < 0) m_OutputImageG[i][k] = 0;
			else m_OutputImageG[i][k] = m_InputImageG[i][k] * value;
			if (m_InputImageB[i][k] * value > 255) m_OutputImageB[i][k] = 255;
			else if (m_InputImageB[i][k] * value < 0) m_OutputImageB[i][k] = 0;
			else m_OutputImageB[i][k] = m_InputImageB[i][k] * value;
		}
	}
	CallDialog();
}


void CImageProcessingDoc::OnGammaImage()
{
	// TODO: 여기에 구현 코드 추가.
	if (m_InputImageR == NULL) {
		MessageBox(NULL, L"파일이 열리지 않았습니다.", L"", 0);
		return;
	}
	// 기존 출력 메모리 해제
	free2D(m_OutputImageR, m_outH);
	free2D(m_OutputImageG, m_outH);
	free2D(m_OutputImageB, m_outH);
	//(중요!) 출력 영상의 높이와 폭을 결정 --> 알고리즘에 따름
	m_outH = m_inH;
	m_outW = m_inW;
	// 출력 영상 메모리 할당
	m_OutputImageR = malloc2D(m_outH, m_outW);
	m_OutputImageG = malloc2D(m_outH, m_outW);
	m_OutputImageB = malloc2D(m_outH, m_outW);

	//Input1 으로 입력받기
	CInputDlg dlg;					// 인스턴스
	if (dlg.DoModal() != IDOK)		// 화면띄우기
		return;
	float value = (float)dlg.m_input1;	// 대화상자 내에 Value 받음	

	// 진짜 영상 처리 알고리즘
	//m_OutputImage[i][k] = (unsigned int)255.0 * pow(m_InputImage[i][k] / 255.0, value); // pow(a,b) ; a의 b 제곱
	for (int i = 0; i < m_inH; i++) {
		for (int k = 0; k < m_inW; k++) {
			m_OutputImageR[i][k] = (unsigned int)255.0 * pow(m_InputImageR[i][k] / 255.0, value);
			m_OutputImageG[i][k] = (unsigned int)255.0 * pow(m_InputImageG[i][k] / 255.0, value);
			m_OutputImageB[i][k] = (unsigned int)255.0 * pow(m_InputImageB[i][k] / 255.0, value);
		}
	}
	CallDialog();
}



void CImageProcessingDoc::OnParaCapImage()
{
	// TODO: 여기에 구현 코드 추가.
	if (m_InputImageR == NULL) {
		MessageBox(NULL, L"파일이 열리지 않았습니다.", L"", 0);
		return;
	}
	// 기존 출력 메모리 해제
	free2D(m_OutputImageR, m_outH);
	free2D(m_OutputImageG, m_outH);
	free2D(m_OutputImageB, m_outH);
	//(중요!) 출력 영상의 높이와 폭을 결정 --> 알고리즘에 따름
	m_outH = m_inH;
	m_outW = m_inW;
	// 출력 영상 메모리 할당
	m_OutputImageR = malloc2D(m_outH, m_outW);
	m_OutputImageG = malloc2D(m_outH, m_outW);
	m_OutputImageB = malloc2D(m_outH, m_outW);

	// 진짜 영상 처리 알고리즘
	//m_OutputImage[i][k] = (unsigned int)255.0 * pow(m_InputImage[i][k] / 255.0, value); // pow(a,b) ; a의 b 제곱
	for (int i = 0; i < m_inH; i++) {
		for (int k = 0; k < m_inW; k++) {
			double valueR = 255.0 - 255.0 * pow((m_InputImageR[i][k] / 128.0 - 1.0), 2); //밝은 곳 입체형 (CAP)
			if (valueR > 255.0) valueR = 255.0;
			else if (valueR < 0.0) valueR = 0.0;
			m_OutputImageR[i][k] = (unsigned)valueR;

			double valueG = 255.0 - 255.0 * pow((m_InputImageG[i][k] / 128.0 - 1.0), 2); //밝은 곳 입체형 (CAP)
			if (valueG > 255.0) valueG = 255.0;
			else if (valueG < 0.0) valueG = 0.0;
			m_OutputImageG[i][k] = (unsigned)valueG;

			double valueB = 255.0 - 255.0 * pow((m_InputImageB[i][k] / 128.0 - 1.0), 2); //밝은 곳 입체형 (CAP)
			if (valueB > 255.0) valueB = 255.0;
			else if (valueB < 0.0) valueB = 0.0;
			m_OutputImageB[i][k] = (unsigned)valueB;
		}
	}
	CallDialog();
}


void CImageProcessingDoc::OnParaCupImage()
{
	// TODO: 여기에 구현 코드 추가.
	if (m_InputImageR == NULL) {
		MessageBox(NULL, L"파일이 열리지 않았습니다.", L"", 0);
		return;
	}
	// 기존 출력 메모리 해제
	free2D(m_OutputImageR, m_outH);
	free2D(m_OutputImageG, m_outH);
	free2D(m_OutputImageB, m_outH);
	//(중요!) 출력 영상의 높이와 폭을 결정 --> 알고리즘에 따름
	m_outH = m_inH;
	m_outW = m_inW;
	// 출력 영상 메모리 할당
	m_OutputImageR = malloc2D(m_outH, m_outW);
	m_OutputImageG = malloc2D(m_outH, m_outW);
	m_OutputImageB = malloc2D(m_outH, m_outW);

	// 진짜 영상 처리 알고리즘
	//m_OutputImage[i][k] = (unsigned int)255.0 * pow(m_InputImage[i][k] / 255.0, value); // pow(a,b) ; a의 b 제곱
	for (int i = 0; i < m_inH; i++) {
		for (int k = 0; k < m_inW; k++) {
			double valueR = 255.0 * pow((m_InputImageR[i][k] / 128.0 - 1.0), 2); //밝은 곳 입체형 (CAP)
			if (valueR > 255.0) valueR = 255.0;
			else if (valueR < 0.0) valueR = 0.0;
			m_OutputImageR[i][k] = (unsigned)valueR;

			double valueG = 255.0 * pow((m_InputImageG[i][k] / 128.0 - 1.0), 2); //밝은 곳 입체형 (CAP)
			if (valueG > 255.0) valueG = 255.0;
			else if (valueG < 0.0) valueG = 0.0;
			m_OutputImageG[i][k] = (unsigned)valueG;

			double valueB = 255.0 * pow((m_InputImageB[i][k] / 128.0 - 1.0), 2); //밝은 곳 입체형 (CAP)
			if (valueB > 255.0) valueB = 255.0;
			else if (valueB < 0.0) valueB = 0.0;
			m_OutputImageB[i][k] = (unsigned)valueB;
		}
	}
	CallDialog();
}


void CImageProcessingDoc::OnZminNrmlImage()
{
	// TODO: 여기에 구현 코드 추가.
	if (m_InputImageR == NULL) {
		MessageBox(NULL, L"파일이 열리지 않았습니다.", L"", 0);
		return;
	}
	// 기존 출력 메모리 해제
	free2D(m_OutputImageR, m_outH);
	free2D(m_OutputImageG, m_outH);
	free2D(m_OutputImageB, m_outH);


	//Input1 으로 입력받기
	CInputDlg dlg;					// 인스턴스
	if (dlg.DoModal() != IDOK)		// 화면띄우기
		return;
	int scale = (int)dlg.m_input1;	// 대화상자 내에 Value 받음


	m_outH = m_inH * scale;
	m_outW = m_inW * scale;

	// 출력 영상 메모리 할당
	m_OutputImageR = malloc2D(m_outH, m_outW);
	m_OutputImageG = malloc2D(m_outH, m_outW);
	m_OutputImageB = malloc2D(m_outH, m_outW);

	for (int i = 0; i < m_inH; i++) {
		for (int k = 0; k < m_inW; k++) {
			m_OutputImageR[i * scale][k * scale] = m_InputImageR[i][k];
			m_OutputImageG[i * scale][k * scale] = m_InputImageG[i][k];
			m_OutputImageB[i * scale][k * scale] = m_InputImageB[i][k];
		}
	}
	CallDialog();
}
 

void CImageProcessingDoc::OnZminNhImage()
{
	// TODO: 여기에 구현 코드 추가.
	if (m_InputImageR == NULL) {
		MessageBox(NULL, L"파일이 열리지 않았습니다.", L"", 0);
		return;
	}
	// 기존 출력 메모리 해제
	free2D(m_OutputImageR, m_outH);
	free2D(m_OutputImageG, m_outH);
	free2D(m_OutputImageB, m_outH);


	//Input1 으로 입력받기
	CInputDlg dlg;					// 인스턴스
	if (dlg.DoModal() != IDOK)		// 화면띄우기
		return;
	int scale = (int)dlg.m_input1;	// 대화상자 내에 Value 받음


	m_outH = m_inH * scale;
	m_outW = m_inW * scale;

	// 출력 영상 메모리 할당
	m_OutputImageR = malloc2D(m_outH, m_outW);
	m_OutputImageG = malloc2D(m_outH, m_outW);
	m_OutputImageB = malloc2D(m_outH, m_outW);

	for (int i = 0; i < m_outH; i++) {
		for (int k = 0; k < m_outW; k++) {
			m_OutputImageR[i][k] = m_InputImageR[i / scale][k / scale];
			m_OutputImageG[i][k] = m_InputImageG[i / scale][k / scale];
			m_OutputImageB[i][k] = m_InputImageB[i / scale][k / scale];
		}
	}
	CallDialog();
}



void CImageProcessingDoc::OnZminYsImage()
{
	// TODO: 여기에 구현 코드 추가.
	if (m_InputImageR == NULL) {
		MessageBox(NULL, L"파일이 열리지 않았습니다.", L"", 0);
		return;
	}
	// 기존 출력 메모리 해제
	free2D(m_OutputImageR, m_outH);
	free2D(m_OutputImageG, m_outH);
	free2D(m_OutputImageB, m_outH);


	//Input1 으로 입력받기
	CInputDlg dlg;					// 인스턴스
	if (dlg.DoModal() != IDOK)		// 화면띄우기
		return;
	int scale = (int)dlg.m_input1;	// 대화상자 내에 Value 받음


	m_outH = m_inH * scale;
	m_outW = m_inW * scale;

	// Temp 메모리 할당
	double** tempImageR = malloc2D_double(m_inH, m_inW);
	double** tempImageG = malloc2D_double(m_inH, m_inW);
	double** tempImageB = malloc2D_double(m_inH, m_inW);
	// 출력 영상 메모리 할당
	m_OutputImageR = malloc2D(m_outH, m_outW);
	m_OutputImageG = malloc2D(m_outH, m_outW);
	m_OutputImageB = malloc2D(m_outH, m_outW);

	for (int i = 0; i < m_inH; i++) {
		for (int k = 0; k < m_inW; k++) {
			tempImageR[i][k] = (double)m_InputImageR[i][k];
			tempImageG[i][k] = (double)m_InputImageG[i][k];
			tempImageB[i][k] = (double)m_InputImageB[i][k];
		}
	}

	double r_H, r_W, s_H, s_W;
	int i_W, i_H;
	double C1R, C2R, C3R, C4R, C1G, C2G, C3G, C4G, C1B, C2B, C3B, C4B;
	int vR = 0; int vG = 0; int vB = 0;

	for (int i = 0; i < m_outH; i++) {
		for (int k = 0; k < m_outW; k++) {
			r_H = i / scale;
			r_W = k / scale;

			i_H = (int)floor(r_H); //  내림. celi = 올림
			i_W = (int)floor(r_W);

			s_H = r_H - i_H;
			s_W = r_W - i_W;

			if (i_H < 0 || i_H >= (m_inH - 1) || i_W < 0 || i_W >= (m_inW - 1)) {
				m_OutputImageR[i][k] = m_OutputImageG[i][k] = m_OutputImageB[i][k] = 255;
			}
			else
			{
				C1R = (double)tempImageR[i_H][i_W];           // A
				C1G = (double)tempImageG[i_H][i_W];           // A
				C1B = (double)tempImageB[i_H][i_W];           // A
				C2R = (double)tempImageR[i_H][i_W + 1];       // B
				C2G = (double)tempImageG[i_H][i_W + 1];       // B
				C2B = (double)tempImageB[i_H][i_W + 1];       // B
				C3R = (double)tempImageR[i_H + 1][i_W + 1];   // C
				C3G = (double)tempImageG[i_H + 1][i_W + 1];   // C
				C3B = (double)tempImageB[i_H + 1][i_W + 1];   // C
				C4R = (double)tempImageR[i_H + 1][i_W];       // D
				C4G = (double)tempImageG[i_H + 1][i_W];       // D
				C4B = (double)tempImageB[i_H + 1][i_W];       // D
			}
			vR = (unsigned char)(C1R * (1 - s_H) * (1 - s_W) + C2R * s_W * (1 - s_H)
				+ C3R * s_W * s_H + C4R * (1 - s_W) * s_H);
			vG = (unsigned char)(C1G * (1 - s_H) * (1 - s_W) + C2G * s_W * (1 - s_H)
				+ C3G * s_W * s_H + C4G * (1 - s_W) * s_H);
			vB = (unsigned char)(C1B * (1 - s_H) * (1 - s_W) + C2B * s_W * (1 - s_H)
				+ C3B * s_W * s_H + C4B * (1 - s_W) * s_H);

			m_OutputImageR[i][k] = vR;
			m_OutputImageG[i][k] = vG;
			m_OutputImageB[i][k] = vB;
		}
	}
	free2D_double(tempImageR, m_inH); free2D_double(tempImageG, m_inH); free2D_double(tempImageB, m_inH);
	CallDialog();
}



void CImageProcessingDoc::OnZoutImage()
{
	// TODO: 여기에 구현 코드 추가.
	if (m_InputImageR == NULL) {
		MessageBox(NULL, L"파일이 열리지 않았습니다.", L"", 0);
		return;
	}
	// 기존 출력 메모리 해제
	free2D(m_OutputImageR, m_outH);
	free2D(m_OutputImageG, m_outH);
	free2D(m_OutputImageB, m_outH);

	//Input1 으로 입력받기
	CInputDlg dlg;					// 인스턴스
	if (dlg.DoModal() != IDOK)		// 화면띄우기
		return;
	int scale = (int)dlg.m_input1;	// 대화상자 내에 Value 받음

	m_outH = m_inH / scale;
	m_outW = m_inW / scale;

	// 출력 영상 메모리 할당
	m_OutputImageR = malloc2D(m_outH, m_outW);
	m_OutputImageG = malloc2D(m_outH, m_outW);
	m_OutputImageB = malloc2D(m_outH, m_outW);

	for (int i = 0; i < m_outH; i++) {
		for (int k = 0; k < m_outW; k++) {
			m_OutputImageR[i][k] = m_InputImageR[i * scale][k * scale];
			m_OutputImageG[i][k] = m_InputImageG[i * scale][k * scale];
			m_OutputImageB[i][k] = m_InputImageB[i * scale][k * scale];
		}
	}
	CallDialog();
}



void CImageProcessingDoc::OnTurnImage()
{
	// TODO: 여기에 구현 코드 추가.
	if (m_InputImageR == NULL) {
		MessageBox(NULL, L"파일이 열리지 않았습니다.", L"", 0);
		return;
	}
	// 기존 출력 메모리 해제
	free2D(m_OutputImageR, m_outH);
	free2D(m_OutputImageG, m_outH);
	free2D(m_OutputImageB, m_outH);

	int angle; // 0 ~ 360

	//Input1 으로 입력받기
	CInputDlg dlg;					// 인스턴스
	if (dlg.DoModal() != IDOK)		// 화면띄우기
		return;
	angle = (int)dlg.m_input1;	// 대화상자 내에 Value 받음

	double PI = 3.14159265358979;
	double radian = angle * PI / 180.0;
	double tmp_radian = angle % 90 * PI / 180.0;
	double tmp_radian90 = (90 - angle % 90) * PI / 180.0;

	//(중요!) 출력 영상의 높이와 폭을 결정 --> 알고리즘에 따름
	int tmp_inSize = 0; // 원본의 가로세로 크기중에 가장 긴 값을 담는다.
	m_inH >= m_inW ? tmp_inSize = m_inH : tmp_inSize = m_inW;
	m_outH = (int)(tmp_inSize * cos(tmp_radian90) + tmp_inSize * cos(tmp_radian));
	m_outW = (int)(tmp_inSize * cos(tmp_radian) + tmp_inSize * cos(tmp_radian90));
	// 출력 영상 메모리 할당
	m_OutputImageR = malloc2D(m_outH, m_outW);
	m_OutputImageG = malloc2D(m_outH, m_outW);
	m_OutputImageB = malloc2D(m_outH, m_outW);

	// 임시 입력 영상 ---> 출력과 크기가 같게 하고, 입력 영상을 중앙에 두기.
	// Temp 메모리 할당
	unsigned char** tempImageR = malloc2D(m_outH, m_outW);
	unsigned char** tempImageG = malloc2D(m_outH, m_outW);
	unsigned char** tempImageB = malloc2D(m_outH, m_outW);
	int dx = (m_outH - m_inH) / 2;
	int dy = (m_outW - m_inW) / 2;
	for (int i = 0; i < m_inH; i++) {
		for (int k = 0; k < m_inW; k++) {
			tempImageR[i + dx][k + dy] = m_InputImageR[i][k];
			tempImageG[i + dx][k + dy] = m_InputImageG[i][k];
			tempImageB[i + dx][k + dy] = m_InputImageB[i][k];
		}
	}

	// 중앙에 회전하여 출력
	int cx = m_outH / 2;
	int cy = m_outW / 2;
	for (int i = 0; i < m_outH; i++) {
		for (int k = 0; k < m_outW; k++) {
			int oldI = (cos(-radian) * (i - cx) + sin(-radian) * (k - cy)) + cx;
			int oldK = (-sin(-radian) * (i - cx) + cos(-radian) * (k - cy)) + cy;
			if (((0 <= oldI) && (oldI < m_outH)) && ((0 <= oldK) && (oldK < m_outW))) {
				m_OutputImageR[i][k] = tempImageR[oldI][oldK];
				m_OutputImageG[i][k] = tempImageG[oldI][oldK];
				m_OutputImageB[i][k] = tempImageB[oldI][oldK];
			}
		}
	}
	free2D(tempImageR, m_outH); free2D(tempImageG, m_outH); free2D(tempImageB, m_outH);
	CallDialog();
}


//x y 가 같이 움직인다.
void CImageProcessingDoc::OnMoveImage()
{
	// TODO: 여기에 구현 코드 추가.
	if (m_InputImageR == NULL) {
		MessageBox(NULL, L"파일이 열리지 않았습니다.", L"", 0);
		return;
	}
	// 기존 출력 메모리 해제
	free2D(m_OutputImageR, m_outH);
	free2D(m_OutputImageG, m_outH);
	free2D(m_OutputImageB, m_outH);


	//Input1 으로 입력받기
	CInputDlg dlg;					// 인스턴스
	if (dlg.DoModal() != IDOK)		// 화면띄우기
		return;
	int inputx = (int)dlg.m_input1;	// 대화상자 내에 Value 받음
	int inputy = inputx;	// TODO : 여러값 받아서 하기

	if (m_inH < inputx) inputx = 0;
	if (m_inH < inputy) inputy = 0;

	m_outH = m_inH + inputx;
	m_outW = m_inW + inputy;
	m_OutputImageR = malloc2D(m_outH, m_outW);
	m_OutputImageG = malloc2D(m_outH, m_outW);
	m_OutputImageB = malloc2D(m_outH, m_outW);

	for (int i = 0; i < m_inH; i++) {
		for (int k = 0; k < m_inW; k++) {
			m_OutputImageR[i + inputx][k + inputy] = m_InputImageR[i][k];
			m_OutputImageG[i + inputx][k + inputy] = m_InputImageG[i][k];
			m_OutputImageB[i + inputx][k + inputy] = m_InputImageB[i][k];
		}
	}
	CallDialog();
}



void CImageProcessingDoc::OnMirrorUdImage()
{
	// TODO: 여기에 구현 코드 추가.
	if (m_InputImageR == NULL) {
		MessageBox(NULL, L"파일이 열리지 않았습니다.", L"", 0);
		return;
	}
	// 기존 출력 메모리 해제
	free2D(m_OutputImageR, m_outH);
	free2D(m_OutputImageG, m_outH);
	free2D(m_OutputImageB, m_outH);

	m_outH = m_inH;
	m_outW = m_inW;
	m_OutputImageR = malloc2D(m_outH, m_outW);
	m_OutputImageG = malloc2D(m_outH, m_outW);
	m_OutputImageB = malloc2D(m_outH, m_outW);

	for (int i = 0; i < m_inH; i++) {
		for (int k = 0; k < m_inW; k++) {
			m_OutputImageR[i][k] = m_InputImageR[m_inH - i - 1][k];
			m_OutputImageG[i][k] = m_InputImageG[m_inH - i - 1][k];
			m_OutputImageB[i][k] = m_InputImageB[m_inH - i - 1][k];
		}
	}
	CallDialog();
}


void CImageProcessingDoc::OnMirrorLrImage()
{
	// TODO: 여기에 구현 코드 추가.
	if (m_InputImageR == NULL) {
		MessageBox(NULL, L"파일이 열리지 않았습니다.", L"", 0);
		return;
	}
	// 기존 출력 메모리 해제
	free2D(m_OutputImageR, m_outH);
	free2D(m_OutputImageG, m_outH);
	free2D(m_OutputImageB, m_outH);

	m_outH = m_inH;
	m_outW = m_inW;
	m_OutputImageR = malloc2D(m_outH, m_outW);
	m_OutputImageG = malloc2D(m_outH, m_outW);
	m_OutputImageB = malloc2D(m_outH, m_outW);

	for (int i = 0; i < m_inH; i++) {
		for (int k = 0; k < m_inW; k++) {
			m_OutputImageR[i][k] = m_InputImageR[i][m_inW - k - 1];
			m_OutputImageG[i][k] = m_InputImageG[i][m_inW - k - 1];
			m_OutputImageB[i][k] = m_InputImageB[i][m_inW - k - 1];
		}
	}
	CallDialog();
}


void CImageProcessingDoc::OnEmbosImage()
{
	// TODO: 여기에 구현 코드 추가.
	if (m_InputImageR == NULL) {
		MessageBox(NULL, L"파일이 열리지 않았습니다.", L"", 0);
		return;
	}
	
	// TODO: 여기에 구현 코드 추가.
	free2D(m_OutputImageR, m_outH);
	free2D(m_OutputImageG, m_outH);
	free2D(m_OutputImageB, m_outH);
	m_outH = m_inH;
	m_outW = m_inW;
	m_OutputImageR = malloc2D(m_outH, m_outW);
	m_OutputImageG = malloc2D(m_outH, m_outW);
	m_OutputImageB = malloc2D(m_outH, m_outW);
	double mask[3][3] = {{	-1.,	0.,		0.},
						 {	0.,		0.,		0.},
						 {	0.,		0.,		1.}};
	// 임시 입력/출력 메모리 준비
	double** tmpInput = malloc2D_double(m_inH + 2, m_inW + 2); //엣지구간 마스킹을 간단하게 하기위해.(넓어진 영역들은 삭제될 예정)
	double** tmpOutput = malloc2D_double(m_outH, m_outW);
	for (int i = 0; i < m_inH; i++) {
		for (int k = 0; k < m_inW; k++) {
			tmpInput[i + 1][k + 1] = (unsigned char)((m_InputImageR[i][k]+ m_InputImageG[i][k] + m_InputImageB[i][k])/3); // 임시Input의 중심에 사진을 넣는중.
		}
	}

	// 진짜 영상 처리 알고리즘 ==> 회선연산
	double S; // 마스크9개 X 입력영상 9개의 합
	for (int i = 0; i < m_inH; i++) {
		for (int k = 0; k < m_inW; k++) {
			S = 0.0; // 누적 값은 초기화하는 것 권장
			for (int m = 0; m < 3; m++) {
				for (int n = 0; n < 3; n++) {
					S += tmpInput[i + m][k + n] * mask[m][n];
				}
			}
			tmpOutput[i][k] = S;
		}
	}

	// 마무리 (다듬기) --> 마스크합계가 0 또는 1 이면 127 더하기
	for (int i = 0; i < m_outH; i++) {
		for (int k = 0; k < m_outW; k++) {
			tmpOutput[i][k] += 127.0;
		}
	}
	// tmpOutput -> m_OutputImage // 오버플로우, 언더플로우 확인 후.
	for (int i = 0; i < m_outH; i++) {
		for (int k = 0; k < m_outW; k++) {
			double v = tmpOutput[i][k];
			if (v > 255.0) v = 255.0;
			if (v < 0.0) v = 0.0;
			m_OutputImageR[i][k]= m_OutputImageG[i][k] = m_OutputImageB[i][k] = (unsigned char)v;
		}
	}
	free2D_double(tmpInput, m_inH + 2);
	free2D_double(tmpOutput, m_outH);

	CallDialog();
}


void CImageProcessingDoc::OnBlrAvgImage()
{
	// TODO: 여기에 구현 코드 추가.
	if (m_InputImageR == NULL) {
		MessageBox(NULL, L"파일이 열리지 않았습니다.", L"", 0);
		return;
	}
	// TODO: 여기에 구현 코드 추가.
	// 기존 출력 메모리 해제
	free2D(m_OutputImageR, m_outH);
	free2D(m_OutputImageG, m_outH);
	free2D(m_OutputImageB, m_outH);
	m_outH = m_inH;
	m_outW = m_inW;
	m_OutputImageR = malloc2D(m_outH, m_outW);
	m_OutputImageG = malloc2D(m_outH, m_outW);
	m_OutputImageB = malloc2D(m_outH, m_outW);
	int input = 0;

	//Input1 으로 입력받기
	CInputDlg dlg;					// 인스턴스
	if (dlg.DoModal() != IDOK)		// 화면띄우기
		return;
	input = (int)dlg.m_input1;	// 대화상자 내에 Value 받음
	if (input % 2 == 0) {
		input += 1;
	}

	double** mask = malloc2D_double(input, input);
	for (int i = 0; i < input; i++) {
		for (int k = 0; k < input; k++) {
			mask[i][k] = (double)(1.0 / ((double)(input * input)));
		}
	}
	// 임시 입력/출력 메모리 준비
	double** tmpInputR = malloc2D_double(m_inH + (int)(input / 2) * 2, m_inW + (int)(input / 2) * 2);
	double** tmpInputG = malloc2D_double(m_inH + (int)(input / 2) * 2, m_inW + (int)(input / 2) * 2);
	double** tmpInputB = malloc2D_double(m_inH + (int)(input / 2) * 2, m_inW + (int)(input / 2) * 2);
	double** tmpOutputR = malloc2D_double(m_outH, m_outW);
	double** tmpOutputG = malloc2D_double(m_outH, m_outW);
	double** tmpOutputB = malloc2D_double(m_outH, m_outW);
	for (int i = 0; i < m_inH; i++) {
		for (int k = 0; k < m_inW; k++) {
			tmpInputR[i + (int)(input / 2)][k + (int)(input / 2)] = m_InputImageR[i][k];
			tmpInputG[i + (int)(input / 2)][k + (int)(input / 2)] = m_InputImageG[i][k];
			tmpInputB[i + (int)(input / 2)][k + (int)(input / 2)] = m_InputImageB[i][k];
		}
	}

	// 진짜 영상 처리 알고리즘 ==> 회선연산
	// 마스크9개 X 입력영상 9개의 합...
	double SR = 0.0;
	double SG = 0.0;
	double SB = 0.0;
	for (int i = 0; i < m_inH; i++) {
		for (int k = 0; k < m_inW; k++) {
			SR = 0.0; // 누적 값은 초기화하는 것 권장
			SG = 0.0; // 누적 값은 초기화하는 것 권장
			SB = 0.0; // 누적 값은 초기화하는 것 권장
			for (int m = 0; m < input; m++) {
				for (int n = 0; n < input; n++) {
					SR += tmpInputR[i + m][k + n] * mask[m][n];
					SG += tmpInputG[i + m][k + n] * mask[m][n];
					SB += tmpInputB[i + m][k + n] * mask[m][n];
				}
			}
			tmpOutputR[i][k] = SR;
			tmpOutputG[i][k] = SG;
			tmpOutputB[i][k] = SB;
		}
	}

	for (int i = 0; i < m_outH; i++) {
		for (int k = 0; k < m_outW; k++) {
			double vR = tmpOutputR[i][k];
			double vG = tmpOutputG[i][k];
			double vB = tmpOutputB[i][k];
			if (vR > 255.0) vR = 255.0;
			else if (vR < 0.0) vR = 0.0;
			m_OutputImageR[i][k] = (unsigned char)vR;
			if (vG > 255.0) vG = 255.0;
			else if (vG < 0.0) vG = 0.0;
			m_OutputImageG[i][k] = (unsigned char)vG;
			if (vB > 255.0) vB= 255.0;
			else if (vB < 0.0) vB = 0.0;
			m_OutputImageB[i][k] = (unsigned char)vB;
		}
	}
	free2D_double(tmpInputR, m_inH + (int)(input / 2) * 2);
	free2D_double(tmpInputG, m_inH + (int)(input / 2) * 2);
	free2D_double(tmpInputB, m_inH + (int)(input / 2) * 2);
	free2D_double(tmpOutputR, m_outH);
	free2D_double(tmpOutputG, m_outH);
	free2D_double(tmpOutputB, m_outH);

	CallDialog();
}



void CImageProcessingDoc::OnBlrGausImage()
{
	// TODO: 여기에 구현 코드 추가.
	if (m_InputImageR == NULL) {
		MessageBox(NULL, L"파일이 열리지 않았습니다.", L"", 0);
		return;
	}
	// TODO: 여기에 구현 코드 추가.
	// 기존 출력 메모리 해제
	free2D(m_OutputImageR, m_outH);
	free2D(m_OutputImageG, m_outH);
	free2D(m_OutputImageB, m_outH);
	m_outH = m_inH;
	m_outW = m_inW;
	m_OutputImageR = malloc2D(m_outH, m_outW);
	m_OutputImageG = malloc2D(m_outH, m_outW);
	m_OutputImageB = malloc2D(m_outH, m_outW);
	
	int input = 3; // 마스크 크기
	double mask[3][3] = { {1 / 16.,1 / 8.,1 / 16.},
					  {1 / 8.,1 / 4.,1 / 8.},
					  {1 / 16.,1 / 8.,1 / 16.} };

	// 임시 입력/출력 메모리 준비
	double** tmpInputR = malloc2D_double(m_inH + (int)(input / 2) * 2, m_inW + (int)(input / 2) * 2);
	double** tmpInputG = malloc2D_double(m_inH + (int)(input / 2) * 2, m_inW + (int)(input / 2) * 2);
	double** tmpInputB = malloc2D_double(m_inH + (int)(input / 2) * 2, m_inW + (int)(input / 2) * 2);
	double** tmpOutputR = malloc2D_double(m_outH, m_outW);
	double** tmpOutputG = malloc2D_double(m_outH, m_outW);
	double** tmpOutputB = malloc2D_double(m_outH, m_outW);
	for (int i = 0; i < m_inH; i++) {
		for (int k = 0; k < m_inW; k++) {
			tmpInputR[i + (int)(input / 2)][k + (int)(input / 2)] = m_InputImageR[i][k];
			tmpInputG[i + (int)(input / 2)][k + (int)(input / 2)] = m_InputImageG[i][k];
			tmpInputB[i + (int)(input / 2)][k + (int)(input / 2)] = m_InputImageB[i][k];
		}
	}

	// 진짜 영상 처리 알고리즘 ==> 회선연산
	// 마스크9개 X 입력영상 9개의 합...
	double SR = 0.0;
	double SG = 0.0;
	double SB = 0.0;
	for (int i = 0; i < m_inH; i++) {
		for (int k = 0; k < m_inW; k++) {
			SR = 0.0; // 누적 값은 초기화하는 것 권장
			SG = 0.0; // 누적 값은 초기화하는 것 권장
			SB = 0.0; // 누적 값은 초기화하는 것 권장
			for (int m = 0; m < input; m++) {
				for (int n = 0; n < input; n++) {
					SR += tmpInputR[i + m][k + n] * mask[m][n];
					SG += tmpInputG[i + m][k + n] * mask[m][n];
					SB += tmpInputB[i + m][k + n] * mask[m][n];
				}
			}
			tmpOutputR[i][k] = SR;
			tmpOutputG[i][k] = SG;
			tmpOutputB[i][k] = SB;
		}
	}

	for (int i = 0; i < m_outH; i++) {
		for (int k = 0; k < m_outW; k++) {
			double vR = tmpOutputR[i][k];
			double vG = tmpOutputG[i][k];
			double vB = tmpOutputB[i][k];
			if (vR > 255.0) vR = 255.0;
			else if (vR < 0.0) vR = 0.0;
			m_OutputImageR[i][k] = (unsigned char)vR;
			if (vG > 255.0) vG = 255.0;
			else if (vG < 0.0) vG = 0.0;
			m_OutputImageG[i][k] = (unsigned char)vG;
			if (vB > 255.0) vB = 255.0;
			else if (vB < 0.0) vB = 0.0;
			m_OutputImageB[i][k] = (unsigned char)vB;
		}
	}
	free2D_double(tmpInputR, m_inH + (int)(input / 2) * 2);
	free2D_double(tmpInputG, m_inH + (int)(input / 2) * 2);
	free2D_double(tmpInputB, m_inH + (int)(input / 2) * 2);
	free2D_double(tmpOutputR, m_outH);
	free2D_double(tmpOutputG, m_outH);
	free2D_double(tmpOutputB, m_outH);

	CallDialog();
}


void CImageProcessingDoc::OnHpfImage()
{
	// TODO: 여기에 구현 코드 추가.
	if (m_InputImageR == NULL) {
		MessageBox(NULL, L"파일이 열리지 않았습니다.", L"", 0);
		return;
	}
	// TODO: 여기에 구현 코드 추가.
	// 기존 출력 메모리 해제
	free2D(m_OutputImageR, m_outH);
	free2D(m_OutputImageG, m_outH);
	free2D(m_OutputImageB, m_outH);
	m_outH = m_inH;
	m_outW = m_inW;
	m_OutputImageR = malloc2D(m_outH, m_outW);
	m_OutputImageG = malloc2D(m_outH, m_outW);
	m_OutputImageB = malloc2D(m_outH, m_outW);

	int input = 3; // 마스크 크기
	double mask[3][3] = { {0.,-1.,0},
						  {-1., 5.,-1.},
						  {0,-1.,0.} };

	// 임시 입력/출력 메모리 준비
	double** tmpInputR = malloc2D_double(m_inH + (int)(input / 2) * 2, m_inW + (int)(input / 2) * 2);
	double** tmpInputG = malloc2D_double(m_inH + (int)(input / 2) * 2, m_inW + (int)(input / 2) * 2);
	double** tmpInputB = malloc2D_double(m_inH + (int)(input / 2) * 2, m_inW + (int)(input / 2) * 2);
	double** tmpOutputR = malloc2D_double(m_outH, m_outW);
	double** tmpOutputG = malloc2D_double(m_outH, m_outW);
	double** tmpOutputB = malloc2D_double(m_outH, m_outW);
	for (int i = 0; i < m_inH; i++) {
		for (int k = 0; k < m_inW; k++) {
			tmpInputR[i + (int)(input / 2)][k + (int)(input / 2)] = m_InputImageR[i][k];
			tmpInputG[i + (int)(input / 2)][k + (int)(input / 2)] = m_InputImageG[i][k];
			tmpInputB[i + (int)(input / 2)][k + (int)(input / 2)] = m_InputImageB[i][k];
		}
	}

	// 진짜 영상 처리 알고리즘 ==> 회선연산
	// 마스크9개 X 입력영상 9개의 합...
	double SR = 0.0;
	double SG = 0.0;
	double SB = 0.0;
	for (int i = 0; i < m_inH; i++) {
		for (int k = 0; k < m_inW; k++) {
			SR = 0.0; // 누적 값은 초기화하는 것 권장
			SG = 0.0; // 누적 값은 초기화하는 것 권장
			SB = 0.0; // 누적 값은 초기화하는 것 권장
			for (int m = 0; m < input; m++) {
				for (int n = 0; n < input; n++) {
					SR += tmpInputR[i + m][k + n] * mask[m][n];
					SG += tmpInputG[i + m][k + n] * mask[m][n];
					SB += tmpInputB[i + m][k + n] * mask[m][n];
				}
			}
			tmpOutputR[i][k] = SR;
			tmpOutputG[i][k] = SG;
			tmpOutputB[i][k] = SB;
		}
	}

	for (int i = 0; i < m_outH; i++) {
		for (int k = 0; k < m_outW; k++) {
			double vR = tmpOutputR[i][k];
			double vG = tmpOutputG[i][k];
			double vB = tmpOutputB[i][k];
			if (vR > 255.0) vR = 255.0;
			else if (vR < 0.0) vR = 0.0;
			m_OutputImageR[i][k] = (unsigned char)vR;
			if (vG > 255.0) vG = 255.0;
			else if (vG < 0.0) vG = 0.0;
			m_OutputImageG[i][k] = (unsigned char)vG;
			if (vB > 255.0) vB = 255.0;
			else if (vB < 0.0) vB = 0.0;
			m_OutputImageB[i][k] = (unsigned char)vB;
		}
	}
	free2D_double(tmpInputR, m_inH + (int)(input / 2) * 2);
	free2D_double(tmpInputG, m_inH + (int)(input / 2) * 2);
	free2D_double(tmpInputB, m_inH + (int)(input / 2) * 2);
	free2D_double(tmpOutputR, m_outH);
	free2D_double(tmpOutputG, m_outH);
	free2D_double(tmpOutputB, m_outH);

	CallDialog();
}


void CImageProcessingDoc::OnLpfImage()
{
	// TODO: 여기에 구현 코드 추가.
	if (m_InputImageR == NULL) {
		MessageBox(NULL, L"파일이 열리지 않았습니다.", L"", 0);
		return;
	}
	// TODO: 여기에 구현 코드 추가.
		// 기존 출력 메모리 해제
	free2D(m_OutputImageR, m_outH);
	free2D(m_OutputImageG, m_outH);
	free2D(m_OutputImageB, m_outH);
	m_outH = m_inH;
	m_outW = m_inW;
	m_OutputImageR = malloc2D(m_outH, m_outW);
	m_OutputImageG = malloc2D(m_outH, m_outW);
	m_OutputImageB = malloc2D(m_outH, m_outW);

	int input = 3; // 마스크 크기
	double mask[3][3] = { {-1. / 9.,1 / 9.,-1. / 9.},
						  {-1. / 9.,8. / 9.,-1. / 8.},
						  {-1. / 9.,1 / 9.,-1. / 9.} };

	// 임시 입력/출력 메모리 준비
	double** tmpInputR = malloc2D_double(m_inH + (int)(input / 2) * 2, m_inW + (int)(input / 2) * 2);
	double** tmpInputG = malloc2D_double(m_inH + (int)(input / 2) * 2, m_inW + (int)(input / 2) * 2);
	double** tmpInputB = malloc2D_double(m_inH + (int)(input / 2) * 2, m_inW + (int)(input / 2) * 2);
	double** tmpOutputR = malloc2D_double(m_outH, m_outW);
	double** tmpOutputG = malloc2D_double(m_outH, m_outW);
	double** tmpOutputB = malloc2D_double(m_outH, m_outW);
	for (int i = 0; i < m_inH; i++) {
		for (int k = 0; k < m_inW; k++) {
			tmpInputR[i + (int)(input / 2)][k + (int)(input / 2)] = m_InputImageR[i][k];
			tmpInputG[i + (int)(input / 2)][k + (int)(input / 2)] = m_InputImageG[i][k];
			tmpInputB[i + (int)(input / 2)][k + (int)(input / 2)] = m_InputImageB[i][k];
		}
	}

	// 진짜 영상 처리 알고리즘 ==> 회선연산
	// 마스크9개 X 입력영상 9개의 합...
	double SR = 0.0;
	double SG = 0.0;
	double SB = 0.0;
	for (int i = 0; i < m_inH; i++) {
		for (int k = 0; k < m_inW; k++) {
			SR = 0.0; // 누적 값은 초기화하는 것 권장
			SG = 0.0; // 누적 값은 초기화하는 것 권장
			SB = 0.0; // 누적 값은 초기화하는 것 권장
			for (int m = 0; m < input; m++) {
				for (int n = 0; n < input; n++) {
					SR += tmpInputR[i + m][k + n] * mask[m][n];
					SG += tmpInputG[i + m][k + n] * mask[m][n];
					SB += tmpInputB[i + m][k + n] * mask[m][n];
				}
			}
			tmpOutputR[i][k] = SR;
			tmpOutputG[i][k] = SG;
			tmpOutputB[i][k] = SB;
		}
	}

	for (int i = 0; i < m_outH; i++) {
		for (int k = 0; k < m_outW; k++) {
			double vR = tmpOutputR[i][k];
			double vG = tmpOutputG[i][k];
			double vB = tmpOutputB[i][k];
			if (vR > 255.0) vR = 255.0;
			else if (vR < 0.0) vR = 0.0;
			m_OutputImageR[i][k] = (unsigned char)vR;
			if (vG > 255.0) vG = 255.0;
			else if (vG < 0.0) vG = 0.0;
			m_OutputImageG[i][k] = (unsigned char)vG;
			if (vB > 255.0) vB = 255.0;
			else if (vB < 0.0) vB = 0.0;
			m_OutputImageB[i][k] = (unsigned char)vB;
		}
	}
	free2D_double(tmpInputR, m_inH + (int)(input / 2) * 2);
	free2D_double(tmpInputG, m_inH + (int)(input / 2) * 2);
	free2D_double(tmpInputB, m_inH + (int)(input / 2) * 2);
	free2D_double(tmpOutputR, m_outH);
	free2D_double(tmpOutputG, m_outH);
	free2D_double(tmpOutputB, m_outH);

	CallDialog();
}


void CImageProcessingDoc::OnCalcUsaImage()
{
	// TODO: 여기에 구현 코드 추가.
	if (m_InputImageR == NULL) {
		MessageBox(NULL, L"파일이 열리지 않았습니다.", L"", 0);
		return;
	}
	
	//컬러 Usa연산자
	// TODO: 여기에 구현 코드 추가.
		// 기존 출력 메모리 해제
	free2D(m_OutputImageR, m_outH);
	free2D(m_OutputImageG, m_outH);
	free2D(m_OutputImageB, m_outH);
	m_outH = m_inH;
	m_outW = m_inW;
	m_OutputImageR = malloc2D(m_outH, m_outW);
	m_OutputImageG = malloc2D(m_outH, m_outW);
	m_OutputImageB = malloc2D(m_outH, m_outW);

	int input = 3; // 마스크 크기
	// 임시 입력/출력 메모리 준비
	double** tmpInputR = malloc2D_double(m_inH + (int)(input / 2) * 2, m_inW + (int)(input / 2) * 2);
	double** tmpInputG = malloc2D_double(m_inH + (int)(input / 2) * 2, m_inW + (int)(input / 2) * 2);
	double** tmpInputB = malloc2D_double(m_inH + (int)(input / 2) * 2, m_inW + (int)(input / 2) * 2);
	double** tmpOutputR = malloc2D_double(m_outH, m_outW);
	double** tmpOutputG = malloc2D_double(m_outH, m_outW);
	double** tmpOutputB = malloc2D_double(m_outH, m_outW);
	for (int i = 0; i < m_inH; i++) {
		for (int k = 0; k < m_inW; k++) {
			tmpInputR[i + (int)(input / 2)][k + (int)(input / 2)] = m_InputImageR[i][k];
			tmpInputG[i + (int)(input / 2)][k + (int)(input / 2)] = m_InputImageG[i][k];
			tmpInputB[i + (int)(input / 2)][k + (int)(input / 2)] = m_InputImageB[i][k];
		}
	}
	double maxR = 0.0; // 블록이 이동할 때 마다 최대값
	double maxG = 0.0; // 블록이 이동할 때 마다 최대값
	double maxB = 0.0; // 블록이 이동할 때 마다 최대값
	for (int i = 0; i < m_inH; i++) {
		for (int k = 0; k < m_inW; k++) {
			maxR = 0.0; // 최대값도 초기화
			maxG = 0.0; // 최대값도 초기화
			maxB = 0.0; // 최대값도 초기화
			for (int m = 0; m < 3; m++) {
				for (int n = 0; n < 3; n++) {
					if (doubleABS(tmpInputR[i + 1][k + 1] -
						tmpInputR[i + m][k + n]) >= maxR)
						// 블록의 가운대값 - 블록의 주변 픽셀값의 절대값 중에서
						// 최대값을 찾는다.
						maxR = doubleABS(tmpInputR[i + 1][k + 1] - tmpInputR[i + m][k + n]);
					if (doubleABS(tmpInputG[i + 1][k + 1] -
						tmpInputG[i + m][k + n]) >= maxG)
						// 블록의 가운대값 - 블록의 주변 픽셀값의 절대값 중에서
						// 최대값을 찾는다.
						maxG = doubleABS(tmpInputG[i + 1][k + 1] - tmpInputG[i + m][k + n]);
					if (doubleABS(tmpInputB[i + 1][k + 1] -
						tmpInputB[i + m][k + n]) >= maxB)
						// 블록의 가운대값 - 블록의 주변 픽셀값의 절대값 중에서
						// 최대값을 찾는다.
						maxB = doubleABS(tmpInputB[i + 1][k + 1] - tmpInputB[i + m][k + n]);
				}
			}
			tmpOutputR[i][k] = maxR;
			tmpOutputG[i][k] = maxG;
			tmpOutputB[i][k] = maxB;
		}
	}

	for (int i = 0; i < m_outH; i++) {
		for (int k = 0; k < m_outW; k++) {
			double vR = tmpOutputR[i][k];
			double vG = tmpOutputG[i][k];
			double vB = tmpOutputB[i][k];
			if (vR > 255.0) vR = 255.0;
			else if (vR < 0.0) vR = 0.0;
			m_OutputImageR[i][k] = (unsigned char)vR;
			if (vG > 255.0) vG = 255.0;
			else if (vG < 0.0) vG = 0.0;
			m_OutputImageG[i][k] = (unsigned char)vG;
			if (vB > 255.0) vB = 255.0;
			else if (vB < 0.0) vB = 0.0;
			m_OutputImageB[i][k] = (unsigned char)vB;
		}
	}

	free2D_double(tmpInputR, m_inH + (int)(input / 2) * 2);
	free2D_double(tmpInputG, m_inH + (int)(input / 2) * 2);
	free2D_double(tmpInputB, m_inH + (int)(input / 2) * 2);
	free2D_double(tmpOutputR, m_outH);
	free2D_double(tmpOutputG, m_outH);
	free2D_double(tmpOutputB, m_outH);

	CallDialog();
}


void CImageProcessingDoc::OnCalcMnsImage()
{
	if (m_InputImageR == NULL) {
		MessageBox(NULL, L"파일이 열리지 않았습니다.", L"", 0);
		return;
	}
	 
	//컬러 Usa연산자 
	// TODO: 여기에 구현 코드 추가.
		// 기존 출력 메모리 해제
	free2D(m_OutputImageR, m_outH);
	free2D(m_OutputImageG, m_outH);
	free2D(m_OutputImageB, m_outH);
	m_outH = m_inH;
	m_outW = m_inW;
	m_OutputImageR = malloc2D(m_outH, m_outW);
	m_OutputImageG = malloc2D(m_outH, m_outW);
	m_OutputImageB = malloc2D(m_outH, m_outW);

	int input = 3; // 마스크 크기
	// 임시 입력/출력 메모리 준비
	double** tmpInputR = malloc2D_double(m_inH + (int)(input / 2) * 2, m_inW + (int)(input / 2) * 2);
	double** tmpInputG = malloc2D_double(m_inH + (int)(input / 2) * 2, m_inW + (int)(input / 2) * 2);
	double** tmpInputB = malloc2D_double(m_inH + (int)(input / 2) * 2, m_inW + (int)(input / 2) * 2);
	double** tmpOutputR = malloc2D_double(m_outH, m_outW);
	double** tmpOutputG = malloc2D_double(m_outH, m_outW);
	double** tmpOutputB = malloc2D_double(m_outH, m_outW);
	for (int i = 0; i < m_inH; i++) {
		for (int k = 0; k < m_inW; k++) {
			tmpInputR[i + (int)(input / 2)][k + (int)(input / 2)] = m_InputImageR[i][k];
			tmpInputG[i + (int)(input / 2)][k + (int)(input / 2)] = m_InputImageG[i][k];
			tmpInputB[i + (int)(input / 2)][k + (int)(input / 2)] = m_InputImageB[i][k];
		}
	}
	double maxR = 0.0; // 블록이 이동할 때 마다 최대값
	double maxG = 0.0; // 블록이 이동할 때 마다 최대값
	double maxB = 0.0; // 블록이 이동할 때 마다 최대값
	for (int i = 0; i < m_inH; i++) {
		for (int k = 0; k < m_inW; k++) {
			maxR = 0.0; // 최대값도 초기화
			maxG = 0.0; // 최대값도 초기화
			maxB = 0.0; // 최대값도 초기화
			for (int m = 0; m < 3; m++) {
				for (int n = 0; n < 3; n++) {
					// 블록의 가운대값 - 블록의 주변 픽셀값의 절대값 중에서 최대값을 찾는다.
					if (doubleABS(tmpInputR[i + 1][k + 1] - tmpInputR[i + m][k + n]) >= maxR)
						maxR = doubleABS(tmpInputR[i + 1][k + 1] - tmpInputR[i + m][k + n]);
					if (doubleABS(tmpInputG[i + 1][k + 1] - tmpInputG[i + m][k + n]) >= maxG)
						maxG = doubleABS(tmpInputG[i + 1][k + 1] - tmpInputG[i + m][k + n]);
					if (doubleABS(tmpInputB[i + 1][k + 1] - tmpInputB[i + m][k + n]) >= maxB)
						maxB = doubleABS(tmpInputB[i + 1][k + 1] - tmpInputB[i + m][k + n]);
				}
			}
			tmpOutputR[i][k] = maxR;
			tmpOutputG[i][k] = maxG;
			tmpOutputB[i][k] = maxB;
		}
	}
	double maskR[4] = { 0, };
	double maskG[4] = { 0, };
	double maskB[4] = { 0, };
	for (int i = 0; i < m_inH; i++) {
		for (int k = 0; k < m_inW; k++) {
			maxR = 0.0; // 최대값 초기화
			maskR[0] = doubleABS(tmpInputR[i][k] - tmpInputR[i + 2][k + 2]);
			maskR[1] = doubleABS(tmpInputR[i][k + 1] - tmpInputR[i + 2][k + 1]);
			maskR[2] = doubleABS(tmpInputR[i][k + 2] - tmpInputR[i + 2][k]);
			maskR[3] = doubleABS(tmpInputR[i + 1][k] - tmpInputR[i + 1][k + 2]);
			maxG = 0.0; // 최대값 초기화
			maskG[0] = doubleABS(tmpInputG[i][k] - tmpInputG[i + 2][k + 2]);
			maskG[1] = doubleABS(tmpInputG[i][k + 1] - tmpInputG[i + 2][k + 1]);
			maskG[2] = doubleABS(tmpInputG[i][k + 2] - tmpInputG[i + 2][k]);
			maskG[3] = doubleABS(tmpInputG[i + 1][k] - tmpInputG[i + 1][k + 2]);
			maxB = 0.0; // 최대값 초기화
			maskB[0] = doubleABS(tmpInputB[i][k] - tmpInputB[i + 2][k + 2]);
			maskB[1] = doubleABS(tmpInputB[i][k + 1] - tmpInputB[i + 2][k + 1]);
			maskB[2] = doubleABS(tmpInputB[i][k + 2] - tmpInputB[i + 2][k]);
			maskB[3] = doubleABS(tmpInputB[i + 1][k] - tmpInputB[i + 1][k + 2]);
			for (int m = 0; m < 4; m++) {
				if (maskR[m] >= maxR) maxR = maskR[m];
				if (maskG[m] >= maxG) maxR = maskG[m];
				if (maskB[m] >= maxB) maxR = maskB[m];
			}
			tmpOutputR[i][k] = maxR;
			tmpOutputG[i][k] = maxG;
			tmpOutputB[i][k] = maxB;
		}
	}

	for (int i = 0; i < m_outH; i++) {
		for (int k = 0; k < m_outW; k++) {
			double vR = tmpOutputR[i][k];
			double vG = tmpOutputG[i][k];
			double vB = tmpOutputB[i][k];
			if (vR > 255.0) vR = 255.0;
			else if (vR < 0.0) vR = 0.0;
			m_OutputImageR[i][k] = (unsigned char)vR;
			if (vG > 255.0) vG = 255.0;
			else if (vG < 0.0) vG = 0.0;
			m_OutputImageG[i][k] = (unsigned char)vG;
			if (vB > 255.0) vB = 255.0;
			else if (vB < 0.0) vB = 0.0;
			m_OutputImageB[i][k] = (unsigned char)vB;
		}
	}

	free2D_double(tmpInputR, m_inH + (int)(input / 2) * 2);
	free2D_double(tmpInputG, m_inH + (int)(input / 2) * 2);
	free2D_double(tmpInputB, m_inH + (int)(input / 2) * 2);
	free2D_double(tmpOutputR, m_outH);
	free2D_double(tmpOutputG, m_outH);
	free2D_double(tmpOutputB, m_outH);

	CallDialog();
}


void CImageProcessingDoc::OnRbrtsImage()
{
	//  컬러 처리
	// TODO: 여기에 구현 코드 추가.
	if (m_InputImageR == NULL) {
		MessageBox(NULL, L"파일이 열리지 않았습니다.", L"", 0);
		return;
	}
	// TODO: 여기에 구현 코드 추가.
		// 기존 출력 메모리 해제
	free2D(m_OutputImageR, m_outH);
	free2D(m_OutputImageG, m_outH);
	free2D(m_OutputImageB, m_outH);
	m_outH = m_inH;
	m_outW = m_inW;
	m_OutputImageR = malloc2D(m_outH, m_outW);
	m_OutputImageG = malloc2D(m_outH, m_outW);
	m_OutputImageB = malloc2D(m_outH, m_outW);

	int input = 3; // 마스크 크기
	double maskV[3][3]
		= { {-1,0,0} ,
			{0,1,0} ,
			{0,0,0} };
	double maskH[3][3]
		= { {0,0,-1} ,
			{0,1,0} ,
			{0,0,0} };

	// 임시 입력/출력 메모리 준비
	double** tmpInputR = malloc2D_double(m_inH + (int)(input / 2) * 2, m_inW + (int)(input / 2) * 2);
	double** tmpInputG = malloc2D_double(m_inH + (int)(input / 2) * 2, m_inW + (int)(input / 2) * 2);
	double** tmpInputB = malloc2D_double(m_inH + (int)(input / 2) * 2, m_inW + (int)(input / 2) * 2);
	double** tmpOutputR = malloc2D_double(m_outH, m_outW);
	double** tmpOutputG = malloc2D_double(m_outH, m_outW);
	double** tmpOutputB = malloc2D_double(m_outH, m_outW);
	double** tmpOutputR2 = malloc2D_double(m_outH, m_outW);
	double** tmpOutputG2 = malloc2D_double(m_outH, m_outW);
	double** tmpOutputB2 = malloc2D_double(m_outH, m_outW);
	for (int i = 0; i < m_inH; i++) {
		for (int k = 0; k < m_inW; k++) {
			tmpInputR[i + (int)(input / 2)][k + (int)(input / 2)] = m_InputImageR[i][k];
			tmpInputG[i + (int)(input / 2)][k + (int)(input / 2)] = m_InputImageG[i][k];
			tmpInputB[i + (int)(input / 2)][k + (int)(input / 2)] = m_InputImageB[i][k];
		}
	}

	// 진짜 영상 처리 알고리즘 ==> 회선연산
	// 마스크9개 X 입력영상 9개의 합...
	double SR = 0.0;
	double SG = 0.0;
	double SB = 0.0;
	for (int i = 0; i < m_inH; i++) {
		for (int k = 0; k < m_inW; k++) {
			SR = 0.0; // 누적 값은 초기화하는 것 권장
			SG = 0.0; // 누적 값은 초기화하는 것 권장
			SB = 0.0; // 누적 값은 초기화하는 것 권장
			for (int m = 0; m < input; m++) {
				for (int n = 0; n < input; n++) {
					SR += tmpInputR[i + m][k + n] * maskH[m][n];
					SG += tmpInputG[i + m][k + n] * maskH[m][n];
					SB += tmpInputB[i + m][k + n] * maskH[m][n];
				}
			}
			tmpOutputR[i][k] = SR;
			tmpOutputG[i][k] = SG;
			tmpOutputB[i][k] = SB;
		}
	}
	for (int i = 0; i < m_inH; i++) {
		for (int k = 0; k < m_inW; k++) {
			SR = 0.0; // 누적 값은 초기화하는 것 권장
			SG = 0.0; // 누적 값은 초기화하는 것 권장
			SB = 0.0; // 누적 값은 초기화하는 것 권장
			for (int m = 0; m < input; m++) {
				for (int n = 0; n < input; n++) {
					SR += tmpInputR[i + m][k + n] * maskV[m][n];
					SG += tmpInputG[i + m][k + n] * maskV[m][n];
					SB += tmpInputB[i + m][k + n] * maskV[m][n];
				}
			}
			tmpOutputR2[i][k] = SR;
			tmpOutputG2[i][k] = SG;
			tmpOutputB2[i][k] = SB;
		}
	}

	for (int i = 0; i < m_outH; i++) {
		for (int k = 0; k < m_outW; k++) {
			double vR1 = tmpOutputR[i][k];
			double vG1 = tmpOutputG[i][k];
			double vB1 = tmpOutputB[i][k];
			double vR2 = tmpOutputR2[i][k];
			double vG2 = tmpOutputG2[i][k];
			double vB2 = tmpOutputB2[i][k];
			double vR = vR1 + vR2;
			double vG = vG1 + vG2;
			double vB = vB1 + vB2;
			if (vR > 255.0) vR = 255.0;
			else if (vR < 0.0) vR = 0.0;
			m_OutputImageR[i][k] = (unsigned char)vR;
			if (vG > 255.0) vG = 255.0;
			else if (vG < 0.0) vG = 0.0;
			m_OutputImageG[i][k] = (unsigned char)vG;
			if (vB > 255.0) vB = 255.0;
			else if (vB < 0.0) vB = 0.0;
			m_OutputImageB[i][k] = (unsigned char)vB;
		}
	}
	free2D_double(tmpInputR, m_inH + (int)(input / 2) * 2);
	free2D_double(tmpInputG, m_inH + (int)(input / 2) * 2);
	free2D_double(tmpInputB, m_inH + (int)(input / 2) * 2);
	free2D_double(tmpOutputR, m_outH);
	free2D_double(tmpOutputG, m_outH);
	free2D_double(tmpOutputB, m_outH);
	free2D_double(tmpOutputR2, m_outH);
	free2D_double(tmpOutputG2, m_outH);
	free2D_double(tmpOutputB2, m_outH);
	
	CallDialog();
}


void CImageProcessingDoc::OnPrwImage()
{
	//  컬러 처리
	// TODO: 여기에 구현 코드 추가.
	if (m_InputImageR == NULL) {
		MessageBox(NULL, L"파일이 열리지 않았습니다.", L"", 0);
		return;
	}
	// TODO: 여기에 구현 코드 추가.
		// 기존 출력 메모리 해제
	free2D(m_OutputImageR, m_outH);
	free2D(m_OutputImageG, m_outH);
	free2D(m_OutputImageB, m_outH);
	m_outH = m_inH;
	m_outW = m_inW;
	m_OutputImageR = malloc2D(m_outH, m_outW);
	m_OutputImageG = malloc2D(m_outH, m_outW);
	m_OutputImageB = malloc2D(m_outH, m_outW);

	int input = 3; // 마스크 크기
	double maskV[3][3]
		= { {-1,0,1} ,
			{-1,0,1} ,
			{-1,0,1} };
	double maskH[3][3]
		= { {1,1,1} ,
			{0,0,0} ,
			{-1,-1,-1} };

	// 임시 입력/출력 메모리 준비
	double** tmpInputR = malloc2D_double(m_inH + (int)(input / 2) * 2, m_inW + (int)(input / 2) * 2);
	double** tmpInputG = malloc2D_double(m_inH + (int)(input / 2) * 2, m_inW + (int)(input / 2) * 2);
	double** tmpInputB = malloc2D_double(m_inH + (int)(input / 2) * 2, m_inW + (int)(input / 2) * 2);
	double** tmpOutputR = malloc2D_double(m_outH, m_outW);
	double** tmpOutputG = malloc2D_double(m_outH, m_outW);
	double** tmpOutputB = malloc2D_double(m_outH, m_outW);
	double** tmpOutputR2 = malloc2D_double(m_outH, m_outW);
	double** tmpOutputG2 = malloc2D_double(m_outH, m_outW);
	double** tmpOutputB2 = malloc2D_double(m_outH, m_outW);
	for (int i = 0; i < m_inH; i++) {
		for (int k = 0; k < m_inW; k++) {
			tmpInputR[i + (int)(input / 2)][k + (int)(input / 2)] = m_InputImageR[i][k];
			tmpInputG[i + (int)(input / 2)][k + (int)(input / 2)] = m_InputImageG[i][k];
			tmpInputB[i + (int)(input / 2)][k + (int)(input / 2)] = m_InputImageB[i][k];
		}
	}

	// 진짜 영상 처리 알고리즘 ==> 회선연산
	// 마스크9개 X 입력영상 9개의 합...
	double SR = 0.0;
	double SG = 0.0;
	double SB = 0.0;
	for (int i = 0; i < m_inH; i++) {
		for (int k = 0; k < m_inW; k++) {
			SR = 0.0; // 누적 값은 초기화하는 것 권장
			SG = 0.0; // 누적 값은 초기화하는 것 권장
			SB = 0.0; // 누적 값은 초기화하는 것 권장
			for (int m = 0; m < input; m++) {
				for (int n = 0; n < input; n++) {
					SR += tmpInputR[i + m][k + n] * maskH[m][n];
					SG += tmpInputG[i + m][k + n] * maskH[m][n];
					SB += tmpInputB[i + m][k + n] * maskH[m][n];
				}
			}
			tmpOutputR[i][k] = SR;
			tmpOutputG[i][k] = SG;
			tmpOutputB[i][k] = SB;
		}
	}
	for (int i = 0; i < m_inH; i++) {
		for (int k = 0; k < m_inW; k++) {
			SR = 0.0; // 누적 값은 초기화하는 것 권장
			SG = 0.0; // 누적 값은 초기화하는 것 권장
			SB = 0.0; // 누적 값은 초기화하는 것 권장
			for (int m = 0; m < input; m++) {
				for (int n = 0; n < input; n++) {
					SR += tmpInputR[i + m][k + n] * maskV[m][n];
					SG += tmpInputG[i + m][k + n] * maskV[m][n];
					SB += tmpInputB[i + m][k + n] * maskV[m][n];
				}
			}
			tmpOutputR2[i][k] = SR;
			tmpOutputG2[i][k] = SG;
			tmpOutputB2[i][k] = SB;
		}
	}

	for (int i = 0; i < m_outH; i++) {
		for (int k = 0; k < m_outW; k++) {
			double vR1 = tmpOutputR[i][k];
			double vG1 = tmpOutputG[i][k];
			double vB1 = tmpOutputB[i][k];
			double vR2 = tmpOutputR2[i][k];
			double vG2 = tmpOutputG2[i][k];
			double vB2 = tmpOutputB2[i][k];
			double vR = vR1 + vR2;
			double vG = vG1 + vG2;
			double vB = vB1 + vB2;
			if (vR > 255.0) vR = 255.0;
			else if (vR < 0.0) vR = 0.0;
			m_OutputImageR[i][k] = (unsigned char)vR;
			if (vG > 255.0) vG = 255.0;
			else if (vG < 0.0) vG = 0.0;
			m_OutputImageG[i][k] = (unsigned char)vG;
			if (vB > 255.0) vB = 255.0;
			else if (vB < 0.0) vB = 0.0;
			m_OutputImageB[i][k] = (unsigned char)vB;
		}
	}
	free2D_double(tmpInputR, m_inH + (int)(input / 2) * 2);
	free2D_double(tmpInputG, m_inH + (int)(input / 2) * 2);
	free2D_double(tmpInputB, m_inH + (int)(input / 2) * 2);
	free2D_double(tmpOutputR, m_outH);
	free2D_double(tmpOutputG, m_outH);
	free2D_double(tmpOutputB, m_outH);
	free2D_double(tmpOutputR2, m_outH);
	free2D_double(tmpOutputG2, m_outH);
	free2D_double(tmpOutputB2, m_outH);

	CallDialog();
}


void CImageProcessingDoc::OnSoblImage()
{
	//  컬러 처리
// TODO: 여기에 구현 코드 추가.
	if (m_InputImageR == NULL) {
		MessageBox(NULL, L"파일이 열리지 않았습니다.", L"", 0);
		return;
	}
	// TODO: 여기에 구현 코드 추가.
		// 기존 출력 메모리 해제
	free2D(m_OutputImageR, m_outH);
	free2D(m_OutputImageG, m_outH);
	free2D(m_OutputImageB, m_outH);
	m_outH = m_inH;
	m_outW = m_inW;
	m_OutputImageR = malloc2D(m_outH, m_outW);
	m_OutputImageG = malloc2D(m_outH, m_outW);
	m_OutputImageB = malloc2D(m_outH, m_outW);

	int input = 3; // 마스크 크기
	double maskV[3][3]
		= { {1,2,1} ,
			{0,0,0} ,
			{-1,-2,-1} };
	double maskH[3][3]
		= { {-1,0,1} ,
		   {-2,0,2} ,
		   {-1,0,1} };

	// 임시 입력/출력 메모리 준비
	double** tmpInputR = malloc2D_double(m_inH + (int)(input / 2) * 2, m_inW + (int)(input / 2) * 2);
	double** tmpInputG = malloc2D_double(m_inH + (int)(input / 2) * 2, m_inW + (int)(input / 2) * 2);
	double** tmpInputB = malloc2D_double(m_inH + (int)(input / 2) * 2, m_inW + (int)(input / 2) * 2);
	double** tmpOutputR = malloc2D_double(m_outH, m_outW);
	double** tmpOutputG = malloc2D_double(m_outH, m_outW);
	double** tmpOutputB = malloc2D_double(m_outH, m_outW);
	double** tmpOutputR2 = malloc2D_double(m_outH, m_outW);
	double** tmpOutputG2 = malloc2D_double(m_outH, m_outW);
	double** tmpOutputB2 = malloc2D_double(m_outH, m_outW);
	for (int i = 0; i < m_inH; i++) {
		for (int k = 0; k < m_inW; k++) {
			tmpInputR[i + (int)(input / 2)][k + (int)(input / 2)] = m_InputImageR[i][k];
			tmpInputG[i + (int)(input / 2)][k + (int)(input / 2)] = m_InputImageG[i][k];
			tmpInputB[i + (int)(input / 2)][k + (int)(input / 2)] = m_InputImageB[i][k];
		}
	}

	// 진짜 영상 처리 알고리즘 ==> 회선연산
	// 마스크9개 X 입력영상 9개의 합...
	double SR = 0.0;
	double SG = 0.0;
	double SB = 0.0;
	for (int i = 0; i < m_inH; i++) {
		for (int k = 0; k < m_inW; k++) {
			SR = 0.0; // 누적 값은 초기화하는 것 권장
			SG = 0.0; // 누적 값은 초기화하는 것 권장
			SB = 0.0; // 누적 값은 초기화하는 것 권장
			for (int m = 0; m < input; m++) {
				for (int n = 0; n < input; n++) {
					SR += tmpInputR[i + m][k + n] * maskH[m][n];
					SG += tmpInputG[i + m][k + n] * maskH[m][n];
					SB += tmpInputB[i + m][k + n] * maskH[m][n];
				}
			}
			tmpOutputR[i][k] = SR;
			tmpOutputG[i][k] = SG;
			tmpOutputB[i][k] = SB;
		}
	}
	for (int i = 0; i < m_inH; i++) {
		for (int k = 0; k < m_inW; k++) {
			SR = 0.0; // 누적 값은 초기화하는 것 권장
			SG = 0.0; // 누적 값은 초기화하는 것 권장
			SB = 0.0; // 누적 값은 초기화하는 것 권장
			for (int m = 0; m < input; m++) {
				for (int n = 0; n < input; n++) {
					SR += tmpInputR[i + m][k + n] * maskV[m][n];
					SG += tmpInputG[i + m][k + n] * maskV[m][n];
					SB += tmpInputB[i + m][k + n] * maskV[m][n];
				}
			}
			tmpOutputR2[i][k] = SR;
			tmpOutputG2[i][k] = SG;
			tmpOutputB2[i][k] = SB;
		}
	}

	for (int i = 0; i < m_outH; i++) {
		for (int k = 0; k < m_outW; k++) {
			double vR1 = tmpOutputR[i][k];
			double vG1 = tmpOutputG[i][k];
			double vB1 = tmpOutputB[i][k];
			double vR2 = tmpOutputR2[i][k];
			double vG2 = tmpOutputG2[i][k];
			double vB2 = tmpOutputB2[i][k];
			double vR = vR1 + vR2;
			double vG = vG1 + vG2;
			double vB = vB1 + vB2;
			if (vR > 255.0) vR = 255.0;
			else if (vR < 0.0) vR = 0.0;
			m_OutputImageR[i][k] = (unsigned char)vR;
			if (vG > 255.0) vG = 255.0;
			else if (vG < 0.0) vG = 0.0;
			m_OutputImageG[i][k] = (unsigned char)vG;
			if (vB > 255.0) vB = 255.0;
			else if (vB < 0.0) vB = 0.0;
			m_OutputImageB[i][k] = (unsigned char)vB;
		}
	}
	free2D_double(tmpInputR, m_inH + (int)(input / 2) * 2);
	free2D_double(tmpInputG, m_inH + (int)(input / 2) * 2);
	free2D_double(tmpInputB, m_inH + (int)(input / 2) * 2);
	free2D_double(tmpOutputR, m_outH);
	free2D_double(tmpOutputG, m_outH);
	free2D_double(tmpOutputB, m_outH);
	free2D_double(tmpOutputR2, m_outH);
	free2D_double(tmpOutputG2, m_outH);
	free2D_double(tmpOutputB2, m_outH);
	CallDialog();
}


void CImageProcessingDoc::OnLaplaImage()
{
	// TODO: 여기에 구현 코드 추가.
	if (m_InputImageR == NULL) {
		MessageBox(NULL, L"파일이 열리지 않았습니다.", L"", 0);
		return;
	}
	
		// 기존 출력 메모리 해제
	free2D(m_OutputImageR, m_outH);
	free2D(m_OutputImageG, m_outH);
	free2D(m_OutputImageB, m_outH);
	m_outH = m_inH;
	m_outW = m_inW;
	m_OutputImageR = malloc2D(m_outH, m_outW);
	m_OutputImageG = malloc2D(m_outH, m_outW);
	m_OutputImageB = malloc2D(m_outH, m_outW);

	int input = 3; // 마스크 크기
	double mask[3][3]
		= { {-1,-1,-1} ,
			{-1,8,-1} ,
			{-1,-1,-1} };

	// 임시 입력/출력 메모리 준비
	double** tmpInputR = malloc2D_double(m_inH + (int)(input / 2) * 2, m_inW + (int)(input / 2) * 2);
	double** tmpInputG = malloc2D_double(m_inH + (int)(input / 2) * 2, m_inW + (int)(input / 2) * 2);
	double** tmpInputB = malloc2D_double(m_inH + (int)(input / 2) * 2, m_inW + (int)(input / 2) * 2);
	double** tmpOutputR = malloc2D_double(m_outH, m_outW);
	double** tmpOutputG = malloc2D_double(m_outH, m_outW);
	double** tmpOutputB = malloc2D_double(m_outH, m_outW);
	for (int i = 0; i < m_inH; i++) {
		for (int k = 0; k < m_inW; k++) {
			tmpInputR[i + (int)(input / 2)][k + (int)(input / 2)] = m_InputImageR[i][k];
			tmpInputG[i + (int)(input / 2)][k + (int)(input / 2)] = m_InputImageG[i][k];
			tmpInputB[i + (int)(input / 2)][k + (int)(input / 2)] = m_InputImageB[i][k];
		}
	}

	// 진짜 영상 처리 알고리즘 ==> 회선연산
	// 마스크9개 X 입력영상 9개의 합...
	double SR = 0.0;
	double SG = 0.0;
	double SB = 0.0;
	for (int i = 0; i < m_inH; i++) {
		for (int k = 0; k < m_inW; k++) {
			SR = 0.0; // 누적 값은 초기화하는 것 권장
			SG = 0.0; // 누적 값은 초기화하는 것 권장
			SB = 0.0; // 누적 값은 초기화하는 것 권장
			for (int m = 0; m < input; m++) {
				for (int n = 0; n < input; n++) {
					SR += tmpInputR[i + m][k + n] * mask[m][n];
					SG += tmpInputG[i + m][k + n] * mask[m][n];
					SB += tmpInputB[i + m][k + n] * mask[m][n];
				}
			}
			tmpOutputR[i][k] = SR;
			tmpOutputG[i][k] = SG;
			tmpOutputB[i][k] = SB;
		}
	}

	for (int i = 0; i < m_outH; i++) {
		for (int k = 0; k < m_outW; k++) {
			double vR = tmpOutputR[i][k];
			double vG = tmpOutputG[i][k];
			double vB = tmpOutputB[i][k];
			if (vR > 255.0) vR = 255.0;
			else if (vR < 0.0) vR = 0.0;
			m_OutputImageR[i][k] = (unsigned char)vR;
			if (vG > 255.0) vG = 255.0;
			else if (vG < 0.0) vG = 0.0;
			m_OutputImageG[i][k] = (unsigned char)vG;
			if (vB > 255.0) vB = 255.0;
			else if (vB < 0.0) vB = 0.0;
			m_OutputImageB[i][k] = (unsigned char)vB;
		}
	}
	free2D_double(tmpInputR, m_inH + (int)(input / 2) * 2);
	free2D_double(tmpInputG, m_inH + (int)(input / 2) * 2);
	free2D_double(tmpInputB, m_inH + (int)(input / 2) * 2);
	free2D_double(tmpOutputR, m_outH);
	free2D_double(tmpOutputG, m_outH);
	free2D_double(tmpOutputB, m_outH);
	CallDialog();
}


void CImageProcessingDoc::OnLogImage()
{
	// TODO: 여기에 구현 코드 추가.
	if (m_InputImageR == NULL) {
		MessageBox(NULL, L"파일이 열리지 않았습니다.", L"", 0);
		return;
	}
	
		// 기존 출력 메모리 해제
	free2D(m_OutputImageR, m_outH);
	free2D(m_OutputImageG, m_outH);
	free2D(m_OutputImageB, m_outH);
	m_outH = m_inH;
	m_outW = m_inW;
	m_OutputImageR = malloc2D(m_outH, m_outW);
	m_OutputImageG = malloc2D(m_outH, m_outW);
	m_OutputImageB = malloc2D(m_outH, m_outW);

	int input = 5; // 마스크 크기
	double mask[5][5]
		= { {0,0,-1,0,0} ,
			{0,-1,-2,-1,0} ,
			{-1,-2,16,-2,-1},
			{0,-1,-2,-1,0} ,
			{0,0,-1,0,0} };

	// 임시 입력/출력 메모리 준비
	double** tmpInputR = malloc2D_double(m_inH + (int)(input / 2) * 2, m_inW + (int)(input / 2) * 2);
	double** tmpInputG = malloc2D_double(m_inH + (int)(input / 2) * 2, m_inW + (int)(input / 2) * 2);
	double** tmpInputB = malloc2D_double(m_inH + (int)(input / 2) * 2, m_inW + (int)(input / 2) * 2);
	double** tmpOutputR = malloc2D_double(m_outH, m_outW);
	double** tmpOutputG = malloc2D_double(m_outH, m_outW);
	double** tmpOutputB = malloc2D_double(m_outH, m_outW);
	for (int i = 0; i < m_inH; i++) {
		for (int k = 0; k < m_inW; k++) {
			tmpInputR[i + (int)(input / 2)][k + (int)(input / 2)] = m_InputImageR[i][k];
			tmpInputG[i + (int)(input / 2)][k + (int)(input / 2)] = m_InputImageG[i][k];
			tmpInputB[i + (int)(input / 2)][k + (int)(input / 2)] = m_InputImageB[i][k];
		}
	}

	// 진짜 영상 처리 알고리즘 ==> 회선연산
	// 마스크9개 X 입력영상 9개의 합...
	double SR = 0.0;
	double SG = 0.0;
	double SB = 0.0;
	for (int i = 0; i < m_inH; i++) {
		for (int k = 0; k < m_inW; k++) {
			SR = 0.0; // 누적 값은 초기화하는 것 권장
			SG = 0.0; // 누적 값은 초기화하는 것 권장
			SB = 0.0; // 누적 값은 초기화하는 것 권장
			for (int m = 0; m < input; m++) {
				for (int n = 0; n < input; n++) {
					SR += tmpInputR[i + m][k + n] * mask[m][n];
					SG += tmpInputG[i + m][k + n] * mask[m][n];
					SB += tmpInputB[i + m][k + n] * mask[m][n];
				}
			}
			tmpOutputR[i][k] = SR;
			tmpOutputG[i][k] = SG;
			tmpOutputB[i][k] = SB;
		}
	}

	for (int i = 0; i < m_outH; i++) {
		for (int k = 0; k < m_outW; k++) {
			double vR = tmpOutputR[i][k];
			double vG = tmpOutputG[i][k];
			double vB = tmpOutputB[i][k];
			if (vR > 255.0) vR = 255.0;
			else if (vR < 0.0) vR = 0.0;
			m_OutputImageR[i][k] = (unsigned char)vR;
			if (vG > 255.0) vG = 255.0;
			else if (vG < 0.0) vG = 0.0;
			m_OutputImageG[i][k] = (unsigned char)vG;
			if (vB > 255.0) vB = 255.0;
			else if (vB < 0.0) vB = 0.0;
			m_OutputImageB[i][k] = (unsigned char)vB;
		}
	}
	free2D_double(tmpInputR, m_inH + (int)(input / 2) * 2);
	free2D_double(tmpInputG, m_inH + (int)(input / 2) * 2);
	free2D_double(tmpInputB, m_inH + (int)(input / 2) * 2);
	free2D_double(tmpOutputR, m_outH);
	free2D_double(tmpOutputG, m_outH);
	free2D_double(tmpOutputB, m_outH);
	CallDialog();

}


void CImageProcessingDoc::OnDogImage()
{
	// TODO: 여기에 구현 코드 추가.
	if (m_InputImageR == NULL) {
		MessageBox(NULL, L"파일이 열리지 않았습니다.", L"", 0);
		return;
	}
	
		// 기존 출력 메모리 해제
	free2D(m_OutputImageR, m_outH);
	free2D(m_OutputImageG, m_outH);
	free2D(m_OutputImageB, m_outH);
	m_outH = m_inH;
	m_outW = m_inW;
	m_OutputImageR = malloc2D(m_outH, m_outW);
	m_OutputImageG = malloc2D(m_outH, m_outW);
	m_OutputImageB = malloc2D(m_outH, m_outW);

	int input = 9; // 마스크 크기
	double mask[9][9]
		= { {0,0,0,-1,-1,-1,0,0,0},
			{0,-2,-3,-3,-3,-3,-3,-2,0},
			{0,-3,-2,-1,-1,-1,-2,-3,0},
			{0,-3,-1,9,9,9,-1,-3,0},
			{-1,-3,-1,9,19,9,-1,-3,-1},
			{0,-3,-1,9,9,9,-1,-3,0},
			{0,-3,-2,-1,-1,-1,-2,-3,0},
			{0,-2,-3,-3,-3,-3,-3,-2,0},
			{0,0,0,-1,-1,-1,0,0,0}};

	// 임시 입력/출력 메모리 준비
	double** tmpInputR = malloc2D_double(m_inH + (int)(input / 2) * 2, m_inW + (int)(input / 2) * 2);
	double** tmpInputG = malloc2D_double(m_inH + (int)(input / 2) * 2, m_inW + (int)(input / 2) * 2);
	double** tmpInputB = malloc2D_double(m_inH + (int)(input / 2) * 2, m_inW + (int)(input / 2) * 2);
	double** tmpOutputR = malloc2D_double(m_outH, m_outW);
	double** tmpOutputG = malloc2D_double(m_outH, m_outW);
	double** tmpOutputB = malloc2D_double(m_outH, m_outW);
	for (int i = 0; i < m_inH; i++) {
		for (int k = 0; k < m_inW; k++) {
			tmpInputR[i + (int)(input / 2)][k + (int)(input / 2)] = m_InputImageR[i][k];
			tmpInputG[i + (int)(input / 2)][k + (int)(input / 2)] = m_InputImageG[i][k];
			tmpInputB[i + (int)(input / 2)][k + (int)(input / 2)] = m_InputImageB[i][k];
		}
	}

	// 진짜 영상 처리 알고리즘 ==> 회선연산
	// 마스크9개 X 입력영상 9개의 합...
	double SR = 0.0;
	double SG = 0.0;
	double SB = 0.0;
	for (int i = 0; i < m_inH; i++) {
		for (int k = 0; k < m_inW; k++) {
			SR = 0.0; // 누적 값은 초기화하는 것 권장
			SG = 0.0; // 누적 값은 초기화하는 것 권장
			SB = 0.0; // 누적 값은 초기화하는 것 권장
			for (int m = 0; m < input; m++) {
				for (int n = 0; n < input; n++) {
					SR += tmpInputR[i + m][k + n] * mask[m][n];
					SG += tmpInputG[i + m][k + n] * mask[m][n];
					SB += tmpInputB[i + m][k + n] * mask[m][n];
				}
			}
			tmpOutputR[i][k] = SR;
			tmpOutputG[i][k] = SG;
			tmpOutputB[i][k] = SB;
		}
	}

	for (int i = 0; i < m_outH; i++) {
		for (int k = 0; k < m_outW; k++) {
			double vR = tmpOutputR[i][k];
			double vG = tmpOutputG[i][k];
			double vB = tmpOutputB[i][k];
			if (vR > 255.0) vR = 255.0;
			else if (vR < 0.0) vR = 0.0;
			m_OutputImageR[i][k] = (unsigned char)vR;
			if (vG > 255.0) vG = 255.0;
			else if (vG < 0.0) vG = 0.0;
			m_OutputImageG[i][k] = (unsigned char)vG;
			if (vB > 255.0) vB = 255.0;
			else if (vB < 0.0) vB = 0.0;
			m_OutputImageB[i][k] = (unsigned char)vB;
		}
	}
	free2D_double(tmpInputR, m_inH + (int)(input / 2) * 2);
	free2D_double(tmpInputG, m_inH + (int)(input / 2) * 2);
	free2D_double(tmpInputB, m_inH + (int)(input / 2) * 2);
	free2D_double(tmpOutputR, m_outH);
	free2D_double(tmpOutputG, m_outH);
	free2D_double(tmpOutputB, m_outH);

	CallDialog();
}

// RGB 전부 히스토그램 뽑음
void CImageProcessingDoc::OnHistChartImage()
{
	// TODO: 여기에 구현 코드 추가.
	free2D(m_OutputImageR, m_outH);
	free2D(m_OutputImageG, m_outH);
	free2D(m_OutputImageB, m_outH);
	m_outH = 256;
	m_outW = 256;
	m_OutputImageR = malloc2D(m_outH, m_outW);
	m_OutputImageG = malloc2D(m_outH, m_outW);
	m_OutputImageB = malloc2D(m_outH, m_outW);

	// 히스토그램 이미지 크기 (1차원배열화)
	int reSize = m_outH * m_outW;
	int LOW = 0;
	int HIGH = 255;
	// 히스토그램 행렬
	unsigned char histR[256];
	unsigned char histG[256];
	unsigned char histB[256];
	unsigned char valueR = 0;
	unsigned char valueG = 0;
	unsigned char valueB = 0;

	//초기화
	for (int i = 0; i < 256; i++) {
		histR[i] = 0;
		histG[i] = 0;
		histB[i] = 0;
	}
	//빈도수 조사
	for (int i = 0; i < m_inH; i++) {
		for (int k = 0; k < m_inW; k++) {
			valueR = m_InputImageR[i][k];
			histR[valueR]++;
			valueG = m_InputImageG[i][k];
			histG[valueG]++;
			valueB = m_InputImageB[i][k];
			histB[valueB]++;

		}
	}
	// 정규화
	// 최소값 초기화
	unsigned char minR = 0;
	unsigned char minG = 0;
	unsigned char minB = 0;
	// 최대값 초기화
	unsigned char maxR = 0;
	unsigned char maxG = 0;
	unsigned char maxB = 0;
	unsigned char difR = 0;
	unsigned char difG = 0;
	unsigned char difB = 0;
	int hNum = 0;
	for (int i = 0; i < 256; i++) {
		if (histR[i] <= minR) minR = (unsigned char)histR[i];
		if (histR[i] >= maxR) maxR = (unsigned char)histR[i];
		if (histG[i] <= minG) minG = (unsigned char)histG[i];
		if (histG[i] >= maxG) maxG = (unsigned char)histG[i];
		if (histB[i] <= minB) minB = (unsigned char)histB[i];
		if (histB[i] >= maxB) maxB = (unsigned char)histB[i];
	}
	difR = maxR - minR; difG = maxG - minG; difB = maxB - minB;
	unsigned char scaleHistR[256];
	unsigned char scaleHistG[256];
	unsigned char scaleHistB[256];
	// 정규화 된 히스토그램
	for (int i = 0; i < 256; i++) {
		scaleHistR[i] = (unsigned char)((histR[i] - minR) * HIGH / difR);
		scaleHistG[i] = (unsigned char)((histG[i] - minG) * HIGH / difG);
		scaleHistB[i] = (unsigned char)((histB[i] - minB) * HIGH / difB);
	}
	// 정규화된 히스토그램 출력
	unsigned char* OutImageR = (unsigned char*)malloc((reSize) * sizeof(unsigned char));
	unsigned char* OutImageG = (unsigned char*)malloc((reSize) * sizeof(unsigned char));
	unsigned char* OutImageB = (unsigned char*)malloc((reSize) * sizeof(unsigned char));
	// 정규화된히스토그램의값은출력배열에검은점(0)으로표현
	for (int i = 0; i < m_outH; i++) {
		for (int k = 0; k < scaleHistR[i]; k++) {
			OutImageR[m_outW * (m_outH - k - 1) + i] = 0;
		}
		for (int k = 0; k < scaleHistG[i]; k++) {
			OutImageG[m_outW * (m_outH - k - 1) + i] = 0;
		}
		for (int k = 0; k < scaleHistB[i]; k++) {
			OutImageB[m_outW * (m_outH - k - 1) + i] = 0;
		}
	}
	hNum = 0;
	for (int i = 0; i < m_outH; i++) {
		for (int k = 0; k < m_outW; k++) {
			m_OutputImageR[i][k] = OutImageR[hNum];
			m_OutputImageG[i][k] = OutImageG[hNum];
			m_OutputImageB[i][k] = OutImageB[hNum];
			hNum++;
		}
	}
	free(OutImageR); free(OutImageG); free(OutImageB);

	CallDialog();
}


void CImageProcessingDoc::OnHistStchImage()
{
	if (m_InputImageR == NULL) {
		MessageBox(NULL, L"파일이 열리지 않았습니다.", L"", 0);
		return;
	}
	// TODO: 여기에 구현 코드 추가.
	free2D(m_OutputImageR, m_outH);
	free2D(m_OutputImageG, m_outH);
	free2D(m_OutputImageB, m_outH);
	m_outH = m_inH;
	m_outW = m_inW;
	m_OutputImageR = malloc2D(m_outH, m_outW);
	m_OutputImageG = malloc2D(m_outH, m_outW);
	m_OutputImageB = malloc2D(m_outH, m_outW);
	int LOW = 0;
	int HIGH = 255;
	// 최소값 초기화
	unsigned char minR = (unsigned char)HIGH;
	unsigned char minG = (unsigned char)HIGH;
	unsigned char minB = (unsigned char)HIGH;
	// 최대값 초기화
	unsigned char maxR = (unsigned char)LOW; 
	unsigned char maxG = (unsigned char)LOW;
	unsigned char maxB = (unsigned char)LOW;

	//입력 영상의 최소값 찾기
	for (int i = 0; i < m_inH; i++) {
		for (int k = 0; k < m_inW; k++) {
			if (m_InputImageR[i][k] <= minR)
				minR = m_InputImageR[i][k];
			if (m_InputImageG[i][k] <= minG)
				minG = m_InputImageG[i][k];
			if (m_InputImageB[i][k] <= minB)
				minB = m_InputImageB[i][k];
		}
	}
	//입력 영상의 최대값 찾기
	for (int i = 0; i < m_inH; i++) {
		for (int k = 0; k < m_inW; k++) {
			if (m_InputImageR[i][k] >= maxR)
				maxR = m_InputImageR[i][k];
			if (m_InputImageG[i][k] >= maxG)
				maxG = m_InputImageG[i][k];
			if (m_InputImageB[i][k] >= maxB)
				maxB = m_InputImageB[i][k];
		}
	}
	//출력값에 전달
	for (int i = 0; i < m_outH; i++) {
		for (int k = 0; k < m_outW; k++) {
			m_OutputImageR[i][k] = (unsigned char)(m_InputImageR[i][k] - minR) * HIGH / (maxR - minR);
			m_OutputImageG[i][k] = (unsigned char)(m_InputImageG[i][k] - minG) * HIGH / (maxG - minG);
			m_OutputImageB[i][k] = (unsigned char)(m_InputImageB[i][k] - minB) * HIGH / (maxB - minB);
		}
	}
	CallDialog();
}


void CImageProcessingDoc::OnHistEqualImage()
{
	if (m_InputImageR == NULL) {
		MessageBox(NULL, L"파일이 열리지 않았습니다.", L"", 0);
		return;
	}
	// TODO: 여기에 구현 코드 추가.
	free2D(m_OutputImageR, m_outH);
	free2D(m_OutputImageG, m_outH);
	free2D(m_OutputImageB, m_outH);
	m_outH = m_inH;
	m_outW = m_inW;
	m_OutputImageR = malloc2D(m_outH, m_outW);
	m_OutputImageG = malloc2D(m_outH, m_outW);
	m_OutputImageB = malloc2D(m_outH, m_outW);

	int i, j, vmax, vmin;
	double m_HISTR[256];
	double m_HISTG[256];
	double m_HISTB[256];
	double m_Sum_of_HISTR[256];
	double m_Sum_of_HISTG[256];
	double m_Sum_of_HISTB[256];
	int value;
	unsigned char LOW, HIGH, Temp;
	double SUMR = 0.0;
	double SUMG = 0.0;
	double SUMB = 0.0;

	int size = m_inH * m_inW;
	LOW = 0;
	HIGH = 255;

	// 초기화
	for (int i = 0; i < 256; i++) {
		m_HISTR[i] = LOW;
		m_HISTG[i] = LOW;
		m_HISTB[i] = LOW;
	}
		
	// 빈도수조사
	for (int i = 0; i < m_inH; i++) {
		for (int k = 0; k < m_inW; k++) {
			value = m_InputImageR[i][k];
			m_HISTR[value]++;
			value = m_InputImageG[i][k];
			m_HISTG[value]++;
			value = m_InputImageB[i][k];
			m_HISTB[value]++;
		}
	}
	// 누적히스토그램생성
	for (int i = 0; i < 256; i++) {
		SUMR += m_HISTR[i];
		m_Sum_of_HISTR[i] = SUMR;
		SUMG += m_HISTG[i];
		m_Sum_of_HISTG[i] = SUMG;
		SUMB += m_HISTB[i];
		m_Sum_of_HISTB[i] = SUMB;
	}

	
	// 입력영상을평활화된영상으로출력
	for (int i = 0; i < m_outH; i++) {
		for (int k = 0; k < m_outW; k++) {
			Temp = m_InputImageR[i][k];
			m_OutputImageR[i][k] = (unsigned char)(m_Sum_of_HISTR[Temp] * HIGH / size);
			Temp = m_InputImageG[i][k];
			m_OutputImageG[i][k] = (unsigned char)(m_Sum_of_HISTG[Temp] * HIGH / size);
			Temp = m_InputImageB[i][k];
			m_OutputImageB[i][k] = (unsigned char)(m_Sum_of_HISTB[Temp] * HIGH / size);
		}
	}
	CallDialog();
}



void CImageProcessingDoc::CallDialog()
{
	CModelessDlg* pDlg = new CModelessDlg();
	pDlg->Create(IDD_DIALOG1);
	pDlg->ShowWindow(SW_SHOWNORMAL);
	
	pDlg->SetRGB(m_OutputImageR, m_OutputImageG, m_OutputImageB);
	pDlg->SetHeightWidth(m_inH, m_inW, m_outH, m_outW);
}

