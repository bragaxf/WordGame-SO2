#include "../utils/utils.h"

Jogador bot;
Game* game;
HANDLE hSharedMemory;
TCHAR* dicionario[DICIONARIO_TAM];

void ProcessarErro(TCHAR* mensagem) {
    _tprintf_s(_T("[BOT][ERRO] %s (%lu)\n"), mensagem, GetLastError());
    exit(1);
}

void Limpeza() {
    if (bot.pipe != INVALID_HANDLE_VALUE)
        CloseHandle(bot.pipe);

    if (hSharedMemory != NULL) {
        UnmapViewOfFile(game);
        CloseHandle(hSharedMemory);
    }
}

void EnviarMensagem(Mensagem msg) {
    OVERLAPPED ov = { 0 };
    ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (!WriteFile(bot.pipe, &msg, sizeof(Mensagem), NULL, &ov)) {
        if (GetLastError() != ERROR_IO_PENDING)
            ProcessarErro(_T("Erro ao escrever no pipe."));
        WaitForSingleObject(ov.hEvent, INFINITE);
    }
    CloseHandle(ov.hEvent);
}

BOOL PedidoServidor() {
    Mensagem enviado, recebido;
    _tcscpy_s(enviado.username, USERNAME_TAM, bot.username);
    _tcscpy_s(enviado.texto, TAM_PALAVRA, _T("juntou-se"));

    EnviarMensagem(enviado);
    ReadFile(bot.pipe, &recebido, sizeof(Mensagem), NULL, NULL);

    return (_tcsicmp(recebido.texto, _T("aceito")) == 0);
}

void InicializarMemoriaPartilhada() {
    hSharedMemory = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, SHARED_MEM_NOME);
    if (hSharedMemory == NULL)
        ProcessarErro(_T("Falha ao abrir memoria partilhada."));

    game = (Game*)MapViewOfFile(hSharedMemory, FILE_MAP_ALL_ACCESS, 0, 0, SHARED_MEM_TAM);
    if (game == NULL)
        ProcessarErro(_T("Falha ao mapear memoria partilhada."));
}

void IniciarDicionario() {
    for (int i = 0; i < DICIONARIO_TAM; i++) {
        dicionario[i] = (TCHAR*)malloc(TAM_PALAVRA * sizeof(TCHAR));
    }

    _tcscpy_s(dicionario[0], TAM_PALAVRA, _T("a"));//gato
    _tcscpy_s(dicionario[1], TAM_PALAVRA, _T("b"));//elefante
    _tcscpy_s(dicionario[2], TAM_PALAVRA, _T("c"));//leao
    _tcscpy_s(dicionario[3], TAM_PALAVRA, _T("zebra"));
    _tcscpy_s(dicionario[4], TAM_PALAVRA, _T("girafa"));
    _tcscpy_s(dicionario[5], TAM_PALAVRA, _T("ovelha"));
    _tcscpy_s(dicionario[6], TAM_PALAVRA, _T("jacare"));
    _tcscpy_s(dicionario[7], TAM_PALAVRA, _T("pinguim"));
    _tcscpy_s(dicionario[8], TAM_PALAVRA, _T("tartaruga"));
    _tcscpy_s(dicionario[9], TAM_PALAVRA, _T("lobo"));
    _tcscpy_s(dicionario[10], TAM_PALAVRA, _T("coelho"));
    _tcscpy_s(dicionario[11], TAM_PALAVRA, _T("vaca"));
    _tcscpy_s(dicionario[12], TAM_PALAVRA, _T("pato"));
    _tcscpy_s(dicionario[13], TAM_PALAVRA, _T("galinha"));
    _tcscpy_s(dicionario[14], TAM_PALAVRA, _T("papagaio"));
    _tcscpy_s(dicionario[15], TAM_PALAVRA, _T("aguia"));
    _tcscpy_s(dicionario[16], TAM_PALAVRA, _T("tucano"));
    _tcscpy_s(dicionario[17], TAM_PALAVRA, _T("tubarao"));
    _tcscpy_s(dicionario[18], TAM_PALAVRA, _T("polvo"));
    _tcscpy_s(dicionario[19], TAM_PALAVRA, _T("lagosta"));
    _tcscpy_s(dicionario[20], TAM_PALAVRA, _T("formiga"));
    _tcscpy_s(dicionario[21], TAM_PALAVRA, _T("aranha"));
    _tcscpy_s(dicionario[22], TAM_PALAVRA, _T("mosca"));
    _tcscpy_s(dicionario[23], TAM_PALAVRA, _T("cigarra"));
    _tcscpy_s(dicionario[24], TAM_PALAVRA, _T("besouro"));
    _tcscpy_s(dicionario[25], TAM_PALAVRA, _T("cachorro"));
    _tcscpy_s(dicionario[26], TAM_PALAVRA, _T("tigre"));
    _tcscpy_s(dicionario[27], TAM_PALAVRA, _T("urso"));
    _tcscpy_s(dicionario[28], TAM_PALAVRA, _T("cavalo"));
    _tcscpy_s(dicionario[29], TAM_PALAVRA, _T("cobra"));
    _tcscpy_s(dicionario[30], TAM_PALAVRA, _T("canguru"));
    _tcscpy_s(dicionario[31], TAM_PALAVRA, _T("golfinho"));
    _tcscpy_s(dicionario[32], TAM_PALAVRA, _T("camelo"));
    _tcscpy_s(dicionario[33], TAM_PALAVRA, _T("raposa"));
    _tcscpy_s(dicionario[34], TAM_PALAVRA, _T("porco"));
    _tcscpy_s(dicionario[35], TAM_PALAVRA, _T("cervo"));
    _tcscpy_s(dicionario[36], TAM_PALAVRA, _T("ganso"));
    _tcscpy_s(dicionario[37], TAM_PALAVRA, _T("pavao"));
    _tcscpy_s(dicionario[38], TAM_PALAVRA, _T("arara"));
    _tcscpy_s(dicionario[39], TAM_PALAVRA, _T("falcao"));
    _tcscpy_s(dicionario[40], TAM_PALAVRA, _T("coruja"));
    _tcscpy_s(dicionario[41], TAM_PALAVRA, _T("peixe"));
    _tcscpy_s(dicionario[42], TAM_PALAVRA, _T("baleia"));
    _tcscpy_s(dicionario[43], TAM_PALAVRA, _T("estrela"));
    _tcscpy_s(dicionario[44], TAM_PALAVRA, _T("caranguejo"));
    _tcscpy_s(dicionario[45], TAM_PALAVRA, _T("abelha"));
    _tcscpy_s(dicionario[46], TAM_PALAVRA, _T("borboleta"));
    _tcscpy_s(dicionario[47], TAM_PALAVRA, _T("escorpiao"));
    _tcscpy_s(dicionario[48], TAM_PALAVRA, _T("grilo"));
    _tcscpy_s(dicionario[49], TAM_PALAVRA, _T("joaninha"));
}

