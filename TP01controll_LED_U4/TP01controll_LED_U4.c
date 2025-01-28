#include <stdio.h>
#include "hardware/timer.h"
#include "pico/stdlib.h"

// Configuração dos pinos para o LED e botão
const uint LED_PIN = 13;   
const uint BTN_A_PIN = 5;

// Variáveis globais voláteis para sincronização com interrupções
volatile int button_press_count = 0; // Contador de pressões do botão
volatile int blink_count = 0;        // Contador de pisca-piscas do LED
volatile bool led_state = false;     // Estado do LED

// Função de callback para desligar o LED após 10 segundos
bool turn_off_callback(struct repeating_timer *t) {
    blink_count++;
    led_state = !led_state;
    gpio_put(LED_PIN, led_state);

    // Piscar por 10 segundos (10 Hz = 100 ms por pisca)
    if (blink_count >= 100) {
        blink_count = 0;
        button_press_count = 0;
        gpio_put(LED_PIN, false); // Garantir que o LED termine desligado
        printf("LED desligado\n"); // Exibe mensagem na saída serial indicando que o LED foi desligado
        return false; // Para de chamar o callback
    }
    return true; // Continua chamando o callback
}

// Função de callback do temporizador repetitivo para verificar o botão
bool repeating_timer_callback(struct repeating_timer *t) {
    static absolute_time_t last_press_time = 0;
    static bool button_last_state = false;
     
    // Verifica o estado atual do botão (pressionado = LOW, liberado = HIGH) 
    bool button_pressed = !gpio_get(BTN_A_PIN);

    // Verifica se o botão foi pressionado e realiza o debounce. 
    if (button_pressed && !button_last_state &&
        absolute_time_diff_us(last_press_time, get_absolute_time()) > 200000) {
        last_press_time = get_absolute_time();
        button_last_state = true;
        button_press_count++;
        
        // Verifica se o botão foi pressionado cinco vezes
        if (button_press_count == 5) {
            printf("Botão pressionado 5 vezes. LED piscando...\n");
            add_repeating_timer_ms(100, turn_off_callback, NULL, NULL);
        }
    } else if (!button_pressed) {
        button_last_state = false; // Atualiza o estado do botão como liberado quando ele não está pressionado.
    }

    return true; // Retorna true para continuar o temporizador de repetição.
}
