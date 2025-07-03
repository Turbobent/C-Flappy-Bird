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
int bird2Y = 200, velocity2 = 0;
int isGameOver1 = 0;
int isGameOver2 = 0;


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
            if (wParam == VK_UP)
            velocity = -10;
        else if (wParam == 'W' || wParam == 'w')  
            velocity2 = -10;
            break;


   case WM_TIMER:
    if (isGameOver1 && isGameOver2) break;

    velocity += 1;
    birdY += velocity;
    if (birdY < 0) birdY = 0;
    if (birdY > 570) birdY = 570;

    velocity2 += 1;
    bird2Y += velocity2;
    if (bird2Y < 0) bird2Y = 0;
    if (bird2Y > 570) bird2Y = 570;

    // Move pipes
    for (int i = 0; i < MAX_PIPES; i++) {
        pipes[i].x -= PIPE_SPEED;
        if (pipes[i].x + PIPE_WIDTH < 0) {
            pipes[i].x = 800;
            pipes[i].gapY = 100 + rand() % 300;
        }
    }

    // Collision detection for Player 1
    if (!isGameOver1) {
        for (int i = 0; i < MAX_PIPES; i++) {
            int pipeX = pipes[i].x;
            int pipeTop = pipes[i].gapY;
            int pipeBottom = pipeTop + PIPE_GAP;

            int birdLeft = 100;
            int birdRight = 100 + BIRD_WIDTH;
            int birdTop = birdY;
            int birdBottom = birdY + BIRD_HEIGHT;

            if (birdRight > pipeX && birdLeft < pipeX + PIPE_WIDTH) {
                if (birdTop < pipeTop || birdBottom > pipeBottom) {
                    isGameOver1 = 1;
                }
            }
        }
    }

    // Collision detection for Player 2
    if (!isGameOver2) {
        for (int i = 0; i < MAX_PIPES; i++) {
            int pipeX = pipes[i].x;
            int pipeTop = pipes[i].gapY;
            int pipeBottom = pipeTop + PIPE_GAP;

            int birdLeft = 150;
            int birdRight = 150 + BIRD_WIDTH;
            int birdTop = bird2Y;
            int birdBottom = bird2Y + BIRD_HEIGHT;

            if (birdRight > pipeX && birdLeft < pipeX + PIPE_WIDTH) {
                if (birdTop < pipeTop || birdBottom > pipeBottom) {
                    isGameOver2 = 1;
                }
            }
        }
    }

    // If both dead, ask to restart
    if (isGameOver1 && isGameOver2) {
        KillTimer(hwnd, 1);
        int result = MessageBox(hwnd, "Both players lost! Restart?", "Game Over", MB_YESNO | MB_ICONQUESTION);
        if (result == IDYES) {
            birdY = bird2Y = 200;
            velocity = velocity2 = 0;
            for (int j = 0; j < MAX_PIPES; j++) {
                pipes[j].x = 800 + j * 300;
                pipes[j].gapY = 100 + rand() % 300;
            }
            isGameOver1 = isGameOver2 = 0;
            SetTimer(hwnd, 1, 16, NULL);
        } else {
            PostQuitMessage(0);
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

            // Draw bird 1 (yellow)
            HBRUSH birdBrush1 = CreateSolidBrush(RGB(255, 255, 0));
            SelectObject(hdc, birdBrush1);
            Rectangle(hdc, 100, birdY, 100 + BIRD_WIDTH, birdY + BIRD_HEIGHT);
            DeleteObject(birdBrush1);

            // Draw bird 2 (red)
            HBRUSH birdBrush2 = CreateSolidBrush(RGB(255, 0, 0));
            SelectObject(hdc, birdBrush2);
            Rectangle(hdc, 150, bird2Y, 150 + BIRD_WIDTH, bird2Y + BIRD_HEIGHT);
            DeleteObject(birdBrush2);


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
