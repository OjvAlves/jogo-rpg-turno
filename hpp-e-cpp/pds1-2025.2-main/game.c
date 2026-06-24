//MODIFICACAO DA PROVA DO TP A PARTIR DA LINHA 365
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

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "game.h"
#include "cartas.h"
#include "jogador.h"
#include "inimigo.h"
#include "utils.h"

#include <allegro5/keycodes.h>


// ---------------------------------------
// VARIÁVEIS ESTÁTICAS (seleção na interface)
// ---------------------------------------
int carta_sel = 0;     // carta selecionada na mão
int inimigo_sel = 0;   // inimigo alvo selecionado


// ============================================================================
// VERIFICA SE UMA CARTA PRECISA SELECIONAR INIMIGO ALVO
// ============================================================================
int CartaPrecisaDeAlvo(Carta carta) {
    // Cartas que NÃO precisam de alvo
    if (carta.tipo == CARTA_DEFESA) {
        return 0; // Defesa não precisa de alvo
    }
    
    if (carta.tipo == CARTA_ESPECIAL) {
        // Verifica pelo nome/texto da ação
        const char* acao = carta.acao;
        
        // Cartas especiais que NÃO precisam de alvo
        if (strstr(acao, "Cura") != NULL ||
            strstr(acao, "Energia") != NULL ||
            strstr(acao, "Recicla") != NULL ||
            strstr(acao, "area") != NULL) {  // Dano em área
            return 0;
        }
        
        // Cartas especiais que PRECISAM de alvo
        if (strstr(acao, "Veneno") != NULL ||
            strstr(acao, "Golpe Protetor") != NULL) {
            return 1;
        }
    }
    
    // Ataque sempre precisa de alvo
    if (carta.tipo == CARTA_ATAQUE) {
        return 1;
    }
    
    return 0;
}








// ============================================================================
// INICIAR COMBATE
// Nota: NÃO reseta vida do jogador; vida persiste entre combates.
// O campo rodada_atual, e_chefao e cura_usada devem ser gerenciados no main.
// ============================================================================
void IniciarCombat(Combat* c) {
    // ====================================================
    // Garantir que os CardGroup tenham memória alocada
    // ====================================================
    if (c->deck_compra.cartas == NULL) CardGroupInit(&c->deck_compra, 50);
    if (c->deck_descarte.cartas == NULL) CardGroupInit(&c->deck_descarte, 50);
    if (c->mao.cartas == NULL) CardGroupInit(&c->mao, 15);
    
    // Limpar conteúdo dos grupos (size)
    c->deck_compra.size = 0;
    c->deck_descarte.size = 0;
    c->mao.size = 0;
    
    // -------------------------
    // CONFIGURAR JOGADOR (não reseta vida)
    // -------------------------
    if (c->jogador.vida_max == 0) {
        // primeira vez
        JogadorInit(&c->jogador);
    }
    c->energia = 3;
    c->turno = 1;
    c->jogador.escudo = 0;
    
    // -------------------------
    // GERAR INIMIGOS (chefão na rodada 11)
    // -------------------------
    if (c->rodada_atual == 11 || c->e_chefao) {
        // chefão final
        c->num_inimigos = 1;
        c->inimigos[0] = CriarInimigoChefao(); // implemente em inimigo.c
        c->e_chefao = 1;
        printf("==== CHEFÃO FINAL! ====\n");
    }
    else {
        int fortes = 0;
        for (int i = 0; i < 2; i++) {
            Inimigo e = CriarInimigo();
            
            if (e.tipo == INIMIGO_FORTE) {
                if (fortes == 0) fortes = 1;
                else e = CriarInimigoFraco();
            }
            
            c->inimigos[i] = e;
        }
        c->num_inimigos = 2;
    }
    
    // -------------------------
    // GRUPOS DE CARTAS
    // -------------------------
    // Montar deck inicial e embaralhar corretamente
    Carta deck_inicial[DECK_FINAL];
    MontarDeckInicial(deck_inicial);
    
    // Use a função ShuffleArray que aceita (Carta*, int)
   ShuffleArray((void*)deck_inicial, DECK_FINAL);
    
    for (int i = 0; i < DECK_FINAL; i++) {
        CardGroupAdd(&c->deck_compra, deck_inicial[i]);
    }
    
    // -------------------------
    // MÃO INICIAL
    // -------------------------
    c->mao.size = 0;
    ComprarMao(c);
    
    // reset seleção
    carta_sel = 0;
    inimigo_sel = 0;
}

