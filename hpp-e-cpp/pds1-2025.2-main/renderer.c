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

#include "renderer.h"
#include "constants.h"
#include "utils.h"

#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// seleções externas
extern int carta_sel;
extern int inimigo_sel;

// utilitárias já presentes no seu código (mantive)

// Função auxiliar para desenhar cartas
static void DrawCardVisual(ALLEGRO_FONT* font, Carta cr, int x, int y, int selected, int playable) {
    ALLEGRO_COLOR bg = playable ? al_map_rgb(245, 245, 245) : al_map_rgb(255, 0, 0);
    al_draw_filled_rounded_rectangle(x, y, x + CARD_WIDTH, y + CARD_HEIGHT, 8, 8, bg);
    al_draw_rounded_rectangle(x, y, x + CARD_WIDTH, y + CARD_HEIGHT, 8, 8, al_map_rgb(0, 0, 0), 2);

    if (selected) al_draw_rectangle(x - 4, y - 8, x + CARD_WIDTH + 4, y + CARD_HEIGHT + 8, al_map_rgb(255, 215, 0), 3);

    const char* tipo_txt =
        cr.tipo == CARTA_ATAQUE ? "ATK" :
        cr.tipo == CARTA_DEFESA ? "DEF" :
        cr.tipo == CARTA_ESPECIAL ? "SPC" : "UNK";

    char buf[64];
    sprintf(buf, "%s  C:%d  E:%d", tipo_txt, cr.custo, cr.efeito);
    al_draw_text(font, al_map_rgb(0, 0, 0), x + 8, y + 8, 0, buf);

    if (cr.tipo == CARTA_ESPECIAL && cr.acao[0] != '\0') {
        al_draw_text(font, al_map_rgb(0, 0, 0), x + 8, y + 40, 0, cr.acao);
    }
}

// Função auxiliar para fundo
static void RenderBackground(Renderer* r) {
    al_clear_to_color(al_map_rgb(30, 30, 30));
}

// Função auxiliar para renderizar inimigo individual
static void RenderSingleEnemy(Renderer* r, Inimigo* inim, int idx) {
    Combat* c = r->combat;
    int base_x = DISPLAY_WIDTH - 220;
    int base_y = 120 + idx * 220;

    if (inim->vida <= 0) return;

    // COR DA SELEÇÃO - NOVO: Destaque visual para inimigo selecionado
    ALLEGRO_COLOR cor_borda = al_map_rgb(255, 255, 255);
    int espessura_borda = 2;

    if (c->estado == 1 && idx == c->inimigo_selecionado) {
        cor_borda = al_map_rgb(255, 255, 0); // Amarelo para selecionado
        espessura_borda = 6;
    }

    // Corpo do inimigo
    al_draw_filled_circle(base_x, base_y, 64, al_map_rgb(255, 180, 180));
    al_draw_circle(base_x, base_y, 64, cor_borda, espessura_borda);

    // Barra de vida
    int bar_w = 140;
    int bar_h = 16;
    int bar_x = base_x - bar_w / 2;
    int bar_y = base_y + 80;
    float vida_frac = (float)inim->vida / inim->vida_max;
    if (vida_frac < 0) vida_frac = 0;
    if (vida_frac > 1) vida_frac = 1;

    al_draw_filled_rounded_rectangle(bar_x - 2, bar_y - 2, bar_x + bar_w + 2,
        bar_y + bar_h + 2, 6, 6, al_map_rgb(255, 255, 255));
    al_draw_filled_rounded_rectangle(bar_x, bar_y,
        bar_x + (int)(bar_w * vida_frac), bar_y + bar_h,
        6, 6, al_map_rgb(200, 0, 0));
    al_draw_rounded_rectangle(bar_x, bar_y, bar_x + bar_w, bar_y + bar_h,
        6, 6, al_map_rgb(0, 0, 0), 2);

    char vida_txt[32];
    sprintf(vida_txt, "%d/%d", inim->vida, inim->vida_max);
    al_draw_text(r->font, al_map_rgb(0, 0, 0), bar_x + 6, bar_y + 1, 0, vida_txt);

    if (inim->escudo > 0) {
        char escudo_txt[32];
        sprintf(escudo_txt, "Esc: %d", inim->escudo);
        al_draw_text(r->font, al_map_rgb(150, 200, 255), bar_x + bar_w - 60, bar_y + 1, 0, escudo_txt);
    }
     // ============================================
    // NOVO: INDICADOR DE VENENO
    // ============================================
    if (inim->veneno_cargas > 0) {
        char veneno_txt[32];
        sprintf(veneno_txt, "Ven: %d(%d)", inim->veneno_dano, inim->veneno_cargas);
        
        // Desenha em verde venenoso
        // Posiciona abaixo da barra de vida
        al_draw_text(r->font, al_map_rgb(0, 200, 0), 
                     bar_x + bar_w/2, bar_y + bar_h + 5,
                     ALLEGRO_ALIGN_CENTER, veneno_txt);
    }
    

    // ================================
    // ÍCONE DA PRÓXIMA AÇÃO
    // ================================
    if (inim->ai.num_acoes > 0) {
        Carta prox = inim->ai.acoes[inim->ai.indice];

        int icon_y1 = base_y - 110;
        ALLEGRO_COLOR cor;

        if (prox.tipo == CARTA_ATAQUE)      cor = al_map_rgb(255, 80, 80);
        else if (prox.tipo == CARTA_DEFESA) cor = al_map_rgb(80, 160, 255);
        else                                 cor = al_map_rgb(255, 200, 80);

        // Ícone da ação
        al_draw_filled_circle(base_x, icon_y1 + 25, 25, cor);
        al_draw_circle(base_x, icon_y1 + 25, 25, al_map_rgb(0, 0, 0), 2);

        // LETRA NO ÍCONE
        const char* letra =
            prox.tipo == CARTA_ATAQUE ? "A" :
            prox.tipo == CARTA_DEFESA ? "D" : "S";

        al_draw_text(r->font, al_map_rgb(0, 0, 0),
            base_x, icon_y1 + 15,
            ALLEGRO_ALIGN_CENTER,
            letra);

        // VALOR DA AÇÃO
        char buf[32];
        sprintf(buf, "%d", prox.efeito);
        al_draw_text(r->font, al_map_rgb(255, 255, 255),
            base_x, icon_y1 + 40,
            ALLEGRO_ALIGN_CENTER,
            buf);
    }
}

