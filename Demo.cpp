
#include "stdafx.h"
#include "Demo.h"
#include "data.h"


CDemo::CDemo() :m_imageXscale(1000), m_imageYscale(1000)
{
	initGame();
	setLayout();

	m_gameBackColor = RGB(255, 255, 255);
	//m_gameBackColor = RGB(122, 122, 122);
	m_brBack.CreateSolidBrush(m_gameBackColor);

	//m_brBack.CreateSolidBrush(0x0000FF);
	//m_brInfoBack.CreateSolidBrush(0x00FF00);
	//m_brBack = CBrush(0x0000FF);
	//m_brInfoBack = CBrush(0x00FF00);

	showDetail = false;			/// corner info
	showDetailInner = false;	/// Debug  info

	m_AStartColors[0] = RGB(0, 0, 0);
	m_AStartColors[1] = RGB(255, 0, 0);
	m_AStartColors[2] = RGB(0, 255, 0);
	m_AStartColors[3] = RGB(0, 0, 255);
	m_AStartColors[4] = RGB(255, 255, 0);
	m_AStartColors[5] = RGB(255, 0, 255);
	m_AStartColors[6] = RGB(0, 255, 255);
	m_AStartColors[7] = RGB(255, 128, 0);
	m_AStartColors[8] = RGB(255, 0, 128);
	m_AStartColors[9] = RGB(0, 128, 255);

	m_uShellRcX[0] = m_showRect.right;		   // 边界的坐标X方向右端点	
	m_uShellRcY[0] = m_gameRect.bottom;		   // 边界的坐标Y方向下部端点
	m_uShellRcX[1] = m_showRect.right - 9;				// 时间框的坐标X方向右端点
	m_uShellRcY[1] = m_gameRect.bottom - 9;			    // 雷区的坐标Y方向下部端点

	m_uNumRect[0] = m_showRect.right - 3 * 24 - 5;  // 剩余时间LED区域最左端端点坐标
	m_uNumRect[1] = 360;				          // 剩余时间LED区域
	m_uNumRect[2] = 360;				          // 剩余时间LED区域

	// Mine area start-end point
	m_startP.x = ((m_gameRect.right - m_gameRect.left) - m_uRowNum * m_uRowPixCount) / 2;
	m_startP.y = ((m_gameRect.bottom - m_gameRect.top) - m_uColNum * m_uColPixCount) / 2;
}


CDemo::~CDemo()
{
}


void CDemo::setLayout()
{
	m_wndRect.left = 20;			// Frame Rect Scale
	m_wndRect.top = 20;				// Frame Rect Scale
	m_wndRect.right = 20 + 1000;	// Frame Rect Scale
	m_wndRect.bottom = 20 + 700;	// Frame Rect Scale
	resizeLayout(m_wndRect);
}


void CDemo::resizeLayout(const CRect& rect){
	if (rect.Width() < 1000){
		m_leftArea = CRect(10, 10, 710, 710);
		m_rightArea = CRect(710 + 5, 10, 950, 710);
	}
	else{
		m_leftArea = CRect(10, 10, rect.Width() - 290, 710);
		m_rightArea = CRect(rect.Width() - 285, 10, rect.Width() - 50, 710);
	}
	m_showRect = m_leftArea;
	m_showRect.bottom = 50;
	m_gameRect = m_leftArea;
	m_gameRect.top = 55;
	m_infoRect = m_rightArea;
	m_rcClient = m_gameRect;
}


void CDemo::switchMode(UINT newMode){
	switch (newMode)
	{
	case DEMO_NILL:
	case DEMO_START:
	case DEMO_MAZE:
		mode = newMode;
		break;
	case DEMO_NEXT:
		if (isLegalGameMode(mode + 1)){
			mode++;
		}
		else{
			mode = DEMO_NILL + 1;
		}
		break;
	default:
		break;
	}
}


bool CDemo::isLegalGameMode(UINT newMode){
	if (newMode > DEMO_NILL &&  newMode < DEMO_END){
		return true;
	}
	else{
		return false;
	}
}


void CDemo::initGame(){
	mode = DEMO_MAZE;

	m_isGameOver = false;
	m_stepCount = 0;
	m_maxValue = 0;
	m_fillCount = 0;
	m_uRowNum = 1;
	m_uColNum = 1;

	hideTheEdge();
	hideInnerEdge();

	initAStar();
	initMaze();
}


void CDemo::edgeStateTransition(){
	m_showEdge ^= true;
}


void CDemo::innerEdgeStateTransition(){
	m_showInnerEdge ^= true;
}


void CDemo::LeftBtnDown(CWnd* pWnd, UINT nFlags, CPoint point)
{
	m_slideS = point;
}


void CDemo::LeftBtnUp(CWnd* pWnd, UINT nFlags, CPoint point)
{
	m_slideT = point;
	if ((m_slideT.x - m_slideS.x)*(m_slideT.x - m_slideS.x) + (m_slideT.y - m_slideS.y)*(m_slideT.y - m_slideS.y) < 26){
		return;
	}
	switch (mode)
	{
	case DEMO_NILL:
		break;
	case DEMO_START:
		if (adaptShowScale(true)){
			pWnd->Invalidate();
		}
		break;
	case DEMO_MAZE:
		if (adaptShowScale(true)){
			pWnd->Invalidate();
		}
		break;

	default:
		break;
	}
}


void CDemo::RightBtnDown(CWnd* pWnd, UINT nFlags, CPoint point)
{
	m_slideS = point;
}


void CDemo::RightBtnUp(CWnd* pWnd, UINT nFlags, CPoint point)
{
	m_slideT = point;
	switch (mode)
	{
	case DEMO_NILL:
		break;
	case DEMO_START:
		if (adaptShowScale()){
			pWnd->Invalidate();
		}
		break;
	case DEMO_MAZE:
		if (adaptShowScale()){
			pWnd->Invalidate();
		}
		break;

	default:
		break;
	}
}


void CDemo::hideTheEdge(bool isHiding){
	m_showEdge = isHiding;
}


void CDemo::hideInnerEdge(bool isHiding){
	m_showInnerEdge = isHiding;
}


void CDemo::Initialize(CWnd* pWnd)
{
	//pWnd->MoveWindow(&m_wndRect, TRUE);		   // 设定显示窗口在屏幕的位置及大小
	initGame();
	setLayout();
}


void CDemo::DrawCurr(CPaintDC* pDC)
{
	/// Draw all the showing information and the chess board to realize the man-machine interactive interface.
	//////////////////////////////////////////////////////////////////////////////////////////////////////

	CPen pen(PS_SOLID, 2, RGB(111, 111, 255));
	CPen penNew(PS_SOLID, 2, RGB(188, 0, 0));
	CPen *pOldPen = pDC->SelectObject(&pen);
	pDC->SelectObject(&penNew);
	drawFrame(pDC);		/// Draw Framework  Directly
	drawInfo(pDC);		/// Draw Infomation Directly
	pDC->SelectObject(pOldPen);

	CBrush newBrush(m_gameBackColor);
	CDC dcMemory;
	CBitmap bitmap;
	if (!pDC->IsPrinting())
	{
		if (dcMemory.CreateCompatibleDC(pDC))
		{
			if (bitmap.CreateCompatibleBitmap(pDC, m_rcClient.right, m_rcClient.bottom))
			{
				dcMemory.SelectObject(&bitmap);		/// Select the bitmap as the source image to copy to current game-area.
				m_gameRectShown = m_gameRect;
				m_gameRectShown.OffsetRect(-m_gameRect.left, -m_gameRect.top);
				dcMemory.FillRect(&m_gameRectShown, &newBrush);	/// Background Color Filling

				//drawStartButton((CPaintDC&)dcMemory);
				//drawNumber((CPaintDC&)dcMemory);

				drawShell((CPaintDC&)dcMemory);

				switch (mode)
				{
				case DEMO_NILL:
					break;
				case DEMO_START:
					drawAStartHeuristicResult((CPaintDC&)dcMemory);
					break;
				case DEMO_MAZE:
					drawMazeResult((CPaintDC&)dcMemory);
					break;

				default:
					break;
				}

				pDC->BitBlt(m_rcClient.left, m_rcClient.top, m_rcClient.Width(), m_rcClient.Height(),
					&dcMemory, 0, 0, SRCCOPY);	/// Copy source image to appointed area
				bitmap.DeleteObject();
			}
		}
	}
}


void CDemo::drawFrame(CPaintDC* pDC){
	CPoint point01(200, 200);
	CPoint point02(400, 400);
	if (!m_showEdge){
		// Area 001 --- m_showRect
		point01.x = m_showRect.left;
		point01.y = m_showRect.top;
		point02.x = m_showRect.right;
		point02.y = m_showRect.bottom;
		pDC->MoveTo(point01);
		pDC->LineTo(point02.x, point01.y);
		pDC->LineTo(point02);
		pDC->LineTo(point01.x, point02.y);
		pDC->LineTo(point01);
		// Area 002 --- m_gameRect
		point01.x = m_gameRect.left;
		point01.y = m_gameRect.top;
		point02.x = m_gameRect.right;
		point02.y = m_gameRect.bottom;
		pDC->MoveTo(point01);
		pDC->LineTo(point02.x, point01.y);
		pDC->LineTo(point02);
		pDC->LineTo(point01.x, point02.y);
		pDC->LineTo(point01);
		// Area 003 --- m_infoRect
		point01.x = m_infoRect.left;
		point01.y = m_infoRect.top;
		point02.x = m_infoRect.right;
		point02.y = m_infoRect.bottom;
		pDC->MoveTo(point01);
		pDC->LineTo(point02.x, point01.y);
		pDC->LineTo(point02);
		pDC->LineTo(point01.x, point02.y);
		pDC->LineTo(point01);
		// Area ...
	}
}


void CDemo::drawInfo(CPaintDC* pDC){
	UINT deltaHorizonal = UINT(m_infoRect.Width() / 8);
	UINT deltaVertical = UINT(m_infoRect.Height() / 4);

	const CString strInfo01("  ------------------LE------------------");
	pDC->TextOutW(m_infoRect.left + deltaHorizonal, m_infoRect.top + UINT(deltaVertical*0.02), strInfo01);
	/// show the step count info
	CString strInfo;
	strInfo.Format(_T("You have proceed %d steps."), m_stepCount);
	pDC->TextOutW(m_infoRect.left + deltaHorizonal, m_infoRect.top + UINT(deltaVertical*0.3), strInfo);

	const CString strInfo02("-----------------Wonderful-----------------");
	pDC->TextOutW(m_infoRect.left + deltaHorizonal, m_infoRect.top + deltaVertical * 1, strInfo02);
	/// show the current maximum value which has been reach
	if (m_isGameOver){
		strInfo.Format(_T("Game acheived."));
	}
	else{
		strInfo.Format(_T("You have done very well. "));
	}
	pDC->TextOutW(m_infoRect.left + deltaHorizonal, m_infoRect.top + UINT(deltaVertical*(1 + 0.3)), strInfo);

	const CString strInfo03("   -----------------YY-----------------");
	pDC->TextOutW(m_infoRect.left + deltaHorizonal, m_infoRect.top + deltaVertical * 2, strInfo03);

	const CString strInfo04(" -----------------Fr. TT-----------------");
	pDC->TextOutW(m_infoRect.left + deltaHorizonal, m_infoRect.top + deltaVertical * 2.6, strInfo04);
}


void CDemo::drawShell(CPaintDC &dc)
{
	/// Test 001
	if (false){
		CPoint temp(100, 100);
		dc.MoveTo(temp);
		dc.LineTo(m_gameRect.Width() / 2, m_gameRect.Height() / 2);
		dc.LineTo(m_gameRect.Width() * 3 / 4, m_gameRect.Height() / 2);
		dc.LineTo(m_gameRect.Width() * 3 / 4, m_gameRect.Height() * 1 / 3);
		int count = 8;
		for (int i = 0; i < count; i++){
			temp.x += m_gameRect.Width() / (count * 2);
			temp.y += m_gameRect.Height() / (count * 2);
			drawSubInfo(dc, temp);
		}
	}

	/// Test 002
	if (true){
		CPoint sPnt(300, 100);
		CPoint tPnt(400, 200);
		for (int i = 0; i < 3; i++){
			sPnt.x = 300;
			sPnt.y = 100 + i * 150;
			tPnt.x = 400;
			tPnt.y = 200 + i * 150;
			for (int j = 0; j < 3; j++){
				drawSubStructure(dc, sPnt, tPnt, i * 3 + j);
				sPnt.Offset(120, 0);
				tPnt.Offset(120, 0);
			}
		}
	}
}


void CDemo::drawNumber(CPaintDC &dc)
{
	int leftDistaceBlank = 0;
	int topDistaceBlank = 0;
	if (m_gameRect.Width() > m_uColNum*(m_uColPixCount + m_uBlankWidthPixCount)){
		leftDistaceBlank = (m_gameRect.Width() - m_uColNum*(m_uColPixCount + m_uBlankWidthPixCount)) / 2;
	}
	if (m_gameRect.Height() > m_uRowNum*(m_uRowPixCount + m_uBlankHeightPixCount)){
		topDistaceBlank = (m_gameRect.Height() - m_uRowNum*(m_uRowPixCount + m_uBlankHeightPixCount)) / 4;
	}
	for (INT i = 0; i<m_uRowNum; i++)
	{
		for (INT j = 0; j<m_uColNum; j++)
		{
			m_startP.x = m_gameRect.left + leftDistaceBlank + (m_uColPixCount + m_uBlankWidthPixCount)*j;
			m_startP.y = m_gameRect.top + topDistaceBlank + (m_uRowPixCount + m_uBlankHeightPixCount)*i;
			if (m_isGameOver){
				drawOneNumber(dc, m_startP, m_data[i][j] % m_uColNum, m_data[i][j] / m_uColNum);
			}
			else{
				if (m_uRowNum * m_uColNum - 1 == m_data[i][j]){	// m_blankOneGrid  m_uRowNum * m_uColNum - 1
					drawSpecialImageSegment(dc, m_startP, m_data[i][j] % m_uColNum, m_data[i][j] / m_uColNum);
				}
				else{
					drawOneNumber(dc, m_startP, m_data[i][j] % m_uColNum, m_data[i][j] / m_uColNum);
				}
			}
		}
	}
}


