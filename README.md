# Uso de Interrupções em Microcontroladores Raspberry Pico W 2040

**Autor do Projeto:**  
Dr. Marlon da Silva Garrido, Professor Associado IV (CENAMB - PPGEA)  
Universidade Federal do Vale do São Francisco (UNIVASF)  

Este projeto faz parte das atividades do **EMBARCATECH 2024/25**.

---

## Índice
1. [Objetivos](#objetivos)  
2. [Descrição do Projeto](#descrição-do-projeto)  
3. [Funcionalidades](#funcionalidades)  
4. [Requisitos do Projeto](#requisitos-do-projeto)  
5. [Diagrama Simplificado de Conexões](#diagrama-simplificado-de-conexões)  
6. [Como Executar o Projeto](#como-executar-o-projeto)  
7. [Estrutura do Código](#estrutura-do-código)  
8. [Trecho de Código](#trecho-de-código)  
9. [Referências](#referências)  
10. [Demonstração em Vídeo](#demonstração-em-vídeo)

---

## Objetivos
- **Compreender** o funcionamento e a aplicação de interrupções em microcontroladores.  
- **Identificar e corrigir** o fenômeno de bouncing em botões por meio de debouncing via software.  
- **Manipular e controlar** LEDs comuns e LEDs endereçáveis WS2812.  
- **Fixar o estudo** do uso de resistores de pull-up internos em botões de acionamento.  
- **Desenvolver** um projeto funcional que combine hardware e software, demonstrando o uso prático de interrupções e controle de LEDs.

---

## Descrição do Projeto
Neste projeto, utilizamos uma placa **BitDogLab** (ou similar ao Raspberry Pi Pico) para demonstrar o uso de interrupções e debouncing de botões. O firmware controla:

- Uma **matriz 5×5** de LEDs WS2812 (endereçáveis), conectada ao pino **GPIO 7**.  
- Um **LED RGB**, com pinos ligados aos GPIOs **11 (Verde)**, **12 (Azul)** e **13 (Vermelho)**. No código, utilizamos o pino **13 (vermelho)** para piscar.  
- Dois **botões** (A e B), conectados às GPIOs **5** e **6**, respectivamente, utilizando resistores de **pull-up internos**.

### Principais Destaques
- **Interrupções (IRQ)**: O acionamento dos botões **A** e **B** dispara rotinas de interrupção que incrementam ou decrementam o valor exibido na matriz.  
- **Debouncing via software**: Implementado para evitar leituras falsas causadas pelo bouncing do botão.  
- **Controle de LEDs comuns e endereçáveis**: O LED vermelho pisca 5 vezes por segundo, enquanto a matriz WS2812 exibe números de **0 a 9** em formato 5×5.

---

## Funcionalidades
1. **Piscar do LED Vermelho**  
   - O LED vermelho do LED RGB (pino GPIO 13) pisca continuamente, aproximadamente **5 vezes por segundo**.  

2. **Incremento de Número (Botão A)**  
   - Cada vez que o botão A (GPIO 5) for pressionado, o número exibido na matriz **aumenta** em 1.  
   - A contagem vai de 0 a 9 e volta a 0.  

3. **Decremento de Número (Botão B)**  
   - Cada vez que o botão B (GPIO 6) for pressionado, o número exibido na matriz **diminui** em 1.  
   - Se estiver em 0, passa para 9 (comportamento circular).  

4. **Exibição de Números na Matriz WS2812 (5x5)**  
   - São criados efeitos visuais para representar cada número de 0 a 9.  
   - O estilo pode ser fixo (segmentos estilo **display digital**) ou criativo, desde que o algarismo seja identificável.  
   - O array `numbers` no código contém o mapeamento dos LEDs que devem estar acesos para cada dígito.

---

## Requisitos do Projeto
1. **Uso de Interrupções**  
   - Todo o tratamento de botões deve ser feito via rotinas de interrupção.  

2. **Debouncing**  
   - É obrigatório implementar o tratamento do bouncing dos botões via software.  

3. **Controle de LEDs**  
   - O projeto deve demonstrar o domínio do controle de LEDs **comuns** (LED RGB) e **endereçáveis** (WS2812).  

4. **Resistores de Pull-up Internos**  
   - Os botões utilizam resistores de pull-up internos disponibilizados pelo microcontrolador.

---

## Diagrama Simplificado de Conexões

```
        +---------------------+
        |   Placa BitDogLab  |
        |                     |
 GPIO 7 -------------------> Matriz WS2812 5x5
 GPIO 13 -----------------> LED (Vermelho do RGB)
 GPIO 5 <------------------ Botão A (Pull-up Interno)
 GPIO 6 <------------------ Botão B (Pull-up Interno)
        +---------------------+
```

> Observação: As GPIOs 11 e 12 (correspondentes aos outros canais do LED RGB) não são utilizadas no exemplo fornecido.

---

## Como Executar o Projeto

1. **Clonar ou copiar** o código-fonte para o seu ambiente local.  
2. **Configurar** o [Raspberry Pi Pico SDK](https://github.com/raspberrypi/pico-sdk) ou um ambiente compatível.  
3. **Compilar** o projeto (exemplo via CMake):  
   ```bash
   mkdir build
   cd build
   cmake ..
   make
   ```
4. **Transferir** o arquivo `.uf2` resultante para a placa (arrastar para o volume USB do Pico).  
5. **Testar** pressionando os botões A e B para incrementar/decrementar o número, verificar o LED vermelho piscando e os números na matriz de LEDs.

---

## Estrutura do Código

- **`inter.pio.h`**  
  Arquivo gerado pelo assembler do PIO para controlar os LEDs WS2812.  

- **`main.c`**  
  - Configurações iniciais (GPIOs, interrupções, PIO).  
  - Função de interrupção (`button_isr`) para detectar pressão de botões.  
  - Rotina `update_led_matrix` para enviar dados aos LEDs WS2812.  
  - Loop principal que pisca o LED vermelho e atualiza a matriz conforme o valor atual.  

---

## Trecho de Código

A seguir, um **trecho** do código principal. Para acessar o arquivo completo, consulte o seu repositório:

```c
// Matriz de números 0-9 (5x5)
const uint32_t numbers[10][25] = {
    {1,1,1,1,1,  1,0,0,0,1,  1,0,0,0,1,  1,0,0,0,1,  1,1,1,1,1}, // 0
    {0,0,1,0,0,  0,1,1,0,0,  1,0,1,0,0,  0,0,1,0,0,  1,1,1,1,1}, // 1
    ...
};

void update_led_matrix(uint8_t number) {
    for (int i = 0; i < 25; i++) {
        uint8_t g = 0;
        uint8_t r = numbers[number][i] ? 255 : 0;  
        uint8_t b = 0;
        uint32_t color = (g << 16) | (r << 8) | b;
        pio_sm_put_blocking(pio0, 0, color);
    }
    sleep_us(50);
}

void button_isr(uint gpio, uint32_t events) {
    static uint64_t last_press = 0;
    uint64_t now = time_us_64() / 1000;

    if (now - last_press > DEBOUNCE_DELAY) {
        if (gpio == BUTTON_A_PIN && gpio_get(BUTTON_A_PIN) == 0) {
            button_action = 1; // Incrementar
            update_matrix = true;
        }
        if (gpio == BUTTON_B_PIN && gpio_get(BUTTON_B_PIN) == 0) {
            button_action = -1; // Decrementar
            update_matrix = true;
        }
        last_press = now;
    }
}

int main() {
    // Configurações iniciais
    // ...

    while (true) {
        // Piscar LED vermelho
        gpio_put(LED_R_PIN, 1);
        sleep_ms(100);
        gpio_put(LED_R_PIN, 0);
        sleep_ms(100);

        // Verificar se precisa atualizar a matriz
        if (update_matrix) {
            if (button_action == 1) {
                current_number = (current_number + 1) % 10;
            } else if (button_action == -1) {
                current_number = (current_number == 0) ? 9 : current_number - 1;
            }
            update_led_matrix(current_number);
            update_matrix = false;
        }
    }
}
```

---

## Referências
- [Raspberry Pi Pico SDK](https://github.com/raspberrypi/pico-sdk)  
- [Documentação oficial do Microcontrolador RP2040](https://www.raspberrypi.com/documentation/microcontrollers/)  

---

## Demonstração em Vídeo
**Explicações e demonstração do funcionamento do código, integração VSCode e Wokwi e na Placa física BitDogLab:**  
[Link do Vídeo](https://youtu.be/YqCcCaqnhxA)
```