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

#define _CRT_SECURE_NO_WARNINGS

#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#include "game.h"
#include "inimigo.h"
#include "cartas.h"

// =====================================================
// CRIA UM INIMIGO COMPLETO (aleatório forte ou fraco)
// =====================================================
Inimigo CriarInimigo() {
    Inimigo e;

    // 5% de chance de ser forte
    int forte = (rand() % 100) < 5;

    if (forte) {
        // ================= INIMIGO FORTE =================
        e.tipo = INIMIGO_FORTE;
        e.vida_max = rand() % 61 + 40;       // 40–100 de vida
        e.ai.num_acoes = (rand() % 2) + 2;   // 2 ou 3 açőes

        int custo1_usado = 0;

        for (int i = 0; i < e.ai.num_acoes; i++) {

            Carta acao = CriarAcaoInimigoForte();

            // Permitir no máximo uma carta de custo 1
            if (acao.custo == 1) {
                if (custo1_usado) {
                    acao = CriarCartaAtaque(2 + rand() % 2);  // força um ataque custo 2 ou 3
                }
                else {
                    custo1_usado = 1;
                }
            }

            e.ai.acoes[i] = acao;
        }
    }
    else {
        // ================= INIMIGO FRACO =================
        e.tipo = INIMIGO_FRACO;
        e.vida_max = rand() % 21 + 10;       // 10–30 de vida
        e.ai.num_acoes = (rand() % 2) + 1;   // 1 ou 2 açőes

        for (int i = 0; i < e.ai.num_acoes; i++) {
            e.ai.acoes[i] = CriarAcaoInimigoFraco();
        }
    }

    // Valores iniciais
    e.vida = e.vida_max;
    e.escudo = 0;
    e.ai.indice = 0;
    e.congelado = 0;
    e.veneno_cargas = 0;  // NOVO: inicializa sem veneno
    e.veneno_dano = 0;    // NOVO: inicializa dano do veneno

    return e;
}

// =====================================================
// EXECUTA UMA AÇĂO DO INIMIGO CONTRA O JOGADOR
// =====================================================
void ExecutarAcaoInimigo(Inimigo* i, void* combat_ptr)
{
    if (i->vida <= 0 || i->congelado) return;
    
    Combat* c = (Combat*)combat_ptr;
    
    Carta acao = i->ai.acoes[i->ai.indice];  // açăo atual do ciclo

    // ===================== DEFESA =====================
    if (acao.tipo == CARTA_DEFESA) {
        i->escudo += acao.efeito;
        printf("Inimigo ganhou %d de escudo.\n", acao.efeito);
    }

    // ===================== ATAQUE =====================
    else if (acao.tipo == CARTA_ATAQUE) {

        int dano = acao.efeito;

        // Escudo absorve primeiro
        if (c->jogador.escudo > 0) {
            int reducao = (dano <= c->jogador.escudo ? dano : c->jogador.escudo);
            c->jogador.escudo -= reducao;
            dano -= reducao;  // sobra do dano
        }

        // Aplica dano ŕ vida
        if (dano > 0) {
            c->jogador.vida -= dano;
            if (c->jogador.vida < 0)
                c->jogador.vida = 0;
        }

        printf("Inimigo causou %d de dano.\n", acao.efeito);
    }

    // Próxima açăo do inimigo
    i->ai.indice = (i->ai.indice + 1) % i->ai.num_acoes;
}

// =====================================================
// APLICA EFEITO DE VENENO NO INIMIGO
// =====================================================
void AplicarVenenoInimigo(Inimigo* i) {
    if (i->veneno_cargas > 0 && i->vida > 0) {
        // Aplica dano do veneno (ignora escudo)
        i->vida -= i->veneno_dano;
        if (i->vida < 0) i->vida = 0;
        
        // Reduz uma carga
        i->veneno_cargas--;
        
        printf("Veneno ativo! Inimigo perdeu %d de vida. Cargas restantes: %d\n", 
               i->veneno_dano, i->veneno_cargas);
    }
}

// =====================================================
// GERA UMA AÇĂO DE INIMIGO FRACO
// =====================================================
Carta CriarAcaoInimigoFraco()
{
    Carta c;
    int custo = rand() % 2;  // 0 ou 1

    c.tipo = (rand() % 2 == 0 ? CARTA_ATAQUE : CARTA_DEFESA);
    c.custo = custo;
    c.efeito = GerarEfeito(custo);
    c.acao[0] = '\0';

    return c;
}