void CDemo::drawOneNumber(CPaintDC &dc, const CPoint &point, int posX, int posY)
{
	CDC cdc;
	cdc.CreateCompatibleDC(&dc);
	cdc.SelectObject(m_bmpNumber);
	if (m_unitColorStyle != 0)
	{
		m_unitColorStyle = 0;
	}
	dc.StretchBlt(point.x, point.y, m_uColPixCount, m_uRowPixCount, &cdc,
		posX*m_uColPixCount, m_uRowPixCount * posY, m_uColPixCount, m_uRowPixCount, SRCCOPY);
}


void CDemo::drawSpecialImageSegment(CPaintDC &dc, const CPoint &point, int posX, int posY)
{
	CDC cdc;
	cdc.CreateCompatibleDC(&dc);
	cdc.SelectObject(m_bmpNumber);
	if (m_unitColorStyle != 0)
	{
		m_unitColorStyle = 0;
	}
	dc.Rectangle(point.x, point.y, point.x + m_uColPixCount, point.y + m_uRowPixCount);
}


void CDemo::drawSubInfo(CPaintDC &dc, const CPoint point, int type){
	CPoint newPnt = point;
	CPoint infoPnt = point;
	int width = 60;
	int height = 20;
	const CString strInfo("YY--TT");
	switch (type){
	case 0:
		newPnt.x -= width / 2;
		newPnt.y -= height / 2;
		dc.MoveTo(newPnt);
		newPnt.x += width;
		dc.LineTo(newPnt);
		newPnt.y += height;
		dc.LineTo(newPnt);
		newPnt.x -= width;
		dc.LineTo(newPnt);
		newPnt.y -= height;
		dc.LineTo(newPnt);
		infoPnt = newPnt;
		infoPnt.x += 4;
		infoPnt.y += 3;
		dc.TextOutW(infoPnt.x, infoPnt.y, strInfo);
		break;
	default:
		newPnt.x -= 10;
		dc.MoveTo(newPnt);
		newPnt.x += 20;
		dc.LineTo(newPnt);
		newPnt.y += 20;
		dc.LineTo(newPnt);
		newPnt.x -= 20;
		dc.LineTo(newPnt);
		newPnt.y -= 20;
		dc.LineTo(newPnt);
		break;
	}
}


void CDemo::drawSubStructure(CPaintDC &dc, const CPoint& sPoint, const CPoint& tPoint, int type){
	CPoint LTpnt = sPoint;
	CPoint RBpnt = tPoint;
	CRect rect;
	CRgn rgn;
	CPoint* pPoints = NULL;
	if (sPoint.x > tPoint.x){
		LTpnt.x = sPoint.x;
		RBpnt.x = tPoint.x;
	}
	if (sPoint.y > tPoint.y){
		LTpnt.y = sPoint.y;
		RBpnt.y = tPoint.y;
	}
	rect = CRect(LTpnt, RBpnt);
	int width = RBpnt.x - LTpnt.x;
	int height = RBpnt.y - LTpnt.y;

	COLORREF color(RGB(200, 200, 255));
	CPen penNew(PS_SOLID, 1, color);
	CPen *pOldPen = NULL;
	CBrush brush(color);
	CBrush* pOldBrush = dc.SelectObject(&brush);
	switch (type){
	case 0:
		/// plot rectangle only with bound line
		dc.FrameRect(&rect, &brush);
		break;
	case 1:
		/// plot rectangle without bound line
		dc.FillRect(&rect, &brush);
		break;
	case 2:
		/// plot rectangle with diffirent color bound line
		dc.Rectangle(LTpnt.x, LTpnt.y, RBpnt.x, RBpnt.y);
		break;
	case 3:
		/// plot ellipse with the same color bound line
		dc.SelectStockObject(NULL_BRUSH);
		pOldPen = dc.SelectObject(&penNew);
		dc.Ellipse(rect);
		dc.SelectObject(pOldPen);
		break;
	case 4:
		/// plot ellipse with the same color bound line
		pOldPen = dc.SelectObject(&penNew);
		dc.Ellipse(rect);
		dc.SelectObject(pOldPen);
		break;
	case 5:
		/// plot ellipse with diffirent color bound line
		dc.Ellipse(rect);
		break;
	case 6:
		/// plot rhombus only with bound line
		pPoints = new CPoint[4];
		pPoints[0].x = LTpnt.x + width / 2;
		pPoints[0].y = LTpnt.y;
		pPoints[3] = pPoints[2] = pPoints[1] = pPoints[0];
		pPoints[1].x += width / 2;
		pPoints[1].y += height / 2;
		pPoints[2].y += height;
		pPoints[3].x -= width / 2;
		pPoints[3].y += height / 2;
		rgn.CreatePolygonRgn(pPoints, 4, WINDING);
		dc.FrameRgn(&rgn, &brush, 3, 1);
		break;
	case 7:
		/// plot rhombus without bound line
		pPoints = new CPoint[4];
		pPoints[0].x = LTpnt.x + width / 2;
		pPoints[0].y = LTpnt.y;
		pPoints[3] = pPoints[2] = pPoints[1] = pPoints[0];
		pPoints[1].x += width / 2;
		pPoints[1].y += height / 2;
		pPoints[2].y += height;
		pPoints[3].x -= width / 2;
		pPoints[3].y += height / 2;
		rgn.CreatePolygonRgn(pPoints, 4, WINDING);
		dc.FillRgn(&rgn, &brush);
		break;
	case 8:
		/// plot rhombus with diffirent color bound line
		pPoints = new CPoint[4];
		pPoints[0].x = LTpnt.x + width / 2;
		pPoints[0].y = LTpnt.y;
		pPoints[3] = pPoints[2] = pPoints[1] = pPoints[0];
		pPoints[1].x += width / 2;
		pPoints[1].y += height / 2;
		pPoints[2].y += height;
		pPoints[3].x -= width / 2;
		pPoints[3].y += height / 2;
		rgn.CreatePolygonRgn(pPoints, 4, WINDING);
		dc.PaintRgn(&rgn);
		dc.MoveTo(pPoints[0]);
		for (int i = 1; i < 5; i++){
			dc.LineTo(pPoints[i % 4]);
		}
		break;
	default:
		break;
	}
	dc.SelectObject(pOldBrush);
}


bool CDemo::adaptShowScale(bool isShift){
	CPoint sPnt = m_slideS;
	CPoint tPnt = m_slideT;
	if (isShift){
		/// Shift movement
		int deltaX = tPnt.x - sPnt.x;
		int deltaY = tPnt.y - sPnt.y;
		int deltaXPix = 1;
		int deltaYPix = 1;
		switch (mode)
		{
		case DEMO_NILL:
			break;
		case DEMO_START:
			deltaXPix = deltaX / m_AStarFactorX;
			deltaYPix = deltaY / m_AStarFactorY;
			if (deltaXPix > 0){
				if (sPntAStarShow.x - deltaXPix < 0){
					deltaXPix = sPntAStarShow.x;
				}
			}
			else if (deltaXPix < 0){
				if (tPntAStarShow.x - deltaXPix > m_showImageXscale){
					deltaXPix = tPntAStarShow.x - m_showImageXscale;
				}
			}
			deltaXPix *= -1;
			if (deltaYPix > 0){
				if (sPntAStarShow.y - deltaYPix < 0){
					deltaYPix = sPntAStarShow.y;
				}
			}
			else if (deltaYPix < 0){
				if (tPntAStarShow.y - deltaYPix > m_showImageYscale){
					deltaYPix = tPntAStarShow.y - m_showImageYscale;
				}
			}
			deltaYPix *= -1;
			if (0 == deltaXPix && 0 == deltaYPix){
				return false;
			}
			sPntAStarShow.x += deltaXPix;
			tPntAStarShow.x += deltaXPix;
			sPntAStarShow.y += deltaYPix;
			tPntAStarShow.y += deltaYPix;
			break;
		case DEMO_MAZE:
			deltaXPix = deltaX / m_MazeFactorX;
			deltaYPix = deltaY / m_MazeFactorY;
			if (deltaXPix > 0){
				if (sPntMazeShow.x - deltaXPix < 0){
					deltaXPix = sPntMazeShow.x;
				}
			}
			else if (deltaXPix < 0){
				if (tPntMazeShow.x - deltaXPix > m_showImageXscaleMaze){
					deltaXPix = tPntMazeShow.x - m_showImageXscaleMaze;
				}
			}
			deltaXPix *= -1;
			if (deltaYPix > 0){
				if (sPntMazeShow.y - deltaYPix < 0){
					deltaYPix = sPntMazeShow.y;
				}
			}
			else if (deltaYPix < 0){
				if (tPntMazeShow.y - deltaYPix > m_showImageYscaleMaze){
					deltaYPix = tPntMazeShow.y - m_showImageYscaleMaze;
				}
			}
			deltaYPix *= -1;
			if (0 == deltaXPix && 0 == deltaYPix){
				return false;
			}
			sPntMazeShow.x += deltaXPix;
			tPntMazeShow.x += deltaXPix;
			sPntMazeShow.y += deltaYPix;
			tPntMazeShow.y += deltaYPix;
			break;
		default:
			break;
		}


		return true;
	}
	else{
		/// Zoom out to watch the inner detail
		CPoint newSPntAStarShow;
		CPoint newTPntAStarShow;
		int newAStarFactorX = 1;
		int newAStarFactorY = 1;
		/// Pre-deal
		if (m_slideS.x > m_slideT.x){
			sPnt.x = m_slideT.x;
			tPnt.x = m_slideS.x;
		}
		if (m_slideS.y > m_slideT.y){
			sPnt.y = m_slideT.y;
			tPnt.y = m_slideS.y;
		}
		if (tPnt.x - sPnt.x < 4 || tPnt.y - sPnt.y < 4){
		}
		sPnt.Offset(-m_gameRect.left, -m_gameRect.top);
		tPnt.Offset(-m_gameRect.left, -m_gameRect.top);

		switch (mode)
		{
		case DEMO_NILL:
			break;
		case DEMO_START:
			return false;
			newSPntAStarShow = sPntAStarShow;
			newTPntAStarShow = sPntAStarShow;
			/// To compute the new show scale
			newSPntAStarShow.x += sPnt.x / m_AStarFactorX;
			newSPntAStarShow.y += sPnt.y / m_AStarFactorY;
			newTPntAStarShow.x += tPnt.x / m_AStarFactorX;
			newTPntAStarShow.y += tPnt.y / m_AStarFactorY;
			if (newTPntAStarShow.x == newSPntAStarShow.x || newTPntAStarShow.y == newSPntAStarShow.y){
				return false;
			}
			newAStarFactorX = m_gameRect.Width() / (newTPntAStarShow.x - newSPntAStarShow.x);
			newAStarFactorY = m_gameRect.Height() / (newTPntAStarShow.y - newSPntAStarShow.y);
			if (newAStarFactorX == m_AStarFactorX && newAStarFactorY == m_AStarFactorY){
				newAStarFactorX++;
				newAStarFactorY++;
			}
			setAStartFactor(m_AStarFactorX, newAStarFactorX, newAStarFactorY);
			newTPntAStarShow = newSPntAStarShow;
			newTPntAStarShow.x += m_gameRect.Width() / newAStarFactorX;
			newTPntAStarShow.y += m_gameRect.Height() / newAStarFactorY;
			setAStartScale(newSPntAStarShow.x, newSPntAStarShow.y, newTPntAStarShow.x, newTPntAStarShow.y);
			break;
		case DEMO_MAZE:
			newSPntAStarShow = sPntMazeShow;
			newTPntAStarShow = sPntMazeShow;
			/// To compute the new show scale
			newSPntAStarShow.x += sPnt.x / m_MazeFactorX;
			newSPntAStarShow.y += sPnt.y / m_MazeFactorY;
			newTPntAStarShow.x += tPnt.x / m_MazeFactorX;
			newTPntAStarShow.y += tPnt.y / m_MazeFactorY;
			if (newTPntAStarShow.x == newSPntAStarShow.x || newTPntAStarShow.y == newSPntAStarShow.y){
				return false;
			}
			newAStarFactorX = m_gameRect.Width() / (newTPntAStarShow.x - newSPntAStarShow.x);
			newAStarFactorY = m_gameRect.Height() / (newTPntAStarShow.y - newSPntAStarShow.y);
			if (newAStarFactorX == m_MazeFactorX && newAStarFactorY == m_MazeFactorY){
				newAStarFactorX++;
				newAStarFactorY++;
			}
			setMazeFactor(newAStarFactorX, newAStarFactorX, newAStarFactorY);
			newTPntAStarShow = newSPntAStarShow;
			newTPntAStarShow.x += m_gameRect.Width() / newAStarFactorX;
			newTPntAStarShow.y += m_gameRect.Height() / newAStarFactorY;
			setMazeScale(newSPntAStarShow.x, newSPntAStarShow.y, newTPntAStarShow.x, newTPntAStarShow.y);
			break;

		default:
			break;
		}

		return true;
	}
}


bool CDemo::adaptShowScaleZoomOutInterface(){
	return adaptShowScaleZoomOut();
}