// ============================================================================
// DESENHA A ENERGIA ATUAL DO JOGADOR
void RenderEnergy(Renderer* r) {
    Combat* c = r->combat;
    char buf[64];
    sprintf(buf, "Energia: %d", c->energia);
    al_draw_text(r->font, al_map_rgb(255, 220, 0), 20, 16, 0, buf);
}

// ============================================================================
// DESENHA BOTÃO ELIMINAR INIMIGOS (Tecla K)
void RenderKillAllButton(Renderer* r) {
    // CORREÇÃO: removida variável não usada 'c'
    // Combat* c = r->combat; // REMOVIDO - não usada

    int x1 = DISPLAY_WIDTH - 300;
    int y1 = DISPLAY_HEIGHT - 600;
    int w = 280;
    int h = 30;

    int x2 = x1 + w;
    int y2 = y1 + h;

    // Cor do botão (vermelho para ação destrutiva)
    ALLEGRO_COLOR cor_fundo = al_map_rgb(180, 0, 0);
    ALLEGRO_COLOR cor_borda = al_map_rgb(255, 255, 255);
    ALLEGRO_COLOR cor_texto = al_map_rgb(255, 255, 255);

    // Desenha o botão
    al_draw_filled_rounded_rectangle(x1, y1, x2, y2, 8, 8, cor_fundo);
    al_draw_rounded_rectangle(x1, y1, x2, y2, 8, 8, cor_borda, 2);

    // Texto do botão
    al_draw_text(r->font, cor_texto, x1 + w / 2, y1 + 12, ALLEGRO_ALIGN_CENTER,
        "ELIMINAR TODOS INIMIGOS (K)");

    // Texto adicional (debug)
    al_draw_text(r->font, al_map_rgb(200, 200, 200), x1 + w / 2, y1 + h + 5,
        ALLEGRO_ALIGN_CENTER, "(Debug)");
}

// REINICIA O COMBATE
void RenderRestartButton(Renderer* r) {
    int x1 = DISPLAY_WIDTH - 300;
    int y1 = DISPLAY_HEIGHT - 565;
    int w = 280, h = 30;

    al_draw_filled_rounded_rectangle(x1, y1, x1 + w, y1 + h, 6, 6, al_map_rgb(0, 100, 200));
    al_draw_rounded_rectangle(x1, y1, x1 + w, y1 + h, 6, 6, al_map_rgb(255, 255, 255), 2);
    al_draw_text(r->font, al_map_rgb(255, 255, 255), x1 + w / 2, y1 + 8,
        ALLEGRO_ALIGN_CENTER, "REINICIAR COMBATE (R)");
}

