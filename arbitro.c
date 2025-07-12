#include "../utils/utils.h"
#include "arbitro.h"

Game* game;
HANDLE hSharedMemory, newEvent;
TCHAR* dicionario[DICIONARIO_TAM];

//auxiliares

void Limpeza() {
    for (int i = 0; i < game->playerCount; i++)
        if (game->jogadores[i].pipe != INVALID_HANDLE_VALUE) {
            FlushFileBuffers(game->jogadores[i].pipe);
            DisconnectNamedPipe(game->jogadores[i].pipe);
            CloseHandle(game->jogadores[i].pipe);
        }

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

void IniciarDicionario() {
    for (int i = 0; i < DICIONARIO_TAM; i++) {
        dicionario[i] = (TCHAR*)malloc(TAM_PALAVRA * sizeof(TCHAR));
        if (dicionario[i] == NULL)
            ProcessarErro(_T("Falha ao alocar memória para o dicionário."));
    }
    _tcscpy_s(dicionario[0], TAM_PALAVRA, _T("ph"));//gato
    _tcscpy_s(dicionario[1], TAM_PALAVRA, _T("v"));//elefante
    _tcscpy_s(dicionario[2], TAM_PALAVRA, _T("ax"));//leao
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

void LerConfiguracoesRegistry() {
    HKEY hKey;
    DWORD tam = sizeof(DWORD);
    DWORD valor = NULL;
    if (RegCreateKeyEx(HKEY_CURRENT_USER, REGISTRY_KEY, 0, NULL, 0, KEY_READ | KEY_WRITE, NULL, &hKey, NULL) != ERROR_SUCCESS) {
        _tprintf_s(_T("Erro ao criar ou abrir chave de registro."));
        game->maxLetras = MAX_LETRAS;
        game->ritmo = RITMO;
    }
    else {
        if (RegQueryValueEx(hKey, REGISTRY_MAX_LETRAS, NULL, NULL, (LPBYTE)&valor, &tam) == ERROR_SUCCESS) {
            game->maxLetras = (int)valor; // Garante máximo de 12
            if (game->maxLetras > 12)
                game->maxLetras = 12; // Garante mínimo de 1 letra
        }
        else {
            valor = MAX_LETRAS;
            RegSetValueEx(hKey, REGISTRY_MAX_LETRAS, 0, REG_DWORD, (const BYTE*)&valor, sizeof(DWORD));
        }

        if (RegQueryValueEx(hKey, REGISTRY_RITMO, NULL, NULL, (LPBYTE)&valor, &tam) == ERROR_SUCCESS) {
            game->ritmo = (int)valor;
            if (game->ritmo < 1)
                game->ritmo = 1; // Garante mínimo de 1 segundo
        }
        else {
            valor = RITMO;
            RegSetValueEx(hKey, REGISTRY_RITMO, 0, REG_DWORD, (const BYTE*)&valor, sizeof(DWORD));
        }
    }

    RegCloseKey(hKey);
}

void EscreverMensagem(HANDLE hPipe, Mensagem msg) {
    OVERLAPPED ov = { 0 };
    ov.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (ov.hEvent == NULL) {
        ProcessarErro(_T("Falha ao criar evento de escrita."));
    }
    if (!WriteFile(hPipe, &msg, sizeof(Mensagem), NULL, &ov)) {
        if (GetLastError() != ERROR_IO_PENDING)
            ProcessarErro(_T("Erro ao escrever no pipe."));
        WaitForSingleObject(ov.hEvent, INFINITE);
    }
    CloseHandle(ov.hEvent);
}

void LerMensagem(HANDLE hPipe, Mensagem* msg) {
    if (!ReadFile(hPipe, msg, sizeof(Mensagem), NULL, NULL)) {
        ProcessarErro(_T("Erro ao ler do pipe."));
    }
}

void InicializarMemoriaPartilhada() {
    for (int i = 0; i < game->maxLetras; i++) {
        game->letras[i] = '_';
    }
    for (int i = 0; i < MAX_JOGADORES; i++) {
        game->jogadores[i].eAtivo = FALSE;
    }
}

void CriarMemoriaPartilhada() {
    hSharedMemory = CreateFileMapping(INVALID_HANDLE_VALUE, NULL,
        PAGE_READWRITE, 0, SHARED_MEM_TAM, SHARED_MEM_NOME);

    if (hSharedMemory == NULL) {
        ProcessarErro(_T("Erro ao abrir memória compartilhada."));
    }
    game = (Game*)MapViewOfFile(hSharedMemory, FILE_MAP_ALL_ACCESS, 0, 0, SHARED_MEM_TAM);
    if (game == NULL) {
        ProcessarErro(_T("Erro ao mapear memória compartilhada."));
    }
    memset(game, 0, sizeof(Game));
}

//principais

void SortearNovaLetra() {
    static const TCHAR alfabeto[] = _T("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    TCHAR nova = alfabeto[rand() % 26];

    int espaco = -1;
    for (int i = 0; i < game->maxLetras; i++) {
        if (game->letras[i] == '_') {
            espaco = i;
            break;
        }
    }
    static int index = 0;

    if (espaco < 0) {
        game->letras[index] = nova;
        index = ((index + 1) % game->maxLetras);
    }
    else {
        game->letras[espaco] = nova;
    }
}

int IndiceJogador(const TCHAR* username) {
    for (int i = 0; i < game->playerCount; i++) {
        if (_tcscmp(game->jogadores[i].username, username) == 0)
            return i;
    }
    return -1;
}

DWORD WINAPI ThreadSortearLetras(LPVOID lpParam) {
    newEvent = CreateEvent(NULL, TRUE, FALSE, LETRA_EVENTO_NOME);
    if (newEvent == NULL) {
        ProcessarErro(_T("Erro ao criar evento de letras."));
    }
    while (1) {
        _tprintf_s(_T("Esperando por mais jogadores...\n"));
        while (game->playerCount < 2) {
            Sleep(1000); // Espera até que haja pelo menos 2 jogadores
        }
        _tprintf_s(_T("Iniciando sorteio de letras...\n"));
        while (game->playerCount >= 2) {
            Sleep(game->ritmo * 1000); // Espera o ritmo definido
            SortearNovaLetra();

            SetEvent(newEvent);
            ResetEvent(newEvent);

        }
        _tprintf_s(_T("Jogo encerrado. Aguardando novos jogadores...\n"));
    }
}

void NotificarTodos(Mensagem msg) {
    for (int i = 0; i < game->playerCount; i++) {
        if (game->jogadores[i].eAtivo) {
            EscreverMensagem(game->jogadores[i].pipe, msg);
        }
    }
}

void AdicionarJogador(HANDLE* hPipe, const TCHAR* username) {
    Mensagem msg;
    _tcscpy_s(msg.username, USERNAME_TAM, username);
    _tcscpy_s(msg.texto, TAM_PALAVRA, _T("entrou"));
    NotificarTodos(msg);

    Jogador jogador;
    _tcscpy_s(jogador.username, USERNAME_TAM, username);
    jogador.pontos = 0;
    jogador.eBot = FALSE;
    jogador.eAtivo = TRUE;
    jogador.pipe = *hPipe;
    for (int i = 0; i < MAX_JOGADORES; i++) {
        if (!game->jogadores[i].eAtivo) {
            game->jogadores[i] = jogador;
            break;
        }
    }
    game->playerCount++;
}

void RemoverJogador(Jogador* jogador) {
    jogador->eAtivo = FALSE;

    FlushFileBuffers(jogador->pipe);
    DisconnectNamedPipe(jogador->pipe);
    CloseHandle(jogador->pipe);

    game->playerCount--;

    Mensagem msg;
    _tcscpy_s(msg.username, USERNAME_TAM, jogador->username);
    _tcscpy_s(msg.texto, TAM_PALAVRA, _T("saiu"));
    NotificarTodos(msg);
}

void ProcessarMensagem(Mensagem msg) {
    if (_tcsicmp(msg.texto, _T("listar")) == 0) {
        for (int i = 0; i < game->playerCount; i++)
            if (game->jogadores[i].eAtivo)
                _tprintf_s(_T("Jogador: %s, Pontos: %.2f\n"), game->jogadores[i].username, game->jogadores[i].pontos);
    }
    else if (_tcsnicmp(msg.texto, _T("excluir "), 8) == 0) {
        int index = IndiceJogador(msg.texto + 8);

        EscreverMensagem(game->jogadores[index].pipe, msg);
        RemoverJogador(&game->jogadores[index]);
    }
    else if (_tcsnicmp(msg.texto, _T("iniciarbot "), 11) == 0) {
        TCHAR comando[100];
        TCHAR* nome = msg.texto + 11;
        _stprintf_s(comando, 100, _T("bot.exe %s"), nome);

        STARTUPINFO si = { sizeof(si) };
        PROCESS_INFORMATION pi;

        if (!CreateProcess(NULL, comando, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
            _tprintf_s(_T("Erro ao iniciar bot: %lu\n"), GetLastError());
        }
        else {
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        }
    }
    else if (_tcsicmp(msg.texto, _T("acelerar")) == 0) {
        if (game->ritmo > 1)
            game->ritmo--;
        _tprintf_s(_T("Ritmo acelerado para %d segundos.\n"), game->ritmo);
    }
    else if (_tcsicmp(msg.texto, _T("travar")) == 0) {
        game->ritmo++;
        _tprintf_s(_T("Ritmo travado em %d segundos.\n"), game->ritmo);
    }
    else if (_tcsicmp(msg.texto, _T("encerrar")) == 0) {
        for (int i = 0; i < MAX_JOGADORES; i++)
            if (game->jogadores[i].eAtivo)
                EscreverMensagem(game->jogadores[i].pipe, msg);

        _tprintf_s(_T("Terminando jogo...\n"));
        Limpeza();
        exit(0);
    }
}

DWORD WINAPI ThreadLerMensagens() {
    Mensagem msg;
    _tcscpy_s(msg.username, USERNAME_TAM, _T("Arbitro"));
    while (1) {
        _fgetts(msg.texto, TAM_PALAVRA, stdin);
        msg.texto[_tcslen(msg.texto) - 1] = '\0';
        ProcessarMensagem(msg);
    }
}

BOOL ValidarPalavra(const TCHAR* palavra) {
    // Verifica se a palavra existe no dicionário
    BOOL encontrada = FALSE;
    for (int i = 0; i < DICIONARIO_TAM; i++) {
        if (_tcscmp(palavra, dicionario[i]) == 0) {
            encontrada = TRUE;
            break;
        }
    }

    if (!encontrada)
        return FALSE;

    // Verifica se todas as letras da palavra estao nas letras visíveis 
    for (int i = 0; i < _tcslen(palavra); i++) {
        TCHAR letra = _totupper(palavra[i]);
        BOOL visivel = FALSE;

        for (int j = 0; j < game->maxLetras; j++) {
            if (_totupper(game->letras[j]) == letra) {
                visivel = TRUE;
                break;
            }
        }

        if (!visivel)
            return FALSE;
    }


    return TRUE;
}

void VerificarNovoLider(Jogador* jogador) {
    static TCHAR liderAtual[USERNAME_TAM] = _T(""); // Mantém o líder anterior 

    // Verifica se o jogador atual ultrapassou os outros
    BOOL novoLider = TRUE;
    for (int i = 0; i < game->playerCount; i++) {
        if (game->jogadores[i].eAtivo &&
            _tcscmp(game->jogadores[i].username, jogador->username) != 0 &&
            game->jogadores[i].pontos >= jogador->pontos) {
            novoLider = FALSE;
            break;
        }
    }

    // verificar se ja n era
    if (novoLider && _tcscmp(liderAtual, jogador->username) != 0) {
        _tcscpy_s(liderAtual, USERNAME_TAM, jogador->username);

        Mensagem msg;
        _tcscpy_s(msg.username, USERNAME_TAM, _T("Arbitro"));
        _stprintf_s(msg.texto, TAM_PALAVRA, _T("%s passou à frente"),
            jogador->username);

        NotificarTodos(msg);
    }
}

void ProcessarPalavra(Jogador* jogador, Mensagem msg) {
    if (ValidarPalavra(msg.texto)) {
        for (int i = 0; i < _tcslen(msg.texto); i++) {
            TCHAR letra = _totupper(msg.texto[i]);
            for (int j = 0; j < game->maxLetras; j++) {
                if (_totupper(game->letras[j]) == letra) {
                    game->letras[j] = _T('_');
                    jogador->pontos += 1;
                    VerificarNovoLider(jogador);
                    break;
                }
            }
        }
        _tcscpy_s(game->ultimaPalavra, TAM_PALAVRA, msg.texto);
        _tprintf_s(_T("[%s] Palavra válida: %s\n"), msg.username, msg.texto);
        Mensagem resposta;
        _tcscpy_s(resposta.username, USERNAME_TAM, _T("Arbitro"));
        _stprintf_s(resposta.texto, TAM_PALAVRA, _T("%s adivinhou %s"), msg.username, msg.texto);
        NotificarTodos(resposta);

    }
    else {
        jogador->pontos -= 0.5 * _tcslen(msg.texto);
        _tprintf_s(_T("[%s] Palavra inválida: %s\n"), msg.username, msg.texto);
    }
}

DWORD WINAPI MonitorJogadorThread(Jogador* jogador) {
    Mensagem msg;
    while (1) {
        LerMensagem(jogador->pipe, &msg);
        if (_tcsicmp(msg.texto, _T(":sair")) == 0) {
            _tprintf_s(_T("[%s] Saiu\n"), msg.username);
            RemoverJogador(jogador);
            return 0;
        }

        ProcessarPalavra(jogador, msg);
    }
}

BOOL ProcessarPedido(HANDLE* hPipe, TCHAR* username) {
    Mensagem recebido, enviado;
    _tcscpy_s(enviado.username, USERNAME_TAM, _T("Servidor"));

    LerMensagem(*hPipe, &recebido);
    _tcscpy_s(username, USERNAME_TAM, recebido.username);

    if (IndiceJogador(username) >= 0) {
        _tcscpy_s(enviado.texto, USERNAME_TAM, _T("Ocupado"));
        EscreverMensagem(*hPipe, recebido);

        CloseHandle(*hPipe);
        return FALSE;
    }
    else if (game->playerCount == MAX_JOGADORES) {
        _tcscpy_s(enviado.texto, USERNAME_TAM, _T("Cheio"));
        EscreverMensagem(*hPipe, recebido);

        CloseHandle(*hPipe);
        return FALSE;
    }

    _tcscpy_s(enviado.texto, USERNAME_TAM, _T("Aceito"));
    EscreverMensagem(*hPipe, enviado);

    return TRUE;
}

void ConectarJogadores() {
    while (1) {
        HANDLE hPipe = CreateNamedPipe(
            PIPE_NOME, PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
            PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
            MAX_JOGADORES, PIPE_BUFFER_SIZE, PIPE_BUFFER_SIZE, 5000, NULL);

        if (hPipe == INVALID_HANDLE_VALUE) {
            ProcessarErro(_T("Erro ao criar pipe nomeado."));
        }
        if (!(ConnectNamedPipe(hPipe, NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED))) {
            ProcessarErro(_T("Erro ao conectar ao pipe nomeado."));
        }
        TCHAR username[USERNAME_TAM];
        if (!ProcessarPedido(&hPipe, username))
            continue;

        _tprintf_s(_T("Jogador %s conectado.\n"), username);
        AdicionarJogador(&hPipe, username);

        HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)MonitorJogadorThread, &game->jogadores[game->playerCount - 1], 0, NULL);
        if (hThread == NULL)
            ProcessarErro(_T("Erro ao criar thread para monitorar jogador."));
        else
            CloseHandle(hThread);
    }
}

int _tmain() {
#ifdef UNICODE
    _setmode(_fileno(stdin), _O_WTEXT);
    _setmode(_fileno(stdout), _O_WTEXT);
    _setmode(_fileno(stderr), _O_WTEXT);
#endif

    HANDLE hMutex = CreateMutex(NULL, FALSE, _T("WordGameMutex"));
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        _tprintf_s(_T("Já existe uma instância do arbitro em execução.\n"));
        exit(0);
    }
    CloseHandle(hMutex);

    CriarMemoriaPartilhada();
    LerConfiguracoesRegistry();
    InicializarMemoriaPartilhada();
    IniciarDicionario();
    _tprintf_s(_T("Servidor Aberto!"));

    HANDLE hInputThread = CreateThread(NULL, 0, ThreadLerMensagens, NULL, 0, NULL);
    if (hInputThread == NULL)
        ProcessarErro(_T("Erro ao criar thread de leitura de mensagens."));
    else
        CloseHandle(hInputThread);

    HANDLE hSortearLetrasThread = CreateThread(NULL, 0, ThreadSortearLetras, NULL, 0, NULL);
    if (hSortearLetrasThread == NULL)
        ProcessarErro(_T("Erro ao criar thread de sorteio de letras."));
    else
        CloseHandle(hSortearLetrasThread);

    ConectarJogadores();

    return 0;
}
