#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/irq.h"
#include <stdio.h>

// Definições de Constantes
#define I2C_PORT i2c0
#define DS1307_I2C_ADDR 0x68
#define BUFFER_SIZE 7

// Variáveis Globais
volatile bool data_received = false;
uint8_t data_buffer[BUFFER_SIZE];

// Declaração de funções
void setDateTime(int day, int month, int year, int hour, int minute, int second);
void getDateTime();
int decToBcd(int val);
int bcdToDec(int val);

// Função i2c
void i2c_callback() {
    printf("Callback i2c_callback chamado.\n");
    int result = i2c_read_blocking(I2C_PORT, DS1307_I2C_ADDR, data_buffer, BUFFER_SIZE, false);
    if (result == PICO_ERROR_GENERIC) {
        printf("Erro ao ler do RTC!\n");
    } else {
        printf("Dados lidos com sucesso!\n");
    }
    data_received = true;
}

int main() {
    stdio_init_all();
    printf("Iniciando o programa...\n");

    i2c_init(I2C_PORT, 100 * 1000); // 100 kHz
    printf("Comunicação I2C inicializada.\n");

    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
    printf("Pinos I2C configurados.\n");

    // Configurar a interrupção I2C
    irq_set_exclusive_handler(I2C0_IRQ, i2c_callback);
    irq_set_enabled(I2C0_IRQ, true);
    printf("Interrupção I2C configurada.\n");

    // Configurar a data e hora manualmente
    printf("Configurando data e hora...\n");
    setDateTime(24, 9, 2024, 13, 27, 0);
    printf("Data e hora configuradas!\n");

    while (1) {
        if (data_received) {
            getDateTime();
            data_received = false;
        }
        sleep_ms(5000); // Aguarda 5 segundos entre leituras
    }

    return 0;
}

// Função para configurar data e hora
void setDateTime(int day, int month, int year, int hour, int minute, int second) {
    uint8_t buffer[7];
    buffer[0] = decToBcd(second);
    buffer[1] = decToBcd(minute);
    buffer[2] = decToBcd(hour);
    buffer[3] = 0; // Dia da semana não é utilizado, definir como 0
    buffer[4] = decToBcd(day);
    buffer[5] = decToBcd(month);
    buffer[6] = decToBcd(year - 2000); // Ano em formato YY

    printf("Escrevendo data e hora no RTC...\n");
    int result = i2c_write_blocking(I2C_PORT, DS1307_I2C_ADDR, buffer, 7, false);
    if (result == PICO_ERROR_GENERIC) {
        printf("Erro ao escrever no RTC!\n");
    } else {
        printf("Escrita no RTC realizada com sucesso!\n");
    }
}

// Exibir data e hora:
void getDateTime() {
    int second = bcdToDec(data_buffer[0]);
    int minute = bcdToDec(data_buffer[1]);
    int hour = bcdToDec(data_buffer[2]);
    int day = bcdToDec(data_buffer[4]);
    int month = bcdToDec(data_buffer[5]);
    int year = bcdToDec(data_buffer[6]) + 2000;

    printf("Data: %02d/%02d/%04d - Hora: %02d:%02d:%02d\n", day, month, year, hour, minute, second);
}

// Função para conversão
int decToBcd(int val) {
    return ((val / 10 * 16) + (val % 10));
}

int bcdToDec(int val) {
    return ((val / 16 * 10) + (val % 16));
}