bool CDemo::adaptShowScaleZoomOut(){
	// Zoom out twice to watch the global instance
	int scaleXCurr = 1;
	int scaleYCurr = 1;
	int deltaAddX = 1;
	int deltaAddY = 1;
	switch (mode)
	{
	case DEMO_NILL:
		break;
	case DEMO_START:
		if (1 == m_AStarFactorX && 1 == m_AStarFactorY){
			return false;
		}
		scaleXCurr = tPntAStarShow.x - sPntAStarShow.x;
		scaleYCurr = tPntAStarShow.y - sPntAStarShow.y;
		deltaAddX = scaleXCurr / 2;
		deltaAddY = scaleYCurr / 2;
		if (sPntAStarShow.x - deltaAddX >= 0 && tPntAStarShow.x + deltaAddX <= m_showImageXscale){
			sPntAStarShow.x -= deltaAddX;
			tPntAStarShow.x += deltaAddX;
			m_AStarFactorX /= 2;
		}
		else if (sPntAStarShow.x - deltaAddX < 0){
			sPntAStarShow.x = 0;
			tPntAStarShow.x += 2 * deltaAddX;
			if (tPntAStarShow.x > m_showImageXscale){
				tPntAStarShow.x = m_showImageXscale;
				m_AStarFactorX = 1;
			}
			else{
				m_AStarFactorX /= 2;
			}
		}
		else if (tPntAStarShow.x + deltaAddX > m_showImageXscale){
			tPntAStarShow.x = m_showImageXscale;
			sPntAStarShow.x -= 2 * deltaAddX;
			if (sPntAStarShow.x < 0){
				sPntAStarShow.x = 0;
				m_AStarFactorX = 1;
			}
			else{
				m_AStarFactorX /= 2;
			}
		}
		if (sPntAStarShow.y - deltaAddY >= 0 && tPntAStarShow.y + deltaAddY <= m_showImageYscale){
			sPntAStarShow.y -= deltaAddY;
			tPntAStarShow.y += deltaAddY;
			m_AStarFactorY /= 2;
		}
		else if (sPntAStarShow.y - deltaAddY < 0){
			sPntAStarShow.y = 0;
			tPntAStarShow.y += 2 * deltaAddY;
			if (tPntAStarShow.y > m_showImageYscale){
				tPntAStarShow.y = m_showImageYscale;
				m_AStarFactorY = 1;
			}
			else{
				m_AStarFactorY /= 2;
			}
		}
		else if (tPntAStarShow.y + deltaAddY > m_showImageYscale){
			tPntAStarShow.y = m_showImageYscale;
			sPntAStarShow.y -= 2 * deltaAddY;
			if (sPntAStarShow.y < 0){
				sPntAStarShow.y = 0;
				m_AStarFactorY = 1;
			}
			else{
				m_AStarFactorY /= 2;
			}
		}
		break;
	case DEMO_MAZE:
		if (1 == m_MazeFactorX && 1 == m_MazeFactorY){
			return false;
		}
		scaleXCurr = tPntMazeShow.x - sPntMazeShow.x;
		scaleYCurr = tPntMazeShow.y - sPntMazeShow.y;
		deltaAddX = scaleXCurr / 2;
		deltaAddY = scaleYCurr / 2;
		if (sPntMazeShow.x - deltaAddX >= 0 && tPntMazeShow.x + deltaAddX <= m_showImageXscaleMaze){
			sPntMazeShow.x -= deltaAddX;
			tPntMazeShow.x += deltaAddX;
			m_MazeFactorX /= 2;
		}
		else if (sPntMazeShow.x - deltaAddX < 0){
			sPntMazeShow.x = 0;
			tPntMazeShow.x += 2 * deltaAddX;
			if (tPntMazeShow.x > m_showImageXscaleMaze){
				tPntMazeShow.x = m_showImageXscaleMaze;
				m_MazeFactorX = 1;
			}
			else{
				m_MazeFactorX /= 2;
			}
		}
		else if (tPntMazeShow.x + deltaAddX > m_showImageXscaleMaze){
			tPntMazeShow.x = m_showImageXscaleMaze;
			sPntMazeShow.x -= 2 * deltaAddX;
			if (sPntMazeShow.x < 0){
				sPntMazeShow.x = 0;
				m_MazeFactorX = 1;
			}
			else{
				m_MazeFactorX /= 2;
			}
		}
		if (sPntMazeShow.y - deltaAddY >= 0 && tPntMazeShow.y + deltaAddY <= m_showImageYscaleMaze){
			sPntMazeShow.y -= deltaAddY;
			tPntMazeShow.y += deltaAddY;
			m_MazeFactorY /= 2;
		}
		else if (sPntMazeShow.y - deltaAddY < 0){
			sPntMazeShow.y = 0;
			tPntMazeShow.y += 2 * deltaAddY;
			if (tPntMazeShow.y > m_showImageYscaleMaze){
				tPntMazeShow.y = m_showImageYscaleMaze;
				m_MazeFactorY = 1;
			}
			else{
				m_MazeFactorY /= 2;
			}
		}
		else if (tPntMazeShow.y + deltaAddY > m_showImageYscaleMaze){
			tPntMazeShow.y = m_showImageYscaleMaze;
			sPntMazeShow.y -= 2 * deltaAddY;
			if (sPntMazeShow.y < 0){
				sPntMazeShow.y = 0;
				m_MazeFactorY = 1;
			}
			else{
				m_MazeFactorY /= 2;
			}
		}
		break;

	default:
		break;
	}



	return true;
}


bool CDemo::adaptShowScaleZoomInInterface(){
	return adaptShowScaleZoomIn();
}


bool CDemo::adaptShowScaleZoomIn(){
	// Zoom in to half of prime to watch the detail inner
	int deltaDivX = 1;
	int deltaDivY = 1;
	switch (mode)
	{
	case DEMO_NILL:
		break;
	case DEMO_START:
		if (sPntAStarShow.x == tPntAStarShow.x && sPntAStarShow.y == tPntAStarShow.y){
			return false;
		}
		deltaDivX = (tPntAStarShow.x - sPntAStarShow.x) / 4;
		deltaDivY = (tPntAStarShow.y - sPntAStarShow.y) / 4;
		if (deltaDivX > 0){
			sPntAStarShow.x += deltaDivX;
			tPntAStarShow.x -= deltaDivX;
			m_AStarFactorX *= 2;
			if (sPntAStarShow.x == tPntAStarShow.x){
				m_AStarFactorX = m_gameRect.Width();
			}
		}
		if (deltaDivY > 0){
			sPntAStarShow.y += deltaDivY;
			tPntAStarShow.y -= deltaDivY;
			m_AStarFactorY *= 2;
			if (sPntAStarShow.y == tPntAStarShow.y){
				m_AStarFactorY = m_gameRect.Height();
			}
		}
		break;
	case DEMO_MAZE:
		if (sPntMazeShow.x == tPntMazeShow.x && sPntMazeShow.y == tPntMazeShow.y){
			return false;
		}
		deltaDivX = (tPntMazeShow.x - sPntMazeShow.x) / 4;
		deltaDivY = (tPntMazeShow.y - sPntMazeShow.y) / 4;
		if (deltaDivX > 0){
			sPntMazeShow.x += deltaDivX;
			tPntMazeShow.x -= deltaDivX;
			m_AStarFactorX *= 2;
			if (sPntMazeShow.x == tPntMazeShow.x){
				m_AStarFactorX = m_gameRect.Width();
			}
		}
		if (deltaDivY > 0){
			sPntMazeShow.y += deltaDivY;
			tPntMazeShow.y -= deltaDivY;
			m_AStarFactorY *= 2;
			if (sPntMazeShow.y == tPntMazeShow.y){
				m_AStarFactorY = m_gameRect.Height();
			}
		}
		break;

	default:
		break;
	}


	return true;
}


bool CDemo::isPointInRect(const CPoint& point, const CRect& rect){
	if ((point.x <= rect.left && point.x >= rect.right || point.x <= rect.right && point.x >= rect.left) &&
		(point.y <= rect.top && point.y >= rect.bottom || point.y <= rect.bottom && point.y >= rect.top)){
		return true;
	}
	else{
		return false;
	}
}


void CDemo::shiftListValues(CList<int>& barrierList, int shiftX, int shiftY, int minValF, int maxValF, int minValS, int maxValS, bool isLimited){
	POSITION pos = barrierList.GetHeadPosition();
	if (0 != barrierList.GetCount() % 2){
		return;
	}
	if (isLimited){
		while (NULL != pos){
			barrierList.GetAt(pos) += shiftX;
			if (barrierList.GetAt(pos) < minValF){
				barrierList.GetAt(pos) = minValF;
			}
			else if (barrierList.GetAt(pos) > maxValF){
				barrierList.GetAt(pos) = maxValF;
			}
			barrierList.GetNext(pos);
			barrierList.GetAt(pos) += shiftY;
			if (barrierList.GetAt(pos) < minValS){
				barrierList.GetAt(pos) = minValS;
			}
			else if (barrierList.GetAt(pos) > maxValS){
				barrierList.GetAt(pos) = maxValS;
			}
			barrierList.GetNext(pos);
		}
	}
	else{
		while (pos != barrierList.GetTailPosition()){
			barrierList.GetAt(pos) += shiftX;
			barrierList.GetNext(pos);
			barrierList.GetAt(pos) += shiftY;
			barrierList.GetNext(pos);
		}
	}
}


void CDemo::setAStartFactor(int factor, int factorX, int factorY){
	m_AStarFactor = factor;
	m_AStarFactorX = factorX;
	m_AStarFactorY = factorY;
}


void CDemo::setAStarPrimeScale(int rowCount, int colCount){
	if (rowCount < 0 || rowCount > m_imageXscale ||
		colCount < 0 || colCount > m_imageYscale){
		setMazePrimeScale(m_imageXscale, m_imageYscale);
	}
	m_showImageXscale = rowCount;
	m_showImageYscale = colCount;
	setAStartScale(0, 0, m_showImageXscale, m_showImageYscale);
	memset(m_AStartData, 0, sizeof(int)* m_showImageXscale * m_showImageYscale);
	setAStartTarget(0, 0, m_showImageXscale - 1, m_showImageYscale - 1);
	m_isFreshAStart = true;
}


void CDemo::setAStartTarget(int sX, int sY, int tX, int tY){
	if (sX < 0 || sX >= m_showImageXscale || tX < 0 || tX >= m_showImageXscale ||
		sY < 0 || sY >= m_showImageYscale || tY < 0 || tY >= m_showImageYscale){
		setAStartTarget(0, 0, m_showImageXscale - 1, m_showImageYscale - 1);
	}
	m_AStarSEnd = CPoint(sX, sY);
	m_AStarTEnd = CPoint(tX, tY);
}


void CDemo::setAStartScale(int sX, int sY, int tX, int tY){
	if (sX < 0 || sX > m_showImageXscale || tX < 0 || tX > m_showImageXscale ||
		sY < 0 || sY > m_showImageYscale || tY < 0 || tY > m_showImageYscale){
		setAStartTarget(0, 0, m_showImageXscale, m_showImageYscale);
	}
	sPntAStarShow = CPoint(sX, sY);
	tPntAStarShow = CPoint(tX, tY);
}


void CDemo::initAStar(){
	setAStartFactor(1, 1, 1);
	showDetail = false;
	showDetailInner = false;

	/*
	setAStartScale(0, 0, m_imageXscale, m_imageYscale);
	m_showImageXscale = m_imageXscale;
	m_showImageYscale = m_imageYscale;
	memset(m_AStartData, 0, sizeof(int)* m_imageXscale * m_imageYscale);
	m_isFreshAStart = true;
	*/
	setAStarPrimeScale(m_imageXscale, m_imageYscale);
}


void CDemo::drawAStartHeuristicResult(CPaintDC &dc){
	if (m_isFreshAStart){
		AStartHeuristicAlgorithmTEST(dc);
		m_isFreshAStart = false;
	}
	else{
		AStartHeuristicShow(dc, m_AStartData, m_imageXscale);
	}
}


void CDemo::AStartHeuristicAlgorithmTEST(CPaintDC &dc){
	int routeEnd[4];
	int(*data)[1000] = m_AStartData;
	memset(m_AStartData, 0, sizeof(int) * 1000 * 1000);

	AStartHeuristicAddDefaultBarrier(data, m_imageXscale, routeEnd);
	AStartHeuristicAlgorithm(data, m_imageXscale, routeEnd, 4, NULL, NULL);
	AStartHeuristicShow(dc, data, m_imageXscale);

}


void CDemo::AStartHeuristicShow(CPaintDC &dc, const int const data[][1000], const int length)
{
	int iter;
	int showSX = 0;
	int showSY = 0;
	/// Adapt the horizonal show-scale style
	if ((tPntAStarShow.x - sPntAStarShow.x)*m_AStarFactorX < m_gameRect.Width() && (0 != sPntAStarShow.x || m_showImageXscale != tPntAStarShow.x)){
		int needHalfPixelCount = (m_gameRect.Width() - (tPntAStarShow.x - sPntAStarShow.x)*m_AStarFactorX) / m_AStarFactorX;
		if (sPntAStarShow.x >= needHalfPixelCount / 2 && (m_showImageXscale - tPntAStarShow.x) >= needHalfPixelCount / 2){
			sPntAStarShow.x -= needHalfPixelCount / 2;
			tPntAStarShow.x += needHalfPixelCount / 2;
		}
		else if (sPntAStarShow.x < needHalfPixelCount / 2){
			if ((needHalfPixelCount - sPntAStarShow.x) + tPntAStarShow.x <= m_showImageXscale){
				tPntAStarShow.x += (needHalfPixelCount - sPntAStarShow.x);
			}
			else{
				tPntAStarShow.x = m_showImageXscale;
			}
			sPntAStarShow.x = 0;
		}
		else if ((m_showImageXscale - tPntAStarShow.x) < needHalfPixelCount / 2){
			if (sPntAStarShow.x - (m_showImageXscale - tPntAStarShow.x) >= 0){
				sPntAStarShow.x -= (m_showImageXscale - tPntAStarShow.x);
			}
			else{
				sPntAStarShow.x = 0;
			}
			tPntAStarShow.x = m_showImageXscale;
		}
	}
	/// Adapt the verical show-scale style
	if ((tPntAStarShow.y - sPntAStarShow.y)*m_AStarFactorY < m_gameRect.Height() && (0 != sPntAStarShow.y || m_showImageYscale != tPntAStarShow.y)){
		int needHalfPixelCount = (m_gameRect.Height() - (tPntAStarShow.y - sPntAStarShow.y)*m_AStarFactorY) / m_AStarFactorY;
		if (sPntAStarShow.y >= needHalfPixelCount / 2 && (m_showImageYscale - tPntAStarShow.y) >= needHalfPixelCount / 2){
			sPntAStarShow.y -= needHalfPixelCount / 2;
			tPntAStarShow.y += needHalfPixelCount / 2;
		}
		else if (sPntAStarShow.y < needHalfPixelCount / 2){
			if ((needHalfPixelCount - sPntAStarShow.y) + tPntAStarShow.y <= m_showImageYscale){
				tPntAStarShow.y += (needHalfPixelCount - sPntAStarShow.y);
			}
			else{
				tPntAStarShow.y = m_showImageYscale;
			}
			sPntAStarShow.y = 0;
		}
		else if ((m_showImageYscale - tPntAStarShow.y) < needHalfPixelCount / 2){
			if (sPntAStarShow.y - (m_showImageYscale - tPntAStarShow.y) >= 0){
				sPntAStarShow.y -= (m_showImageYscale - tPntAStarShow.y);
			}
			else{
				sPntAStarShow.y = 0;
			}
			tPntAStarShow.y = m_showImageYscale;
		}
	}
	/// Show the barriers and the route from start point to target point.
	for (int i = sPntAStarShow.y; i < tPntAStarShow.y; i++){
		for (int j = sPntAStarShow.x; j < tPntAStarShow.x; j++){
			if (data[i][j] >= 0 && data[i][j] < 10){
				iter = data[i][j];
			}
			else if (data[i][j] >= 10){
				iter = data[i][j] % 10;
			}
			else{
				iter = -data[i][j];
			}
			for (int k = 0; k < m_AStarFactorX; k++){
				for (int r = 0; r < m_AStarFactorY; r++){
					dc.SetPixel(showSX + m_AStarFactorX * (j - sPntAStarShow.x) + k,
						showSY + m_AStarFactorY * (i - sPntAStarShow.y) + r,
						m_AStartColors[iter]);
				}
			}
		}
	}
}


