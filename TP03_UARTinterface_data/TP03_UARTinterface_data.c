#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/irq.h"

// Definições de pinos
#define UART0_TX_PIN 0
#define UART0_RX_PIN 1
#define UART1_TX_PIN 4
#define UART1_RX_PIN 5

// Buffer para armazenar dados recebidos
char received[100];
volatile bool data_available = false;

// Handler de interrupção da UART1
void on_uart1_rx() {
    while (uart_is_readable(uart1)) {
        static int i = 0;
        received[i++] = uart_getc(uart1);
        if (i >= sizeof(received) - 1 || received[i - 1] == '\n') {
            received[i] = '\0'; // Termina a string
            data_available = true;
            i = 0; // Reinicia o índice para o próximo dado
        }
    }
}

