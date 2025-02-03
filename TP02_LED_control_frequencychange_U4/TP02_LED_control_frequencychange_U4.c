#include "pico/stdlib.h"
#include "hardware/timer.h"
#include <stdio.h>

// Configuração dos pinos para o LED e botões
const uint LED_PIN = 13;   
const uint BTN_A_PIN = 5;
const uint BTN_B_PIN = 6;

// Variáveis globais voláteis para sincronização com interrupções
volatile int button_press_count = 0; // Contador de pressões do botão A
volatile int blink_count = 0;        // Contador de pisca-piscas do LED
volatile bool led_state = false;     // Estado do LED
volatile bool change_frequency = false; // Flag para alterar a frequência do LED

// Função de callback para desligar o LED após 10 segundos
bool turn_off_callback(struct repeating_timer *t) {
    blink_count++;
    led_state = !led_state;
    gpio_put(LED_PIN, led_state);

    // Piscar por 10 segundos (10 Hz = 100 ms por pisca) ou 1 Hz = 1000 ms por pisca
    int frequency = change_frequency ? 1000 : 100;
    if (blink_count >= (change_frequency ? 10 : 100)) {
        blink_count = 0;
        button_press_count = 0;
        gpio_put(LED_PIN, false); // Garantir que o LED termine desligado
        printf("LED desligado\n"); // Exibe mensagem na saída serial indicando que o LED foi desligado
        return false; // Para de chamar o callback
    }
    add_repeating_timer_ms(frequency, turn_off_callback, NULL, t); // Atualiza a frequência do timer
    return false; // Para o callback atual
}

