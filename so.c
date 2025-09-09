#include <stdio.h>
#include <string.h>
#include "so.h"
#include "cpu.h"
#include "memoria.h"
#include "err.h"

processo_t tabela_processos[MAX_PROCESSOS];
int processo_atual = -1;
int prox_pid = 1;  // PID inicial (0 pode ser reservado para init)

// Função auxiliar: encontra índice livre na tabela
static int encontra_entrada_livre() {
    for (int i = 0; i < MAX_PROCESSOS; i++) {
        if (!tabela_processos[i].usado)
            return i;
    }
    return -1;
}

void so_inicia() {
    memset(tabela_processos, 0, sizeof(tabela_processos));
    processo_atual = -1;
    prox_pid = 1;
}

// Salva o estado do processo atual
void so_salva_estado_da_cpu(cpu_estado_t *estado) {
    if (processo_atual >= 0 && tabela_processos[processo_atual].usado) {
        tabela_processos[processo_atual].cpu_estado = *estado;
    }
}

// Restaura estado do processo escolhido
void so_despacha(cpu_estado_t *estado) {
    if (processo_atual >= 0 && tabela_processos[processo_atual].usado) {
        *estado = tabela_processos[processo_atual].cpu_estado;
        tabela_processos[processo_atual].estado = PROC_EXECUTANDO;
    }
}

// Escalonador simples: mantém processo atual se ainda pronto; senão pega o próximo pronto
void so_escalona() {
    if (processo_atual >= 0 && tabela_processos[processo_atual].estado == PROC_PRONTO) {
        return; // continua o mesmo
    }

    for (int i = 0; i < MAX_PROCESSOS; i++) {
        if (tabela_processos[i].usado && tabela_processos[i].estado == PROC_PRONTO) {
            processo_atual = i;
            return;
        }
    }

    processo_atual = -1; // nenhum processo pronto
}

// Cria um novo processo
int so_cria_processo(char *nome_programa) {
    int idx = encontra_entrada_livre();
    if (idx < 0) return -1;

    processo_t *p = &tabela_processos[idx];
    memset(p, 0, sizeof(*p));
    p->pid = prox_pid++;
    p->estado = PROC_PRONTO;
    p->usado = 1;

    // Carrega programa na memória
    if (mem_carrega_programa(nome_programa, &p->cpu_estado.PC) < 0) {
        p->usado = 0;
        return -1;
    }

    // Inicializa registradores
    p->cpu_estado.modo = usuario;
    p->cpu_estado.SP = MEM_TAM - 1; // pilha no final da memória

    return p->pid;
}

// Mata processo
void so_mata_processo(int pid) {
    for (int i = 0; i < MAX_PROCESSOS; i++) {
        if (tabela_processos[i].usado && tabela_processos[i].pid == pid) {
            tabela_processos[i].estado = PROC_MORTO;
            tabela_processos[i].usado = 0;
            if (processo_atual == i) {
                processo_atual = -1;
            }
            return;
        }
    }
}