void CDemo::AStartHeuristicAlgorithmInit(int data[][1000], int length){
	memset(&data, 0, sizeof(int)*length * 1000);
}


void CDemo::AStartHeuristicAddDefaultBarrier(int data[][1000], int length, int* routeEnd){
	int row = 200, column = 1000;
	int i = 0;
	int j = 0;
	int k = 0;
	int barrierSX = 0;
	int barrierSY = 0;
	int barrierTX = 0;
	int barrierTY = 0;
	int barrierXY[4];
	int outRect[4];
	bool setBarrierCurr = false;

	CArray<bool> barrierSet;
	CList<int> barrierList;
	int barrierCount = 11;
	setBarrierCurr = true;
	for (i = 0; i < barrierCount; i++){
		barrierSet.Add(true);
	}

	/// barrier 1, horizonal direction
	barrierSX = row*(1.0 / 5.0);
	barrierTX = barrierSX + 1;
	barrierSY = column*(7.5 / 10);
	barrierTY = column*(8.1 / 10);
	barrierXY[0] = barrierSX;
	barrierXY[1] = barrierTX;
	barrierXY[2] = barrierSY;
	barrierXY[3] = barrierTY;
	if (barrierSet[0])
	{
		barrierList.AddTail(barrierXY[0]);
		barrierList.AddTail(barrierXY[2]);
		barrierList.AddTail(barrierXY[1]);
		barrierList.AddTail(barrierXY[3]);
		AStartHeuristicAlgorithmCreateBarrier(data, barrierXY);
	}
	/// barrier 2, horizonal direction
	barrierSX = row*(1.0 / 4.0);
	barrierTX = barrierSX + 1;
	barrierSY = column*(8.1 / 10) + 1;
	barrierTY = barrierSY + 60;
	barrierXY[0] = barrierSX;
	barrierXY[1] = barrierTX;
	barrierXY[2] = barrierSY;
	barrierXY[3] = barrierTY;
	if (barrierSet[1])
	{
		barrierList.AddTail(barrierXY[0]);
		barrierList.AddTail(barrierXY[2]);
		barrierList.AddTail(barrierXY[1]);
		barrierList.AddTail(barrierXY[3]);
		AStartHeuristicAlgorithmCreateBarrier(data, barrierXY);
	}
	/// barrier 3, horizonal direction
	barrierSX = row*(1.0 / 5.0) - 28;
	barrierTX = barrierSX + 1;
	barrierSY = column*(8.1 / 10) + 1;
	barrierTY = barrierSY + 40;
	barrierXY[0] = barrierSX;
	barrierXY[1] = barrierTX;
	barrierXY[2] = barrierSY;
	barrierXY[3] = barrierTY;
	if (barrierSet[2])
	{
		barrierList.AddTail(barrierXY[0]);
		barrierList.AddTail(barrierXY[2]);
		barrierList.AddTail(barrierXY[1]);
		barrierList.AddTail(barrierXY[3]);
		AStartHeuristicAlgorithmCreateBarrier(data, barrierXY);
	}
	/// barrier 4, vertical direction
	barrierSX = row*(1.0 / 5.0 - 1.0 / 13.0) - 20;
	barrierTX = row*(1.2 / 2.3);
	barrierSY = barrierSY - 1;
	barrierTY = barrierSY + 1;
	barrierXY[0] = barrierSX;
	barrierXY[1] = barrierTX;
	barrierXY[2] = barrierSY;
	barrierXY[3] = barrierTY;
	if (barrierSet[3])
	{
		barrierList.AddTail(barrierXY[0]);
		barrierList.AddTail(barrierXY[2]);
		barrierList.AddTail(barrierXY[1]);
		barrierList.AddTail(barrierXY[3]);
		AStartHeuristicAlgorithmCreateBarrier(data, barrierXY);
	}
	/// barrier 5, vertical direction
	barrierSX = row*(1.0 / 5.0) - 20 - 8;
	barrierTX = row*(1.0 / 5.0) - 12;	// row*(1.0 / 5.0)
	barrierSY = column*(8.1 / 10) - 30;
	barrierTY = barrierSY + 1;
	barrierXY[0] = barrierSX;
	barrierXY[1] = barrierTX;
	barrierXY[2] = barrierSY;
	barrierXY[3] = barrierTY;
	if (barrierSet[4])
	{
		barrierList.AddTail(barrierXY[0]);
		barrierList.AddTail(barrierXY[2]);
		barrierList.AddTail(barrierXY[1]);
		barrierList.AddTail(barrierXY[3]);
		AStartHeuristicAlgorithmCreateBarrier(data, barrierXY);
	}
	/// Show the local detail information of rectangle scale region
	int outPutRowS = 0;
	int outPutRowT = 0;
	int outPutColS = 0;
	int outPutColT = 0;
	outPutRowS = row*(1.0 / 5.0) - 40;
	outPutRowT = row*(1.2 / 2.3) + 40;
	outPutColS = column*(8.1 / 10) - 70;
	outPutColT = column*(8.1 / 10) + 80;
	if (outPutRowS < 0)
	{
		outPutRowS = 0;
	}
	if (outPutColS < 0)
	{
		outPutColS = 0;
	}
	if (outPutRowT > row)
	{
		outPutRowT = row;
	}
	if (outPutColT > column)
	{
		outPutColT = column;
	}
	outRect[0] = outPutRowS;
	outRect[1] = outPutRowT;
	outRect[2] = outPutColS;
	outRect[3] = outPutColT;
	/// set the route start and end points.
	routeEnd[0] = row*(1.2 / 2.3) + 30;
	routeEnd[1] = column*(8.1 / 10) - 40;
	routeEnd[2] = row*(1.0 / 5.0) - 20;
	routeEnd[3] = column*(8.1 / 10) + 50;
	// To judge if the index is legal
	while (routeEnd[0] < row && routeEnd[1] > 0 && data[routeEnd[0]][routeEnd[1]] < 0)
	{
		routeEnd[0] ++;
		routeEnd[1] --;
	}
	// To judge if the index is legal
	while (routeEnd[2] > 0 && routeEnd[3] < column && data[routeEnd[2]][routeEnd[3]] < 0)
	{
		routeEnd[2] --;
		routeEnd[3] ++;
	}

	//AStartHeuristicAlgorithmShow(data, outRect, routeEnd);	/// show the info
	//AStartHeuristicAlgorithm(data, row, routeEnd, 4, NULL, NULL); 			/// find the path 
	//AStartHeuristicAlgorithmShow(data, outRect, routeEnd);	/// show the info

	//AStartHeuristicAlgorithmClearRoute(data, row);
	routeEnd[2] += 0;	/// terminate end point move top  1 steps.
	routeEnd[3] -= 20;	/// terminate end point move left 8 steps.
	//AStartHeuristicAlgorithmShow(data, outRect, routeEnd);	/// show the info
	//AStartHeuristicAlgorithm(data, row, routeEnd, 4, NULL, NULL); 			/// find the path 
	//AStartHeuristicAlgorithmShow(data, outRect, routeEnd);	/// show the info

	/// barrier 6, vertical direction
	barrierSX = row*(1.0 / 5.0) - 32;
	barrierTX = row*(1.0 / 5.0) - 16;
	barrierSY = column*(8.1 / 10) + 42;
	barrierTY = barrierSY + 1;
	barrierXY[0] = barrierSX;
	barrierXY[1] = barrierTX;
	barrierXY[2] = barrierSY;
	barrierXY[3] = barrierTY;
	if (barrierSet[5])
	{
		barrierList.AddTail(barrierXY[0]);
		barrierList.AddTail(barrierXY[2]);
		barrierList.AddTail(barrierXY[1]);
		barrierList.AddTail(barrierXY[3]);
		AStartHeuristicAlgorithmCreateBarrier(data, barrierXY);
	}
	//AStartHeuristicAlgorithmClearRoute(data, row);
	routeEnd[2] += 0;	/// terminate end point move down 1  steps.
	routeEnd[3] -= 10;	/// terminate end point move left 18 steps.
	//AStartHeuristicAlgorithmShow(data, outRect, routeEnd);	/// show the info
	//AStartHeuristicAlgorithm(data, row, routeEnd, 4, NULL, NULL); 		/// find the path   outRect, routeEnd
	//AStartHeuristicAlgorithmShow(data, outRect, routeEnd);	/// show the info

	/// barrier 7, horizonal direction
	barrierSX = row*(1.0 / 5.0) - 15;
	barrierTX = row*(1.0 / 5.0) - 14;
	barrierSY = column*(8.1 / 10) + 42 - 21;
	barrierTY = column*(8.1 / 10) + 42 + 10;
	barrierXY[0] = barrierSX;
	barrierXY[1] = barrierTX;
	barrierXY[2] = barrierSY;
	barrierXY[3] = barrierTY;
	if (barrierSet[6])
	{
		barrierList.AddTail(barrierXY[0]);
		barrierList.AddTail(barrierXY[2]);
		barrierList.AddTail(barrierXY[1]);
		barrierList.AddTail(barrierXY[3]);
		AStartHeuristicAlgorithmCreateBarrier(data, barrierXY);
	}
	//AStartHeuristicAlgorithmClearRoute(data, row);
	routeEnd[2] -= 3;	/// terminate end point move down 1  steps.
	routeEnd[3] += 5;	/// terminate end point move left 18 steps.
	//AStartHeuristicAlgorithmShow(data, outRect, routeEnd);	/// show the info
	//AStartHeuristicAlgorithm(data, row, routeEnd, 4, NULL, NULL); 		/// find the path   outRect, routeEnd
	//AStartHeuristicAlgorithmShow(data, outRect, routeEnd);	/// show the info

	/// barrier 8, vertical direction
	barrierSX = row*(1.0 / 5.0) - 24;
	barrierTX = row*(1.0 / 5.0) - 8;
	barrierSY = column*(8.1 / 10) + 42 - 21 - 2;
	barrierTY = barrierSY + 1;
	barrierXY[0] = barrierSX;
	barrierXY[1] = barrierTX;
	barrierXY[2] = barrierSY;
	barrierXY[3] = barrierTY;
	if (barrierSet[7])
	{
		barrierList.AddTail(barrierXY[0]);
		barrierList.AddTail(barrierXY[2]);
		barrierList.AddTail(barrierXY[1]);
		barrierList.AddTail(barrierXY[3]);
		AStartHeuristicAlgorithmCreateBarrier(data, barrierXY);
	}
	/// barrier 9, horizonal direction
	barrierSX = row*(1.0 / 5.0) - 8 + 1;
	barrierTX = barrierSX + 1;
	barrierSY = column*(8.1 / 10) + 42 - 21 - 2 - 3;
	barrierTY = column*(8.1 / 10) + 42 - 21 - 2 + 7;
	barrierXY[0] = barrierSX;
	barrierXY[1] = barrierTX;
	barrierXY[2] = barrierSY;
	barrierXY[3] = barrierTY;
	if (barrierSet[8])
	{
		barrierList.AddTail(barrierXY[0]);
		barrierList.AddTail(barrierXY[2]);
		barrierList.AddTail(barrierXY[1]);
		barrierList.AddTail(barrierXY[3]);
		AStartHeuristicAlgorithmCreateBarrier(data, barrierXY);
	}
	//AStartHeuristicAlgorithmClearRoute(data, row);
	routeEnd[2] += 1;	/// terminate end point move down  07 steps.
	routeEnd[3] += 8;	/// terminate end point move right 08 steps.
	//AStartHeuristicAlgorithmShow(data, outRect, routeEnd);	/// show the info
	//AStartHeuristicAlgorithm(data, row, routeEnd, 4, outRect, routeEnd); 		/// find the path   outRect, routeEnd
	//AStartHeuristicAlgorithmShow(data, outRect, routeEnd, true);	/// show the info


	/// barrier 10, vertical direction
	barrierSX = row*(1.0 / 5.0) - 24 + 18;
	barrierTX = row*(1.0 / 5.0) - 8 + 18;
	barrierSY = column*(8.1 / 10) + 42 - 21 - 2 + 30;
	barrierTY = barrierSY + 1;
	barrierXY[0] = barrierSX;
	barrierXY[1] = barrierTX;
	barrierXY[2] = barrierSY;
	barrierXY[3] = barrierTY;
	if (barrierSet[9])
	{
		barrierList.AddTail(barrierXY[0]);
		barrierList.AddTail(barrierXY[2]);
		barrierList.AddTail(barrierXY[1]);
		barrierList.AddTail(barrierXY[3]);
		AStartHeuristicAlgorithmCreateBarrier(data, barrierXY);
	}
	/// barrier 11, horizonal direction
	barrierSX = row*(1.2 / 2.3) + 5;
	barrierTX = barrierSX + 1;
	barrierSY = column*(8.1 / 10) + 40 - 80;
	barrierTY = column*(8.1 / 10) + 40 + 30;
	barrierXY[0] = barrierSX;
	barrierXY[1] = barrierTX;
	barrierXY[2] = barrierSY;
	barrierXY[3] = barrierTY;
	if (barrierSet[10])
	{
		barrierList.AddTail(barrierXY[0]);
		barrierList.AddTail(barrierXY[2]);
		barrierList.AddTail(barrierXY[1]);
		barrierList.AddTail(barrierXY[3]);
		AStartHeuristicAlgorithmCreateBarrier(data, barrierXY);
	}
	//AStartHeuristicAlgorithmClearRoute(data, row);
	routeEnd[2] += 4;	/// terminate end point move down  07 steps.
	routeEnd[3] -= 4;	/// terminate end point move right 08 steps.
	//AStartHeuristicAlgorithmShow(data, outRect, routeEnd);	/// show the info
	//AStartHeuristicAlgorithm(data, row, routeEnd, 4, NULL, NULL); 		/// find the path   outRect, routeEnd
	//AStartHeuristicAlgorithmShow(data, outRect, routeEnd, true);	/// show the info

	if (0 == barrierList.GetCount() % 4){
		shiftListValues(barrierList, 200, -200, 0, m_imageXscale, 0, m_imageYscale, true);
		AStartHeuristicAddBarrierList(data, length, barrierList);
		shiftListValues(barrierList, 0, -200, 0, m_imageXscale, 0, m_imageYscale, true);
		AStartHeuristicAddBarrierList(data, length, barrierList);
		shiftListValues(barrierList, 200, 0, 0, m_imageXscale, 0, m_imageYscale, true);
		AStartHeuristicAddBarrierList(data, length, barrierList);
	}

}


