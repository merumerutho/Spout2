/*
=========================================================================

                        WinSpoutDX.cpp

    A Windows Desktop Application project generated by Visual Studio
    and adapted for Spout sender output (http://spout.zeal.co/).

	This is an example using the SpoutDX support class and SendImage

	bool SendImage(ID3D11Device* pDevice, unsigned char * pData, unsigned int width, unsigned int height);

    Captures the screen to a pixel buffer using Windows bitmap functions.
    Search on "SPOUT" for additions.

    Compare with DirectX 11 Tutorial04 sender example.

                 Copyright(C) 2020 Lynn Jarvis.

    This program is free software : you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.If not, see < http://www.gnu.org/licenses/>.

===============================================================================
*/


#include "framework.h"
#include "WinSpoutDX.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                      // current instance
WCHAR szTitle[MAX_LOADSTRING];        // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];  // the main window class name

// SPOUT
HWND g_hWnd = NULL;                     // Window handle
ID3D11Device* g_pd3dDevice = nullptr;   // DirectX 11.0 device pointer
spoutDX sender;                         // Sender object
HBITMAP g_hBitmap = NULL;               // Image bitmap
unsigned int g_BitmapWidth = 0;         // Image bitmap width
unsigned int g_BitmapHeight = 0;        // Image bitmap height
unsigned char *g_pixelBuffer = nullptr; // Sending pixel buffer
unsigned char g_SenderName[256];        // Sender name
unsigned int g_SenderWidth = 0;         // Sender width
unsigned int g_SenderHeight = 0;        // Sender height

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

// SPOUT
void Render();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

 	// SPOUT
	// Optionally enable Spout logging
	// OpenSpoutConsole(); // Console only for debugging
	// EnableSpoutLog(); // Log to console
	// EnableSpoutLogFile("WinSpoutDXsender.log"); // Log to file
	// SetSpoutLogLevel(SPOUT_LOG_WARNING); // show only warnings and errors

	// Load a bitmap from file
	g_hBitmap = (HBITMAP)LoadImageA(NULL, "koala-on-tree.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	if (!g_hBitmap) {
		MessageBoxA(NULL, "Failed to load image", "WinSpoutDX", MB_OK);
		return FALSE;
	}

	// Get the size of the bitmap
	BITMAP bmpImage;
	GetObjectA(g_hBitmap, sizeof(BITMAP), &bmpImage);
	g_BitmapWidth = bmpImage.bmWidth;
	g_BitmapHeight = bmpImage.bmHeight;

	// Create a sending buffer of that inital size
	// It is resized as necessary - see WM_PAINT
	g_SenderWidth = g_BitmapWidth;
	g_SenderHeight = g_BitmapHeight;
	g_pixelBuffer = new unsigned char[g_SenderWidth * g_SenderHeight * 4];

	// Give the sender a name
	// If no name is specified, the executable name is used
	sender.SetSenderName("WindowsDXsender");

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINSPOUTDX, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

	// SPOUT
	// Initialize DirectX.
	// This is not necessary for applications where 
	// a DirectX 11.0 device is already established.
	g_pd3dDevice = sender.OpenDirectX11();
	if(!g_pd3dDevice)
		return FALSE;

    // Main message loop:
	MSG msg = { 0 };
	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Render();
		}
	}

	// SPOUT
	if(g_hBitmap) 
		DeleteObject(g_hBitmap);
	sender.ReleaseSender();
	sender.CleanupDX11();

    return (int) msg.wParam;
}

