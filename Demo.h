
#pragma once
#include "MyGame.h"

extern const UINT DEMO_NEXT;
extern enum Direction;


class CDemo : public CMyGame
{
public:
	CDemo();
	~CDemo();

private:
	UINT mode;
	bool m_showEdge;
	bool m_showInnerEdge;

	CBitmap     m_bmpNumber;			/// Digital bitmap
	CBrush		m_brBack;				// 背景画刷
	CBrush		m_brInfoBack;			// 右侧信息背景画刷

	CRect m_gameRectShown;
	COLORREF m_gameBackColor;		/// Game area background color

	CRect m_leftArea;
	CRect m_rightArea;
	CRect m_showRect;		/// Game panel
	CRect m_rcClient;
	CRect m_gameRect;		/// Game Rectangular
	CRect m_infoRect;		/// Game-information Rectangular
	CPoint m_startP;		/// Chess plot start point in the DC
	CPoint m_endP;			/// Chess plot terminate point in the DC
	CPoint m_slideS;
	CPoint m_slideT;

	unsigned int m_uRowPixCount;	/// Bitmap One-Unit Height Pixel count
	unsigned int m_uColPixCount;	/// Bitmap One-Unit Width  Pixel count
	unsigned int m_unitColorStyle;	/// Bitmap One-Unit color style
	unsigned int m_uBlankHeightPixCount;	/// Bitmap One-Unit Height Pixel count of blank height
	unsigned int m_uBlankWidthPixCount;		/// Bitmap One-Unit Height Pixel count of blank width
	bool m_isGameOver;
	int m_stepCount;	/// Steps number
	int m_maxValue;	/// Steps number
	int m_fillCount;	/// Filled value count
	int m_uRowNum;	/// Row count
	int m_uColNum;	/// Column count
	int m_data[100][100];	/// 棋局
	//int arr[BINARY_SCALEX][BINARY_SCALEY];	/// 辅助棋局
	//int m_randArrH[BINARY_SCALEY];		/// 辅助随机数列
	//int m_randArrHAssit[BINARY_SCALEY];		/// 辅助随机数列
	//int m_randArrV[BINARY_SCALEX];		/// 辅助随机数列
	//int m_randArrVAssit[BINARY_SCALEX];		/// 辅助随机数列

	UINT		m_uNumRect[3];			// 时间坐标区域数组
	UINT		m_uShellRcX[2];			// 内框以及边界的坐标X方向
	UINT		m_uShellRcY[2];			// 内框以及边界的坐标Y方向

public:
	CRect m_wndRect;

public:
	void Initialize(CWnd* pWnd);
	void DrawCurr(CPaintDC*);
	void resizeLayout(const CRect& rect);

	void edgeStateTransition();
	void innerEdgeStateTransition();

	void LeftBtnDown(CWnd* pWnd, UINT nFlags, CPoint point);
	void LeftBtnUp(CWnd* pWnd, UINT nFlags, CPoint point);
	void RightBtnDown(CWnd* pWnd, UINT nFlags, CPoint point);
	void RightBtnUp(CWnd* pWnd, UINT nFlags, CPoint point);

	void switchMode(UINT newMode = DEMO_NEXT);

private:
	void initGame();
	bool isLegalGameMode(UINT newMode);
	void hideTheEdge(bool isHiding = false);
	void hideInnerEdge(bool isHiding = true);
	void setLayout();
	void drawFrame(CPaintDC* pDC);
	void drawInfo(CPaintDC* pDC);
	void drawShell(CPaintDC &dc);
	void drawNumber(CPaintDC &dc);
	void drawOneNumber(CPaintDC &dc, const CPoint &point, int posX, int posY);
	void drawSpecialImageSegment(CPaintDC &dc, const CPoint &point, int posX, int posY);

	void drawSubInfo(CPaintDC &dc, const CPoint point, int type = 0);
	void drawSubStructure(CPaintDC &dc, const CPoint& sPoint, const CPoint& tPoint, int type);