// ============================================================================
// VERIFICA SE O COMBATE ACABOU
// ============================================================================
int CombatAcabou(Combat* c) {
    if (c->jogador.vida <= 0) return 1;
    
    int mortos = 0;
    for (int i = 0; i < c->num_inimigos; i++) {
        if (c->inimigos[i].vida <= 0) mortos++;
    }
    
    return (mortos == c->num_inimigos);
}

// ============================================================================
// COMPRAR UMA CARTA
// ============================================================================
void ComprarCarta(Combat* c) {
    // Se deck de compra estiver vazio → reciclar descarte
    if (c->deck_compra.size == 0) {
        if (c->deck_descarte.size == 0) return; // Nada para comprar
        
        for (int i = 0; i < c->deck_descarte.size; i++) {
            CardGroupAdd(&c->deck_compra, c->deck_descarte.cartas[i]);
        }
        c->deck_descarte.size = 0;
        
        // embaralhar o novo deck de compra
       ShuffleArray(c->deck_compra.cartas, c->deck_compra.size);  // Isso já está correto
    }
    
    // Pega o topo da pilha (index 0)
    Carta carta = c->deck_compra.cartas[0];
    
    // shift à esquerda
    for (int i = 0; i < c->deck_compra.size - 1; i++) {
        c->deck_compra.cartas[i] = c->deck_compra.cartas[i + 1];
    }
    c->deck_compra.size--;
    
    CardGroupAdd(&c->mao, carta);
}

// ============================================================================
// COMPRAR MÃO INICIAL (5 CARTAS)
// ============================================================================
void ComprarMao(Combat* c) {
    c->mao.size = 0;
    for (int i = 0; i < 5; i++) ComprarCarta(c);
}

// ============================================================================
// DESCARTAR TODAS AS CARTAS DA MÃO
// ============================================================================
void DescartarMao(Combat* c) {
    for (int i = 0; i < c->mao.size; i++) {
        CardGroupAdd(&c->deck_descarte, c->mao.cartas[i]);
    }
    c->mao.size = 0;
}

