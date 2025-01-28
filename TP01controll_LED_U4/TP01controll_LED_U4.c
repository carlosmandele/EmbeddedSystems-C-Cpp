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
