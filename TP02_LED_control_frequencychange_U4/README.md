## Implementando o botão B para mudar a frequência do LED para 1 Hz.

- ### Objetivo do programa
  <p style="font-size:14px; font-style: arial; clor: black">O objetivo deste programa é controlar um LED e alterar sua frequência de piscar com base nas interações do usuário com dois botões.</p>

  <a href="https://wokwi.com/projects/420662112933292033">Link do programa para testes</a>

- **Funcionamento:**
  Neste programa, eu controlo o LED e dois botões para criar uma interação dinâmica. Cada método tem um papel específico para garantir que o LED pisque conforme as condições estabelecidas:

  - Controle do LED: Piscar o LED conforme ações específicas realizadas nos botões A e B.

  - Botão A: Quando pressionado 5 vezes, o LED começa a piscar a uma frequência de 10 Hz (100 ms por pisca) durante 10 segundos.

  - Botão B: Alterna a frequência do LED entre 10 Hz (100 ms por pisca) e 1 Hz (1000 ms por pisca).

### Principais componentes do código fonte
- **Inclusão de bibliotecas:**
  
  No codigo a baixo, estou importando as bibliotecas essenciais para controlar o LED e os botões, bem como para utilizar temporizadores.

```
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include <stdio.h>
```

- **Configuração de Pinos:**

  Defino os pinos do LED e dos botões **``(BTN_A e BTN_B)``**. O LED está no pino ``13``, **``BTN_A``** no pino ``5`` e **``BTN_B``** no pino ``6``, conforme trecho a baixo.

```
const uint LED_PIN = 13;   
const uint BTN_A_PIN = 5;
const uint BTN_B_PIN = 6;
```

- **Variáveis globais**

  Utilizo variáveis globais para controlar o estado do LED, a contagem de pressões dos botões e uma flag para alterar a frequência do LED.

```
volatile int button_press_count = 0;
volatile int blink_count = 0;
volatile bool led_state = false;
volatile bool change_frequency = false;
```

- **Função  ``turn_off_callback:``**

  Esta função é chamada repetidamente para controlar o piscar do LED. Alterna o estado do LED (ligado/desligado) e verifica se ele já piscou pelo tempo determinado (10 segundos). A frequência de piscar é alterada com base no estado da flag **``change_frequency``**.

```
bool turn_off_callback(struct repeating_timer *t) {
    blink_count++;
    led_state = !led_state;
    gpio_put(LED_PIN, led_state);

    int frequency = change_frequency ? 1000 : 100;
    if (blink_count >= (change_frequency ? 10 : 100)) {
        blink_count = 0;
        button_press_count = 0;
        gpio_put(LED_PIN, false);
        printf("LED desligado\n");
        return false;
    }
    add_repeating_timer_ms(frequency, turn_off_callback, NULL, t);
    return false; // Para o callback atual
}
```

- **Função  ``repeating_timer_callback:``**

  Esta função verifica o estado dos botões a cada 100 ms. Ela também implementa o debounce para garantir que pressões rápidas dos botões não sejam lidas várias vezes. Quando **``BTN_A``** é pressionado 5 vezes, inicia o piscar do LED. Quando **``BTN_B``** é pressionado, a frequência de piscar do LED é alternada entre 10 Hz e 1 Hz.
  ```
  bool repeating_timer_callback(struct repeating_timer *t) {
    static absolute_time_t last_press_time = 0;
    static bool button_a_last_state = false;
    static bool button_b_last_state = false;

    bool button_a_pressed = !gpio_get(BTN_A_PIN);
    bool button_b_pressed = !gpio_get(BTN_B_PIN);

    if (button_a_pressed && !button_a_last_state &&
        absolute_time_diff_us(last_press_time, get_absolute_time()) > 200000) {
        last_press_time = get_absolute_time();
        button_a_last_state = true;
        button_press_count++;

        if (button_press_count == 5) {
            printf("Botão A pressionado 5 vezes. LED piscando...\n");
            add_repeating_timer_ms(100, turn_off_callback, NULL, NULL);
        }
    } else if (!button_a_pressed) {
        button_a_last_state = false;
    }

    if (button_b_pressed && !button_b_last_state &&
        absolute_time_diff_us(last_press_time, get_absolute_time()) > 200000) {
        last_press_time = get_absolute_time();
        button_b_last_state = true;
        change_frequency = !change_frequency;
        printf("Botão B pressionado. Frequência do LED %s para %d Hz.\n", change_frequency ? "alterada" : "alterada", change_frequency ? 1 : 10);
    } else if (!button_b_pressed) {
        button_b_last_state = false;
    }

    return true;
}

- **Função principal ``main:``**

  Inicializo o sistema, a comunicação serial para depuração e configuro os pinos do LED e dos botões. Configuro um temporizador repetitivo para verificar os botões a cada 100 ms e deixo o loop principal vazio, pois o controle do LED e dos botões é gerenciado pelas funções de callback.

```
int main() {
    stdio_init_all();

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 0);
    
    gpio_init(BTN_A_PIN);
    gpio_set_dir(BTN_A_PIN, GPIO_IN);
    gpio_pull_up(BTN_A_PIN);

    gpio_init(BTN_B_PIN);
    gpio_set_dir(BTN_B_PIN, GPIO_IN);
    gpio_pull_up(BTN_B_PIN);
    
    struct repeating_timer timer;
    add_repeating_timer_ms(100, repeating_timer_callback, NULL, &timer);
    
    while (true) {
        tight_loop_contents();
    }
}
```