// ============================================================================
// JOGAR UMA CARTA
// ============================================================================
void JogarCarta(Combat* c, int icard, int ienemy) {
    if (icard < 0 || icard >= c->mao.size) return;
    
    Carta carta = c->mao.cartas[icard];
    
    if (carta.custo > c->energia) return;
    
    c->energia -= carta.custo;
    
    // ATAQUE
    if (carta.tipo == CARTA_ATAQUE) {
        if (ienemy < 0 || ienemy >= c->num_inimigos) return;
        Inimigo* alvo = &c->inimigos[ienemy];
        int dano = carta.efeito;
        
        if (alvo->escudo >= dano) {
            alvo->escudo -= dano;
        }
        else {
            int resto = dano - alvo->escudo;
            alvo->escudo = 0;
            alvo->vida -= resto;
            if (alvo->vida < 0) alvo->vida = 0;
        }
    }
    // DEFESA
    else if (carta.tipo == CARTA_DEFESA) {
        c->jogador.escudo += carta.efeito;
    }
    // ESPECIAL
    else if (carta.tipo == CARTA_ESPECIAL) {
        // Verifica qual carta especial
        if (strstr(carta.acao, "Recicla") != NULL) {
            // Carta original - reciclar mão
            DescartarMao(c);
            ComprarMao(c);
            printf("Mão reciclada!\n");
        }
        else if (strstr(carta.acao, "Energia") != NULL) {
            // Nova carta - +3 energia
            c->energia += carta.efeito;
            printf("Energia +%d! Total: %d\n", carta.efeito, c->energia);
        }
        else if (strstr(carta.acao, "Cura") != NULL) {
            // Carta de cura
            c->jogador.vida += carta.efeito;
            if (c->jogador.vida > c->jogador.vida_max) {
                c->jogador.vida = c->jogador.vida_max;
            }
            printf("Vida recuperada: +%d | Vida atual: %d/%d\n",
                carta.efeito, c->jogador.vida, c->jogador.vida_max);
        }
        else if (strstr(carta.acao, "area") != NULL) {
            // Carta de dano em área
            for (int i = 0; i < c->num_inimigos; i++) {
                if (c->inimigos[i].vida > 0) {
                    int dano = carta.efeito;
                    
                    if (c->inimigos[i].escudo >= dano) {
                        c->inimigos[i].escudo -= dano;
                    }
                    else {
                        int resto = dano - c->inimigos[i].escudo;
                        c->inimigos[i].escudo = 0;
                        c->inimigos[i].vida -= resto;
                        if (c->inimigos[i].vida < 0) c->inimigos[i].vida = 0;
                    }
                }
            }
            printf("Dano em área: %d de dano a todos inimigos!\n", carta.efeito);
        }
        else if (strstr(carta.acao, "Golpe Protetor") != NULL) {
        // NOVA: Golpe Protetor - Dano 10 + Escudo 10
        int dano_escudo = carta.efeito; // 10
        
        // 1. CAUSA DANO AO INIMIGO
        if (ienemy >= 0 && ienemy < c->num_inimigos) {
            Inimigo* alvo = &c->inimigos[ienemy];
            
            if (alvo->escudo >= dano_escudo) {
                alvo->escudo -= dano_escudo;
            }
            else {
                int resto = dano_escudo - alvo->escudo;
                alvo->escudo = 0;
                alvo->vida -= resto;
                if (alvo->vida < 0) alvo->vida = 0;
            }
        }
        
        // 2. DÁ ESCUDO AO JOGADOR
        c->jogador.escudo += dano_escudo;
        
        printf("Golpe Protetor! ⚔️🛡️\n");
        printf("  → Causou %d de dano ao inimigo\n", dano_escudo);
        printf("  → Ganhou %d de escudo\n", dano_escudo);
    }
        //carta de veneno
     else if (strstr(carta.acao, "Veneno") != NULL) {
    // Carta de Veneno - precisa de alvo
    if (ienemy >= 0 && ienemy < c->num_inimigos) {
        Inimigo* alvo = &c->inimigos[ienemy];
        
        // CORREÇÃO: Aplica veneno no inimigo
        alvo->veneno_cargas = carta.efeito; // carta.efeito = 4 (cargas)
        alvo->veneno_dano = 7; // Dano por turno (7)
        
        printf("☠️ Veneno aplicado no inimigo %d!\n", ienemy + 1);
        printf("  → Sofrerá %d de dano por %d turnos\n",
               alvo->veneno_dano, alvo->veneno_cargas);
    } else {
        printf("Erro: Veneno precisa de um alvo válido!\n");
    }
}
    }
    
    // manda pro descarte
    CardGroupAdd(&c->deck_descarte, carta);
    
    // remover da mão (shift)
    for (int i = icard; i < c->mao.size - 1; i++) {
        c->mao.cartas[i] = c->mao.cartas[i + 1];
    }
    c->mao.size--;
}

