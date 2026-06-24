#ifndef GAME_H
#define GAME_H

#define _CRT_SECURE_NO_WARNINGS

#include "cartas.h"
#include "jogador.h"
#include "inimigo.h"

// Representa um combate inteiro
typedef struct {
    Jogador jogador;
    Inimigo inimigos[2];
    int num_inimigos;

    CardGroup deck_compra;
    CardGroup deck_descarte;
    CardGroup mao;

    int energia;
    int turno;
    int num_vitorias;
    int rodada_atual;
    int e_chefao;
    int cura_usada; // 0 = ainda não usou, 1 = já usou
    int estado; // 0 = selecionar inimigo, 1 = selecionar carta
    int inimigo_selecionado;
} Combat;

// protótipos para controle geral do jogo
void IniciarCombat(Combat* c);
void AtualizarCombat(Combat* c, int tecla);
int CombatAcabou(Combat* c);

// Funções da mão do jogador
void ComprarMao(Combat* c);
void ComprarCarta(Combat* c);
void DescartarMao(Combat* c);

void JogarCarta(Combat* c, int icard, int ienemy);
// Função para verificar se carta precisa de alvo
int CartaPrecisaDeAlvo(Carta carta);
#endif