void CDemo::AStartHeuristicAddBarrierList(int data[][1000], int length, const CList<int>& barrierList){

	POSITION pos = barrierList.GetHeadPosition();
	if (0 != barrierList.GetCount() % 4){
		return;
	}
	int barrierSX = 0;
	int barrierSY = 0;
	int barrierTX = 0;
	int barrierTY = 0;

	while (NULL != pos){
		barrierSX = barrierList.GetAt(pos);
		barrierList.GetNext(pos);
		barrierSY = barrierList.GetAt(pos);
		barrierList.GetNext(pos);
		barrierTX = barrierList.GetAt(pos);
		barrierList.GetNext(pos);
		barrierTY = barrierList.GetAt(pos);
		barrierList.GetNext(pos);
		AStartHeuristicAlgorithmCreateBarrier(data, barrierSX, barrierSY, barrierTX, barrierTY);
	}
}


void CDemo::AStartHeuristicAlgorithmCreateBarrier(int data[][1000], const int const scale[]){
	int i = 0;
	int j = 0;
	for (i = scale[0]; i <= scale[1]; i++){
		for (j = scale[2]; j <= scale[3]; j++)
		{
			data[i][j] = -1;
		}
	}
}


void CDemo::AStartHeuristicAlgorithmCreateBarrier(int data[][1000], const int sX, const int sY, const int tX, const int tY){
	int i = 0;
	int j = 0;
	for (i = sX; i <= tX; i++){
		for (j = sY; j <= tY; j++)
		{
			data[i][j] = -1;
		}
	}
}


void CDemo::AStartHeuristicAlgorithmClearRoute(int data[][1000], int length){
	int i = 0;
	int j = 0;
	for (i = 0; i < length; i++){
		for (j = 0; j < 1000; j++)
		{
			if (data[i][j] > 0){
				data[i][j] = 0;
			}
		}
	}
}


bool CDemo::AStartHeuristicAlgorithm(int data[][1000], int rowNum, const int* const stInfo, int count, const int const scale[], const int const routeEnd[]){

#pragma region statement of variables
	showDetail = false;			/// corner info
	showDetailInner = false;		/// Debug  info
	int i = 0, j = 0, k = 0;
	int col = 1000;
	int row = rowNum;
	int sX = stInfo[0];
	int sY = stInfo[1];
	int tX = stInfo[2];
	int tY = stInfo[3];

	bool findTarget = false;
	bool findNext = false;
	int currStepX = 0;
	int currStepY = 0;
	int nextStepX = 0;
	int nextStepY = 0;
	currStepX = sX;
	currStepY = sY;
	if (tX == currStepX && tY == currStepY)
	{
		findTarget = true;
	}
	int stateH = -1;
	int stateV = -1;
	int deltaXToT = -1;
	int deltaYToT = -1;
	double angle = 0.1;

	currStepX = sX;
	currStepY = sY;
	complex cNum(3.0, 4.0);
	int distance = 0;
	int distanceCurr = 0;
	int distanceAss = 0;
	int topLimitWallLengthTemp = 0;	/// debug test used, to record the maximum steps to walk form 0
	int lastState = 0;
	bool stateChanged = false;
	bool isRetrogradeMotion = false;
	bool isOnCoastMiddle = false;	/// Flag whether current point is on the coast middle
	bool isTrackOffCoast = false;	/// Flag whether current are tracking the off coast state
	bool isCoastConstraint = false;	/// Flag whether following behavior are with constraint on coast
	int aroundRouteNodeNum = 0;
	CList<Int2DNode> offCoastRoute;
	bool isWallExisted = false;
	int endWallNum = 0;
	int horizonalWallNum = 0;
	int verticalWallNum = 0;
#pragma endregion


	while (!findTarget)
	{


#pragma region pre-deal
		isWallExisted = false;		/// flag whether there are coast around current point
		endWallNum = 0;				/// number of coast point around current point
		horizonalWallNum = 0;		/// number of horizonal wall
		verticalWallNum = 0;		/// number of vertical wall
		if (isCoastConstraint)
		{
			for (i = -1; i < 2; i++)
			{
				for (j = -1; j < 2; j++)
				{
					if (-1 == data[currStepX + i][currStepY + j])
					{
						/// Judge whether there exist the wall
						horizonalWallNum += i;	/// single-side is available, or will be balanced out
						verticalWallNum += j;
						endWallNum++;
						isWallExisted = true;
					}
				}
			}
			goto WalkCoast;
		}

		distance = 0;
		findNext = false;			/// flag whether it has found next point
		deltaXToT = tX - currStepX;
		deltaYToT = tY - currStepY;
		aroundRouteNodeNum = 0;			/// number of passed points
		isRetrogradeMotion = false;
		if (deltaXToT > 0)
		{
			distance += deltaXToT;
		}
		else{
			distance -= deltaXToT;
		}
		if (deltaYToT > 0)
		{
			distance += deltaYToT;
		}
		else{
			distance -= deltaYToT;
		}
		distanceCurr = distance;
		for (i = -1; i < 2; i++)
		{
			for (j = -1; j < 2; j++)
			{
				/// Greedy strategy, distance is added by two distance of both horizonal and vertical direction.
				if (i == 0 && j == 0)
				{
					continue;
				}
				distanceAss = 0;
				deltaXToT = tX - currStepX - i;
				deltaYToT = tY - currStepY - j;
				if (deltaXToT > 0)
				{
					distanceAss += deltaXToT;
				}
				else{
					distanceAss -= deltaXToT;
				}
				if (deltaYToT > 0)
				{
					distanceAss += deltaYToT;
				}
				else{
					distanceAss -= deltaYToT;
				}
				if (currStepX + i >= 0 && currStepX + i < row &&
					currStepY + j >= 0 && currStepY + j < col){
					if (0 == data[currStepX + i][currStepY + j])
					{
						/// fresh the minimum distance to target point
						if (distanceAss < distance){
							findNext = true;
							distance = distanceAss;
							stateH = i;
							stateV = j;
						}
					}
					else if (data[currStepX + i][currStepY + j] > 0){
						/// To remember the number of statistics of passed point around current point 
						aroundRouteNodeNum++;
						if (distanceAss <= distanceCurr &&  !isRetrogradeMotion){
							/// Pre-deal the phenomenon of backtrace, and then give coasting-walk restrictive practices
							isRetrogradeMotion = true;
							//goto NextCode;
						}
					}
					else if (-1 == data[currStepX + i][currStepY + j])
					{
						/// Judge whether there exist the wall
						horizonalWallNum += i;	/// single-side is available, or will be balanced out
						verticalWallNum += j;
						endWallNum++;
						isWallExisted = true;
					}
				}
			}
		}
#pragma endregion


#pragma region deal-without-wall Get-next-step-1
	NextCode:
		if (findNext){

			/// 当存在贪心下一步落脚点时，需要考量选择贪心行走还是贴壁爬行
			/// 如果目前正贴壁爬行，需要判断下一步贴壁爬行的话是否在靠近终点，如果是，则贪心，否，则贴壁。
			/// 考量贴壁行走与贪心行走的 趋势是否相同，相同则贪心行走，否则贴壁爬行。。。。。。。。。

			if (isWallExisted && isRetrogradeMotion){
				;
			}
			else if (tX == currStepX + stateH && tY == currStepY + stateV){
				data[currStepX][currStepY] = 5;
				findTarget = true;
				break;
			}
			else{
				data[currStepX][currStepY] = 5;
				lastState = data[currStepX][currStepY];
				if (isOnCoastMiddle){
					/// Note the point of off coast
					offCoastRoute.AddTail({ currStepX, currStepY });
					isTrackOffCoast = true;
				}
				else if (isTrackOffCoast){
					/// wander  and  in shore
					/// 从岸之中间离岸开始漂泊，可能非法，追踪之
					/// 从岸之码头离岸开始漂泊，一般合法，放任之
					/// 从岸之中间离岸转为漂泊的情况：记录离岸点开始的漂泊路径，链表记录。
					///							    如果能再次安全着陆或者到达终点，则链表路径安全，可释放。
					///							    如果下次着陆时不安全，则链表无效，原路返回，继续贴壁行走。
					offCoastRoute.AddTail({ currStepX, currStepY });
					if (aroundRouteNodeNum >= 3){
						/// 此时靠岸或者靠航线（此处为靠航线），非法离岸，遣返之，并限制其行为
						/// 追踪岸之中间离岸是否非法，此处判断，或遣返，或放行
						/// 回溯处理
						POSITION pos = offCoastRoute.GetHeadPosition();
						while (NULL != pos)
						{
							data[offCoastRoute.GetAt(pos).X][offCoastRoute.GetAt(pos).Y] = 3;
							offCoastRoute.GetNext(pos);
						}
						isCoastConstraint = false;
						isTrackOffCoast = false;
						isCoastConstraint = true;
						currStepX = offCoastRoute.GetHead().X;
						currStepY = offCoastRoute.GetHead().Y;
						offCoastRoute.RemoveAll();
						continue;
					}
					else if (endWallNum > 0)
					{
						/// Go on, Restriction lifted
						isCoastConstraint = false;
						isTrackOffCoast = false;
						offCoastRoute.RemoveAll();
					}
				}
				currStepX += stateH;
				currStepY += stateV;
				isOnCoastMiddle = false;
				continue;
			}
		}
#pragma endregion


#pragma region deal-with-the-wall

	WalkCoast :
		stateH = 0;
		stateV = 0;
		if (isWallExisted)
		{
			if (3 == verticalWallNum || -3 == verticalWallNum)
			{
				// 180 degree angle, vertical direction
				/// Only right or left has single-wall
				/// Divided into both direction search
				topLimitWallLengthTemp++;
				if (0 == data[currStepX - 1][currStepY])
				{
					stateH = -1;
				}
				else if (0 == data[currStepX + 1][currStepY]){
					stateH = 1;
				}
				else{
					break;
				}
				data[currStepX][currStepY] = 6;
				lastState = data[currStepX][currStepY];
				currStepX += stateH;
				currStepY += stateV;
				isOnCoastMiddle = true;
				continue;
			}
			else if (3 == horizonalWallNum || -3 == horizonalWallNum)
			{
				/// 180 degree angle, horizonal direction
				/// Only top or bottom has single-wall
				/// Divided into both direction search
				topLimitWallLengthTemp++;
				if (0 == data[currStepX][currStepY - 1])
				{
					stateV = -1;
				}
				else if (0 == data[currStepX][currStepY + 1]){
					stateV = 1;
				}
				else{
					break;
				}
				data[currStepX][currStepY] = 6;
				lastState = data[currStepX][currStepY];
				currStepX += stateH;
				currStepY += stateV;
				isOnCoastMiddle = true;
				continue;
			}
			else if ((2 == verticalWallNum || -2 == verticalWallNum) &&
				(2 == horizonalWallNum || -2 == horizonalWallNum))
			{
				// 90 degree corner
				if (2 == verticalWallNum && 2 == horizonalWallNum)
				{
					if (data[currStepX - 1][currStepY] > 0)
					{
						stateV = -1;
					}
					else{
						stateH = -1;
					}
				}
				else if (-2 == verticalWallNum && -2 == horizonalWallNum)
				{
					if (data[currStepX + 1][currStepY] > 0)
					{
						stateV = 1;
					}
					else{
						stateH = 1;
					}
				}
				else if (2 == verticalWallNum && -2 == horizonalWallNum)
				{
					if (data[currStepX + 1][currStepY] > 0)
					{
						stateV = -1;
					}
					else{
						stateH = 1;
					}
				}
				else if (-2 == verticalWallNum && 2 == horizonalWallNum){
					if (data[currStepX - 1][currStepY] > 0)
					{
						stateV = 1;
					}
					else{
						stateH = -1;
					}
				}
				topLimitWallLengthTemp = 0;
				data[currStepX][currStepY] = 9;
				lastState = data[currStepX][currStepY];
				currStepX += stateH;
				currStepY += stateV;
				isOnCoastMiddle = false;
				continue;
			}
			else if (((2 == verticalWallNum || -2 == verticalWallNum) &&
				(1 == horizonalWallNum || -1 == horizonalWallNum))
				||
				((1 == verticalWallNum || -1 == verticalWallNum) &&
				(2 == horizonalWallNum || -2 == horizonalWallNum)))
			{
				// 270 degree corner, i.e. end of wall
				if (1 == verticalWallNum && 2 == horizonalWallNum)
				{
					if (data[currStepX][currStepY + 1] > 0)
					{
						// enter the left-up corner 001
						stateH = 1;
						stateV = -1;
					}
					else{
						// leave off the left-up corner 001
						stateV = 1;
					}
				}
				else if (1 == verticalWallNum && -2 == horizonalWallNum)
				{
					if (data[currStepX][currStepY + 1] > 0)
					{
						// enter the left-down corner 002
						stateH = -1;
						stateV = -1;
					}
					else{
						// leave off the left-down corner 002
						stateV = 1;
					}
				}
				else if (2 == verticalWallNum && -1 == horizonalWallNum)
				{
					if (data[currStepX - 1][currStepY] > 0)
					{
						// enter the down-right corner 003
						stateH = 1;
						stateV = 1;
					}
					else{
						// leave off the down-right corner 003
						stateH = -1;
					}
				}
				else if (-2 == verticalWallNum && -1 == horizonalWallNum)
				{
					if (data[currStepX - 1][currStepY] > 0)
					{
						// enter the down-left corner 004
						stateH = 1;
						stateV = -1;
					}
					else{
						// leave off the down-left corner 004
						stateH = -1;
					}
				}
				else if (-1 == verticalWallNum && -2 == horizonalWallNum)
				{
					if (data[currStepX][currStepY - 1] > 0)
					{
						// enter the right-down corner 005
						stateH = -1;
						stateV = 1;
					}
					else{
						// leave off the right-down corner 005
						stateV = -1;
					}
				}
				else if (-1 == verticalWallNum && 2 == horizonalWallNum)
				{
					if (data[currStepX][currStepY - 1] > 0)
					{
						// enter the right-up corner 006
						stateH = 1;
						stateV = 1;
					}
					else{
						// leave off the right-up corner 006
						stateV = -1;
					}
				}
				else if (-2 == verticalWallNum && 1 == horizonalWallNum)
				{
					if (data[currStepX + 1][currStepY] > 0)
					{
						// enter the up-right corner 007
						stateH = -1;
						stateV = -1;
					}
					else{
						// leave off the up-right corner 007
						stateH = 1;
					}
				}
				else if (2 == verticalWallNum && 1 == horizonalWallNum)
				{
					if (data[currStepX + 1][currStepY] > 0)
					{
						// enter the up-left corner 008
						stateH = -1;
						stateV = 1;
					}
					else{
						// leave off the up-left corner 008
						stateH = -1;
					}
				}
				data[currStepX][currStepY] = 8;
				lastState = data[currStepX][currStepY];
				currStepX += stateH;
				currStepY += stateV;
				isOnCoastMiddle = false;
				if (isCoastConstraint){
					isCoastConstraint = false;
				}
				continue;
			}
		}
#pragma endregion


#pragma region deal-without-wall Get-next-step-2
		/// Greedy strategy, to get the next step by judge the angle of terminate point VS start point.
		deltaXToT = tX - currStepX;
		deltaYToT = tY - currStepY;
		angle = cNum.arg();
		stateH = 0;
		stateV = 0;
		if (angle > 7.0*pi / 8 || angle < -7.0*pi / 8)
		{
			stateH = -1;
		}
		else if (angle > -7.0*pi / 8 && angle < -5.0*pi / 8)
		{
			stateH = -1;
			stateV = -1;
		}
		else if (angle > -5.0*pi / 8 && angle < -3.0*pi / 8)
		{
			stateV = -1;
		}
		else if (angle > -3.0*pi / 8 && angle < -pi / 8)
		{
			stateH = 1;
			stateV = -1;
		}
		else if (angle > -pi / 8 && angle < pi / 8)
		{
			stateH = 1;
		}
		else if (angle > pi / 8 && angle < 3.0*pi / 8)
		{
			stateH = 1;
			stateV = 1;
		}
		else if (angle > 3.0*pi / 8 && angle < 5.0*pi / 8)
		{
			stateV = 1;
		}
		else if (angle > 5.0*pi / 8 && angle < 7.0*pi / 8)
		{
			stateH = -1;
		}
		if (currStepX + stateH >= 0 &&
			currStepX + stateH < row &&
			currStepY + stateV >= 0 &&
			currStepY + stateV < col){

			currStepX += stateH;
			currStepY += stateV;
			data[currStepX][currStepY] = 5;
		}
		if (tX == currStepX && tY == currStepY)
		{
			findTarget = true;
		}
		continue;
#pragma endregion


	}

CodeOver:
	return true;
}


