#ifndef CARTAS_H
#define CARTAS_H

#define _CRT_SECURE_NO_WARNINGS

#define DECK_FINAL 20

#define CARTA_ATAQUE 0
#define CARTA_DEFESA 1
#define CARTA_ESPECIAL 2

typedef struct {
    int tipo;        // 0 ataque, 1 defesa, 2 especial
    int custo;       // 0 a 3
    int efeito;      // dano ou defesa
    char acao[100];  // usado apenas para cartas especiais
} Carta;

typedef struct {
    Carta* cartas;
    int size;
    int capacity;
} CardGroup;

// ----------- funções de criação de cartas ----------
Carta CriarCartaAtaque(int custo);
Carta CriarCartaDefesa(int custo);
Carta CriarCartaEspecial();
Carta CriarCartaDeCura(); // extra  
Carta CriarCartaEnergia(); 
Carta CriarCartaDanoEmArea();
Carta CriarCartaMultEfeitos();
Carta CriarCartaGolpeProtetor(); // NOVA: Dano 10 + Escudo 10, custo 0

// ----------- deck inicial ----------
// CORREÇÃO: mudar para ponteiro para evitar warning de array
void MontarDeckInicial(Carta* deck); // antes era Carta[DECK_FINAL]

// ----------- grupos de cartas ----------
void CardGroupInit(CardGroup* g, int capacidade);
void CardGroupAdd(CardGroup* g, Carta c);

// Função para gerar efeito (adicionada)
int GerarEfeito(int custo);

#endif