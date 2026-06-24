#define _CRT_SECURE_NO_WARNINGS

#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "constants.h"
#include "game.h"
#include "cartas.h"
#include "jogador.h"
#include "renderer.h"
#include "utils.h"

// =============================================================
// MAIN — EXECUTA OS 10 COMBATES + CHEFÃO
// =============================================================
int main() {
    // --- Inicialização Allegro ---
    must_init(al_init(), "allegro");
    must_init(al_init_image_addon(), "image");
    must_init(al_init_primitives_addon(), "primitives");
    must_init(al_install_keyboard(), "keyboard");
    must_init(al_init_font_addon(), "font");
    
    srand(time(NULL));
    
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);
    must_init(timer, "timer");
    
    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    must_init(queue, "queue");
    
    // Criar display antes do renderer
    al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);
    al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);
    
    ALLEGRO_DISPLAY* display = al_create_display(DISPLAY_WIDTH, DISPLAY_HEIGHT);
    must_init(display, "display");
    
    // Registrar fontes de eventos
    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_register_event_source(queue, al_get_display_event_source(display));
    
    unsigned char keyboard_keys[ALLEGRO_KEY_MAX];
    ClearKeyboardKeys(keyboard_keys);
    
    ALLEGRO_EVENT event;
    
    // --- Estado do jogo ---
    Renderer renderer;
    Combat combat;
    memset(&combat, 0, sizeof(Combat));
    
    combat.rodada_atual = 1;  // INICIA NA RODADA 1
    combat.cura_usada = 0;
    combat.estado = 0;
    combat.inimigo_selecionado = 0;
    combat.e_chefao = 0;
    combat.num_vitorias = 0;
    
    // Inicializa o jogador
    JogadorInit(&combat.jogador);
    
    // Inicializar grupos de cartas com NULL
    combat.deck_compra.cartas = NULL;
    combat.deck_descarte.cartas = NULL;
    combat.mao.cartas = NULL;
    
    // --- Criar Renderer ---
    renderer.combat = &combat;
    renderer.display = display;
    renderer.display_buffer = al_create_bitmap(DISPLAY_BUFFER_WIDTH, DISPLAY_BUFFER_HEIGHT);
    must_init(renderer.display_buffer, "display buffer");
    
    renderer.font = al_create_builtin_font();
    must_init(renderer.font, "font");
    
    // ============================================================
    // LOOP PRINCIPAL
    // ============================================================
    al_start_timer(timer);
    
    int jogo_rodando = 1;
    int rodada_atual = 1;
    
    while (jogo_rodando) {
        printf("\n=== INICIANDO RODADA %d ===\n", rodada_atual);
        
        // Configurar chefão na rodada 11
        if (rodada_atual == 11) {
            combat.e_chefao = 1;
            printf("CHEFÃO FINAL!\n");
        } else {
            combat.e_chefao = 0;
        }
        
        combat.rodada_atual = rodada_atual;
        
        // Iniciar combate
        IniciarCombat(&combat);
        
        int combate_ativo = 1;
        
        while (combate_ativo) {
            al_wait_for_event(queue, &event);
            
            int redraw = 0;
            
            switch (event.type) {
                case ALLEGRO_EVENT_TIMER:
                    redraw = 1;
                    for (int i = 0; i < ALLEGRO_KEY_MAX; i++) {
                        keyboard_keys[i] &= ~GAME_KEY_SEEN;
                    }
                    break;
                    
                case ALLEGRO_EVENT_KEY_DOWN:
                    keyboard_keys[event.keyboard.keycode] = GAME_KEY_SEEN | GAME_KEY_DOWN;
                    
                    // --- Cura P (só 1 vez no jogo) ---
                    if (event.keyboard.keycode == ALLEGRO_KEY_P) {
                        if (combat.cura_usada == 0) {
                            combat.jogador.vida += 40;
                            if (combat.jogador.vida > combat.jogador.vida_max) {
                                combat.jogador.vida = combat.jogador.vida_max;
                            }
                            
                            combat.cura_usada = 1;
                            printf("Cura usada! +40 de vida (Vida: %d/%d)\n", 
                                   combat.jogador.vida, combat.jogador.vida_max);
                        } else {
                            printf("Cura já foi usada nesta partida!\n");
                        }
                        break;
                    }
                    
                    // --- Teclas normais do combate ---
                    AtualizarCombat(&combat, event.keyboard.keycode);
                    break;
                    
                case ALLEGRO_EVENT_KEY_UP:
                    keyboard_keys[event.keyboard.keycode] &= ~GAME_KEY_DOWN;
                    break;
                    
                case ALLEGRO_EVENT_DISPLAY_CLOSE:
                    jogo_rodando = 0;
                    combate_ativo = 0;
                    break;
            }
            
            // --- Combate terminou ---
            if (CombatAcabou(&combat)) {
                if (combat.jogador.vida <= 0) {
                    printf("\n=== GAME OVER! ===\n");
                    printf("Você morreu na rodada %d\n", rodada_atual);
                    jogo_rodando = 0;
                    break;
                }
                
                printf("\n=== COMBATE %d VENCIDO! ===\n", rodada_atual);
                printf("Vida restante: %d/%d\n", 
                       combat.jogador.vida, combat.jogador.vida_max);
                
                rodada_atual++;
                
                // Rodada 12 = acabou jogo (10 combates + 1 chefão)
                if (rodada_atual == 12) {
                    printf("\n=== PARABÉNS! VOCÊ VENCEU O CHEFÃO!!! ===\n");
                    printf("=== JOGO COMPLETADO COM SUCESSO! ===\n");
                    jogo_rodando = 0;
                } else if (rodada_atual == 11) {
                    printf("\n=== PRÓXIMO: CHEFÃO FINAL! ===\n");
                }
                
                combate_ativo = 0;
            }
            
            if (redraw) {
                Render(&renderer);
            }
        }
    }
    
    // --- Encerrar ---
    printf("\n=== FIM DO JOGO ===\n");
    
    // Liberar memória dos grupos de cartas
    if (combat.deck_compra.cartas != NULL) {
        free(combat.deck_compra.cartas);
    }
    if (combat.deck_descarte.cartas != NULL) {
        free(combat.deck_descarte.cartas);
    }
    if (combat.mao.cartas != NULL) {
        free(combat.mao.cartas);
    }
    
    // Liberar recursos Allegro
    if (renderer.font) al_destroy_font(renderer.font);
    if (renderer.display_buffer) al_destroy_bitmap(renderer.display_buffer);
    al_destroy_display(display);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);
    
    return 0;
}