void CDemo::initMaze(){

	m_mazeRowMax = 1024;
	m_mazeColMax = 1024;
	m_routeFlagColor = 6;
	m_singleRouteFlagColor = 9;
	m_colorMazeEnd = RGB(0xFF, 0xFF, 0xFF);
	setMazeFactor(2, 2, 2);

	/*
	m_mazeRow = 400;
	m_mazeCol = 400;
	m_showImageXscaleMaze = m_mazeRow;
	m_showImageYscaleMaze = m_mazeCol;
	m_mazeEntr = CPoint(0, 0);
	m_mazeExit = CPoint(399, 399);
	*/
	setMazePrimeScale(400, 400);
}


void CDemo::setMazePrimeScale(int rowCount, int colCount){
	if (rowCount < 0 || rowCount > m_mazeRowMax ||
		colCount < 0 || colCount > m_mazeColMax){
		setMazePrimeScale(400, 400);
	}
	m_mazeRow = rowCount;
	m_mazeCol = colCount;
	m_showImageXscaleMaze = m_mazeRow;
	m_showImageYscaleMaze = m_mazeCol;
	setMazeScale(0, 0, m_mazeRow, m_mazeCol);
	memset(m_MazeData, 0, sizeof(int) * m_mazeRowMax * m_mazeColMax);
	memset(m_MazeFlag, 0, sizeof(int) * m_mazeRowMax * m_mazeColMax);
	setMazeTarget(0, 0, m_mazeRow - 1, m_mazeCol - 1);
	isFreshMazeRslt = true;
	defaultMazeLayout();
}


void CDemo::setMazeScale(int sX, int sY, int tX, int tY){
	if (sX < 0 || sX >= m_mazeRowMax || tX < 0 || tX >= m_mazeRowMax ||
		sY < 0 || sY >= m_mazeColMax || tY < 0 || tY >= m_mazeColMax ||
		sX > tX || sY > tY){
		setMazeScale(0, 0, 400, 400);
	}
	sPntMazeShow = CPoint(sX, sY);
	tPntMazeShow = CPoint(tX, tY);
}


void CDemo::setMazeTarget(int sX, int sY, int tX, int tY){
	if (sX < 0 || sX >= m_mazeRow || tX < 0 || tX >= m_mazeRow ||
		sY < 0 || sY >= m_mazeCol || tY < 0 || tY >= m_mazeCol){
		setMazeTarget(0, 0, m_mazeRow - 1, m_mazeCol - 1);
		return;
	}
	m_mazeEntr = CPoint(sX, sY);
	m_mazeExit = CPoint(tX, tY);
}


void CDemo::setMazeFactor(int factor, int factorX, int factorY){
	m_MazeFactor = factor;
	m_MazeFactorX = factorX;
	m_MazeFactorY = factorY;
}


void CDemo::defaultMazeLayout(int mazeMode){
	memset(m_MazeData, 0, sizeof(int) * m_mazeRowMax * m_mazeColMax);
	if (-1 == mazeMode){
		defaultMazeLayoutFirst();
	}
	else{
		defaultPermutaionMazeMode(mazeMode);
	}
}


void CDemo::defaultMazeLayoutFirst(){
	int i = 0;
	int j = 0;
	int k = 0;
	int kRowS = 0;
	int kRowT = m_mazeRow / 2;
	int kRowSPre = 0;
	int kRowTPre = 0;
	int deltaS = 0;
	int deltaT = 0;
	int backDis = 0;
	srand(NULL);
	for (int i = 0; i <m_mazeCol; i++){
		/// 处理偶数列情况
		if (m_mazeCol - 1 == i){
			for (k = kRowS + (deltaS + kRowT) / 2 - 1; k < m_mazeRow; k++){
				m_MazeData[k][i] = 3;
			}
			break;
		}
		for (k = kRowS; k < kRowT; k++){
			m_MazeData[k][i] = 1;
		}
		/// 处理前后两列无联通情况
		if (i > 0 && (kRowS >= kRowTPre || kRowT <= kRowSPre || (kRowS >= kRowSPre  && kRowT <= kRowTPre))){
			if (kRowS >= kRowTPre){
				kRowS = (kRowTPre + kRowSPre) / 2;
				for (k = kRowS; k <= kRowT; k++){
					m_MazeData[k][i] = 1;
				}
			}
			else if (kRowT <= kRowSPre){
				kRowT = (kRowTPre + kRowSPre) / 2;
				for (k = kRowS; k <= kRowT; k++){
					m_MazeData[k][i] = 1;
				}
			}
			else if (kRowS >= kRowSPre  && kRowT <= kRowTPre){
				kRowS = kRowSPre - 2 - rand() % 8;
				kRowT = kRowTPre + 3 + rand() % 12;
				if (kRowS < 0){
					kRowS = 0;
				}
				if (kRowT > m_mazeRow - 1){
					kRowT = m_mazeRow - 1;
				}
				for (k = kRowS; k <= kRowT; k++){
					m_MazeData[k][i] = 1;
				}
			}
		}
		/// 处理触底情况
		if (kRowT > m_mazeRow - 5){
			backDis = kRowS - rand() % (m_mazeRow / 8 + 1);
		}

		/// 处理奇数列情况
		i++;
		if (i < m_mazeCol - 1){
			deltaS = (kRowT - kRowS) / 4;
			deltaT = 2 * (kRowT - kRowS) / 3;
			if (deltaT + kRowS < m_mazeRow){
				m_MazeData[kRowS + deltaT][i] = 2;
			}
			else if (deltaS + 3 + kRowS < m_mazeRow){
				deltaT = deltaS + 3;
				m_MazeData[kRowS + deltaT][i] = 2;
			}
			else if (deltaS - 3 + kRowS < m_mazeRow){
				deltaT = deltaS - 3;
				m_MazeData[kRowS + deltaT][i] = 2;
			}
			else if (deltaS - 1 + kRowS < m_mazeRow){
				deltaT = deltaS - 1;
				m_MazeData[kRowS + deltaT][i] = 2;
			}
			m_MazeData[kRowS + deltaS][i] = 2;
		}
		else if (m_mazeCol - 1 == i){
			for (k = kRowT - 1; k < m_mazeRow; k++){
				m_MazeData[k][i] = 3;
			}
		}

		kRowSPre = kRowS + deltaS;
		kRowTPre = kRowS + deltaT;
		/// 生成新的偶数列起始结束为止		
		// + i * m_mazeRow / m_mazeCol
		kRowS = (1 + i + rand() % 3) * m_mazeRow / m_mazeCol + (rand() % (m_mazeRow / 8 + 1) - rand() % (m_mazeRow / 4 + 1));
		if (kRowS > kRowS + deltaT){
			kRowS = kRowS + deltaT - 1;
		}
		// + (i + rand() % 3) * m_mazeRow * 2 / (m_mazeCol + 1)
		kRowT = (i + 3 + rand() % 7)*m_mazeRow / m_mazeCol + (rand() % (3 * m_mazeRow / (m_mazeCol * 4) + 2) - rand() % (2 * m_mazeRow / (m_mazeCol * 7) + 1));
		kRowS -= backDis;
		kRowT -= backDis;
		if (kRowT >= m_mazeRow){
			kRowT = m_mazeRow - 1;
		}
		if (kRowS < 0){
			kRowS = 0;
		}
		if (kRowT < kRowS){
			kRowS ^= kRowT;
			kRowT ^= kRowS;
			kRowS ^= kRowT;
		}
	}
	for (i = 0; i < m_mazeRow; i++){
		for (j = 0; j < m_mazeCol; j++){
			;
		}
	}
}


void CDemo::defaultPermutaionMazeMode(int mazeMode){
	CPoint sPnt(0, 0);
	CPoint tPnt(0, 0);
	CPoint offEnter(0, 0);
	CPoint offExit(0, 0);
	if (mazeMode > 0){
		sPnt = CPoint(0, 0);
		tPnt = CPoint(m_mazeRow, m_mazeCol);
		//offEnter = CPoint(0, 0);
		offExit = CPoint(m_mazeRow, m_mazeCol);
		if (m_mazeRow > 10 && m_mazeCol > 10){
			sPnt.x += 10;
			sPnt.y += 10;
			tPnt.x -= 10;
			tPnt.y -= 10;
			offExit.x = tPnt.x - sPnt.x;
			offExit.y = tPnt.y - sPnt.y;
			defaultMazeSubLayout(mazeMode, sPnt, tPnt, offEnter, offExit);
			sPnt = tPnt;
			tPnt.x += 10;
			tPnt.y += 10;
			offExit.x = tPnt.x - sPnt.x;
			offExit.y = tPnt.y - sPnt.y;
			//tPnt.y++;
			//offExit.y++;
			sPnt.x--;
			offExit.x++;
			defaultMazeSubLayout(mazeMode, sPnt, tPnt, offEnter, offExit);
			//tPnt.y++;
			sPnt.x = 0;
			sPnt.y = 0;
			tPnt.x = 10;
			tPnt.y = 10;
			offExit.x = tPnt.x - sPnt.x;
			offExit.y = tPnt.y - sPnt.y;
			tPnt.y++;
			offExit.y++;
			defaultMazeSubLayout(mazeMode, sPnt, tPnt, offEnter, offExit);
		}
		else{
			defaultMazeSubLayout(mazeMode, sPnt, tPnt, offEnter, offExit);
		}
	}
}


