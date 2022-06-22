#include <windows.h>
#include <stdio.h>

/*int main(int argc, int **argv){
    HDC deskHandle = GetDC(GetwindowWindow());
    if (deskHandle == NULL){
        printf("failed to get screen handle");
    }

    HDC memHandle = GetDC(NULL);
    //HDC memHandle = CreateCompatibleDC(deskHandle);

    if (memHandle == NULL){
        printf("failed to convert screen handle");
    }

    int result = BitBlt(memHandle, 0, 0, 1600, 1200, deskHandle, 0, 0, SRCCOPY);

    DeleteDC(deskHandle);
    DeleteDC(memHandle);
    return 0;
}*/
void SaveScreenshot(HWND hwnd){
    //HDC window = GetDC(hwnd);
}

HWND openWindow(const char *winName){
    HWND hwnd = CreateWindowEx(0, "Sample Window Class", winName, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, GetModuleHandle("screenshot.exe"), NULL);
    if (hwnd == NULL)
    {
        return NULL;
    }

    SetWindowLongPtr(hwnd, GWL_EXSTYLE, WS_EX_APPWINDOW | WS_EX_TOPMOST);
    SetWindowLongPtr(hwnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);
    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 1600, 1200, SWP_SHOWWINDOW);
    //ShowWindow(hwnd, SW_SHOWMAXIMIZED);
    return hwnd;
}


void ScreenGearInit(){
    BITMAP bmpScreen;
    BITMAPFILEHEADER bmfHeader;
    BITMAPINFOHEADER bi;
    HDC screen = GetDC(NULL);

    HDC hscreenDC = CreateCompatibleDC(screen);
    HBITMAP captureBmpS = CreateCompatibleBitmap(screen, 1600, 1200);
    SelectObject(hscreenDC, captureBmpS);

    BitBlt(hscreenDC, 0, 0, 1600, 1200, screen, 0, 0, SRCCOPY);
    GetObject(captureBmpS, sizeof(BITMAP), &bmpScreen);

    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = bmpScreen.bmWidth;
    bi.biHeight = bmpScreen.bmHeight;
    bi.biPlanes = 1;
    bi.biBitCount = 32;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;

    //DWORD dwBmpSize = ((bmpScreen.bmWidth * bi.biBitCount + 31) / 32) * 4 * bmpScreen.bmHeight;
    //HANDLE hDIB = GlobalAlloc(GHND, dwBmpSize);
    //HBITMAP* lpbitmap = (HBITMAP*)GlobalLock(hDIB);
    //GetDIBits(screen, captureBmpS, 0, (UINT)bmpScreen.bmHeight, lpbitmap, (BITMAPINFO*)&bi, DIB_RGB_COLORS);

    HWND mainHwnd = openWindow("Program");
    if (mainHwnd == NULL){
        MessageBox(0, "Cannot Initialize Window", "Error", MB_SETFOREGROUND);
        return;
    }

    HDC window = GetDC(mainHwnd);
    HDC hwindowDC = CreateCompatibleDC(window);
    HBITMAP captureBmpW = CreateCompatibleBitmap(window, 1600, 1200);

    //SetDIBits(window, captureBmpW, 0, (UINT)bmpScreen.bmHeight, lpbitmap, (BITMAPINFO*)&bi, DIB_RGB_COLORS);
    for (int y = 0; y < 1200; y++){
        for (int x = 0; x < 1600; x++){
            COLORREF pixColor = GetPixel(hscreenDC, x, y);
            INT r = (int)(GetRValue(pixColor) / 2);
            INT g = (int)(GetGValue(pixColor) / 2);
            INT b = (int)(GetBValue(pixColor) / 2);
            SetPixel(hscreenDC, x, y, RGB(r, g, b));
        }
    }
    //MessageBox(0, "Blah blah...", "My Windows app!", MB_SETFOREGROUND);
    SelectObject(hwindowDC, captureBmpW);
    BitBlt(window, 0, 0, 1600, 1200, hscreenDC, 0, 0, SRCCOPY);

    for (int i=0; i <= 10000; i++){
        //BitBlt(screen, 0, 0, 1600, 1200, hscreenDC, 0, 0, SRCCOPY);
    }

    DeleteDC(screen);
}


LRESULT CALLBACK KeyboardHook(int nCode, WPARAM wParam, LPARAM lParam){
    if (nCode == HC_ACTION) {
		if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
			PKBDLLHOOKSTRUCT p = (PKBDLLHOOKSTRUCT)lParam;
			int virualKey = p->vkCode;
            switch(virualKey){
                case VK_F8:
                    ScreenGearInit();
            }
        }
    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}


LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam){
    switch (uMsg){
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_PAINT:
        {
            //PAINTSTRUCT ps;
            //HDC hdc = BeginPaint(hwnd, &ps);
            //FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW+1));
            //EndPaint(hwnd, &ps);
        }
        return 0;

    case WM_KEYDOWN:
        switch(wParam){
            case VK_ESCAPE:
                DestroyWindow(hwnd);
                break;

            case VK_RETURN:
                SaveScreenshot(hwnd);
                break;
        }
        break;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

/*int main(int argc, int **argv)
{
    MessageBox( 0, "Blah blah...", "My Windows app!", MB_SETFOREGROUND );
    return 0;
}*/

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd){
    WNDCLASS wc = { };
    wc.lpfnWndProc   = WindowProc;
    wc.hInstance     = GetModuleHandle("screenshot.exe");//hInstance;
    wc.lpszClassName = "Sample Window Class";
    RegisterClass(&wc);

	HHOOK hook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardHook, 0, 0);
    MSG msg = { };
    while (GetMessage(&msg, NULL, 0, 0) > 0){
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(hook);
    return 0;
}