// ============================================================================
// DESENHA BOTÃO CONGELAR INIMIGOS (Tecla F)
// ============================================================================
void RenderFreezeButton(Renderer* r) {
    // CORREÇÃO: removida variável não usada 'c'
    // Combat* c = r->combat; // REMOVIDO - não usada

    int x1 = DISPLAY_WIDTH - 300;
    int y1 = DISPLAY_HEIGHT - 530;
    int w = 280;
    int h = 30;

    // Cor azul para o botão de congelar
    ALLEGRO_COLOR cor_fundo = al_map_rgb(100, 150, 255);
    ALLEGRO_COLOR cor_borda = al_map_rgb(255, 255, 255);
    ALLEGRO_COLOR cor_texto = al_map_rgb(255, 255, 255);

    // Desenha o botão
    al_draw_filled_rounded_rectangle(x1, y1, x1 + w, y1 + h, 6, 6, cor_fundo);
    al_draw_rounded_rectangle(x1, y1, x1 + w, y1 + h, 6, 6, cor_borda, 2);

    // Texto do botão
    al_draw_text(r->font, cor_texto, x1 + w / 2, y1 + 8,
        ALLEGRO_ALIGN_CENTER, "CONGELAR INIMIGOS (F)");

    // Texto adicional explicativo
    al_draw_text(r->font, al_map_rgb(200, 200, 255), x1 + w / 2, y1 + h + 5,
        ALLEGRO_ALIGN_CENTER, "(Inimigos perdem 1 turno)");
}

// ============================================================================
// DESENHA O DECK DO JOGADOR
void RenderDeck(Renderer* renderer, int x_left, int y_top) {
    al_draw_filled_rounded_rectangle(x_left, y_top, x_left + DECK_WIDTH, y_top + DECK_HEIGHT, 10, 10, al_map_rgb(255, 255, 255));
    al_draw_rounded_rectangle(x_left, y_top, x_left + DECK_WIDTH, y_top + DECK_HEIGHT, 10, 10, al_map_rgb(0, 0, 0), 2);
    al_draw_text(renderer->font, al_map_rgb(0, 0, 0), x_left + 20, y_top + 20, 0, "Deck");
}

// ============================================================================
// DESENHA O JOGADOR COM BARRA DE VIDA
void RenderPlayer(Renderer* r) {
    Combat* c = r->combat;
    int x = 80, y = 160, radius = 64;
    al_draw_filled_circle(x, y, radius, al_map_rgb(200, 200, 255));
    al_draw_circle(x, y, radius, al_map_rgb(255, 255, 255), 2);

    int bar_w = 160, bar_h = 18, bar_x = x - bar_w / 2, bar_y = y + radius + 16;
    float vida_frac = 0.0f;
    if (c->jogador.vida_max > 0) vida_frac = (float)c->jogador.vida / (float)c->jogador.vida_max;
    // CORREÇÃO: indentação correta
    if (vida_frac < 0) vida_frac = 0; 
    if (vida_frac > 1) vida_frac = 1;

    al_draw_filled_rounded_rectangle(bar_x - 2, bar_y - 2, bar_x + bar_w + 2, bar_y + bar_h + 2, 6, 6, al_map_rgb(255, 255, 255));
    al_draw_filled_rounded_rectangle(bar_x, bar_y, bar_x + (int)(bar_w * vida_frac), bar_y + bar_h, 6, 6, al_map_rgb(100, 200, 0));
    al_draw_rounded_rectangle(bar_x, bar_y, bar_x + bar_w, bar_y + bar_h, 6, 6, al_map_rgb(0, 0, 0), 2);

    char vida_txt[64];
    sprintf(vida_txt, "Vida: %d / %d", c->jogador.vida, c->jogador.vida_max);
    al_draw_text(r->font, al_map_rgb(0, 0, 0), bar_x + 6, bar_y + 1, 0, vida_txt);

    if (c->jogador.escudo > 0) {
        char escudo_txt[32];
        sprintf(escudo_txt, "Esc: %d", c->jogador.escudo);
        al_draw_text(r->font, al_map_rgb(180, 220, 255), bar_x + bar_w - 50, bar_y + 1, 0, escudo_txt);
    }
}

