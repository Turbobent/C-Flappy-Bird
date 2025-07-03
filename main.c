#include <windows.h>
#include <stdlib.h>
#include <time.h>

#define BIRD_WIDTH 30
#define BIRD_HEIGHT 30
#define PIPE_WIDTH 60
#define PIPE_GAP 150
#define PIPE_SPEED 5
#define MAX_PIPES 3

typedef struct {
    int x;
    int gapY;
} Pipe;

Pipe pipes[MAX_PIPES];
int birdY = 200, velocity = 0;
int isGameOver = 0;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static int frame = 0;

    switch (uMsg) {
        case WM_CREATE:
            // 60 FPS
            SetTimer(hwnd, 1, 16, NULL); 
            srand((unsigned int)time(NULL));
            // Initialize pipes
            for (int i = 0; i < MAX_PIPES; i++) {
                pipes[i].x = 800 + i * 300;
                pipes[i].gapY = 100 + rand() % 300;
            }
            break;

        case WM_KEYDOWN:
            if (wParam == VK_SPACE)
                velocity = -10;
            break;

   case WM_TIMER:
    if (isGameOver) break;

    velocity += 1;
    birdY += velocity;
    if (birdY < 0) birdY = 0;
    if (birdY > 570) birdY = 570;

    // Move pipes
    for (int i = 0; i < MAX_PIPES; i++) {
        pipes[i].x -= PIPE_SPEED;
        if (pipes[i].x + PIPE_WIDTH < 0) {
            pipes[i].x = 800;
            pipes[i].gapY = 100 + rand() % 300;
        }
    }

    // Collision detection
    for (int i = 0; i < MAX_PIPES; i++) {
        int pipeX = pipes[i].x;
        int pipeTop = pipes[i].gapY;
        int pipeBottom = pipeTop + PIPE_GAP;

        int birdLeft = 100;
        int birdRight = 100 + BIRD_WIDTH;
        int birdTop = birdY;
        int birdBottom = birdY + BIRD_HEIGHT;

        int pipeLeft = pipeX;
        int pipeRight = pipeX + PIPE_WIDTH;

        if (birdRight > pipeLeft && birdLeft < pipeRight) {
            if (birdTop < pipeTop || birdBottom > pipeBottom) {
                isGameOver = 1;
                KillTimer(hwnd, 1);
                int result = MessageBox(hwnd, "Game Over! Try again?", "Collision", MB_YESNO | MB_ICONQUESTION);
                if (result == IDYES) {
                    // Reset game state
                    birdY = 200;
                    velocity = 0;
                    for (int j = 0; j < MAX_PIPES; j++) {
                        pipes[j].x = 800 + j * 300;
                        pipes[j].gapY = 100 + rand() % 300;
                    }
                    isGameOver = 0;
                    SetTimer(hwnd, 1, 16, NULL);
                } else {
                    PostQuitMessage(0);
                }
                break;
            }
        }
    }

    InvalidateRect(hwnd, NULL, FALSE);
    break;



        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            // Background
            HBRUSH sky = CreateSolidBrush(RGB(135, 206, 235));
            FillRect(hdc, &ps.rcPaint, sky);
            DeleteObject(sky);

            // Draw bird
            HBRUSH birdBrush = CreateSolidBrush(RGB(255, 255, 0));
            SelectObject(hdc, birdBrush);
            Rectangle(hdc, 100, birdY, 100 + BIRD_WIDTH, birdY + BIRD_HEIGHT);
            DeleteObject(birdBrush);

            // Draw pipes
            HBRUSH pipeBrush = CreateSolidBrush(RGB(0, 200, 0));
            SelectObject(hdc, pipeBrush);

            for (int i = 0; i < MAX_PIPES; i++) {
                int x = pipes[i].x;
                int gapY = pipes[i].gapY;

                // Top pipe
                Rectangle(hdc, x, 0, x + PIPE_WIDTH, gapY);
                // Bottom pipe
                Rectangle(hdc, x, gapY + PIPE_GAP, x + PIPE_WIDTH, 600);
            }

            DeleteObject(pipeBrush);
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