BOOL LetrasEstaoVisiveis(const TCHAR* palavra) {
    for (int i = 0; i < _tcslen(palavra); i++) {
        TCHAR letra = _totupper(palavra[i]);
        BOOL encontrada = FALSE;
        for (int j = 0; j < game->maxLetras; j++) {
            if (_totupper(game->letras[j]) == letra) {
                encontrada = TRUE;
                break;
            }
        }
        if (!encontrada)
            return FALSE;
    }
    return TRUE;
}

void JogarAutomaticamente() {
    srand((unsigned int)time(NULL));
    while (1) {
        Sleep((rand() % 26 + 5) * 100); // entre 5 e 30 segundos

        int tentativa = rand() % DICIONARIO_TAM;
        TCHAR* palavra = dicionario[tentativa];

        if (LetrasEstaoVisiveis(palavra)) {
            Mensagem msg;
            _tcscpy_s(msg.username, USERNAME_TAM, bot.username);
            _tcscpy_s(msg.texto, TAM_PALAVRA, palavra);
            EnviarMensagem(msg);
            //_tprintf_s(_T("[BOT] Tentou: %s\n"), palavra);
        }
    }
}

int _tmain(int argc, TCHAR* argv[]) {
#ifdef UNICODE
    _setmode(_fileno(stdin), _O_WTEXT);
    _setmode(_fileno(stdout), _O_WTEXT);
    _setmode(_fileno(stderr), _O_WTEXT);
#endif

    if (argc != 2) {
        _tprintf_s(_T("Uso: %s <username>\n"), argv[0]);
        return 1;
    }

    _tcscpy_s(bot.username, USERNAME_TAM, argv[1]);

    bot.pipe = CreateFile(
        PIPE_NOME, GENERIC_READ | GENERIC_WRITE, 0,
        NULL, OPEN_EXISTING, 0, NULL);

    if (bot.pipe == INVALID_HANDLE_VALUE)
        ProcessarErro(_T("Erro ao conectar ao pipe."));

    if (!PedidoServidor()) {
        _tprintf_s(_T("[BOT] Nome de utilizador em uso ou jogo cheio.\n"));
        return 1;
    }

    InicializarMemoriaPartilhada();
    IniciarDicionario();

    JogarAutomaticamente();
    Limpeza();
    return 0;
}
