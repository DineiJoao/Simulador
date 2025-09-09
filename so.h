s#ifndef SO_H
#define SO_H

#include "cpu.h"

#define MAX_PROCESSOS 32   // limite de processos
#define PILHA_TAM 256      // tamanho da pilha de cada processo

typedef enum {
    PROC_NOVO,
    PROC_PRONTO,
    PROC_EXECUTANDO,
    PROC_BLOQUEADO,
    PROC_MORTO
} estado_proc_t;

typedef struct {
    int pid;                  // identificador único do processo
    estado_proc_t estado;     // estado do processo
    cpu_estado_t cpu_estado;  // registradores salvos
    int usado;                // indica se a entrada da tabela está em uso
    int entrada;              // número do terminal de entrada
    int saida;                // número do terminal de saída
} processo_t;

extern processo_t tabela_processos[MAX_PROCESSOS];
extern int processo_atual;   // índice na tabela_processos do processo em execução
extern int prox_pid;         // próximo pid a ser usado

void so_inicia();
void so_salva_estado_da_cpu(cpu_estado_t *estado);
void so_despacha(cpu_estado_t *estado);
void so_escalona();
int so_cria_processo(char *nome_programa);
void so_mata_processo(int pid);

#endif
