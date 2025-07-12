#include "../utils/utils.h"

Jogador jogador;
Game* game;
HANDLE hSharedMemory, newEvent;

//auxiliares

void Limpeza() {
    if (jogador.pipe != INVALID_HANDLE_VALUE)
        CloseHandle(jogador.pipe);

    if (hSharedMemory != NULL) {
        UnmapViewOfFile(game);
        CloseHandle(hSharedMemory);
    }

    if (newEvent != NULL)
        CloseHandle(newEvent);
}

void ProcessarErro(TCHAR* mensagem) {
    _tprintf_s(_T("[ERRO] %s (%lu)\n"), mensagem, GetLastError());
    Limpeza();
    exit(1);
}

void EnviarMensagem(Mensagem msg) {
    OVERLAPPED ov = { 0 };
    ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    if (!WriteFile(jogador.pipe, &msg, sizeof(Mensagem), NULL, &ov)) {
        if (GetLastError() != ERROR_IO_PENDING)
            ProcessarErro(_T("Erro ao escrever no pipe."));
        else
            WaitForSingleObject(ov.hEvent, INFINITE);

    }
    CloseHandle(ov.hEvent);
}

void ReceberMensagem(Mensagem* msg) {
    if (!ReadFile(jogador.pipe, msg, sizeof(Mensagem), NULL, NULL)) {
        Sleep(100); // Aguarda um pouco antes de tentar novamente
        ProcessarErro(_T("Erro ao ler do pipe."));
    }
}

BOOL PedidoServidor() {
    Mensagem enviado, recebido;

    _tcscpy_s(enviado.username, USERNAME_TAM, jogador.username);
    _tcscpy_s(enviado.texto, TAM_PALAVRA, _T("juntou-se"));

    EnviarMensagem(enviado);
    ReceberMensagem(&recebido);

    if (_tcsicmp(recebido.texto, _T("aceito")) != 0)
        return FALSE;

    return TRUE;
}

void ConectarArbitro() {
    _tprintf_s(_T("[JOGADOR] A conectar ao servidor...\n"));

    while (!WaitNamedPipe(PIPE_NOME, NMPWAIT_WAIT_FOREVER)) { Sleep(1000); }

    jogador.pipe = CreateFile(
        PIPE_NOME, GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
        OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);

    if (jogador.pipe == INVALID_HANDLE_VALUE) {
        ProcessarErro(_T("[ERRO] Falha ao conectar ao pipe do servidor."));
    }
    DWORD modo = PIPE_READMODE_MESSAGE;
    if (!SetNamedPipeHandleState(jogador.pipe, &modo, NULL, NULL)) {
        ProcessarErro(_T("[ERRO] Falha ao definir o modo do pipe."));
    }
    if (!PedidoServidor()) {
        _tprintf_s(_T("[ERRO] Falha ao se juntar ao jogo.\n"));
        Limpeza();
        exit(1);
    }
    _tprintf_s(_T("[JOGADOR] Conectado ao servidor.\n"));
}

void InicializarMemoriaPartilhada() {
    hSharedMemory = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, SHARED_MEM_NOME);
    if (hSharedMemory == NULL) {
        ProcessarErro(_T("[ERRO] Falha ao abrir memória partilhada."));
    }
    game = (Game*)MapViewOfFile(hSharedMemory, FILE_MAP_ALL_ACCESS, 0, 0, SHARED_MEM_TAM);
    if (game == NULL) {
        ProcessarErro(_T("[ERRO] Falha ao mapear memória partilhada."));
    }
}

//principais

DWORD WINAPI ThreadGerarLetras() {
    newEvent = OpenEvent(EVENT_MODIFY_STATE | SYNCHRONIZE, FALSE, LETRA_EVENTO_NOME);
    if (newEvent == NULL) {
        ProcessarErro(_T("[ERRO] Falha ao abrir evento de letras."));
    }
    while (1) {
        if (game->playerCount < 2) {
            _tprintf_s(_T("Esperando por mais um jogador...\n"));
            while (game->playerCount < 2)
                Sleep(1000);
        }

        _tprintf_s(_T("Jogo iniciado\n"));
        while (game->playerCount >= 2) {
            if (WaitForSingleObject(newEvent, INFINITE) != WAIT_OBJECT_0) {
                ProcessarErro(_T("[ERRO] Falha ao esperar pelo evento de letras."));
            }
            for (int i = 0; i < game->maxLetras; i++)
                _tprintf_s(_T("%c "), game->letras[i]);
            _tprintf_s(_T("\n"));

            ResetEvent(newEvent);
        }
        _tprintf_s(_T("Jogo terminado\n"));
    }
}

