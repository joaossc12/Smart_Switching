# Smart_Switching

Este projeto tem como objetivo a criação de um sistema embarcado capaz de **monitorar condições de tensão elétrica** e tomar ações preventivas com **feedback visual, sonoro e gráfico** utilizando um display OLED, matriz de LEDs, buzzer, botões e joystick. O sistema é ideal para ser usado em aplicações onde a estabilidade da rede elétrica é crítica.

---

## ⚙️ Funcionalidades

- **Leitura de sinais analógicos via ADC**: captura dos valores de um joystick simulando variações de tensão.
- **Análise da tensão**:
  - Detecta **sobretensão**, **subtensão** e **queda de energia** com base em limites definidos.
  - Em condições normais, sinaliza operação adequada.
- **Interface gráfica com display OLED (SSD1306)**:
  - Visualização de status textual e percentual da tensão.
  - Modo visual alternativo com controle de um cursor pela posição do joystick.
- **Feedback visual com LEDs**:
  - LEDs vermelho e azul indicam o estado da rede elétrica.
- **Matriz de LEDs (5x5)** controlada por PIO:
  - Exibe diferentes ícones de acordo com a condição da tensão.
- **Buzzer PWM**:
  - Emite alertas sonoros distintos para cada tipo de falha detectada.
- **Botões físicos**:
  - Alternam entre modos de exibição e controlam o estado de ativação dos alertas.
- **Uso de PIO para controle preciso da matriz de LEDs.**

---

## 🧰 Tecnologias e Hardware Utilizado

- **Placa**: Raspberry Pi Pico
- **Display OLED**: SSD1306 via I2C
- **Joystick analógico**
- **Matriz de LEDs (5x5)**
- **Buzzer**
- **LEDs indicadores**
- **Botões físicos (A e B)**

---

## 📌 Mapeamento de Pinos

| Componente       | Pino GPIO |
|------------------|-----------|
| I2C SDA (Display)| 14        |
| I2C SCL (Display)| 15        |
| LED Vermelho     | 13        |
| LED Azul         | 12        |
| Botão A          | 5         |
| Botão B          | 6         |
| Joystick X       | 27 (ADC1) |
| Joystick Y       | 26 (ADC0) |
| Botão do Joystick| 22        |
| Buzzer PWM       | 21        |
| Matriz de LEDs   | 7 (PIO)   |

---

## 🚨 Estados de Alerta

| Estado        | Condição               | LED     | Buzzer        | Display/Matriz |
|---------------|------------------------|---------|---------------|----------------|
| Normal        | Tensão dentro da faixa | Azul    | Desligado     | OK             |
| Subtensão     | < 70% do valor central | Vermelho| Tom grave     | "SUBTENSAO"    |
| Sobretensão   | > 120% do valor central| Vermelho| Tom agudo     | "SOBRETENSAO"  |
| Queda de rede | Sem leitura relevante  | Vermelho| Tom agudo     | "QUEDA"        |

---

## 📷 Interface

- **Display OLED**:
  - Alterna entre modo gráfico (posição do joystick) e modo IHM (mensagem e percentual da tensão).
- **LEDs e Matriz**:
  - Indicadores visuais redundantes para garantir compreensão rápida do estado do sistema.
- **Buzzer**:
  - Sons distintos para diferentes estados de falha.

---

## 🚀 Como usar

1. Conecte os periféricos aos pinos conforme mapeado.
2. Clone o repostório
3. Importe o projeto
4. Compile e rode!

---

## 📁 Estrutura

SmartSwitching/ 
├── main.c 
├── lib/ │ 
  ├── ssd1306.h │ 
  ├── font.h │
  └── matrix.h 
├── Smart_Switching.pio.h 
├── README.md

