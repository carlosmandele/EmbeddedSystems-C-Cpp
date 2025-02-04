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


