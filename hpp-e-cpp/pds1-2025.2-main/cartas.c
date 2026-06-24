// Para compatibilidade entre Windows e Linux
#ifndef _WIN32
    #ifndef strcpy_s
        #define strcpy_s(dest, size, src) do { \
            if (src) { \
                strncpy(dest, src, size - 1); \
                dest[size - 1] = '\0'; \
            } else { \
                dest[0] = '\0'; \
            } \
        } while(0)
    #endif
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "cartas.h"

// =====================================================
// GERAR EFEITO BASEADO NO CUSTO (tornada pública)
// =====================================================
int GerarEfeito(int custo) {
    switch (custo) {
    case 0: return (rand() % 5) + 1;      // 1–5
    case 1: return (rand() % 6) + 5;      // 5–10
    case 2: return (rand() % 6) + 10;     // 10–15
    case 3: return (rand() % 16) + 15;    // 15–30
    default: return 1;
    }
}

// =====================================================
// CRIA CARTA DE ATAQUE
// =====================================================
Carta CriarCartaAtaque(int custo) {
    Carta c;
    c.tipo = CARTA_ATAQUE;
    c.custo = custo;
    c.efeito = GerarEfeito(custo);
    strcpy_s(c.acao, sizeof(c.acao), "");
    return c;
}

// =====================================================
// CRIA CARTA DE DEFESA
// =====================================================
Carta CriarCartaDefesa(int custo) {
    Carta c;
    c.tipo = CARTA_DEFESA;
    c.custo = custo;
    c.efeito = GerarEfeito(custo);
    strcpy_s(c.acao, sizeof(c.acao), "");
    return c;
}

// =====================================================
// CRIA CARTA DE CURA (extra)
// =====================================================
Carta CriarCartaDeCura() {
    Carta c;
    c.tipo = CARTA_ESPECIAL;
    c.custo = 0; // custo fixo de 0
    c.efeito = 20;
    strcpy_s(c.acao, sizeof(c.acao), "Cura o jogador em 20");
    return c;
}

// =====================================================
// CRIA CARTA DE ENERGIA (extra)
// =====================================================
Carta CriarCartaEnergia() {
    Carta c;
    c.tipo = CARTA_ESPECIAL;
    c.custo = 0; // custo fixo de 0
    c.efeito = 3; // aumenta energia em 3
    strcpy_s(c.acao, sizeof(c.acao), "Ganha 3 de energia");
    return c;
}

// =====================================================
// CRIA CARTA DE DANO EM AREA (extra)
// =====================================================
Carta CriarCartaDanoEmArea() {
    Carta c;
    c.tipo = CARTA_ESPECIAL;
    c.custo = 0; // custo fixo de 0
    c.efeito = 15; // dano em area de 15
    strcpy_s(c.acao, sizeof(c.acao), "Dano em area!");
    return c;
}
// =====================================================
// CRIA CARTA GOLPE PROTETOR (extra)
// Causa 10 de dano E dá 10 de escudo, custo 0
// =====================================================
Carta CriarCartaGolpeProtetor() {
    Carta c;
    c.tipo = CARTA_ESPECIAL;
    c.custo = 0; // Custo zero
    c.efeito = 10; // efeito = 10 para ambos dano e escudo
    strcpy_s(c.acao, sizeof(c.acao), "Golpe Protetor: Dano 10 + Escudo 10");
    return c;
}

// =====================================================
// CRIA CARTA DE VENENO (extra)
// =====================================================
Carta CriarCartaVeneno() {
    Carta c;
    c.tipo = CARTA_ESPECIAL;
    c.custo = 2; // custo fixo de 2
    c.efeito = 4; // dano de veneno e dura 4 turnos
    strcpy_s(c.acao, sizeof(c.acao),  "Veneno: 4 cargas, 7 dano/turno");
    return c;
}

// =====================================================
// CRIA CARTA ESPECIAL
// =====================================================
Carta CriarCartaEspecial() {
    Carta c;
    c.tipo = CARTA_ESPECIAL;
    c.custo = 0;
    c.efeito = 0;
    strcpy_s(c.acao, sizeof(c.acao), "Recicla sua mao");
    return c;
}