// SPOUT
void Render()
{
	// Trigger a re-paint to draw the bitmap and refresh the sending pixel buffer - see WM_PAINT
	InvalidateRect(g_hWnd, NULL, FALSE);

	// Send the pixels
	sender.SendImage(g_pd3dDevice, g_pixelBuffer, g_SenderWidth, g_SenderHeight);

	//
	// SPOUT - fps control
	//
	// Optionally hold a target frame rate - e.g. 60 or 30fps.
	// This is not necessary if the application already has
	// fps control. But in this example rendering is done
	// during idle time and render rate can be extremely high.
	sender.HoldFps(60);

}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINSPOUTDX));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground  = CreateHatchBrush(HS_DIAGCROSS, RGB(192, 192, 192));
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WINSPOUTDX);
    wcex.lpszClassName  = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_WINSPOUTDX));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   // Create window
   // SPOUT - modified for 640x360 starting client size
   RECT rc = { 0, 0, 640, 360 }; // Desired client size
   AdjustWindowRect(&rc, WS_CAPTION | WS_SYSMENU, TRUE);
   HWND hWnd = CreateWindowW(szWindowClass,
	   szTitle,
	   // SPOUT - enable resize and maximize to demonstrate sender resizing
	   // WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
	   WS_OVERLAPPEDWINDOW | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_THICKFRAME,
	   CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
	   nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   // SPOUT
   // Centre the window on the desktop work area
   GetWindowRect(hWnd, &rc);
   RECT WorkArea;
   int WindowPosLeft = 0;
   int WindowPosTop = 0;
   SystemParametersInfo(SPI_GETWORKAREA, 0, (LPVOID)&WorkArea, 0);
   WindowPosLeft += ((WorkArea.right  - WorkArea.left) - (rc.right - rc.left)) / 2;
   WindowPosTop  += ((WorkArea.bottom - WorkArea.top)  - (rc.bottom - rc.top)) / 2;
   MoveWindow(hWnd, WindowPosLeft, WindowPosTop, (rc.right - rc.left), (rc.bottom - rc.top), false);

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   g_hWnd = hWnd;
  
   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
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

			if (IsIconic(hWnd))
				break;

			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(g_hWnd, &ps);

			// For this example, draw an image, capture the client area
			// and copy pixel data to the sending buffer.

			//
			// Draw the image bitmap
			//

			// Get the client area
			RECT rcClient;
			GetClientRect(hWnd, &rcClient);
			UINT rcWidth = (rcClient.right - rcClient.left);
			UINT rcHeight = (rcClient.bottom - rcClient.top);

			// Create a compatible memory DC and select the image bitmap into it
			HDC hdcMemDC = CreateCompatibleDC(hdc);
			SelectObject(hdcMemDC, g_hBitmap);

			// Stretch blit the image to the client area
			SetStretchBltMode(hdc, HALFTONE);
			StretchBlt(hdc,	0, 0,
						rcWidth, rcHeight, // client size
						hdcMemDC, 0, 0,
						g_BitmapWidth, g_BitmapHeight, // image size
						SRCCOPY);

			//
			// Capture the client area for sending
			//

			// The window is re-sizable - check for client area size change
			if (rcWidth != g_SenderWidth || rcHeight != g_SenderHeight) {
				// Update the sender dimensions
				g_SenderWidth = rcWidth;
				g_SenderHeight = rcHeight;
				// Re-size the the sending buffer to match
				if(g_pixelBuffer) delete g_pixelBuffer;
				g_pixelBuffer = new unsigned char[g_SenderWidth*g_SenderHeight * 4];
			}
			// The sender is now the same size as the client area

			// Create a compatible bitmap sized to the client area
			HBITMAP hBitmap = CreateCompatibleBitmap(hdcMemDC, rcWidth, rcHeight);

			// Select the bitmap into the memory DC
			SelectObject(hdcMemDC, hBitmap);

			// Blit the client screen into it
			BitBlt(hdcMemDC, 0, 0,
				   rcClient.right - rcClient.left,
				   rcClient.bottom - rcClient.top,
				   hdc, 0, 0, SRCCOPY);

			// Describe the memory DC bits
			BITMAPINFOHEADER bi;
			bi.biSize = sizeof(BITMAPINFOHEADER);
			bi.biSizeImage = (LONG)(g_SenderWidth * g_SenderHeight * 4); // Pixel buffer size
			bi.biWidth = (LONG)g_SenderWidth; // Width of buffer
			bi.biHeight = -(LONG)g_SenderHeight; // Height of buffer (negative to flip the data)
			bi.biPlanes = 1;
			bi.biBitCount = 32; // 4 bytes per pixel
			bi.biCompression = BI_RGB;
			bi.biXPelsPerMeter = 0;
			bi.biYPelsPerMeter = 0;
			bi.biClrUsed = 0;
			bi.biClrImportant = 0;

			// Copy to our sending buffer
			GetDIBits(hdcMemDC, hBitmap, 0, (UINT)rcHeight, (LPVOID)g_pixelBuffer, (BITMAPINFO *)&bi, DIB_RGB_COLORS);

			// Clean up
			DeleteObject(hBitmap);
			DeleteDC(hdcMemDC);

			EndPaint(hWnd, &ps);

        }
        break;

	case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
// SPOUT : adapted for this example.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
 	UNREFERENCED_PARAMETER(lParam);
	char tmp[MAX_PATH];
	char about[1024];
	LPDRAWITEMSTRUCT lpdis;
	HWND hwnd = NULL;
	HCURSOR cursorHand = NULL;
	HINSTANCE hInstance = GetModuleHandle(NULL);

    switch (message)
    {
    case WM_INITDIALOG:

		sprintf_s(about, 256, "                WinSpoutDX");
		strcat_s(about, 1024, "\n\n\n");
		strcat_s(about, 1024, "   Windows Spout sender example\n");
		strcat_s(about, 1024, "using DirectX and the SpoutDX class.");
		SetDlgItemTextA(hDlg, IDC_ABOUT_TEXT, (LPCSTR)about);

		// Spout website hyperlink hand cursor
		cursorHand = LoadCursor(NULL, IDC_HAND);
		hwnd = GetDlgItem(hDlg, IDC_SPOUT_URL);
		SetClassLongPtrA(hwnd, GCLP_HCURSOR, (LONG_PTR)cursorHand);
        return (INT_PTR)TRUE;

	case WM_DRAWITEM:
		// The blue hyperlink
		lpdis = (LPDRAWITEMSTRUCT)lParam;
		if (lpdis->itemID == -1) break;
		SetTextColor(lpdis->hDC, RGB(6, 69, 173));
		switch (lpdis->CtlID) {
		case IDC_SPOUT_URL:
			DrawTextA(lpdis->hDC, "http://spout.zeal.co", -1, &lpdis->rcItem, DT_LEFT);
			break;
		default:
			break;
		}
		break;

    case WM_COMMAND:

		if (LOWORD(wParam) == IDC_SPOUT_URL) {
			// Open the website url
			sprintf_s(tmp, MAX_PATH, "http://spout.zeal.co");
			ShellExecuteA(hDlg, "open", tmp, NULL, NULL, SW_SHOWNORMAL);
			EndDialog(hDlg, 0);
			return (INT_PTR)TRUE;
		}

        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

// That's all..
