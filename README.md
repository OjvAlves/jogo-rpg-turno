# jogo-rpg-turno
Jogo de RPG em turno inspirado em Slay the Spire. Desenvolvido em C e Allegro 5 para a disciplina de PDS 1 (1º Semestre)

# Slay the Spire - Versão Simplificada (C / Allegro 5)

An implementation of a turn-based RPG game inspired by "Slay the Spire", developed as the final project for the **PDS 1 (Procedimentos de Desenvolvimento de Software)** course during my 1st semester.

O objetivo deste projeto foi aplicar conceitos fundamentais de programação estruturada em linguagem C, gerência de memória, manipulação de arquivos e modularização, além de criar uma interface gráfica e interativa utilizando a biblioteca Allegro 5.

---

## 🚀 Funcionalidades do Jogo

*   **Combate em Turnos:** Sistema de batalha dinâmico onde o jogador escolhe suas ações a cada turno.
*   **Mecânica de Cartas/Habilidades:** Implementação de ataques, defesas e efeitos simulando o deck-building do jogo original.
*   **Interface Gráfica (Allegro 5):** Renderização de menus, personagens, feedbacks visuais e gerenciamento de inputs (mouse/teclado).
*   **Inteligência Artificial do Inimigo:** Padrões de comportamento automatizados para os adversários.
*   **Persistência de Dados (Opcional - remova se não tiver):** Salvamento de pontuação ou estado do jogo via manipulação de arquivos.

## 🛠️ Tecnologias e Conceitos Aplicados

*   **Linguagem C:** Uso de structs, ponteiros, alocação dinâmica de memória e funções modulares.
*   **Allegro 5:** Biblioteca gráfica utilizada para controle de frames (FPS), carregamento de bitmaps (imagens), fontes e áudio.
*   **Organização de Código:** Divisão do projeto em arquivos de cabeçalho (`.h`) e implementação (`.c`), separando a lógica do jogo da renderização visual.

## 📁 Estrutura do Repositório

```text
├── src/                  # Código-fonte do jogo (.c e .h)
├── assets/               # Imagens, fontes e sons utilizados no jogo
├── allegro_deps/         # Dependências da biblioteca Allegro 5
└── README.md             # Documentação do projeto
