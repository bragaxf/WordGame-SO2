#ifndef UTILS_H
#define UTILS_H

#include <windows.h>
#include <fcntl.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <time.h>
#include <conio.h>

// Constants
#define PIPE_BUFFER_SIZE 2048
#define USERNAME_TAM 21
#define MAX_JOGADORES 20
#define MAX_LETRAS 12
#define RITMO 3
#define DICIONARIO_TAM 50
#define TAM_PALAVRA 25

// Registry Keys
#define REGISTRY_KEY _T("Software\\TrabSO2")
#define REGISTRY_MAX_LETRAS _T("MAXLETRAS")
#define REGISTRY_RITMO _T("RITMO")

// Named Pipes
#define PIPE_NOME _T("\\\\.\\pipe\\WordGame")
#define LETRA_EVENTO_NOME _T("LetraEvento")

// Shared Memory
#define SHARED_MEM_NOME _T("WordGameSharedMemory")
#define SHARED_MEM_TAM 1024

typedef struct {
	TCHAR username[USERNAME_TAM];
	TCHAR texto[TAM_PALAVRA];
} Mensagem;

typedef struct {
	TCHAR username[USERNAME_TAM];
	float pontos;
	BOOL eBot;
	BOOL eAtivo;
	HANDLE pipe;
} Jogador;

typedef struct {
	Jogador jogadores[MAX_JOGADORES];
	int playerCount;
	TCHAR letras[12];
	TCHAR ultimaPalavra[TAM_PALAVRA];
	int indiceLider;
	int maxLetras;
	int ritmo;
} Game;

#endif // UTILS_H