// RENDERIZA TODOS OS INIMIGOS
void RenderEnemies(Renderer* r) {
    Combat* c = r->combat;
    for (int i = 0; i < c->num_inimigos; i++) RenderSingleEnemy(r, &c->inimigos[i], i);
}

void RenderPlayerHand(Renderer* r) {
    Combat* c = r->combat;
    int base_x = HAND_BEGIN_X;
    int y = HAND_BEGIN_Y;
    int spacing = CARD_WIDTH + 24;
    for (int i = 0; i < c->mao.size; i++) {
        int x = base_x + i * spacing;
        int sel = (i == carta_sel);
        int playable = (c->mao.cartas[i].custo <= c->energia) ? 1 : 0;
        DrawCardVisual(r->font, c->mao.cartas[i], x, y + (sel ? -20 : 0), sel, playable);
    }
}

void RenderDeckInfo(Renderer* r) {
    Combat* c = r->combat;
    char buf[64];
    sprintf(buf, "Deck: %d", c->deck_compra.size);
    al_draw_text(r->font, al_map_rgb(255, 255, 255), DRAW_DECK_X, DRAW_DECK_Y + DECK_HEIGHT + 8, 0, buf);
    sprintf(buf, "Desc: %d", c->deck_descarte.size);
    al_draw_text(r->font, al_map_rgb(255, 255, 255), DRAW_DECK_X, DRAW_DECK_Y + DECK_HEIGHT + 28, 0, buf);
}

// NOVA FUNÇÃO: Mostra instruções baseadas no estado
// NOVA FUNÇÃO: Mostra instruções baseadas no estado
void RenderInstructions(Renderer* r) {
    Combat* c = r->combat;
    const char* instrucao = "";

    if (c->turno == 1) {
        if (c->estado == 0) {
            instrucao = "Selecione carta (SETAS) - ENTER para usar - ESC para passar turno";
        }
        else if (c->estado == 1) {
            instrucao = "Selecione inimigo alvo (SETAS) - ENTER para confirmar - ESC para cancelar";
        }
    }
    else {
        instrucao = "Turno dos inimigos...";
    }

    al_draw_text(r->font, al_map_rgb(255, 255, 255), 20, DISPLAY_HEIGHT - 40, 0, instrucao);
}

void Render(Renderer* renderer) {
    al_set_target_bitmap(renderer->display_buffer);

    // fundo + HUD
    RenderBackground(renderer);
    RenderEnergy(renderer);
    RenderPlayer(renderer);
    RenderDeck(renderer, DRAW_DECK_X, DRAW_DECK_Y);

    // BOTÃO DE CURA (P) — muda cor conforme c->cura_usada
    {
        Combat* c = renderer->combat;
        int x1 = 40, y1 = DISPLAY_HEIGHT - 240, w = 260, h = 50;
        int x2 = x1 + w, y2 = y1 + h;
        ALLEGRO_COLOR cor_fundo = (c->cura_usada == 0) ? al_map_rgb(0, 180, 0) : al_map_rgb(180, 0, 0);

        al_draw_filled_rounded_rectangle(x1, y1, x2, y2, 10, 10, cor_fundo);
        al_draw_rounded_rectangle(x1, y1, x2, y2, 10, 10, al_map_rgb(255, 255, 255), 2);
        al_draw_text(renderer->font, al_map_rgb(255, 255, 255), x1 + w / 2, y1 + 14, ALLEGRO_ALIGN_CENTER, "Recuperar 40 de vida (P)");
    }
    
    // BOTÃO DE CONGELAR INIMIGOS (F)
    RenderFreezeButton(renderer);
    // BOTÃO DE MATAR TODOS INIMIGOS (K)
    RenderKillAllButton(renderer);
    // BOTÃO DE REINICIAR COMBATE (R)
    RenderRestartButton(renderer);

    RenderEnemies(renderer);
    RenderPlayerHand(renderer);
    RenderDeckInfo(renderer);
    RenderInstructions(renderer);

    // copiar buffer para display
    al_set_target_backbuffer(renderer->display);
    al_draw_scaled_bitmap(renderer->display_buffer, 0, 0, DISPLAY_BUFFER_WIDTH, DISPLAY_BUFFER_HEIGHT, 0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, 0);
    al_flip_display();
}

void ClearRenderer(Renderer* renderer) {
    if (renderer->font) al_destroy_font(renderer->font);
    if (renderer->display_buffer) al_destroy_bitmap(renderer->display_buffer);
    if (renderer->display) al_destroy_display(renderer->display);
}