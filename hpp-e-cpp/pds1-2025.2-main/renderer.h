#ifndef _RENDERER_H_
#define _RENDERER_H_

#define _CRT_SECURE_NO_WARNINGS

#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>

#include "game.h"

typedef struct {
    ALLEGRO_DISPLAY* display;
    ALLEGRO_BITMAP* display_buffer;
    ALLEGRO_FONT* font;

    Combat* combat;   // agora o renderer "vê" o estado do jogo
} Renderer;

// Funções principais
void FillRenderer(Renderer* renderer, Combat* combat);
void Render(Renderer* renderer);
void ClearRenderer(Renderer* renderer);
void RenderDeck(Renderer* renderer, int x_left, int y_top);

// Funções auxiliares de renderização
void RenderPlayer(Renderer* r);
void RenderEnemies(Renderer* r);
void RenderPlayerHand(Renderer* r);
void RenderEnergy(Renderer* r);
void RenderDeckInfo(Renderer* r);
void RenderInstructions(Renderer* r);
void RenderFreezeButton(Renderer* r);
void RenderKillAllButton(Renderer* r);
void RenderRestartButton(Renderer* r);

#endif