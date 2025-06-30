#include <windows.h>

#define BIRD_WIDTH 30
#define BIRD_HEIGHT 30

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static int birdY = 100, velocity = 0;

    switch (uMsg) {
        case WM_CREATE:
            SetTimer(hwnd, 1, 16, NULL); // Start timer on create
            break;

        case WM_KEYDOWN:
            if (wParam == VK_SPACE)
                velocity = -10;
            break;

        case WM_TIMER:
            velocity += 1;
            birdY += velocity;
            if (birdY < 0) birdY = 0;
            if (birdY > 570) birdY = 570;
            InvalidateRect(hwnd, NULL, FALSE); // Request repaint
            break;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            // Clear background
            HBRUSH bgBrush = CreateSolidBrush(RGB(135, 206, 235)); // Sky blue
            FillRect(hdc, &ps.rcPaint, bgBrush);
            DeleteObject(bgBrush);

            // Draw bird
            HBRUSH birdBrush = CreateSolidBrush(RGB(255, 255, 0)); // Yellow
            SelectObject(hdc, birdBrush);
            Rectangle(hdc, 100, birdY, 100 + BIRD_WIDTH, birdY + BIRD_HEIGHT);
            DeleteObject(birdBrush);

            EndPaint(hwnd, &ps);
            break;
        }

        case WM_DESTROY:
            KillTimer(hwnd, 1);
            PostQuitMessage(0);
            return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) {
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "FlappyWindow";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(0, "FlappyWindow", "Flappy Bird (WinAPI)", WS_OVERLAPPEDWINDOW,
                               100, 100, 800, 600,
                               NULL, NULL, hInstance, NULL);

    ShowWindow(hwnd, nShowCmd);
    UpdateWindow(hwnd);

    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
