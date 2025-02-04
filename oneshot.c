#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "pico/time.h"

#define LED_VERDE   11
#define LED_AMARELO 12
#define LED_VERMELHO 13
#define BOTAO       5

volatile bool processo_ativo = false;
static volatile uint32_t last_time = 0;
int64_t turn_off_callback(alarm_id_t id, void *user_data) {
    static int estado = 0;
    switch (estado) {
        case 0:
            gpio_put(LED_VERDE, 1);
            gpio_put(LED_AMARELO, 1);
            gpio_put(LED_VERMELHO, 1);
            add_alarm_in_ms(3000, turn_off_callback, NULL, false);
            break;
        case 1:
            gpio_put(LED_VERDE, 1);
            gpio_put(LED_AMARELO, 1);
            gpio_put(LED_VERMELHO, 0);
            add_alarm_in_ms(3000, turn_off_callback, NULL, false);
            break;
        case 2:
            gpio_put(LED_VERDE, 1);
            gpio_put(LED_AMARELO, 0);
            gpio_put(LED_VERMELHO, 0);
            add_alarm_in_ms(3000, turn_off_callback, NULL, false);
            break;
        case 3:
            gpio_put(LED_VERDE, 0);
            processo_ativo = false;
            estado = -1; // Reinicia a sequência para permitir nova ativação
            break;
    }
    estado++;
    return 0;
}

void gpio_callback(uint gpio, uint32_t events) {uint32_t current_time = to_us_since_boot(get_absolute_time());

    if (current_time - last_time > 800000) { // Debounce de 800ms
        last_time = current_time;
    if (gpio == BOTAO && !processo_ativo) {
        processo_ativo = true;
        add_alarm_in_ms(0, turn_off_callback, NULL, false);
    }
    }
}

int main() {
    stdio_init_all();
    gpio_init(LED_VERDE);
    gpio_set_dir(LED_VERDE, GPIO_OUT);
    gpio_init(LED_AMARELO);
    gpio_set_dir(LED_AMARELO, GPIO_OUT);
    gpio_init(LED_VERMELHO);
    gpio_set_dir(LED_VERMELHO, GPIO_OUT);
    gpio_init(BOTAO);
    gpio_set_dir(BOTAO, GPIO_IN);
    gpio_pull_up(BOTAO);
    gpio_set_irq_enabled_with_callback(BOTAO, GPIO_IRQ_EDGE_RISE, true, &gpio_callback);

    while (1) {
        tight_loop_contents();
    }
}
