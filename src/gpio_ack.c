#include "gpio_ack.h"
#include "gpio_init.h"
#include "hardware/gpio.h"
#include "buffer_manager.h"
#include "spi_dma.h"
#include "acquisition_control.h"
#include <stdio.h>

extern uint16_t* current_spi_buffer;

void __isr gpio_irq_handler(uint gpio, uint32_t events) {
    if (gpio == ACK_PIN) {
        if (events & GPIO_IRQ_EDGE_RISE) {
            if (current_spi_buffer) {
                buffer_manager_mark_sent(current_spi_buffer);
                current_spi_buffer = NULL;
            }
        }
    } else if (gpio == ACQ_PIN) {
        if (events & GPIO_IRQ_EDGE_RISE) {
            start_acquisition();
        } else if (events & GPIO_IRQ_EDGE_FALL) {
            stop_acquisition();
        }
    }
}

void setup_gpio_irq(void) {
    gpio_set_irq_enabled_with_callback(ACK_PIN, GPIO_IRQ_EDGE_RISE, true, &gpio_irq_handler);
    gpio_set_irq_enabled(ACQ_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
}