void CDemo::defaultMazeSubLayout(int mazeSubStructureMode, const CPoint& sPnt, const CPoint& tPnt, const CPoint& offEnter, const CPoint& offExit){

	CRect rect(0, 0, m_mazeRow, m_mazeCol);
	if (!(isPointInRect(sPnt, rect) && isPointInRect(tPnt, rect))){
		return;
	}
	int i = 0;
	int j = 0;
	int k = 0;
	int kRowS = 0;
	int kRowT = 0;
	int kColS = 0;
	int kColT = 0;
	int height = 0;
	int width = 0;
	int deltaX = 0;
	int deltaY = 0;

	CPoint currSPnt = sPnt;
	CPoint currTPnt = tPnt;
	if (currSPnt.x > currTPnt.x){
		currSPnt.x ^= currTPnt.x;
		currTPnt.x ^= currSPnt.x;
		currSPnt.x ^= currTPnt.x;
	}
	if (currSPnt.y > currTPnt.y){
		currSPnt.y ^= currTPnt.y;
		currTPnt.y ^= currSPnt.y;
		currSPnt.y ^= currTPnt.y;
	}
	height = currTPnt.x - currSPnt.x;
	width = currTPnt.y - currSPnt.y;


	if (mazeSubStructureMode >= 0){
		complexMazeModeAssist(mazeSubStructureMode, sPnt, width, height);
		return;
	}
	/**/

	if (0 == height || 0 == width ||
		offEnter.x < 0 || offExit.x < 0 ||
		offEnter.x > height || offExit.x > height ||
		offEnter.y < 0 || offExit.y < 0 ||
		offEnter.y > width || offExit.y > width){
		return;
	}
	CPoint currOffEnter = offEnter;
	CPoint currOffExit = offExit;
	int offHeight = 0;
	int offWidth = 0;
	int diffEdge = 0;
	if (currOffEnter.x > currOffExit.x){
		currOffEnter.x ^= currOffExit.x;
		currOffExit.x ^= currOffEnter.x;
		currOffEnter.x ^= currOffExit.x;
	}
	if (currOffEnter.y > currOffExit.y){
		currOffEnter.y ^= currOffExit.y;
		currOffExit.y ^= currOffEnter.y;
		currOffEnter.y ^= currOffExit.y;
	}
	offHeight = currOffExit.x - currOffEnter.x;
	offWidth = currOffExit.y - currOffEnter.y;
	diffEdge = offWidth - offHeight;
	if (1 == height){
		for (j = currOffEnter.y; j < currOffExit.y; j++){
			m_MazeData[currOffEnter.x][j] = 1;
		}
		return;
	}
	else if (1 == width){
		for (i = currOffEnter.x; i < currOffExit.x; i++){
			m_MazeData[i][currOffEnter.y] = 1;
		}
		return;
	}
	CPoint iterPnt(currSPnt.x + currOffEnter.x, currSPnt.y + currOffEnter.y);


	switch (mazeSubStructureMode){
	case 0:
		/// MOVE down along the fixed column firstly and then MOVE right along the fixed row as following
		for (i = 0; i < height; i++){
			m_MazeData[iterPnt.x][iterPnt.y] = 1;
			iterPnt.x++;
		}
		if (iterPnt.x < m_mazeRow){
			for (j = 0; j < width; j++){
				m_MazeData[iterPnt.x][iterPnt.y] = 1;
				iterPnt.y++;
			}
		}
		else{
			// Dealing the endge condition
			iterPnt.x--;
			iterPnt.y++;
			for (j = 1; j < width; j++){
				m_MazeData[iterPnt.x][iterPnt.y] = 1;
				iterPnt.y++;
			}
			if (iterPnt.y < m_mazeCol){
				m_MazeData[iterPnt.x][iterPnt.y] = 1;
			}
		}
		break;
	case 1:
		/// MOVE right along the fixed row firstly and then MOVE down along the fixed column as following
		for (j = 0; j < width; j++){
			m_MazeData[iterPnt.x][iterPnt.y] = 1;
			iterPnt.y++;
		}
		if (iterPnt.y < m_mazeCol){
			for (i = 0; i < height; i++){
				m_MazeData[iterPnt.x][iterPnt.y] = 1;
				iterPnt.x++;
			}
		}
		else{
			// Dealing the endge condition
			iterPnt.y--;
			iterPnt.x++;
			for (i = 1; i < height; i++){
				m_MazeData[iterPnt.x][iterPnt.y] = 1;
				iterPnt.x++;
			}
			if (iterPnt.x < m_mazeCol){
				m_MazeData[iterPnt.x][iterPnt.y] = 1;
			}
		}
		break;
	case 2:
		/// MOVE one-step right-down until the positon of row or column of target firstly 
		/// and then MOVE straight to it as following
		if (offHeight == offWidth){
			iterPnt.x = currSPnt.x + currOffEnter.x;
			iterPnt.y = currSPnt.y + currOffEnter.y;
			for (i = 0; i <offHeight; i++){
				m_MazeData[iterPnt.x][iterPnt.y] = 1;
				iterPnt.y++;
				m_MazeData[iterPnt.x][iterPnt.y] = 1;
				iterPnt.x++;
			}
		}
		else if (offHeight > offWidth){
			diffEdge = offHeight - offWidth;
			for (i = 0; i <offWidth; i++){
				m_MazeData[iterPnt.x][iterPnt.y] = 1;
				iterPnt.y++;
				m_MazeData[iterPnt.x][iterPnt.y] = 1;
				iterPnt.x++;
			}
			for (i = offWidth; i < offHeight; i++){
				m_MazeData[iterPnt.x][iterPnt.y] = 1;
				iterPnt.x++;
			}
		}
		else{
			diffEdge = offWidth - offHeight;
			for (i = 0; i <offHeight; i++){
				m_MazeData[iterPnt.x][iterPnt.y] = 1;
				iterPnt.y++;
				m_MazeData[iterPnt.x][iterPnt.y] = 1;
				iterPnt.x++;
			}
			for (i = offHeight; i <offWidth; i++){
				m_MazeData[iterPnt.x][iterPnt.y] = 1;
				iterPnt.y++;
			}
		}
		break;
	case 3:
		/// MOVE straight until the positon of row or column of target firstly 
		/// and then MOVE one-step right-down to it as following
		if (offHeight == offWidth){
			iterPnt.x = currSPnt.x + currOffEnter.x;
			iterPnt.y = currSPnt.y + currOffEnter.y;
			for (i = 0; i <offHeight; i++){
				m_MazeData[iterPnt.x][iterPnt.y] = 1;
				iterPnt.x++;
				m_MazeData[iterPnt.x][iterPnt.y] = 1;
				iterPnt.y++;
			}
		}
		else if (offHeight > offWidth){
			diffEdge = offHeight - offWidth;
			for (i = offWidth; i < offHeight; i++){
				m_MazeData[iterPnt.x][iterPnt.y] = 1;
				iterPnt.x++;
			}
			for (i = 0; i <offWidth; i++){
				m_MazeData[iterPnt.x][iterPnt.y] = 1;
				iterPnt.x++;
				m_MazeData[iterPnt.x][iterPnt.y] = 1;
				iterPnt.y++;
			}
		}
		else{
			diffEdge = offWidth - offHeight;
			for (i = offHeight; i <offWidth; i++){
				m_MazeData[iterPnt.x][iterPnt.y] = 1;
				iterPnt.y++;
			}
			for (i = 0; i <offHeight; i++){
				m_MazeData[iterPnt.x][iterPnt.y] = 1;
				iterPnt.x++;
				m_MazeData[iterPnt.x][iterPnt.y] = 1;
				iterPnt.y++;
			}
		}
		break;
	default:
		if (mazeSubStructureMode > 3){
			if (mazeSubStructureMode >= 10){
				complexMazeModeAssist(mazeSubStructureMode, sPnt, width, height);
			}
			else{
				complexMazeModeAssist(mazeSubStructureMode - 1, sPnt, width, height);
			}
		}
		break;
	}
}


void CDemo::simpleMazeModeAssist(int mazeModeAssist, const CPoint& sPnt, const int length, Direction dir){
	int i = 0;
	int j = 0;
	int dirX = 0;
	int dirY = 0;
	switch (mazeModeAssist){
	case 0:
		if (Direction::Left == dir){
			dirY = -1;
		}
		else{
			dirY = 1;
		}
		for (j = 0; j < length; j++){
			m_MazeData[sPnt.x][sPnt.y + j*dirY] = 1;
		}
		break;
	case 1:
		if (Direction::Up == dir){
			dirX = -1;
		}
		else{
			dirX = 1;
		}
		for (i = 0; i < length; i++){
			m_MazeData[sPnt.x + i*dirX][sPnt.y] = 1;
		}
		break;
	case 2:
		switch (dir){
		case Direction::Left:
			dirX = -1;
			dirY = -1;
			break;
		case Direction::Right:
			dirX = 1;
			dirY = 1;
			break;
		case Direction::Down:
			dirX = 1;
			dirY = -1;
			break;
		case Direction::Up:
			dirX = -1;
			dirY = 1;
			break;
		default:
			dirX = 1;
			dirY = 1;
			break;
		}
		for (j = 0; j < length; j++){
			m_MazeData[sPnt.x + j*dirX][sPnt.y + j*dirY] = 1;
		}
		break;
	default:
		break;
	}
}


void CDemo::complexMazeModeAssist(int mazeModeAssist, const CPoint& sPnt, const int width, const int height){
	if (sPnt.x + height > m_mazeRow && sPnt.y + width > m_mazeCol || width < 1 || height < 1){
		return;
	}
	int i = 0;
	int j = 0;
	int newHeight = 0;
	int newWidth = 0;
	CPoint iterPnt = sPnt;
	int distance = 0;
	int count = 0;
	int superMode = 10;
	distance = 3;
	if (mazeModeAssist < 1000){
		distance = mazeModeAssist / superMode + 1;
		mazeModeAssist = mazeModeAssist % superMode;
	}

	switch (mazeModeAssist){

#pragma region  First ten conditions - 1000 conditions
	case 0:
		/// Right-Down Half-Rectangle
		iterPnt = sPnt;
		for (j = 0; j < width; j++){
			m_MazeData[iterPnt.x][iterPnt.y] = 1;
			iterPnt.y++;
		}
		iterPnt.y--;
		iterPnt.x++;
		for (i = 1; i < height; i++){
			m_MazeData[iterPnt.x][iterPnt.y] = 1;
			iterPnt.x++;
		}
		break;
	case 1:
		/// Down-Right Half-Rectangle
		iterPnt = sPnt;
		for (i = 0; i < height; i++){
			m_MazeData[iterPnt.x][iterPnt.y] = 1;
			iterPnt.x++;
		}
		iterPnt.x--;
		iterPnt.y++;
		for (j = 1; j < width; j++){
			m_MazeData[iterPnt.x][iterPnt.y] = 1;
			iterPnt.y++;
		}
		break;
	case 2:
		/// Closed Rectangle
		iterPnt = sPnt;
		for (j = 0; j < width; j++){
			m_MazeData[iterPnt.x][iterPnt.y] = 1;
			iterPnt.y++;
		}
		iterPnt.y--;
		iterPnt.x++;
		for (i = 1; i < height; i++){
			m_MazeData[iterPnt.x][iterPnt.y] = 1;
			iterPnt.x++;
		}
		iterPnt = sPnt;
		for (i = 0; i < height; i++){
			m_MazeData[iterPnt.x][iterPnt.y] = 1;
			iterPnt.x++;
		}
		iterPnt.x--;
		iterPnt.y++;
		for (j = 1; j < width; j++){
			m_MazeData[iterPnt.x][iterPnt.y] = 1;
			iterPnt.y++;
		}
		break;
	case 3:
		/// Horizonal 'S' type
		if (width < 3 || height < 3){
			return;
		}
		iterPnt = sPnt;
		count = 0;
		for (j = 0; j < width; j+=distance){
			for (i = 0; i < height; i++){
				m_MazeData[iterPnt.x+i][iterPnt.y+j] = 1;
			}
			if (j + distance >= width){
				if (1 == count % 2){
					for (i = j + 1; i < width; i++){
						m_MazeData[iterPnt.x + height - 1][iterPnt.y + i] = 1;
					}
				}
				else{
					for (i = j + 1; i < width; i++){
						m_MazeData[iterPnt.x][iterPnt.y + i] = 1;
					}
				}
				break;
			}
			else{
				if (1 == count % 2){
					for (i = j + 1; i < j + distance; i++){
						m_MazeData[iterPnt.x + height - 1][iterPnt.y + i] = 1;
					}
				}
				else{
					for (i = j + 1; i < j + distance; i++){
						m_MazeData[iterPnt.x][iterPnt.y + i] = 1;
					}
				}
			}
			count++;
		}
		break;
	case 4:
		/// Verical 'S' type
		if (width < 3 || height < 3){
			return;
		}
		iterPnt = sPnt;
		count = 0;
		for (i = 0; i < height; i += distance){
			for (j = 0; j < width; j++ ){
				m_MazeData[iterPnt.x + i][iterPnt.y + j] = 1;
			}
			if (i + distance >= height){
				if (1 == count % 2){
					for (j = i + 1; j < height; j++){
						m_MazeData[iterPnt.x + j][iterPnt.y + width - 1] = 1;
					}
				}
				else{
					for (j = i + 1; j < height; j++){
						m_MazeData[iterPnt.x + j][iterPnt.y] = 1;
					}
				}
				break;
			}
			else{
				if (1 == count % 2){
					for (j = i + 1; j < i + distance; j++){
						m_MazeData[iterPnt.x + j][iterPnt.y + width - 1] = 1;
					}
				}
				else{
					for (j = i + 1; j < i + distance; j++){
						m_MazeData[iterPnt.x + j][iterPnt.y] = 1;
					}
				}
			}
			count++;
		}
		break;
	case 5:
		/// Horizonal 'S' type
		if (width < 3 || height < 3){
			return;
		}
		iterPnt = sPnt;
		count = 0;
		for (j = 0; j < width; j += distance){
			for (i = 0; i < height; i++){
				m_MazeData[iterPnt.x + i][iterPnt.y + j] = 1;
			}
			if (j + distance >= width){
				if (0 == count % 2){
					for (i = j + 1; i < width; i++){
						m_MazeData[iterPnt.x + height - 1][iterPnt.y + i] = 1;
					}
				}
				else{
					for (i = j + 1; i < width; i++){
						m_MazeData[iterPnt.x][iterPnt.y + i] = 1;
					}
				}
				break;
			}
			else{
				if (0 == count % 2){
					for (i = j + 1; i < j + distance; i++){
						m_MazeData[iterPnt.x + height - 1][iterPnt.y + i] = 1;
					}
				}
				else{
					for (i = j + 1; i < j + distance; i++){
						m_MazeData[iterPnt.x][iterPnt.y + i] = 1;
					}
				}
			}
			count++;
		}
		break;
	case 6:
		/// Verical 'S' type
		if (width < 3 || height < 3){
			return;
		}
		iterPnt = sPnt;
		count = 0;
		for (i = 0; i < height; i += distance){
			for (j = 0; j < width; j++){
				m_MazeData[iterPnt.x + i][iterPnt.y + j] = 1;
			}
			if (i + distance >= height){
				if (0 == count % 2){
					for (j = i + 1; j < height; j++){
						m_MazeData[iterPnt.x + j][iterPnt.y + width - 1] = 1;
					}
				}
				else{
					for (j = i + 1; j < height; j++){
						m_MazeData[iterPnt.x + j][iterPnt.y] = 1;
					}
				}
				break;
			}
			else{
				if (0 == count % 2){
					for (j = i + 1; j < i + distance; j++){
						m_MazeData[iterPnt.x + j][iterPnt.y + width - 1] = 1;
					}
				}
				else{
					for (j = i + 1; j < i + distance; j++){
						m_MazeData[iterPnt.x + j][iterPnt.y] = 1;
					}
				}
			}
			count++;
		}
		break;
	case 7:
		complexMazeModeAssist((distance - 1)*superMode + mazeModeAssist - 1 + 10, sPnt, width, height);
		break;
	case 8:
		complexMazeModeAssist((distance - 1)*superMode + mazeModeAssist - 2 + 20, sPnt, width, height);
		break;
	case 9:
		complexMazeModeAssist((distance - 1)*superMode + mazeModeAssist - 3 + 30, sPnt, width, height);
		break;
#pragma endregion

	default:
#pragma region first-level default-case
		if (mazeModeAssist >= superMode){
			distance = ((mazeModeAssist % 1000) % 500) / superMode + 1;
			//distance = mazeModeAssist / superMode + 1;
			switch (mazeModeAssist % superMode){
#pragma region All-case-ten-conditions
			case 0:
				/// Unclosed Rectangle with multi-distance - C.S.
				if (width < distance + 2 || height < distance + 2){
					complexMazeModeAssist(mazeModeAssist + 1, sPnt, width, height);
					return;
				}
				else{
					iterPnt = sPnt;
					complexMazeModeAssist(2, sPnt, width, height);
					for (i = 0; i < distance; i++){
						iterPnt.x++;
						m_MazeData[iterPnt.x][iterPnt.y] = 0;
					}
				}
				break;
			case 1:
				/// Spiral Rectangle with multi-distance - C.S.
				if (width < distance + 2 || height < distance + 2){
					for (j = 0; j < width; j++){
						m_MazeData[sPnt.x][sPnt.y + j] = 1;
					}
					for (i = 1; i < height; i++){
						m_MazeData[sPnt.x + i][sPnt.y + width - 1] = 1;
					}
					return;
				}
				else{
					complexMazeModeAssist(mazeModeAssist - 1, sPnt, width, height);
					iterPnt = sPnt;
					newWidth = width - 2 * (distance + 1);
					newHeight = height - 2 * (distance + 1);
					if (newHeight > 0 && newWidth > 0){
						iterPnt.x += distance + 1;
						for (j = 0; j < distance; j++){
							iterPnt.y++;
							m_MazeData[iterPnt.x][iterPnt.y] = 1;
						}
						iterPnt.y++;
						complexMazeModeAssist(mazeModeAssist, iterPnt, newWidth, newHeight);
					}
					else{
						return;
					}
				}
				break;
			case 2:
				/// Unclosed Rectangle with multi-distance - C.S.
				if (width < distance + 2 || height < distance + 2){
					complexMazeModeAssist(mazeModeAssist + 1, sPnt, width, height);
					return;
				}
				else{
					iterPnt = sPnt;
					complexMazeModeAssist(2, sPnt, width, height);
					for (j = 0; j < distance; j++){
						iterPnt.y++;
						m_MazeData[iterPnt.x][iterPnt.y] = 0;
					}
				}
				break;
			case 3:
				/// Spiral Rectangle with multi-distance - C.S.
				if (width < distance + 2 || height < distance + 2){
					for (i = 0; i < height; i++){
						m_MazeData[sPnt.x + i][sPnt.y] = 1;
					}
					for (j = 1; j < width; j++){
						m_MazeData[sPnt.x + height - 1][sPnt.y + j] = 1;
					}
					return;
				}
				else{
					complexMazeModeAssist(mazeModeAssist - 1, sPnt, width, height);
					iterPnt = sPnt;
					newWidth = width - 2 * (distance + 1);
					newHeight = height - 2 * (distance + 1);
					if (newHeight > 0 && newWidth > 0){
						iterPnt.y += distance + 1;
						for (i = 0; i < distance; i++){
							iterPnt.x++;
							m_MazeData[iterPnt.x][iterPnt.y] = 1;
						}
						iterPnt.x++;
						complexMazeModeAssist(mazeModeAssist, iterPnt, newWidth, newHeight);
					}
					else{
						return;
					}
				}
				break;
			case 4:
				/// To plot the "回"
				iterPnt = sPnt;
				i = 0;
				while (width > 2 * i*(distance + 1) && height > 2 * i*(distance + 1)){
					complexMazeModeAssist(2, iterPnt, width - 2 * i*(distance + 1), height - 2 * i*(distance + 1));
					iterPnt.Offset(distance + 1, distance + 1);
					i++;
				}
				break;
			case 5:
				complexMazeModeAssist(mazeModeAssist - 1 + 10, sPnt, width, height);
				break;
			case 6:
				complexMazeModeAssist(mazeModeAssist - 1 + 20, sPnt, width, height);
				break;
			case 7:
				complexMazeModeAssist(mazeModeAssist - 1 + 30, sPnt, width, height);
				break;
			case 8:
				complexMazeModeAssist(mazeModeAssist - 1 + 40, sPnt, width, height);
				break;
			case 9:
				complexMazeModeAssist(mazeModeAssist - 1 + 50, sPnt, width, height);
				break;
			default:
				break;
#pragma endregion
			}
		}
		break;
#pragma endregion
	}
}


