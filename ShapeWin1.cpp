#include "framework.h"
#include "ShapeWin1.h"
#include <commdlg.h> 


#define MAX_LOADSTRING 100

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];

static std::vector<KPoint*> figures;
static DWORD rgbCurrent = RGB(255, 255, 128);
static DWORD rgbCurrent1 = RGB(255, 255, 0);
int CurrShape;

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    PaintRhombus(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    TriangleChooser(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    PaintTriangle1(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    PaintTriangle2(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    PaintTriangle3(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    PaintColTriangle1(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    PaintColTriangle2(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    PaintColTriangle3(HWND, UINT, WPARAM, LPARAM);
void                ChangeColor(HWND, KPoint*, int);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_SHAPEWIN1, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SHAPEWIN1));

	MSG msg;

	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SHAPEWIN1));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_SHAPEWIN1);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance;

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}
enum KAct { actNone = 0, actHorizLeft = 1, actHorizRight = 2, actVertUp = 3, actVertDown = 4, actRotate = 5, actRotateBack = 6, actExpand = 7, actShrink = 8 };

void FAction(HDC& hdc, KPoint* pt, KAct acts)
{
	double k = 1.25;
	switch (acts)
	{
	case actNone:
		if ((*pt).GetVisibility())
		{
			(*pt).Draw(hdc, false);
		}
		break;
	case actHorizLeft:
	{(*pt).Shift(hdc, -10, 0);
	}
	break;
	case actHorizRight:
	{(*pt).Shift(hdc, 10, 0);
	}
	break;
	case actVertUp:
	{(*pt).Shift(hdc, 0, -10);
	}
	break;
	case actVertDown:
	{(*pt).Shift(hdc, 0, 10);
	}
	break;
	case actRotate:
	{(*pt).Rotate(hdc, 10);
	}
	break;
	case actRotateBack:
	{(*pt).Rotate(hdc, -10);
	}
	break;
	case actExpand:
	{(*pt).Expand(hdc, k);
	}
	break;
	case actShrink:
	{(*pt).Expand(hdc, (double)(1 / k));
	}
	break;
	}

}
void ChangeColor(HWND hWnd, KPoint* iFigure, int var) {
												
	static CHOOSECOLOR cc;
	static COLORREF acrCustClr[16];

	cc.lStructSize = sizeof(cc);
	cc.hwndOwner = hWnd;
	cc.lpCustColors = (LPDWORD)acrCustClr;
	cc.rgbResult = rgbCurrent;
	cc.Flags = CC_FULLOPEN | CC_RGBINIT;

	if (ChooseColor(&cc) == TRUE) {
		rgbCurrent = cc.rgbResult;
	}

	switch (var) {
	case 1:
		iFigure->SetOutline(rgbCurrent);
		break;
	case 2:
		iFigure->SetColor(rgbCurrent);
		break;
	}
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static KAct xAct = actNone;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
	{
		switch (LOWORD(wParam)) {
		case ID_RHOMBUS:
			CurrShape = ID_RHOMBUS;
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ROMBOBOX), hWnd, PaintRhombus);
			InvalidateRect(hWnd, NULL, TRUE);
			UpdateWindow(hWnd);
			break;
		case ID_TRIANGLE:
			CurrShape = ID_TRIANGLE;
			DialogBox(hInst, MAKEINTRESOURCE(IDD_TRICHOICE), hWnd, TriangleChooser);
			InvalidateRect(hWnd, NULL, TRUE);
			UpdateWindow(hWnd);
			break;
		case ID_COLTRIANGLE:
			CurrShape = ID_COLTRIANGLE;
			DialogBox(hInst, MAKEINTRESOURCE(IDD_TRICHOICE), hWnd, TriangleChooser);
			InvalidateRect(hWnd, NULL, TRUE);
			UpdateWindow(hWnd);
			break;
		case IDM_PAINTER:
			for (int i = 0; i < figures.size(); i++) {
				if (figures[i]->GetState()) {
					ChangeColor(hWnd, figures[i], 1);
				}
			}
			InvalidateRect(hWnd, NULL, TRUE);
			UpdateWindow(hWnd);
			break;
		case IDM_PAINTER1:
			if (CurrShape == ID_COLTRIANGLE) {
				for (int i = 0; i < figures.size(); i++) {
					if (figures[i]->GetState()) {
						ChangeColor(hWnd, figures[i], 2);
					}
				}
			}
			InvalidateRect(hWnd, NULL, TRUE);
			UpdateWindow(hWnd);
			break;
		case IDM_DELACTIVE:
			figures.erase(figures.begin() + figures.size() - 1);
			InvalidateRect(hWnd, NULL, TRUE);
			UpdateWindow(hWnd);
			break;
		case IDM_HIDEACTIVE:
			figures[figures.size() - 1]->Hide(hdc);
			InvalidateRect(hWnd, NULL, TRUE);
			UpdateWindow(hWnd);
			break;
		case IDM_SHOWACTIVE:
			figures[figures.size() - 1]->Show(hdc);
			InvalidateRect(hWnd, NULL, TRUE);
			UpdateWindow(hWnd);
			break;
		case IDM_DELETEALL:
			for (int i = 0; i < figures.size(); ++i) {
				delete figures[i];
			}
			figures.clear();
			InvalidateRect(hWnd, NULL, TRUE);
			UpdateWindow(hWnd);
			break;
		case IDM_HIDEALL:
			for (int i = 0; i < figures.size(); ++i) {
				figures[i]->Hide(hdc);
			}
			InvalidateRect(hWnd, NULL, TRUE);
			UpdateWindow(hWnd);
			break;
		case IDM_SHOWALL:
			for (int i = 0; i < figures.size(); ++i) {
				figures[i]->Show(hdc);
			}
			InvalidateRect(hWnd, NULL, TRUE);
			UpdateWindow(hWnd);
			break;
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_CONTROL:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_CONTROLBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		if (!figures.empty()) {
			if (figures.size() == 1) {
				figures[0]->Draw(hdc, true);
				FAction(hdc, figures[0], xAct);
			}
			else {
				for (int i = 0; i < figures.size() - 1; i++) {
					figures[i]->Draw(hdc, false);
				}
				figures[figures.size() - 1]->Draw(hdc, true);
				FAction(hdc, figures[figures.size() - 1], xAct);
			}
		}
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_LBUTTONDOWN:
	{
		if (wParam & MK_SHIFT)
		{
			xAct = actHorizLeft;
		}
		else if (wParam & MK_CONTROL)
		{
			xAct = actHorizRight;
		}
		else
		{
			xAct = actRotate;
		}

		InvalidateRect(hWnd, NULL, TRUE);
		UpdateWindow(hWnd);
	}
	break;
	case WM_RBUTTONDOWN:
	{
		if (wParam & MK_SHIFT)
		{
			xAct = actVertUp;
		}
		else if (wParam & MK_CONTROL)
		{
			xAct = actVertDown;
		}
		else
		{
			xAct = actRotateBack;
		}

		InvalidateRect(hWnd, NULL, TRUE);
		UpdateWindow(hWnd);
	}
	break;
	case WM_KEYDOWN:
	{
		switch (wParam) {
		case VK_TAB:
			xAct = actExpand;
			break;
		case VK_BACK:
			xAct = actShrink;
			break;
		default:
			return 0;
		}
		InvalidateRect(hWnd, NULL, TRUE);
		UpdateWindow(hWnd);
		return 0;
	}
	case WM_DESTROY:
		for (int i = 0; i < figures.size(); ++i) {
			delete figures[i];
		}
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
INT_PTR CALLBACK PaintRhombus(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	BOOL IsError;
	std::vector<double> params{};

	switch (message)
	{
	case WM_INITDIALOG:
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			params.push_back(GetDlgItemInt(hDlg, IDC_EDIT1, &IsError, FALSE));
			params.push_back(GetDlgItemInt(hDlg, IDC_EDIT2, &IsError, FALSE));
			params.push_back(GetDlgItemInt(hDlg, IDC_EDIT3, &IsError, FALSE));
			params.push_back(GetDlgItemInt(hDlg, IDC_EDIT4, &IsError, FALSE));
			params.push_back(GetDlgItemInt(hDlg, IDC_EDIT5, &IsError, FALSE));
			figures.push_back(new KRhombus(params[0], params[1], params[2], rgbCurrent, params[3], params[4]));
			InvalidateRect(GetParent(hDlg), NULL, 1);
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		case IDC_BUTTON:
			static CHOOSECOLOR cc;
			static COLORREF acrCustClr[16];

			cc.lStructSize = sizeof(cc);
			cc.hwndOwner = hDlg;
			cc.lpCustColors = (LPDWORD)acrCustClr;
			cc.rgbResult = rgbCurrent;
			cc.Flags = CC_FULLOPEN | CC_RGBINIT;

			if (ChooseColor(&cc) == TRUE) {
				rgbCurrent = cc.rgbResult;
			}
			break;
			InvalidateRect(GetParent(hDlg), NULL, 1);
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		case IDCANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
INT_PTR CALLBACK TriangleChooser(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {

	UNREFERENCED_PARAMETER(lParam);

	switch (message) {
	case WM_INITDIALOG:
		CheckRadioButton(hDlg, IDC_RADIO1, IDC_RADIO3, IDC_RADIO1);
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			if (CurrShape == ID_TRIANGLE) {
				if (IsDlgButtonChecked(hDlg, IDC_RADIO1)) {
					DialogBox(hInst, MAKEINTRESOURCE(IDD_TRIANGBOX1), hDlg, PaintTriangle1);
				}
				else if (IsDlgButtonChecked(hDlg, IDC_RADIO2)) {
					DialogBox(hInst, MAKEINTRESOURCE(IDD_TRIANGBOX2), hDlg, PaintTriangle2);
				}
				else if (IsDlgButtonChecked(hDlg, IDC_RADIO3)) {
					DialogBox(hInst, MAKEINTRESOURCE(IDD_TRIANGBOX3), hDlg, PaintTriangle3);
				}
			}
			else if (CurrShape == ID_COLTRIANGLE) {
				if (IsDlgButtonChecked(hDlg, IDC_RADIO1)) {
					DialogBox(hInst, MAKEINTRESOURCE(IDD_COLTRIBOX1), hDlg, PaintColTriangle1);
				}
				else if (IsDlgButtonChecked(hDlg, IDC_RADIO2)) {
					DialogBox(hInst, MAKEINTRESOURCE(IDD_COLTRIBOX2), hDlg, PaintColTriangle2);
				}
				else if (IsDlgButtonChecked(hDlg, IDC_RADIO3)) {
					DialogBox(hInst, MAKEINTRESOURCE(IDD_COLTRIBOX3), hDlg, PaintColTriangle3);
				}
			}
			InvalidateRect(GetParent(hDlg), NULL, 1);
			return EndDialog(hDlg, 1);
		case IDCANCEL:
			return EndDialog(hDlg, 0);
		}
	}
	return (INT_PTR)FALSE;
}
INT_PTR CALLBACK PaintTriangle1(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	BOOL IsError;
	std::vector<double> params{};

	switch (message)
	{
	case WM_INITDIALOG:
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			params.push_back(GetDlgItemInt(hDlg, IDC_EDIT6, &IsError, FALSE));  //1 side
			params.push_back(GetDlgItemInt(hDlg, IDC_EDIT7, &IsError, FALSE));  //2 side
			params.push_back(GetDlgItemInt(hDlg, IDC_EDIT8, &IsError, FALSE));   // angle
			params.push_back(GetDlgItemInt(hDlg, IDC_EDIT9, &IsError, FALSE));   // rotangle
			params.push_back(GetDlgItemInt(hDlg, IDC_EDIT10, &IsError, FALSE));   //xcoord
			params.push_back(GetDlgItemInt(hDlg, IDC_EDIT11, &IsError, FALSE));   //ycoord
			figures.push_back(new KTriangle(params[0], params[1], params[2], params[3], rgbCurrent, params[4], params[5]));
			InvalidateRect(GetParent(hDlg), NULL, 1);
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		case IDC_BUTTON:
			static CHOOSECOLOR cc;
			static COLORREF acrCustClr[16];

			cc.lStructSize = sizeof(cc);
			cc.hwndOwner = hDlg;
			cc.lpCustColors = (LPDWORD)acrCustClr;
			cc.rgbResult = rgbCurrent;
			cc.Flags = CC_FULLOPEN | CC_RGBINIT;

			if (ChooseColor(&cc) == TRUE) {
				rgbCurrent = cc.rgbResult;
			}
			break;
			InvalidateRect(GetParent(hDlg), NULL, 1);
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		case IDCANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
INT_PTR CALLBACK PaintTriangle2(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	BOOL IsError;
	std::vector<double> params{};

	switch (message)
	{
	case WM_INITDIALOG:
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			params.push_back(GetDlgItemInt(hDlg, IDC_EDIT12, &IsError, FALSE));  //1 side
			params.push_back(GetDlgItemInt(hDlg, IDC_EDIT13, &IsError, FALSE));  //2 side
			params.push_back(GetDlgItemInt(hDlg, IDC_EDIT14, &IsError, FALSE));   //3 side
			params.push_back(GetDlgItemInt(hDlg, IDC_EDIT15, &IsError, FALSE));   // rotangle
			params.push_back(GetDlgItemInt(hDlg, IDC_EDIT16, &IsError, FALSE));   //xcoord
			params.push_back(GetDlgItemInt(hDlg, IDC_EDIT17, &IsError, FALSE));   //ycoord
			figures.push_back(new KTriangle(params[0], params[1], params[2], params[3], rgbCurrent, params[4], params[5], true));
			InvalidateRect(GetParent(hDlg), NULL, 1);
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		case IDC_BUTTON:
			static CHOOSECOLOR cc;
			static COLORREF acrCustClr[16];

			cc.lStructSize = sizeof(cc);
			cc.hwndOwner = hDlg;
			cc.lpCustColors = (LPDWORD)acrCustClr;
			cc.rgbResult = rgbCurrent;
			cc.Flags = CC_FULLOPEN | CC_RGBINIT;

			if (ChooseColor(&cc) == TRUE) {
				rgbCurrent = cc.rgbResult;
			}
			break;
			InvalidateRect(GetParent(hDlg), NULL, 1);
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		case IDCANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
INT_PTR CALLBACK PaintTriangle3(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	BOOL IsError;
	std::vector<double> params{};

	switch (message)
	{
	case WM_INITDIALOG:
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			params.push_back(GetDlgItemInt(hDlg, IDC_EDIT18, &IsError, FALSE));  //1 side
			params.push_back(GetDlgItemInt(hDlg, IDC_EDIT19, &IsError, FALSE));  //1 angle
			params.push_back(GetDlgItemInt(hDlg, IDC_EDIT20, &IsError, FALSE));   //2 angle
			params.push_back(GetDlgItemInt(hDlg, IDC_EDIT21, &IsError, FALSE));   //rotangle
			params.push_back(GetDlgItemInt(hDlg, IDC_EDIT22, &IsError, FALSE));   //xcoord
			params.push_back(GetDlgItemInt(hDlg, IDC_EDIT23, &IsError, FALSE));   //ycoord
			figures.push_back(new KTriangle(params[0], params[1], params[2], params[3], rgbCurrent, params[4], params[5], false));
			InvalidateRect(GetParent(hDlg), NULL, 1);
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		case IDC_BUTTON:
			static CHOOSECOLOR cc;
			static COLORREF acrCustClr[16];

			cc.lStructSize = sizeof(cc);
			cc.hwndOwner = hDlg;
			cc.lpCustColors = (LPDWORD)acrCustClr;
			cc.rgbResult = rgbCurrent;
			cc.Flags = CC_FULLOPEN | CC_RGBINIT;

			if (ChooseColor(&cc) == TRUE) {
				rgbCurrent = cc.rgbResult;
			}
			break;
			InvalidateRect(GetParent(hDlg), NULL, 1);
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		case IDCANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
INT_PTR CALLBACK PaintColTriangle1(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	BOOL IsError;
	std::vector<double> params{};

	switch (message)
	{
	case WM_INITDIALOG:
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			params.push_back(GetDlgItemInt(hDlg, IDC_EDIT24, &IsError, FALSE));  //1 side
			params.push_back(GetDlgItemInt(hDlg, IDC_EDIT25, &IsError, FALSE));  //2 side
			params.push_back(GetDlgItemInt(hDlg, IDC_EDIT26, &IsError, FALSE));   //angle
			params.push_back(GetDlgItemInt(hDlg, IDC_EDIT27, &IsError, FALSE));   //rotangle
			params.push_back(GetDlgItemInt(hDlg, IDC_EDIT28, &IsError, FALSE));   //xcoord
			params.push_back(GetDlgItemInt(hDlg, IDC_EDIT29, &IsError, FALSE));   //ycoord
			figures.push_back(new KPaintTriangle(params[0], params[1], params[2], params[3], rgbCurrent, rgbCurrent1, params[4], params[5]));
			InvalidateRect(GetParent(hDlg), NULL, 1);
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		case IDC_BUTTON:
			static CHOOSECOLOR cc;
			static COLORREF acrCustClr[16];

			cc.lStructSize = sizeof(cc);
			cc.hwndOwner = hDlg;
			cc.lpCustColors = (LPDWORD)acrCustClr;
			cc.rgbResult = rgbCurrent;
			cc.Flags = CC_FULLOPEN | CC_RGBINIT;

			if (ChooseColor(&cc) == TRUE) {
				rgbCurrent = cc.rgbResult;
			}
			break;
			InvalidateRect(GetParent(hDlg), NULL, 1);
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		case IDC_BUTTON1:
			static CHOOSECOLOR cc1;
			static COLORREF acrCustClr1[16];

			cc1.lStructSize = sizeof(cc1);
			cc1.hwndOwner = hDlg;
			cc1.lpCustColors = (LPDWORD)acrCustClr1;
			cc1.rgbResult = rgbCurrent1;
			cc1.Flags = CC_FULLOPEN | CC_RGBINIT;

			if (ChooseColor(&cc1) == TRUE) {
				rgbCurrent1 = cc1.rgbResult;
			}
			break;
			InvalidateRect(GetParent(hDlg), NULL, 1);
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		case IDCANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
INT_PTR CALLBACK PaintColTriangle2(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	BOOL IsError;
	std::vector<double> params{};

	switch (message)
	{
	case WM_INITDIALOG:
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			params.push_back(GetDlgItemInt(hDlg, IDC_EDIT30, &IsError, FALSE));  //1 side
			params.push_back(GetDlgItemInt(hDlg, IDC_EDIT31, &IsError, FALSE));  //2 side
			params.push_back(GetDlgItemInt(hDlg, IDC_EDIT32, &IsError, FALSE));   //3 side
			params.push_back(GetDlgItemInt(hDlg, IDC_EDIT33, &IsError, FALSE));   // rotangle
			params.push_back(GetDlgItemInt(hDlg, IDC_EDIT34, &IsError, FALSE));   //xcoord
			params.push_back(GetDlgItemInt(hDlg, IDC_EDIT35, &IsError, FALSE));   //ycoord
			figures.push_back(new KPaintTriangle(params[0], params[1], params[2], params[3], rgbCurrent, rgbCurrent1, params[4], params[5], true));
			InvalidateRect(GetParent(hDlg), NULL, 1);
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		case IDC_BUTTON:
			static CHOOSECOLOR cc;
			static COLORREF acrCustClr[16];

			cc.lStructSize = sizeof(cc);
			cc.hwndOwner = hDlg;
			cc.lpCustColors = (LPDWORD)acrCustClr;
			cc.rgbResult = rgbCurrent;
			cc.Flags = CC_FULLOPEN | CC_RGBINIT;

			if (ChooseColor(&cc) == TRUE) {
				rgbCurrent = cc.rgbResult;
			}
			break;
			InvalidateRect(GetParent(hDlg), NULL, 1);
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		case IDC_BUTTON1:
			static CHOOSECOLOR cc1;
			static COLORREF acrCustClr1[16];

			cc1.lStructSize = sizeof(cc1);
			cc1.hwndOwner = hDlg;
			cc1.lpCustColors = (LPDWORD)acrCustClr1;
			cc1.rgbResult = rgbCurrent1;
			cc1.Flags = CC_FULLOPEN | CC_RGBINIT;

			if (ChooseColor(&cc1) == TRUE) {
				rgbCurrent1 = cc1.rgbResult;
			}
			break;
			InvalidateRect(GetParent(hDlg), NULL, 1);
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		case IDCANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
INT_PTR CALLBACK PaintColTriangle3(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	BOOL IsError;
	std::vector<double> params{};

	switch (message)
	{
	case WM_INITDIALOG:
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK:
			params.push_back(GetDlgItemInt(hDlg, IDC_EDIT36, &IsError, FALSE));  //1 side
			params.push_back(GetDlgItemInt(hDlg, IDC_EDIT37, &IsError, FALSE));  //1 angle
			params.push_back(GetDlgItemInt(hDlg, IDC_EDIT38, &IsError, FALSE));   //2 angle
			params.push_back(GetDlgItemInt(hDlg, IDC_EDIT39, &IsError, FALSE));   //rotangle
			params.push_back(GetDlgItemInt(hDlg, IDC_EDIT40, &IsError, FALSE));   //xcoord
			params.push_back(GetDlgItemInt(hDlg, IDC_EDIT41, &IsError, FALSE));   //ycoord
			figures.push_back(new KPaintTriangle(params[0], params[1], params[2], params[3], rgbCurrent, rgbCurrent1, params[4], params[5], false));
			InvalidateRect(GetParent(hDlg), NULL, 1);
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		case IDC_BUTTON:
			static CHOOSECOLOR cc;
			static COLORREF acrCustClr[16];

			cc.lStructSize = sizeof(cc);
			cc.hwndOwner = hDlg;
			cc.lpCustColors = (LPDWORD)acrCustClr;
			cc.rgbResult = rgbCurrent;
			cc.Flags = CC_FULLOPEN | CC_RGBINIT;

			if (ChooseColor(&cc) == TRUE) {
				rgbCurrent = cc.rgbResult;
			}
			break;
			InvalidateRect(GetParent(hDlg), NULL, 1);
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		case IDC_BUTTON1:
			static CHOOSECOLOR cc1;
			static COLORREF acrCustClr1[16];

			cc1.lStructSize = sizeof(cc1);
			cc1.hwndOwner = hDlg;
			cc1.lpCustColors = (LPDWORD)acrCustClr1;
			cc1.rgbResult = rgbCurrent1;
			cc1.Flags = CC_FULLOPEN | CC_RGBINIT;

			if (ChooseColor(&cc1) == TRUE) {
				rgbCurrent1 = cc1.rgbResult;
			}
			break;
			InvalidateRect(GetParent(hDlg), NULL, 1);
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		case IDCANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
