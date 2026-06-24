#define _CRT_SECURE_NO_WARNINGS
#include "jogador.h"

void JogadorInit(Jogador* j) {
    j->vida_max = 100;
    j->vida = 100;      // vida inicial do jogo
    j->escudo = 0;
}