// =====================================================
// MONTA O DECK INICIAL (20 cartas válidas)
// =====================================================
// CORREÇÃO: usar ponteiro em vez de array
void MontarDeckInicial(Carta* deck) {
    int i = 0;
    
    // ---- ATAQUES (10 cartas) ----
    // 1 de custo 0
    deck[i++] = CriarCartaAtaque(0);
    
    // 3 de custo 1
    for (int k = 0; k < 3; k++) {
        deck[i++] = CriarCartaAtaque(1);
    }
    
    // 1 de custo 2
    deck[i++] = CriarCartaAtaque(2);
    
    // 1 de custo 3
    deck[i++] = CriarCartaAtaque(3);
    
    // 4 aleatórias (custo 0-3)
    for (int k = 0; k < 4; k++) {
        int custo_aleatorio = rand() % 4; // 0-3
        deck[i++] = CriarCartaAtaque(custo_aleatorio);
    }
    
    // ---- DEFESAS (8 cartas) ----
    // 1 de custo 0
    deck[i++] = CriarCartaDefesa(0);
    
    // 3 de custo 1
    for (int k = 0; k < 3; k++) {
        deck[i++] = CriarCartaDefesa(1);
    }
    
    // 1 de custo 2
    deck[i++] = CriarCartaDefesa(2);
    
    // 1 de custo 3
    deck[i++] = CriarCartaDefesa(3);
    
    // 2 aleatórias (custo 0-3)
    for (int k = 0; k < 2; k++) {
        int custo_aleatorio = rand() % 4; // 0-3
        deck[i++] = CriarCartaDefesa(custo_aleatorio);
    }
    
    // ---- ESPECIAIS (2 cartas OBRIGATÓRIAS) ----
    // Array com as 6 opções
    Carta especiais[6];
    especiais[0] = CriarCartaEspecial();      // Reciclar
    especiais[1] = CriarCartaEnergia();       // Energia
    especiais[2] = CriarCartaDeCura();        // Cura
    especiais[3] = CriarCartaDanoEmArea();    // Dano área
    especiais[4] = CriarCartaGolpeProtetor(); // Golpe Protetor
    especiais[5] = CriarCartaVeneno();       // Veneno
    
    // Sorteio da primeira especial
    int esp1 = rand() % 6;
    deck[i] = especiais[esp1];
    i++;
    
    // Sorteio da segunda especial (diferente)
    int esp2;
    do {
        esp2 = rand() % 4;
    } while (esp2 == esp1);
    
    deck[i] = especiais[esp2];
    i++;
    
    // ---- VERIFICAÇÃO FINAL ----
    int count_ataque = 0, count_defesa = 0, count_especial = 0;
    
    for (int j = 0; j < DECK_FINAL; j++) {
        if (deck[j].tipo == CARTA_ATAQUE) {
            count_ataque++;
        }
        else if (deck[j].tipo == CARTA_DEFESA) {
            count_defesa++;
        }
        else if (deck[j].tipo == CARTA_ESPECIAL) {
            count_especial++;
        }
    }
    
    if (count_especial != 2) {
        // Remove especiais extras
        for (int j = 0; j < DECK_FINAL && count_especial > 2; j++) {
            if (deck[j].tipo == CARTA_ESPECIAL) {
                // Substitui por ataque básico
                deck[j] = CriarCartaAtaque(1);
                count_especial--;
            }
        }
    }
}

// =====================================================
// INICIALIZA UM CARDGROUP
// =====================================================
void CardGroupInit(CardGroup* g, int capacidade) {
    g->cartas = (Carta*)malloc(sizeof(Carta) * capacidade);
    g->size = 0;
    g->capacity = capacidade;
    
    if (g->cartas == NULL) {
        printf("FATAL: erro de memoria em CardGroupInit\n");
        exit(1);
    }
}

// =====================================================
// ADICIONAR CARTA AO CARDGROUP
// =====================================================
void CardGroupAdd(CardGroup* g, Carta c) {
    if (g->size >= g->capacity) {
        printf("ERRO: tentando adicionar carta acima da capacidade!\n");
        return;
    }
    g->cartas[g->size++] = c;
}