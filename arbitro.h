#ifndef ARBITRO_H
#define ARBITRO_H

void Limpeza();
void ProcessarErro(TCHAR* mensagem);
void IniciarDicionario();
void LerConfiguracoesRegistry();
void EscreverMensagem(HANDLE hPipe, Mensagem msg);
void LerMensagem(HANDLE hPipe, Mensagem* msg);
void InicializarMemoriaPartilhada();
void CriarMemoriaPartilhada();

void SortearNovaLetra();
int IndiceJogador(const TCHAR* username);
DWORD WINAPI ThreadSortearLetras(LPVOID lpParam);
void NotificarTodos(Mensagem msg);
void AdicionarJogador(HANDLE* hPipe, const TCHAR* username);
void RemoverJogador(Jogador* jogador);
void ProcessarMensagem(Mensagem msg);
DWORD WINAPI ThreadLerMensagens();
BOOL ValidarPalavra(const TCHAR* palavra);
void ProcessarPalavra(Jogador* jogador, Mensagem msg);
DWORD WINAPI MonitorJogadorThread(Jogador* jogador);
BOOL ProcessarPedido(HANDLE* hPipe, TCHAR* username);
void ConectarJogadores();

#endif // ARBITRO_H