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

int main() {
    // Inicializa o stdio
    stdio_init_all();

    // Inicializa a UART0
    uart_init(uart0, 9600);
    gpio_set_function(UART0_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART0_RX_PIN, GPIO_FUNC_UART);

    // Inicializa a UART1
    uart_init(uart1, 9600);
    gpio_set_function(UART1_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART1_RX_PIN, GPIO_FUNC_UART);

    // Configura a interrupção da UART1
    irq_set_exclusive_handler(UART1_IRQ, on_uart1_rx);
    irq_set_enabled(UART1_IRQ, true);
    uart_set_irq_enables(uart1, true, false);

    char input[100];

    while (true) {
        // Lê o dado do console
        printf("Digite algo: ");
        scanf("%99s", input);

        // Envia o dado pela UART0
        uart_puts(uart0, input);

        // Espera até que os dados estejam disponíveis
        while (!data_available) {
            tight_loop_contents(); // Mantém a CPU em modo de espera
        }

        // Imprime o dado recebido no console
        printf("Recebido: %s\n", received);
        data_available = false; // Reinicia a flag
    }

    return 0;
}