int IndiceJogador(TCHAR* username) {
    for (int i = 0; i < game->playerCount; i++) {
        if (_tcscmp(game->jogadores[i].username, username) == 0) {
            return i;
        }
    }
    return -1;
}

void ProcessarMensagem(Mensagem msg) {
    if (_tcsicmp(msg.texto, _T("encerrar")) == 0) {
        _tprintf_s(_T("Encerrando jogo...\n"));
        CloseHandle(hSharedMemory);
        exit(0);
    }
    else if (_tcsnicmp(msg.texto, _T("excluir"), 8) == 0) {
        _tprintf_s(_T("Excluindo jogador: %s\n"), msg.texto + 8);
        exit(0);
    }
    else if (_tcsicmp(msg.texto, _T("entrou")) == 0) {
        _tprintf_s(_T("[%s] Entrou\n"), msg.username);
    }
    else if (_tcsicmp(msg.texto, _T("saiu")) == 0) {
        _tprintf_s(_T("[%s] Saiu\n"), msg.username);
    }
    else {
        // Imprime qualquer outra mensagem genérica
        _tprintf_s(_T("[%s] %s\n"), msg.username, msg.texto);
    }
}


DWORD WINAPI ThreadLerMensagens() {
    Mensagem recebida;

    while (1) {
        ReceberMensagem(&recebida);
        ProcessarMensagem(recebida);
    }
}

void ProcessarPalavra(Mensagem msg) {
    if (_tcsicmp(msg.texto, _T(":pont")) == 0) {
        int index = IndiceJogador(jogador.username);

        _tprintf_s(_T(" Você têm %.1f pontos\n"), game->jogadores[index].pontos);
    }
    else if (_tcsicmp(msg.texto, _T(":jogs")) == 0) {
        for (int i = 0; i < MAX_JOGADORES; i++)
            if (game->jogadores[i].eAtivo)
                _tprintf_s(_T("%s\n"), game->jogadores[i].username);
    }
    else if (_tcsicmp(msg.texto, _T(":sair")) == 0) {
        EnviarMensagem(msg);

        _tprintf_s(_T("Deixando o Jogo...\n"));
        CloseHandle(jogador.pipe);
        exit(0);
    }
    else {
        EnviarMensagem(msg);
    }
}

void EnviarParaArbitro() {
    Mensagem enviada;
    _tcscpy_s(enviada.username, USERNAME_TAM, jogador.username);

    while (1) {
        _fgetts(enviada.texto, TAM_PALAVRA, stdin);
        enviada.texto[_tcslen(enviada.texto) - 1] = '\0';
        ProcessarPalavra(enviada);
    }
}

int _tmain(int argc, TCHAR* argv[]) {
    if (argc != 2) {
        _tprintf_s(_T("Sintaxe: %s <username>\n"), argv[0]);
        return 1;
    }

#ifdef UNICODE
    _setmode(_fileno(stdin), _O_WTEXT);
    _setmode(_fileno(stdout), _O_WTEXT);
    _setmode(_fileno(stderr), _O_WTEXT);
#endif

    DWORD tam = _tcslen(argv[1]);
    if (tam > USERNAME_TAM - 1) {
        _tprintf_s(_T("Username possui %d caracteres. O tamanho máximo são %d caracteres.\n"), tam, USERNAME_TAM - 1);
        exit(1);
    }

    memset(&jogador, 0, sizeof(Jogador));

    _tcsncpy_s(jogador.username, USERNAME_TAM, argv[1], USERNAME_TAM - 1);
    jogador.username[USERNAME_TAM - 1] = '\0';
    jogador.pontos = 0;
    jogador.eBot = FALSE;
    jogador.pipe = INVALID_HANDLE_VALUE;

    ConectarArbitro();
    InicializarMemoriaPartilhada();

    HANDLE hThreadServidor = CreateThread(NULL, 0, ThreadLerMensagens, NULL, 0, NULL);
    if (hThreadServidor == NULL) {
        ProcessarErro(_T("[ERRO] Falha ao criar thread do servidor\n"));
    }
    else
        CloseHandle(hThreadServidor);

    HANDLE hThreadLetras = CreateThread(NULL, 0, ThreadGerarLetras, NULL, 0, NULL);
    if (hThreadLetras == NULL) {
        ProcessarErro(_T("[ERRO] Falha ao criar thread de letras\n"));
    }
    else
        CloseHandle(hThreadLetras);

    EnviarParaArbitro();

    return 0;
}
