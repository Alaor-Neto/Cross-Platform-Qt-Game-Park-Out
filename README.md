# 🚌 Park Out 



---

<div id="pt"></div>

### 📖 Sobre o Projeto
**Park Out** é um jogo de lógica e estratégia multiplataforma desenvolvido como Trabalho Final para a unidade curricular de **Paradigmas de Programação** (Engenharia Informática - Universidade Autónoma de Lisboa). 

O objetivo do jogo é libertar autocarros de capacidades variadas (4 a 12 passageiros) de um parque de estacionamento em formato de grelha. O jogador deve respeitar regras estritas de bloqueio, capacidade e direções fixas para encaminhar cada veículo para a plataforma de embarque com a cor correspondente.

### ✨ Funcionalidades
* **Geração Procedural de Níveis:** Algoritmo matemático em "Espiral" que gera tabuleiros infinitos de forma aleatória e solúvel.
* **Níveis Estáticos:** Sistema de carregamento de níveis estruturados via ficheiros JSON.
* **Sistema de "Undo" (Desfazer):** Capacidade de reverter movimentos ilimitados.
* **UI Responsiva e Dinâmica:** A grelha adapta o seu tamanho automaticamente a ecrãs táteis (Mobile) e panorâmicos (Desktop).
* **Persistência Local:** Gravação de recordes e níveis concluídos.

### 🧠 Paradigmas de Programação Aplicados
Este projeto destaca-se pela aplicação rigorosa de múltiplos paradigmas de software:
1. **Programação Orientada a Objetos (POO):** Separação estrita entre a interface visual (`QML`) e a lógica de negócio encapsulada (`C++`).
2. **Paradigma Funcional (Imutabilidade):** O sistema de *Undo* guarda cópias profundas do tabuleiro em estruturas imutáveis (`GameState`), através de funções puras que não causam efeitos colaterais.
3. **Concorrência e Assincronismo:** A geração procedural pesada corre numa *Thread* secundária através de `QtConcurrent::run`, mantendo a interface gráfica fluida e exibindo um ecrã de carregamento assíncrono.

### 🛠️ Tecnologias Utilizadas
* **Linguagem:** C++17
* **Framework:** Qt 6 (QML para Frontend, C++ para Backend)
* **Build System:** CMake
* **Targets:** Desktop (Windows/MinGW 64-bit) & Mobile (Android/arm64-v8a)

### 🚀 Como Compilar e Executar
1. Certifica-te de que tens o **Qt 6.5+** e o **CMake** instalados.
2. Clona o repositório:
   ```bash
   git clone https://github.com/Alaor-Neto/Cross-Platform-Qt-Game-Park-Out.git
