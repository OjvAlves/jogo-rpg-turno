#ifndef JOGADOR_H
#define JOGADOR_H

typedef struct {
    int vida;
    int vida_max;
    int escudo;
} Jogador;

// Inicializa jogador APENAS no início do jogo
void JogadorInit(Jogador* j);

#endif