bool CDemo::findMazeRoute(){
	return permutationSteps(m_mazeEntr);
}


bool CDemo::permutationSteps(CPoint& curr){
	CPoint temp = curr;
	CPoint logPnt(0,0);
	if (curr == m_mazeExit){
		m_MazeFlag[curr.x][curr.y] = m_routeFlagColor;
		return true;
	}
	int deltaX = 0;
	int deltaY = 0;
	m_MazeFlag[curr.x][curr.y] = m_routeFlagColor;
	while (isCurrPointAvai(curr, deltaX, deltaY)){
		temp = curr;
		temp.x += deltaX;
		temp.y += deltaY;
		logPnt = temp;
		if (temp == m_mazeExit){
			m_MazeFlag[temp.x][temp.y] = m_routeFlagColor;
			return true;
		}
		singleRoutePreDeal(temp);
		if (permutationSteps(temp)){
			return true;
		}
		if (logPnt != temp){
			singleRoutePreDealAssistColor(logPnt, m_routeFlagColor + 1);	/// Flag the useless-route
		}
	}
	m_MazeFlag[curr.x][curr.y] = m_routeFlagColor + 1;	/// Flag the useless-route
	return false;
}


void CDemo::singleRoutePreDeal(CPoint& curr){
	int count = 0;
	int deltaX = 0;
	int deltaY = 0;
	int flagS[8] = { 1, 0, 0, 1, -1, 0, 0, -1 };
	while (true){
		count = 0;
		for (int i = 0; i < 4; i++){
			if (curr.x + flagS[2 * i] >= 0 && curr.x + flagS[2 * i]<m_mazeRow &&
				curr.y + flagS[2 * i + 1] >= 0 && curr.y + flagS[2 * i + 1]<m_mazeCol &&
				0 == m_MazeFlag[curr.x + flagS[2 * i]][curr.y + flagS[2 * i + 1]] &&
				0 < m_MazeData[curr.x + flagS[2 * i]][curr.y + flagS[2 * i + 1]] ){
				count++;
				if (count <= 1){
					deltaX = flagS[2 * i];
					deltaY = flagS[2 * i + 1];
				}
				else{
					return;
				}
			}
		}
		if (1 == count){
			m_MazeFlag[curr.x][curr.y] = m_singleRouteFlagColor;	/// To flag the prime ones beside last one in the single-route-linked route
			curr.x += deltaX;
			curr.y += deltaY;
		}
		else if (0 == count){
			m_MazeFlag[curr.x][curr.y] = m_singleRouteFlagColor;	/// to flag the last one ath the end of the single-route-linked route
			return;
		}
	}
}


void CDemo::singleRoutePreDealAssistColor(CPoint curr, const int targetColor, const bool isSingleRoute){
	/// 联通区域内着色
	int prime = m_MazeFlag[curr.x][curr.y];
	if (curr.x < 0 || curr.x >= m_mazeRow || curr.y < 0 || curr.y >= m_mazeCol || targetColor == prime){
		return;
	}
	int count = 0;
	int deltaX = 0;
	int deltaY = 0;
	int flagS[8] = { 1, 0, 0, 1, -1, 0, 0, -1 };
	if (isSingleRoute){
		// 仅仅是单点联通
		while (true){
			count = 0;
			for (int i = 0; i < 4; i++){
				if (curr.x + flagS[2 * i] >= 0 && curr.x + flagS[2 * i]<m_mazeRow &&
					curr.y + flagS[2 * i + 1] >= 0 && curr.y + flagS[2 * i + 1]<m_mazeCol &&
					prime == m_MazeFlag[curr.x + flagS[2 * i]][curr.y + flagS[2 * i + 1]]){
					count++;
					if (count <= 1){
						deltaX = flagS[2 * i];
						deltaY = flagS[2 * i + 1];
					}
					else{
						return;
					}
				}
			}
			if (1 == count){
				m_MazeFlag[curr.x][curr.y] = targetColor;	/// To flag the prime ones beside last one
				curr.x += deltaX;
				curr.y += deltaY;
			}
			else if (0 == count){
				m_MazeFlag[curr.x][curr.y] = targetColor;	/// to flag the Last one
				return;
			}
		}
	}
	else{
		// 所有联通区域

	}
}


bool CDemo::isCurrPointAvai(CPoint& curr, int& deltaX, int &deltaY){
	CPoint temp;
	int disX = m_mazeExit.x - curr.x;
	int disY = m_mazeExit.y - curr.y;
	int flagX = 0;
	int flagY = 0;
	if (disX > 0){
		flagX = 1;
	}
	else if (disX < 0){
		flagX = -1;
	}
	if (disY > 0){
		flagY = 1;
	}
	else if (disY < 0){
		flagY = -1;
	}
	deltaX = 0;
	deltaY = 0;
	int flagS[8] = { 0 };
	if (0 != flagX * flagY){
		if (disX*flagX >= disY*flagY){
			flagS[0] = flagX;
			flagS[3] = flagY;
			flagS[5] = -flagY;
			flagS[6] = -flagX;
		}
		else{
			flagS[1] = flagY;
			flagS[2] = flagX;
			flagS[4] = -flagX;
			flagS[7] = -flagY;
		}
	}
	else if (0 != flagX + flagY){
		if (0 != flagX){
			flagS[0] = flagX;
			flagS[3] = 1;
			flagS[5] = -1;
			flagS[6] = -flagX;
		}
		else{
			flagS[1] = flagY;
			flagS[2] = 1;
			flagS[4] = -1;
			flagS[7] = -flagY;
		}
	}
	for (int i = 0; i < 4; i++){
		temp.x = curr.x + flagS[2 * i];
		temp.y = curr.y + flagS[2 * i + 1];
		if (isCurrPointLegal(temp) && isCurrStepFree(temp)){
			deltaX = flagS[2 * i];
			deltaY = flagS[2 * i + 1];
			return true;
		}
	}
	return false;
}


bool CDemo::isCurrPointLegal(CPoint& curr){
	if (curr.x >= 0 && curr.x < m_mazeRow && curr.y >= 0 && curr.y < m_mazeCol){
		return true;
	}
	return false;
}


bool CDemo::isCurrStepFree(CPoint& curr){
	if (m_MazeData[curr.x][curr.y] > 0 && 0 == m_MazeFlag[curr.x][curr.y]){
		return true;
	}
	return false;
}


void CDemo::drawMazeResult(CPaintDC &dc){
	//initMaze();
	if (isFreshMazeRslt){
		findMazeRoute();
		isFreshMazeRslt = false;
	}
	MazeShow(dc);
}


void CDemo::MazeShow(CPaintDC &dc){
	int iter = 0;
	int showSX = 0;
	int showSY = 0;

	/*
	for (int i = sPntMazeShow.x; i < tPntMazeShow.x; i++){
	for (int j = sPntMazeShow.y; j < tPntMazeShow.y; j++){
	if (0 != m_MazeFlag[i][j]){
	iter = m_MazeFlag[i][j];
	}
	else{
	iter = m_MazeData[i][j];
	}
	iter %= 10;
	for (int k = 0; k < m_MazeFactorX; k++){
	for (int r = 0; r < m_MazeFactorY; r++){
	dc.SetPixel(
	showSY + m_MazeFactorY * (j - sPntMazeShow.y) + r,
	showSX + m_MazeFactorX * (i - sPntMazeShow.x) + k,
	m_AStartColors[iter]);
	}
	}
	}
	}
	*/

	for (int i = sPntMazeShow.y; i < tPntMazeShow.y; i++){
		for (int j = sPntMazeShow.x; j < tPntMazeShow.x; j++){

			/// 以下策略有问题，i、j分别代表数组的列、行，而与数组值相反。当且仅当数组为方阵时放大平移操作正确。
			if (0 != m_MazeFlag[i][j]){
				iter = m_MazeFlag[i][j];
			}
			else{
				iter = m_MazeData[i][j];
			}
			/**/

			/*
			if (0 != m_MazeFlag[j][i]){
			iter = m_MazeFlag[j][i];
			}
			else{
			iter = m_MazeData[j][i];
			}
			*/

			iter %= 10;
			for (int k = 0; k < m_MazeFactorX; k++){
				for (int r = 0; r < m_MazeFactorY; r++){
					dc.SetPixel(showSX + m_MazeFactorX * (j - sPntMazeShow.x) + k,
						showSY + m_MazeFactorY * (i - sPntMazeShow.y) + r,
						m_AStartColors[iter]);
				}
			}
		}
	}
	/**/

	/*
	for (int i = sPntAStarShow.y; i < tPntAStarShow.y; i++){
	for (int j = sPntAStarShow.x; j < tPntAStarShow.x; j++){
	if (data[i][j] >= 0 && data[i][j] < 10){
	iter = data[i][j];
	}
	else if (data[i][j] >= 10){
	iter = data[i][j]%10;
	}
	else{
	iter = -data[i][j];
	}
	for (int k = 0; k < m_AStarFactorX; k++){
	for (int r = 0; r < m_AStarFactorY; r++){
	dc.SetPixel(showSX + m_AStarFactorX * (j - sPntAStarShow.x) + k,
	showSY + m_AStarFactorY * (i - sPntAStarShow.y) + r,
	m_AStartColors[iter]);
	}
	}
	}
	}
	*/

	/*
	for (int i = sPntMazeShow.y; i < tPntMazeShow.y; i++){
	for (int j = sPntMazeShow.x; j < tPntMazeShow.x; j++){
	if (0 != m_MazeFlag[i][j]){
	iter = m_MazeFlag[i][j];
	}
	else{
	iter = m_MazeData[i][j];
	}
	iter %= 10;

	for (int k = 0; k < m_MazeFactorX; k++){
	for (int r = 0; r < m_MazeFactorY; r++){
	dc.SetPixel(showSX + m_MazeFactorX * (j - sPntMazeShow.x) + k, showSY + m_MazeFactorY * (i - sPntMazeShow.y) + r,
	m_AStartColors[iter]);
	}
	}


	for (int k = 0; k < m_MazeFactorY; k++){
	for (int r = 0; r < m_MazeFactorX; r++){
	dc.SetPixel(showSY + m_MazeFactorY * (i - sPntMazeShow.y) + k,
	showSX + m_MazeFactorX * (j - sPntMazeShow.x) + r,
	m_AStartColors[iter]);
	}
	}
	}
	}
	*/
}