// ============================================================================
// ATUALIZAR COMBATE — RECEBE UMA TECLA DO JOGADOR
// ============================================================================
void AtualizarCombat(Combat* c, int tecla) {
    // TURNO DO JOGADOR
    if (c->turno == 1) {
        // ESTADO 0: SELECIONAR CARTA/BOTÃO
        if (c->estado == 0) {
            // Navegação entre cartas
            if (tecla == ALLEGRO_KEY_RIGHT) {
                if (carta_sel < c->mao.size - 1) carta_sel++;
                return;
            }
            if (tecla == ALLEGRO_KEY_LEFT) {
                if (carta_sel > 0) carta_sel--;
                return;
            }
            
            // Tecla ENTER - seleciona carta/botão
            if (tecla == ALLEGRO_KEY_ENTER) {
                if (c->mao.size == 0) return;
                
                Carta carta = c->mao.cartas[carta_sel];
                
                // Verifica se pode jogar a carta (tem energia)
                if (carta.custo > c->energia) {
                    printf("Energia insuficiente para esta carta! Necessário: %d, Disponível: %d\n", 
                           carta.custo, c->energia);
                    return;
                }
                
                // Usa a função CartaPrecisaDeAlvo
                if (CartaPrecisaDeAlvo(carta)) {
                    c->estado = 1; // Vai para seleção de inimigo
                    printf("Selecione o inimigo alvo (SETAS + ENTER)\n");
                } 
                else {
                    // Cartas que não precisam de alvo
                    JogarCarta(c, carta_sel, -1);
                    
                    // Ajusta seleção após jogar carta
                    if (carta_sel >= c->mao.size) carta_sel = c->mao.size - 1;
                    if (carta_sel < 0 && c->mao.size > 0) carta_sel = 0;
                }
                return;
            }
            
            // ESC - pula turno (descartando todas as cartas)
            if (tecla == ALLEGRO_KEY_ESCAPE) {
                DescartarMao(c);
                c->turno = 2;
                return;
            }
            
            // --- BOTÕES ESPECIAIS (funcionam no estado 0) ---
            
            // Tecla P - Cura (só funciona aqui)
            if (tecla == ALLEGRO_KEY_P) {
                if (c->cura_usada == 0) {
                    c->jogador.vida += 40;
                    if (c->jogador.vida > c->jogador.vida_max) {
                        c->jogador.vida = c->jogador.vida_max;
                    }
                    c->cura_usada = 1;
                    printf("Cura usada! +40 de vida (Vida: %d/%d)\n", 
                           c->jogador.vida, c->jogador.vida_max);
                } else {
                    printf("Cura já foi usada!\n");
                }
                return;
            }
            
            // Tecla K - Mata todos inimigos (debug)
            if (tecla == ALLEGRO_KEY_K) {
                for (int i = 0; i < c->num_inimigos; i++) {
                    c->inimigos[i].vida = 0;
                }
                printf("Todos inimigos eliminados!\n");
                return;
            }
            
            // Tecla R - Reinicia combate
            if (tecla == ALLEGRO_KEY_R) {
                for (int i = 0; i < c->num_inimigos; i++) {
                    c->inimigos[i].vida = c->inimigos[i].vida_max;
                }
                c->jogador.vida = c->jogador.vida_max;
                c->jogador.escudo = 0;
                c->energia = 3;
                c->turno = 1;
                c->estado = 0;
                printf("Combate reiniciado!\n");
                return;
            }
            
            // Tecla F - Congela inimigos
            if (tecla == ALLEGRO_KEY_F) {
                int inimigos_congelados = 0;
                for (int i = 0; i < c->num_inimigos; i++) {
                    if (c->inimigos[i].vida > 0) {
                        c->inimigos[i].congelado = 1;
                        inimigos_congelados++;
                    }
                }
                printf("❄️ %d inimigos congelados por 1 turno!\n", inimigos_congelados);
                return;
            }
            
            // Tecla C - Embaralha descarte no deck
            if (tecla == ALLEGRO_KEY_C) {
                if (c->deck_descarte.size == 0) {
                    printf("Nada para embaralhar!\n");
                    return;
                }
            
                for (int i = 0; i < c->deck_descarte.size; i++) {
                    CardGroupAdd(&c->deck_compra, c->deck_descarte.cartas[i]);
                }
                c->deck_descarte.size = 0;
                
                // Embaralha o deck de compra
                ShuffleArray((void*)c->deck_compra.cartas, c->deck_compra.size);
                
                printf("🎴 Deck de descarte embaralhado na pilha de compra! (%d cartas)\n", c->deck_compra.size);
                return;
            }
            
            // debug keys
            if (tecla == ALLEGRO_KEY_SPACE) {
                for (int i = 0; i < c->num_inimigos; i++) c->inimigos[i].vida = 0;
                return;
            }
            if (tecla == ALLEGRO_KEY_X) {
                c->jogador.vida = 1;
                return;
            }
            if (tecla == ALLEGRO_KEY_Q) exit(0);
        }
        
        // ESTADO 1: SELECIONAR INIMIGO (apenas para cartas que precisam de alvo)
        else if (c->estado == 1) {
            // Navegação entre inimigos vivos
            if (tecla == ALLEGRO_KEY_DOWN || tecla == ALLEGRO_KEY_RIGHT) {
                do {
                    c->inimigo_selecionado = (c->inimigo_selecionado + 1) % c->num_inimigos;
                } while (c->inimigos[c->inimigo_selecionado].vida <= 0);
                return;
            }
            if (tecla == ALLEGRO_KEY_UP || tecla == ALLEGRO_KEY_LEFT) {
                do {
                    c->inimigo_selecionado = (c->inimigo_selecionado - 1 + c->num_inimigos) % c->num_inimigos;
                } while (c->inimigos[c->inimigo_selecionado].vida <= 0);
                return;
            }
            
            // ENTER - confirma seleção de inimigo e joga a carta
            if (tecla == ALLEGRO_KEY_ENTER) {
                if (c->inimigos[c->inimigo_selecionado].vida > 0) {
                    // Joga a carta selecionada anteriormente no inimigo escolhido
                    JogarCarta(c, carta_sel, c->inimigo_selecionado);
                    
                    // Volta para seleção de cartas
                    c->estado = 0;
                    
                    // Ajusta seleção de carta
                    if (carta_sel >= c->mao.size) carta_sel = c->mao.size - 1;
                    if (carta_sel < 0 && c->mao.size > 0) carta_sel = 0;
                }
                return;
            }
            
            // ESC - cancela seleção de inimigo, volta para seleção de cartas
            if (tecla == ALLEGRO_KEY_ESCAPE) {
                c->estado = 0; // Volta para seleção de cartas
                printf("Seleção de alvo cancelada\n");
                return;
            }
        }
    }
    
    // TURNO DOS INIMIGOS (COM CONGELAMENTO E VENENO)
else if (c->turno == 2) {
    int inimigos_congelados = 0;
    int inimigos_agiram = 0;
    
    printf("\n=== TURNO DOS INIMIGOS ===\n");
    
    // CORREÇÃO 1: Zera escudo dos inimigos ANTES deles agirem
    // (isso remove o escudo ganho no turno ANTERIOR)
    for (int i = 0; i < c->num_inimigos; i++) {
        if (c->inimigos[i].vida > 0 && c->inimigos[i].escudo > 0) {
            printf("🛡️ Inimigo %d perde escudo acumulado: %d → 0\n", 
                   i, c->inimigos[i].escudo);
            c->inimigos[i].escudo = 0;
        }
    }
    
    for (int i = 0; i < c->num_inimigos; i++) {
        if (c->inimigos[i].vida <= 0) continue;
        
        // APLICA VENENO ANTES DA AÇÃO (se houver)
        if (c->inimigos[i].veneno_cargas > 0) {
            int dano_veneno = c->inimigos[i].veneno_dano;
            c->inimigos[i].vida -= dano_veneno;
            
            if (c->inimigos[i].vida < 0) c->inimigos[i].vida = 0;
            
            c->inimigos[i].veneno_cargas--;
            
            printf("☠️ Inimigo %d: Veneno -%d vida (cargas: %d)\n", 
                   i, dano_veneno, c->inimigos[i].veneno_cargas);
            
            if (c->inimigos[i].vida <= 0) {
                printf("   → Inimigo %d morreu pelo veneno!\n", i);
                continue;
            }
        }
        
        if (c->inimigos[i].congelado) {
            c->inimigos[i].congelado = 0;
            inimigos_congelados++;
            printf("❄️ Inimigo %d: Congelado (não agiu)\n", i);
        }
        else {
            printf("Inimigo %d: ", i);
            ExecutarAcaoInimigo(&c->inimigos[i], (void*)c);
            inimigos_agiram++;
            
            // CORREÇÃO 2: Se o inimigo usou DEFESA, escudo aparece IMEDIATAMENTE
            // (já é feito na função ExecutarAcaoInimigo)
        }
    }
    
    printf("=== FIM DO TURNO DOS INIMIGOS ===\n");
    
    if (c->jogador.vida <= 0) return;
    
    // Reseta para próximo turno do jogador
    c->energia = 3;
    c->jogador.escudo = 0; // Jogador perde escudo
    
    ComprarMao(c);
    
    c->turno = 1;
    c->estado = 0;
    printf("\n=== SEU TURNO (Energia: 3) ===\n");
}
}