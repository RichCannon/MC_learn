#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"
#include "driver/gpio.h"
#include "driver/i2c.h"

namespace AppConfig
{
    namespace Pins
    {
        constexpr gpio_num_t SCL = GPIO_NUM_16;
        constexpr gpio_num_t SDA = GPIO_NUM_17;

    }
    namespace I2C
    {
        constexpr i2c_port_t PORT = I2C_NUM_0;
        constexpr uint32_t SPEED = 400000; // 400 kHz
    }

    namespace ADDRESSES
    {
        constexpr uint32_t OLED = 0x3c;
    }

};

typedef struct
{
    uint8_t s;
    uint8_t m;
    uint8_t h;
    uint8_t W;
    uint8_t D;
    uint8_t M;
    uint8_t Y;
} DateTime;

typedef enum
{
    SETTINGS = 0x00,
    GRAPHIC = 0x40,
} OLED_CONTROL_BYTE;

const uint8_t oled_init_cmds[] = {
    0xAE, // 1. Вимкнути дисплей (Display OFF)

    0xD5, 0x80, // Налаштування частоти осцилятора
    0xA8, 0x3F, // Вказуємо роздільну здатність: 64 рядки (0x3F)
    0xD3, 0x00, // Зсув екрана по вертикалі (без зсуву)

    0x40,       // Початкова лінія екрана (Line 0)
    0xA1,       // Сегменти зліва направо (щоб не було віддзеркалено)
    0xC8,       // Рядки зверху вниз (щоб не було перевернуто)
    0xDA, 0x12, // Апаратна конфігурація COM-пінів (для 128x64)

    0x81, 0xCF, // Яскравість (Контрастність) від 0x00 до 0xFF
    0xD9, 0xF1, // Налаштування періоду попереднього заряду пікселя
    0xDB, 0x40, // Налаштування рівня VCOMH

    0x8D, 0x14, // 2. УВІМКНУТИ Charge Pump (генератор напруги)
    0x20, 0x00, // 3. Горизонтальний режим адресації пам'яті

    0xAF // 4. Увімкнути дисплей (Display ON)
};

// Команди для повернення курсору
const uint8_t oled_draw_boundaries[] = {
    0x21, 0x00, 0x7F, // Колонки від 0 до 127
    0x22, 0x00, 0x07  //  Сторінки від 0 до 7
};

const size_t buffer_length = 1024;
uint8_t buffer[buffer_length] = {0};

const size_t oled_init_cmds_length = sizeof(oled_init_cmds) / sizeof(oled_init_cmds[0]);
const size_t oled_draw_boundaries_length = sizeof(oled_draw_boundaries) / sizeof(oled_draw_boundaries[0]);

void init_i2c(void)
{
    i2c_config_t config = {};
    config.mode = I2C_MODE_MASTER,
    config.sda_io_num = AppConfig::Pins::SDA,
    config.scl_io_num = AppConfig::Pins::SCL,
    config.scl_pullup_en = GPIO_PULLUP_ENABLE,
    config.sda_pullup_en = GPIO_PULLUP_ENABLE,
    config.master.clk_speed = AppConfig::I2C::SPEED,

    i2c_param_config(AppConfig::I2C::PORT, &config);
    i2c_driver_install(AppConfig::I2C::PORT, config.mode, 0, 0, 0);
};

void oled_send_cmd(const uint8_t command[], size_t length, OLED_CONTROL_BYTE controlByte)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    // Старт
    i2c_master_start(cmd);
    // Адреса
    i2c_master_write_byte(cmd, (AppConfig::ADDRESSES::OLED << 1) | I2C_MASTER_WRITE, true);
    // Відправити "тут буде команда (не графіка)"
    i2c_master_write_byte(cmd, controlByte, true);
    // Записати команду
    i2c_master_write(cmd, command, length, true);
    // Стоп
    i2c_master_stop(cmd);
    // Відправити
    i2c_master_cmd_begin(AppConfig::I2C::PORT, cmd, 1000 / portTICK_PERIOD_MS);
    // Почистити пам'ять
    i2c_cmd_link_delete(cmd);
}

void oled_show(uint8_t *buffer)
{
    oled_send_cmd(oled_draw_boundaries, oled_draw_boundaries_length, OLED_CONTROL_BYTE::SETTINGS);
    oled_send_cmd(buffer, buffer_length, OLED_CONTROL_BYTE::GRAPHIC);
}

void oled_clear()
{
    uint8_t empty[buffer_length] = {0};
    oled_send_cmd(empty, buffer_length, OLED_CONTROL_BYTE::GRAPHIC);
}

void oled_init()
{
    oled_send_cmd(oled_init_cmds, oled_init_cmds_length, OLED_CONTROL_BYTE::SETTINGS); // Display ON
    oled_clear();
}

void draw_pixel(uint8_t x, uint8_t y, bool on)
{
    if (x >= 128 || y >= 64)
        return;
    const uint8_t page = y / 8;
    const uint16_t buffer_el_idx = x + (page * 128);
    const uint8_t buffer_bit_idx = y % 8;

    if (on)
    {
        buffer[buffer_el_idx] = buffer[buffer_el_idx] | (1 << buffer_bit_idx);
    }
    else
    {
        buffer[buffer_el_idx] = buffer[buffer_el_idx] & ~(1 << buffer_bit_idx);
    }
}

extern "C" void app_main(void)
{
    init_i2c();
    oled_init();
    draw_pixel(3, 5, true);
    draw_pixel(30, 40, true);
    draw_pixel(100, 50, true);
    draw_pixel(15, 32, true);
    oled_show(buffer);
}