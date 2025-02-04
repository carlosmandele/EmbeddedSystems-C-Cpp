## Contador de pressionamento de botão com piscar de LED integrado

**1.** **Elabore** um programa para acionar um LED quando o botão A for pressionado 5 vezes, tomando cuidado para registrar essa contagem. Quando o valor da contagem chegar a 5, um LED deve piscar por 10 segundos, na frequência de 10 Hz.

- ### Objetivo do programa
  <p style="font-size:14px; font-style: arial; clor: black">O objetivo deste programa é registrar a quantidade de vezes que o botão A é pressionado e, quando a contagem atingir 5, um LED é acionado para piscar por 10 segundos, na frequência de 10 Hz. Basicamente, o programa monitora as interações com o botão e em seguida executa uma ação visual clara após um número específico de pressões.</p>

  <a href="https://wokwi.com/projects/420387966937638913">Link do programa para testes</a>

### Principais componentes do código fonte

- **Inclusão e definição de Bibliotecas:**
  
  Neste primeiro trecho do código fonte importei as bibliotecas essenciais para trabalhar com RP2040, nomeadamente temporizadores de hardware e métodos GPIO de entrada e saída padrão.

```
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include <stdio.h>
```


- **Configuração dos pinos:**
  
  Aqui define os pinos para o LED e para o botão. Essas contantes definem os pinos da RP2040 que serão utilizados para o LED e o botão. Em seguida configurei o LED_PIN como 13 e o BTN_A_PIN como 5.

```
const uint LED_PIN = 13;   
const uint BTN_A_PIN = 5;  
```

- **Variáveis Globais Voláteis:**

  A qui, configurei as variáveis globais para armazenar o state (estado) e contadores. Além disso, essa configuração permite que o valor das variáveis seja atualizado corretamente quando usado em métodos de interrupção.

```
volatile int button_press_count = 0;
volatile int blink_count = 0; 
volatile bool led_state = false;
```

- **Função  ``turn_off_callback``:**

  A função **``turn_off_callback``** é um callback do temporizador repetitivo que é chamado repetidamente para manipular o LED. Esse método permite alternar o estado do LED **``(led_state)``**, incrementa o contador de piscadas **``(blink_count)``** e desliga o LED após 10 segundos. E quando o LED é desligado, a função retorna **``(false)``** para interromper o temporizador.

```
bool turn_off_callback(struct repeating_timer *t) {
    blink_count++;
    led_state = !led_state;
    gpio_put(LED_PIN, led_state);

    if (blink_count >= 100) {
        blink_count = 0;
        button_press_count = 0;
        gpio_put(LED_PIN, false);
        printf("LED desligado\n"); 
        return false;
    }
    return true;
}
```

- **Função ``repeating_timer_callback:``**

  Essa função é um callback do temporizador que permite verificar repetidamente o estado do  verificar o botão a cada 100 ms. Basicamente permite implementar um debounce monitorando se o botão foi pessionado e faz uma pausa de 200 ms entre as interações com o botão. Após o botão ser pressionado 5 vezes, ele inicia o temporizador para controlar o LED.

```
bool repeating_timer_callback(struct repeating_timer *t) {
    static absolute_time_t last_press_time = 0;
    static bool button_last_state = false;
     
    bool button_pressed = !gpio_get(BTN_A_PIN);
 
    if (button_pressed && !button_last_state &&
        absolute_time_diff_us(last_press_time, get_absolute_time()) > 200000) {
        last_press_time = get_absolute_time();
        button_last_state = true;
        button_press_count++;
        
        if (button_press_count == 5) {
            printf("Botão pressionado 5 vezes. LED piscando...\n");
            add_repeating_timer_ms(100, turn_off_callback, NULL, NULL);
        }
    } else if (!button_pressed) {
        button_last_state = false;
    }

    return true;
}
```

- **Função ``main:``**

  Inicializo o sistema, a comunicação serial para depuração e configuro o pino do LED e do botão. Configurei também um temporizador repetitivo para monitorar o estado do botão a cada 100 ms e deixo o loop principal vazio **``while (true)``**, uma vez que todas as ações do LED e do botão são gerenciadas pelas funções de callback.

```
int main() {

    stdio_init_all();
    
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 0);
    
    gpio_init(BTN_A_PIN);
    gpio_set_dir(BTN_A_PIN, GPIO_IN);
    gpio_pull_up(BTN_A_PIN);
    
    struct repeating_timer timer;
    add_repeating_timer_ms(100, repeating_timer_callback, NULL, &timer);
    
    while (true) {
        tight_loop_contents();
    }
}
```
