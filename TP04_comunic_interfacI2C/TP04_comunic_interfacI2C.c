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


