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

// Função de callback do temporizador repetitivo para verificar os botões
bool repeating_timer_callback(struct repeating_timer *t) {
    static absolute_time_t last_press_time = 0;
    static bool button_a_last_state = false;
    static bool button_b_last_state = false;

    // Verifica o estado atual dos botões (pressionado = LOW, liberado = HIGH)
    bool button_a_pressed = !gpio_get(BTN_A_PIN);
    bool button_b_pressed = !gpio_get(BTN_B_PIN);

    // Verifica se o botão A foi pressionado e realiza o debounce.
    if (button_a_pressed && !button_a_last_state &&
        absolute_time_diff_us(last_press_time, get_absolute_time()) > 200000) {
        last_press_time = get_absolute_time();
        button_a_last_state = true;
        button_press_count++;

        // Verifica se o botão A foi pressionado cinco vezes
        if (button_press_count == 5) {
            printf("Botão A pressionado 5 vezes. LED piscando...\n");
            add_repeating_timer_ms(100, turn_off_callback, NULL, NULL);
        }
    } else if (!button_a_pressed) {
        button_a_last_state = false; // Atualiza o estado do botão como liberado quando ele não está pressionado.
    }

    // Verifica se o botão B foi pressionado e realiza o debounce.
    if (button_b_pressed && !button_b_last_state &&
        absolute_time_diff_us(last_press_time, get_absolute_time()) > 200000) {
        last_press_time = get_absolute_time();
        button_b_last_state = true;
        change_frequency = !change_frequency; // Altera a frequência do LED
        printf("Botão B pressionado. Frequência do LED %s para %d Hz.\n", change_frequency ? "alterada" : "alterada", change_frequency ? 1 : 10);
    } else if (!button_b_pressed) {
        button_b_last_state = false; // Atualiza o estado do botão como liberado quando ele não está pressionado.
    }

    return true; // Retorna true para continuar o temporizador de repetição.
}

int main() {

    // Inicializa a comunicação serial para permitir o uso de printf para depuração.
    stdio_init_all();
    
    // Configura o pino do LED como saída.
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 0);
    
    // Configura os pinos dos botões como entrada com resistor de pull-up interno.
    gpio_init(BTN_A_PIN);
    gpio_set_dir(BTN_A_PIN, GPIO_IN);
    gpio_pull_up(BTN_A_PIN);

    gpio_init(BTN_B_PIN);
    gpio_set_dir(BTN_B_PIN, GPIO_IN);
    gpio_pull_up(BTN_B_PIN);
    
    // Configura o temporizador repetitivo para verificar o estado dos botões a cada 100 ms.
    struct repeating_timer timer;
    add_repeating_timer_ms(100, repeating_timer_callback, NULL, &timer);
    
    // O loop principal permanece vazio, pois o controle do LED e dos botões é gerênciado pelo temporizador.
    while (true) {
        tight_loop_contents(); // Função que otimiza o loop vazio para evitar consumo excessivo de CPU.
    }
}