// =====================================================
// GERA UMA AÇĂO DE INIMIGO FORTE
// =====================================================
Carta CriarAcaoInimigoForte()
{
    Carta c;
    int custo = 1 + rand() % 3;  // 1 a 3

    c.tipo = (rand() % 2 == 0 ? CARTA_ATAQUE : CARTA_DEFESA);
    c.custo = custo;
    c.efeito = GerarEfeito(custo);
    c.acao[0] = '\0';

    return c;
}

// =====================================================
// GERAR INIMIGO FRACO ESPECÍFICO
// =====================================================
Inimigo CriarInimigoFraco()
{
    Inimigo i;

    i.tipo = INIMIGO_FRACO;
    i.vida_max = rand() % 21 + 10;
    i.vida = i.vida_max;
    i.escudo = 0;
    i.congelado = 0;
    i.veneno_cargas = 0;  // NOVO: inicializa sem veneno
    i.veneno_dano = 0;    // NOVO: inicializa dano do veneno

    i.ai.num_acoes = 1 + rand() % 2;
    i.ai.indice = 0;

    for (int j = 0; j < i.ai.num_acoes; j++)
        i.ai.acoes[j] = CriarAcaoInimigoFraco();

    return i;
}

// =====================================================
// GERAR INIMIGO FORTE ESPECÍFICO
// =====================================================
Inimigo CriarInimigoForte()
{
    Inimigo i;

    i.tipo = INIMIGO_FORTE;
    i.vida_max = rand() % 61 + 40;
    i.vida = i.vida_max;
    i.escudo = 0;
    i.congelado = 0;
    i.veneno_cargas = 0;  // NOVO: inicializa sem veneno
    i.veneno_dano = 0;    // NOVO: inicializa dano do veneno

    i.ai.num_acoes = 2 + rand() % 2;
    i.ai.indice = 0;

    int usou_custo_1 = 0;

    for (int j = 0; j < i.ai.num_acoes; j++) {

        Carta acao;

        do {
            acao = CriarAcaoInimigoForte();
        } while (acao.custo == 1 && usou_custo_1);

        if (acao.custo == 1)
            usou_custo_1 = 1;

        i.ai.acoes[j] = acao;
    }

    return i;
}

// =====================================================
// CRIA UM CHEFĂO (INIMIGO FORTE TURNO FINAL)
// =====================================================
Inimigo CriarInimigoChefao() {
   
    Inimigo c;

    c.tipo = INIMIGO_CHEFAO;
    c.vida_max = 150;
    c.vida = 150;
    c.escudo = 20;
    c.congelado = 0;
    c.veneno_cargas = 0;  // NOVO: inicializa sem veneno
    c.veneno_dano = 0;    // NOVO: inicializa dano do veneno
    c.ai.num_acoes = 3;
    c.ai.indice = 0;

    for (int j = 0; j < c.ai.num_acoes; j++) {
        Carta c2;
       
        // Usa apenas cartas de custo 2
        c2.custo = 2;
        
        // 50% ataque — 50% defesa
        if (rand() % 2 == 0) {
            c2.tipo = CARTA_ATAQUE;
            c2.efeito = 12 + rand() % 6;  // ataque 12~17
        }
        else {
            c2.tipo = CARTA_DEFESA;
            c2.efeito = 10 + rand() % 6;  // defesa 10~15
        }
        
        c2.acao[0] = '\0';
        c.ai.acoes[j] = c2;
    }

    return c;
}

void SortearAcoesInimigo(Inimigo* ini) {
    // exemplo simples: 3 açőes aleatórias
    for (int i = 0; i < ini->ai.num_acoes; i++) {
        int tipo = rand() % 3;  // ataque, defesa ou especial
        Carta c;

        if (tipo == 0) {
            c = CriarCartaAtaque(1);
        }
        else if (tipo == 1) {
            c = CriarCartaDefesa(1);
        }
        else {
            c = CriarCartaEspecial(); // CORRIGIDO: sem parâmetro
        }

        ini->ai.acoes[i] = c;
    }

    ini->ai.indice = 0; // sempre começa na primeira açăo
}