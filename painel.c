#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#include "../utils/utils.h"

#define ID_MENU_REDEFINIR 1
#define ID_MENU_SAIR      2
#define ID_MENU_INFO      3

Game* game;
HANDLE hSharedMemory;
int maxJogadoresVisiveis = 10;

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

void OrdenarJogadores(Jogador* ordenados, int count) {
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            if (ordenados[j].pontos < ordenados[j + 1].pontos) {
                Jogador temp = ordenados[j];
                ordenados[j] = ordenados[j + 1];
                ordenados[j + 1] = temp;
            }
        }
    }
}

void CriarMenus(HWND hwnd) {
    HMENU hMenu = CreateMenu();
    AppendMenu(hMenu, MF_STRING, ID_MENU_SAIR, _T("Sair"));
    AppendMenu(hMenu, MF_STRING, ID_MENU_INFO, _T("Informação"));
    SetMenu(hwnd, hMenu);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE:
        SetTimer(hwnd, 1, 1000, NULL);
        break;
    case WM_TIMER:
        InvalidateRect(hwnd, NULL, TRUE);
        break;
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        TextOut(hdc, 10, 10, _T("Última palavra:"), 15);
        TextOut(hdc, 150, 10, game->ultimaPalavra, _tcslen(game->ultimaPalavra));

        TextOut(hdc, 10, 40, _T("Letras:"), 7);
        for (int i = 0; i < game->maxLetras; i++) {
            TCHAR letra[2] = { game->letras[i], '\0' };
            TextOut(hdc, 80 + i * 20, 40, letra, 1);
        }

        TextOut(hdc, 10, 70, _T("Jogadores:"), 10);
        Jogador ordenados[MAX_JOGADORES];
        int count = 0;
        for (int i = 0; i < MAX_JOGADORES; i++) {
            if (game->jogadores[i].eAtivo)
                ordenados[count++] = game->jogadores[i];
        }
        OrdenarJogadores(ordenados, count);

        for (int i = 0; i < count && i < maxJogadoresVisiveis; i++) {
            TCHAR buffer[64];
            _stprintf_s(buffer, 64, _T("%s: %.1f"), ordenados[i].username, ordenados[i].pontos);
            TextOut(hdc, 10, 100 + i * 20, buffer, _tcslen(buffer));
        }

        EndPaint(hwnd, &ps);
        break;
    }
    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        
        case ID_MENU_SAIR:
            PostQuitMessage(0);
            break;
        case ID_MENU_INFO:
            MessageBox(hwnd, _T("Trabalho de SO2\nRodrigo Braga e Guilherme Costa"), _T("Informação"), MB_OK | MB_ICONINFORMATION);
            break;
        }
        break;
    case WM_DESTROY:
        KillTimer(hwnd, 1);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI _tWinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPTSTR lpCmdLine, int nCmdShow)
{
    hSharedMemory = OpenFileMapping(FILE_MAP_READ, FALSE, SHARED_MEM_NOME);
    if (!hSharedMemory) {
        MessageBox(NULL, _T("Erro ao abrir memória partilhada"), _T("Erro"), MB_OK | MB_ICONERROR);
        return 1;
    }
    game = (Game*)MapViewOfFile(hSharedMemory, FILE_MAP_READ, 0, 0, SHARED_MEM_TAM);
    if (!game) {
        MessageBox(NULL, _T("Erro ao mapear memória partilhada"), _T("Erro"), MB_OK | MB_ICONERROR);
        return 1;
    }

    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.lpszClassName = _T("PainelClasse");
    RegisterClass(&wc);

    HWND hwnd = CreateWindow(_T("PainelClasse"), _T("Painel do Jogo"), WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 600, 400, NULL, NULL, hInst, NULL);

    CriarMenus(hwnd);
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnmapViewOfFile(game);
    CloseHandle(hSharedMemory);
    return (int)msg.wParam;
}
