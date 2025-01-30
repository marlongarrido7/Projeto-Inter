#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/irq.h"
#include "hardware/clocks.h"
#include "hardware/pio.h"
#include "inter.pio.h"

// Definições de pinos
#define WS2812_PIN     7
#define LED_R_PIN     13
#define BUTTON_A_PIN   5  // Incrementa
#define BUTTON_B_PIN   6  // Decrementa

#define DEBOUNCE_DELAY 200 // Tempo de debounce em milissegundos

volatile uint8_t current_number = 0;  // Número atual exibido
volatile bool update_matrix = false; // Flag para atualizar a matriz
volatile int button_action = 0;  // 1 = Incrementar, -1 = Decrementar

// Matriz de números 0-9 (5x5)
const uint32_t numbers[10][25] = {
    {1,1,1,1,1,  1,0,0,0,1,  1,0,0,0,1,  1,0,0,0,1,  1,1,1,1,1}, // 0
    {0,0,1,0,0,  0,1,1,0,0,  1,0,1,0,0,  0,0,1,0,0,  1,1,1,1,1}, // 1
    {1,1,1,1,1,  1,0,0,0,0,  1,1,1,1,1,  0,0,0,0,1,  1,1,1,1,1}, // 2
    {1,1,1,1,1,  1,0,0,0,0,  0,1,1,1,1,  1,0,0,0,0,  1,1,1,1,1}, // 3
    {1,0,0,0,1,  1,0,0,0,1,  1,1,1,1,1,  1,0,0,0,0,  0,0,0,0,1}, // 4
    {1,1,1,1,1,  0,0,0,0,1,  1,1,1,1,1,  1,0,0,0,0,  1,1,1,1,1}, // 5
    {1,1,1,1,1,  0,0,0,0,1,  1,1,1,1,1,  1,0,0,0,1,  1,1,1,1,1}, // 6
    {1,1,1,1,1,  0,0,0,0,1,  0,0,0,1,0,  0,0,1,0,0,  0,1,0,0,0}, // 7
    {1,1,1,1,1,  1,0,0,0,1,  1,1,1,1,1,  1,0,0,0,1,  1,1,1,1,1}, // 8
    {1,1,1,1,1,  1,0,0,0,1,  1,1,1,1,1,  1,0,0,0,0,  1,1,1,1,1}, // 9
};

// Atualiza a matriz de LEDs para exibir o número atual
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

// ISR para ambos os botões
void button_isr(uint gpio, uint32_t events) {
    static uint64_t last_press = 0;
    uint64_t now = time_us_64() / 1000;  

    if (now - last_press > DEBOUNCE_DELAY) {
        if (gpio == BUTTON_A_PIN && gpio_get(BUTTON_A_PIN) == 0) { 
            button_action = 1; // Incrementar
            update_matrix = true;
            printf("Botão A pressionado! Incrementando\n");
        }
        if (gpio == BUTTON_B_PIN && gpio_get(BUTTON_B_PIN) == 0) { 
            button_action = -1; // Decrementar
            update_matrix = true;
            printf("Botão B pressionado! Decrementando\n");
        }
        last_press = now;
    }
}

int main() {
    stdio_init_all();

    // Configuração do LED vermelho
    gpio_init(LED_R_PIN);
    gpio_set_dir(LED_R_PIN, GPIO_OUT);

    // Configuração dos botões com pull-up
    gpio_init(BUTTON_A_PIN);
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_A_PIN);

    gpio_init(BUTTON_B_PIN);
    gpio_set_dir(BUTTON_B_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_B_PIN);

    // Configuração das interrupções para FALLING e RISING
    gpio_set_irq_enabled_with_callback(BUTTON_A_PIN, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, button_isr);
    gpio_set_irq_enabled_with_callback(BUTTON_B_PIN, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, button_isr);

    // Inicializa o PIO para os LEDs WS2812
    PIO pio = pio0;
    uint sm = 0;
    uint offset = pio_add_program(pio, &inter_program);
    inter_program_init(pio, sm, offset, WS2812_PIN);

    // Exibe o número inicial na matriz
    update_led_matrix(current_number);

    // Loop principal
    while (true) {
        gpio_put(LED_R_PIN, 1);
        sleep_ms(100);
        gpio_put(LED_R_PIN, 0);
        sleep_ms(100);

        if (update_matrix) {
            if (button_action == 1) {
                current_number = (current_number + 1) % 10;
                printf("Incrementando: %d\n", current_number);
            } else if (button_action == -1) {
                current_number = (current_number == 0) ? 9 : current_number - 1;
                printf("Decrementando: %d\n", current_number);
            }
            update_led_matrix(current_number);
            update_matrix = false;
        }
    }
}
