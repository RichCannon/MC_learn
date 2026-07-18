

#include "enc.h"

pcnt_unit_handle_t init_encoder(gpio_num_t gpio_a, gpio_num_t gpio_b)
{
  // Створення структури конфігурації блоку PCNT
  pcnt_unit_config_t unit_config = {};
  unit_config.low_limit = AppConfig::ENC::MIN_COUNT;  // Нижня межа лічильника енкодера
  unit_config.high_limit = AppConfig::ENC::MAX_COUNT; // Верхня межа лічильника енкодера
  unit_config.intr_priority = 0;                      // Пріоритет переривання за замов.
  unit_config.flags.accum_count = 0;                  // Вимкнення режиму накопичення рахунку

  // Створення дескриптора блоку лічильника імпульсів PCNT
  pcnt_unit_handle_t unit = NULL;
  // Ініціалізація нового блоку PCNT з заданою конфігурацією
  ESP_ERROR_CHECK(pcnt_new_unit(&unit_config, &unit));

  // Налаштування фільтра брязкоту контактів для сигналу енкодера
  pcnt_glitch_filter_config_t filter_config = {.max_glitch_ns = AppConfig::ENC::DEBOUNCE_NS};
  // Увімкнення фільтрації коротких імпульсних перешкод у модулі PCNT
  ESP_ERROR_CHECK(pcnt_unit_set_glitch_filter(unit, &filter_config));

  // Налаштування каналу енкодера A
  // Створення та ініціалізація структури конфігурації каналу PCNT
  pcnt_chan_config_t chan_a_config = {};
  chan_a_config.edge_gpio_num = gpio_a;       // Вибір GPIO для (канал A)
  chan_a_config.level_gpio_num = gpio_b;      // Вибір GPIO для (канал В)
  chan_a_config.flags.invert_edge_input = 0;  // Без інверсії сигналу A
  chan_a_config.flags.invert_level_input = 0; // Без інверсії сигналу В
  chan_a_config.flags.virt_edge_io_level = 0;
  chan_a_config.flags.virt_level_io_level = 0;

  // Створення дескриптора каналу PCNT для сигналу A енкодера
  pcnt_channel_handle_t chan_a = NULL;
  // Створення нового каналу PCNT у заданому лічильнику
  ESP_ERROR_CHECK(pcnt_new_channel(unit, &chan_a_config, &chan_a));

  pcnt_chan_config_t chan_b_config = {};
  chan_b_config.edge_gpio_num = gpio_b;       // Вибір GPIO для (канал В)
  chan_b_config.level_gpio_num = gpio_a;      // Вибір GPIO для (канал A)
  chan_b_config.flags.invert_edge_input = 0;  // Без інверсії сигналу A
  chan_b_config.flags.invert_level_input = 0; // Без інверсії сигналу В
  chan_b_config.flags.virt_edge_io_level = 0;
  chan_b_config.flags.virt_level_io_level = 0;

  // Створення нового каналу PCNT у заданому лічильнику
  pcnt_channel_handle_t chan_b = NULL;
  // Створення нового каналу PCNT у заданому лічильнику
  ESP_ERROR_CHECK(pcnt_new_channel(unit, &chan_b_config, &chan_b));

  // Квадратурна логіка
  // Канал A: при B=1 наростаючий фронт (0->1) робить -1, а спадаючий (1->0) робить +1
  ESP_ERROR_CHECK(pcnt_channel_set_edge_action(chan_a,
                                               PCNT_CHANNEL_EDGE_ACTION_DECREASE,
                                               PCNT_CHANNEL_EDGE_ACTION_INCREASE));
  // Канал A: при B=1 зберігає базову логіку фронтів, а при B=0 — інвертує
  ESP_ERROR_CHECK(pcnt_channel_set_level_action(chan_a,
                                                PCNT_CHANNEL_LEVEL_ACTION_KEEP,
                                                PCNT_CHANNEL_LEVEL_ACTION_INVERSE));
  // Канал B: при A=1 наростаючий фронт (0->1) робить +1, а спадаючий (1->0) робить -1
  ESP_ERROR_CHECK(pcnt_channel_set_edge_action(chan_b,
                                               PCNT_CHANNEL_EDGE_ACTION_INCREASE,
                                               PCNT_CHANNEL_EDGE_ACTION_DECREASE));
  // Канал B: при A=1 зберігає базову логіку фронтів, а при A=0 — інвертує
  ESP_ERROR_CHECK(pcnt_channel_set_level_action(chan_b,
                                                PCNT_CHANNEL_LEVEL_ACTION_KEEP,
                                                PCNT_CHANNEL_LEVEL_ACTION_INVERSE));

  ESP_ERROR_CHECK(pcnt_unit_enable(unit));      // Активує) апаратний блок PCNT
  ESP_ERROR_CHECK(pcnt_unit_clear_count(unit)); // Обнуляє значення PCNT
  ESP_ERROR_CHECK(pcnt_unit_start(unit));       // Запускає PCNT

  return unit;
}