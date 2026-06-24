#include "utils.h"
#include "cartas.h"  // CORRIGIDO: caminho correto
#include <allegro5/allegro5.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define _CRT_SECURE_NO_WARNINGS

void must_init(_Bool test, const char* description) {
    if (test) return;
    
    fprintf(stderr, "couldn't initialize %s\n", description);
    exit(1);
}

int NumToDigits(int n) {
    if (n < 0) return 1 + NumToDigits(-n);
    if (n < 10) return 1;
    if (n < 100) return 2;
    if (n < 1000) return 3;
    if (n < 10000) return 4;
    if (n < 100000) return 5;
    if (n < 1000000) return 6;
    if (n < 10000000) return 7;
    if (n < 100000000) return 8;
    if (n < 1000000000) return 9;
    return 10;
}

void ClearKeyboardKeys(unsigned char* keyboard_keys) {
    memset(keyboard_keys, 0, ALLEGRO_KEY_MAX * sizeof(unsigned char));
}

// Fisher-Yates shuffle para array de int
void ShuffleIntArray(int* v, int n) {
    if (v == NULL || n <= 1) return;
    for (int i = n - 1; i > 0; --i) {
        int j = rand() % (i + 1);
        int tmp = v[i];
        v[i] = v[j];
        v[j] = tmp;
    }
}

// Fisher-Yates shuffle para array de Carta (genérico)
void ShuffleCartaArray(void* array, int n) {
    if (array == NULL || n <= 1) return;
    
    Carta* v = (Carta*)array;
    for (int i = n - 1; i > 0; --i) {
        int j = rand() % (i + 1);
        Carta tmp = v[i];
        v[i] = v[j];
        v[j] = tmp;
    }
}

// Alias para ShuffleArray (mantém compatibilidade com código existente)
void ShuffleArray(void* v, int n) {
    ShuffleCartaArray(v, n);
}