#ifndef INIMIGO_H
#define INIMIGO_H

#define _CRT_SECURE_NO_WARNINGS

#include "cartas.h"

// tipos de inimigos
#define INIMIGO_FRACO 1
#define INIMIGO_FORTE 2
#define INIMIGO_CHEFAO 3

// AI: até 3 açőes
typedef struct {
    Carta acoes[3];
    int num_acoes;
    int indice; // próxima açăo a executar
} EnemyAI;

typedef struct {
    int vida;
    int vida_max;
    int escudo;
    int tipo;      // INIMIGO_FRACO ou INIMIGO_FORTE
    int acao_tipo;   // 0 = ataque, 1 = defesa
    int acao_valor;  // dano ou escudo da próxima açăo
    int congelado; // NOVO: 0 = normal, 1 = congelado
    int veneno_cargas; // NOVO: cargas de veneno (0 = sem veneno)
    int veneno_dano;   // NOVO: dano por turno do veneno
    EnemyAI ai;
} Inimigo;

// ---- PROTÓTIPOS ----

// açőes individuais
Carta CriarAcaoInimigoFraco(void);
Carta CriarAcaoInimigoForte(void);

// criaçăo completa dos inimigos
Inimigo CriarInimigoFraco(void);
Inimigo CriarInimigoForte(void);
Inimigo CriarInimigoChefao(void);

// escolhe fraco ou forte (5% forte)
Inimigo CriarInimigo(void);
void SortearAcoesInimigo(Inimigo* ini);

// Executa açăo do inimigo
void ExecutarAcaoInimigo(Inimigo* i, void* combat);
// NOVA: Aplica efeito de veneno
void AplicarVenenoInimigo(Inimigo* i);
#endif