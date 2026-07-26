#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include "led_strip.h"

namespace AppConfig
{
    namespace Pins
    {
        constexpr gpio_num_t LED = GPIO_NUM_48;
        constexpr gpio_num_t UART_TX = GPIO_NUM_17;
        constexpr gpio_num_t UART_RX = GPIO_NUM_18;
        constexpr gpio_num_t BUTTON = GPIO_NUM_0;

    }

    namespace UART
    {
        constexpr uart_port_t NUM = UART_NUM_1;
        constexpr uint32_t BUF_SUZE = 1024;

    }

    namespace System
    {
        constexpr uint32_t SERIAL_BAUD_RATE = 115200;
        constexpr uint32_t STARTUP_DELAY_MS = 3000;
    }

};

void init_uart(void)
{
    uart_config_t uart_config = {};
    uart_config.baud_rate = 9600;
    uart_config.data_bits = UART_DATA_8_BITS;
    uart_config.parity = UART_PARITY_DISABLE;
    uart_config.stop_bits = UART_STOP_BITS_1;
    uart_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
    uart_config.source_clk = UART_SCLK_DEFAULT;

    int intr_alloc_flags = 0;

    ESP_ERROR_CHECK(uart_driver_install(AppConfig::UART::NUM, AppConfig::UART::BUF_SUZE * 2, 0, 0, NULL, intr_alloc_flags));
    ESP_ERROR_CHECK(uart_param_config(AppConfig::UART::NUM, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(AppConfig::UART::NUM, AppConfig::Pins::UART_TX, AppConfig::Pins::UART_RX, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
}

void init_led(led_strip_handle_t *handle)
{
    led_strip_config_t strip_config = {};
    strip_config.strip_gpio_num = AppConfig::Pins::LED;
    strip_config.max_leds = 1; // На платі лише 1 світлодіод

    led_strip_rmt_config_t rmt_config = {};
    rmt_config.flags.with_dma = false;

    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, handle));
    led_strip_clear(*handle); // Вимикаємо при старті
}

static QueueHandle_t gpio_evt_queue = NULL;

static void IRAM_ATTR gpio_isr_handle(void *arg)
{
    uint32_t gpio_num = (uint32_t)arg;
    xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
}

static void button_task(void *arg)
{
    uint32_t io_num;
    for (;;)
    {
        if (xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY))
        {
            static TickType_t last = 0;
            TickType_t now = xTaskGetTickCount();
            if (now - last > pdMS_TO_TICKS(200))
            {
                char cmd = 'E';
                uart_write_bytes(AppConfig::UART::NUM, &cmd, 1);
                last = now;
            }
        }
    }
}

void button_init()
{
    gpio_config_t io_config = {};
    io_config.pin_bit_mask = (1ULL << AppConfig::Pins::BUTTON);
    io_config.mode = GPIO_MODE_INPUT;
    io_config.pull_up_en = GPIO_PULLUP_ENABLE;
    io_config.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_config.intr_type = GPIO_INTR_NEGEDGE;

    gpio_config(&io_config);

    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));

    xTaskCreate(button_task, "button_task", 2048, NULL, 10, NULL);

    gpio_install_isr_service(0);

    gpio_isr_handler_add(AppConfig::Pins::BUTTON, gpio_isr_handle, (void *)AppConfig::Pins::BUTTON);
};

extern "C" void app_main(void)
{

    // gpio_reset_pin(AppConfig::Pins::PIN_LED);
    // gpio_set_direction(AppConfig::Pins::PIN_LED, GPIO_MODE_OUTPUT);
    // gpio_set_level(AppConfig::Pins::PIN_LED, 0);

    led_strip_handle_t led_strip;

    init_uart();
    init_led(&led_strip);
    button_init();

    uint8_t data[1];
    bool led_state = false;

    while (true)
    {
        int len = uart_read_bytes(AppConfig::UART::NUM, data, 1, portMAX_DELAY);
        if (len > 0)
        {
            led_state = !led_state;
            if (led_state)
            {
                led_strip_set_pixel(led_strip, 0, 0, 0, 50);
                led_strip_refresh(led_strip);
            }
            else
            {
                led_strip_clear(led_strip);
            }
        }
    }
}