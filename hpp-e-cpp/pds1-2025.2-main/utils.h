#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdlib.h>

void must_init(_Bool test, const char* description);

int NumToDigits(int n);

void ClearKeyboardKeys(unsigned char* keyboard_keys);

// Versão genérica para int
void ShuffleIntArray(int* array, int size);

// Versão específica para Carta
void ShuffleCartaArray(void* array, int size);

// Alias para compatibilidade
void ShuffleArray(void* array, int size);

#endif