	/// A* heuristic algorithm
private:
	bool showDetail ;			/// corner info
	bool showDetailInner ;		/// Debug  info
	COLORREF m_AStartColors[10];	/// Color Sets
	int m_AStarFactor;		/// 像素放大倍数
	int m_AStarFactorX;		/// 像素水平放大倍数
	int m_AStarFactorY;		/// 像素垂直放大倍数
	CPoint sPntAStarShow;		/// 起始像素坐标
	CPoint tPntAStarShow;		/// 结束像素坐标
	CPoint m_AStarSEnd;
	CPoint m_AStarTEnd;
	int m_showImageXscale;		/// 图像大小
	int m_showImageYscale;		/// 图像大小
	int m_AStartData[1000][1000];
	const int m_imageXscale;
	const int m_imageYscale;
	bool m_isFreshAStart;
public:
	//void showButton();
	void setAStartFactor(int factor = 1, int factorX = 1, int factorY = 1);
	void setAStarPrimeScale(int rowCount, int colCount);
	void setAStartTarget(int sX, int sY, int tX, int tY);
	void setAStartScale(int sX, int sY, int tX, int tY);
	void initAStar();
	bool adaptShowScaleZoomOutInterface();
	bool adaptShowScaleZoomInInterface();
public:
	static bool isPointInRect(const CPoint& point, const CRect& rect);
	static void shiftListValues(CList<int>& barrierList, int shiftX, int shiftY, int minValF, int maxValF, int minValS, int maxValS, bool isLimited = true);
private:
	void drawAStartHeuristicResult(CPaintDC &dc);
	bool adaptShowScale(bool isShift = false);
	bool adaptShowScaleZoomOut();
	bool adaptShowScaleZoomIn();
	void AStartHeuristicAlgorithmTEST(CPaintDC &dc);
	void AStartHeuristicShow(CPaintDC &dc, const int const data[][1000], const int length);
	void AStartHeuristicAlgorithmInit(int data[][1000], int length);
	void AStartHeuristicAddDefaultBarrier(int data[][1000], int length, int* pRouteEnd);
	void AStartHeuristicAddBarrierList(int data[][1000], int length, const CList<int>& barrierList);
	void AStartHeuristicAlgorithmCreateBarrier(int data[][1000], const int const scale[]);
	void AStartHeuristicAlgorithmCreateBarrier(int data[][1000], const int sX, const int sY, const int tX, const int tY);
	void AStartHeuristicAlgorithmClearRoute(int data[][1000], int length);
	bool AStartHeuristicAlgorithm(int data[][1000], int rowNum, const int* const stInfo, int count, const int const scale[], const int const routeEnd[]);


	/// The Crystal Maze
private:
	int m_mazeRow;
	int m_mazeCol;
	int m_mazeRowMax;
	int m_mazeColMax;
	CPoint m_mazeEntr;
	CPoint m_mazeExit;
	COLORREF m_colorMazeEnd;

	int m_MazeData[1024][1024];
	int m_MazeFlag[1024][1024];
	int m_MazeFactor;		/// 像素放大倍数
	int m_MazeFactorX;		/// 像素水平放大倍数
	int m_MazeFactorY;		/// 像素垂直放大倍数
	CPoint m_sPntMazeShow;		/// 起始像素坐标

	bool isFreshMazeRslt;
	int m_routeFlagColor;
	int m_singleRouteFlagColor;

	CPoint sPntMazeShow;		/// 起始像素坐标
	CPoint tPntMazeShow;		/// 结束像素坐标
	int m_showImageXscaleMaze;		/// 图像大小
	int m_showImageYscaleMaze;		/// 图像大小

public:
	void initMaze();
	void setMazePrimeScale(int rowCount, int colCount);
	void setMazeTarget(int sX, int sY, int tX, int tY);
	void defaultMazeLayout(int mazeMode = -1);

private:
	void setMazeScale(int sX, int sY, int tX, int tY);
	void setMazeFactor(int factor, int factorX, int factorY);
	void defaultMazeLayoutFirst();
	void defaultPermutaionMazeMode(int mazeMode);
	void simpleMazeModeAssist(int mazeModeAssist, const CPoint& sPnt, const int length, Direction dir);
	void complexMazeModeAssist(int mazeModeAssist, const CPoint& sPnt, const int width, const int height);
	void defaultMazeSubLayout(int mazeSubStructureMode, const CPoint& sPnt, const CPoint& tPnt, const CPoint& offEnter, const CPoint& offExit);
	bool findMazeRoute();
	bool permutationSteps(CPoint& curr);
	void singleRoutePreDeal(CPoint& curr);
	void singleRoutePreDealAssistColor(CPoint iterPnt, const int targetColor, const bool isSingleRoute = true);
	bool isCurrPointAvai(CPoint& curr, int& deltaX, int &deltaY);
	bool isCurrPointLegal(CPoint& curr);
	bool isCurrStepFree(CPoint& curr);
	void drawMazeResult(CPaintDC &dc);
	void MazeShow(CPaintDC &dc);

};

