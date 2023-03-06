/*
Copyright © 2020 Dmytro Korniienko (kDn)
JeeUI2 lib used under MIT License Copyright (c) 2019 Marsel Akhkamov

    This file is part of FireLamp_JeeUI.

    FireLamp_JeeUI is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    FireLamp_JeeUI is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with FireLamp_JeeUI.  If not, see <https://www.gnu.org/licenses/>.

  (Этот файл — часть FireLamp_JeeUI.

   FireLamp_JeeUI - свободная программа: вы можете перераспространять ее и/или
   изменять ее на условиях Стандартной общественной лицензии GNU в том виде,
   в каком она была опубликована Фондом свободного программного обеспечения;
   либо версии 3 лицензии, либо (по вашему выбору) любой более поздней
   версии.

   FireLamp_JeeUI распространяется в надежде, что она будет полезной,
   но БЕЗО ВСЯКИХ ГАРАНТИЙ; даже без неявной гарантии ТОВАРНОГО ВИДА
   или ПРИГОДНОСТИ ДЛЯ ОПРЕДЕЛЕННЫХ ЦЕЛЕЙ. Подробнее см. в Стандартной
   общественной лицензии GNU.

   Вы должны были получить копию Стандартной общественной лицензии GNU
   вместе с этой программой. Если это не так, см.
   <https://www.gnu.org/licenses/>.)
*/

/*
  !!! ВНОСИТЬ ИЗМЕНЕНИЯ В ЭТОЙ ФАЙЛ НЕЛЬЗЯ !!!
  переименуйте "user_config.h.default" в "user_config.h" и вносите свои правки там

  !!! NEVER EVER CHANGE THIS FILE !!!
  rename "user_config.h.default" into "user_config.h" and adjust to your needs
*/


#pragma once

#if defined CUSTOM_CFG
# include CUSTOM_CFG
#else
# if defined __has_include
#  if __has_include("user_config.h")
#   include "user_config.h"
#  endif
# endif
#endif

//-----------------------------------
//#define ESP_USE_BUTTON                                      // если строка не закомментирована, должна быть подключена кнопка (иначе ESP может регистрировать "фантомные" нажатия и некорректно устанавливать яркость)
//#define LAMP_DEBUG                                          // режим отладки, можно также включать в platformio.ini
//#define DEBUG_TELNET_OUTPUT  (true)                         // true - отладочные сообщения будут выводиться в telnet вместо Serial порта (для удалённой отладки без подключения usb кабелем) // Deprecated
//#define MIC_EFFECTS                                         // Включить использование микрофона для эффектов
//#define MP3PLAYER                                           // Включить использование MP3 плеера (DF Player)
//#define SHOWSYSCONFIG                                       // Показывать системное меню
//#define DISABLE_LED_BUILTIN                                 // Отключить встроенный в плату светодиод, если нужно чтобы светил - закомментировать строку
//-----------------------------------
#ifndef LANG_FILE
#define LANG_FILE                  "text_res-RUS.h"           // Языковой файл по дефолту
#endif

// Disable built-in LED operations if there is no LED :)
#ifndef LED_BUILTIN
#define DISABLE_LED_BUILTIN
#endif

#ifdef RTC
  #ifndef RTC_MODULE
  #define RTC_MODULE          (2U)                          // Поддерживаются модули DS1302 = (1U),  DS1307 = (2U), DS3231 = (3U)
  #endif
  #ifndef RTC_SYNC_PERIOD
  #define RTC_SYNC_PERIOD     (24U)                         // Период синхронизации RTC c ntp (часы)
  #endif
  #if RTC_MODULE > (1U)                                     // Если выбран модуль с I2C (DS1307 или DS3231)
    #ifdef TM1637_CLOCK                                     // Если есть дисплей TM1637, то можем использовать его пины для RTC (но RTC модуль работает не на всех пинах)
      #ifndef pin_SW_SDA
      #define pin_SW_SDA        (TM_CLK_PIN)                // Пин SDA RTC модуля подключаем к CLK пину дисплея
      #endif
      #ifndef pin_SW_SCL
      #define pin_SW_SCL        (TM_DIO_PIN)                // Пин SCL RTC модуля подключаем к DIO пину дисплея
      #endif
    #else                                                   // Пины подбирать экспериментальным путем, точно работает на D2 и D4
      #ifndef pin_SW_SDA
      #define pin_SW_SDA        (4)                         // Назначаем вывод для работы в качестве линии SDA программной шины I2C, D2 on wemos
      #endif
      #ifndef pin_SW_SCL
      #define pin_SW_SCL        (2)                         // Назначаем вывод для работы в качестве линии SCL программной шины I2C, D4 on wemos
      #endif
    #endif
    #if RTC_MODULE == (1U)                                  // Если выбран модуль DS1302.
      #ifndef pin_RST
      #define pin_RST             (15)                      // Назначаем вывод RST, D8 on wemos
      #endif
      #ifndef pin_DAT
      #define pin_DAT             (0)                       // Назначаем вывод DAT, D3 on wemos
      #endif
      #ifndef pin_DAT
      #define pin_CLK             (2)                       // Назначаем вывод CLK, D4 on wemos
      #endif
    #endif
  #endif
#endif

#ifndef MIC_PIN
#ifdef ESP8266
#define MIC_PIN               (A0)                          // ESP8266 Analog Pin ADC0 = A0
#else
#define MIC_PIN               (GPIO_NUM_34)                 // ESP32 Analog Pin
#endif
#define FAST_ADC_READ                                       // использовать полный диапазон звуковых частот, если закомментировано, то будет до 5кГц, но сэкономит память и проще обсчитать...
#endif

#ifndef MIC_POLLRATE
#define MIC_POLLRATE          (50U)                         // как часто опрашиваем микрофон, мс
#endif

#ifndef HIGH_MAP_FREQ
#define HIGH_MAP_FREQ         (20000U)                      // верхняя граница слышимого диапазона, используется для мапинга, дефолтное и общепринятое значение 20000Гц
#endif

#ifdef FAST_ADC_READ
#ifndef SAMPLING_FREQ
#define SAMPLING_FREQ         (18000U*2U)
#endif
#else
#define SAMPLING_FREQ         (5000U*2U)
#endif

#ifndef LOW_FREQ_MAP_VAL
#define LOW_FREQ_MAP_VAL      (35U)                         // Граница логарифмически приведенных значений выше которой уже идут средние частоты (микрофон) ~150Гц
#endif

#ifndef HI_FREQ_MAP_VAL
#define HI_FREQ_MAP_VAL       (188U)                        // Граница логарифмически приведенных значени выше которой уже идут высокие частоты (микрофон) ~4.5кГц
#endif

#ifndef MIN_PEAK_LEVEL
#define MIN_PEAK_LEVEL        (50U)                         // Минимальный амплитудный уровень, для эффектов зависящих от микрофона
#endif

#ifdef MP3PLAYER
// #ifdef ESP32
//  #error ESP32 with DfPlayer is not (yet) supported due to softwareserial dependency (to be fixed)
// #endif
#ifndef MP3_TX_PIN
#define MP3_TX_PIN            (14)                         // TX mp3 player RX (D5)
#endif
#ifndef MP3_RX_PIN
#define MP3_RX_PIN            (12)                         // RX mp3 player TX (D6)
#endif
#ifndef MP3_SERIAL_TIMEOUT
#define MP3_SERIAL_TIMEOUT    (300U)                       // 300мс по умолчанию, диапазон 200...1000, подбирается экспериментально, не желательно сильно повышать
#endif
#ifndef DFPLAYER_START_DELAY
#define DFPLAYER_START_DELAY  (500U)                       // 500мс по умолчанию, диапазон 10...1000, подбирается экспериментально, не желательно сильно повышать, безусловная задержка до инициализации
#endif
#endif

#ifndef LAMP_PIN
#define LAMP_PIN              (0)                          // пин ленты                (D3)
#endif

#ifndef BTN_PIN
#define BTN_PIN               (5U)                          // пин кнопки               (D1)
#if BTN_PIN == 0
#define PULL_MODE             (HIGH_PULL)                   // пин кнопки "FLASH" NodeMCU, подтяжка должна быть PULL_MODE=HIGH_PULL
#endif
#endif

#ifndef DS18B20_PIN
#define DS18B20_PIN           (13)                        // D7 Пин подключения датчика DS18b20. При использовании энкодара, датчик можно назначить на пин кнопки (SW) энкодера. И поставить резистор подтяжки к +3.3в.
#endif
#ifndef DS18B_READ_DELAY
#define DS18B_READ_DELAY      (10U)                       // Секунд - периодичность опроса датчика. Чаще не надо, возможно лучше реже. С учетом теплоемкости датчика, воздуха и подложки матрицы - смысла нет
#endif
#ifndef COOLER_PIN
#define COOLER_PIN            (-1)                        // Пин для управления вентилятором охлаждения лампы. (-1) если управление вентилятором не нужно. 
#endif
#ifndef COOLER_PIN_TYPE
#define COOLER_PIN_TYPE       (0U)                        // 0-дискретный вкл\выкл, 1 - ШИМ (для 4-х пиновых вентиляторов). Убедитесь, что вывод COOLER_PIN поддерживает PWM.
#endif
#ifndef COOLING_FAIL
#define COOLING_FAIL          (6U)                        // Количество циклов DS18B_READ_DELAY. Если за это время снизить температуру до TEMP_DEST (Дискретный режим выхода вентилятора), или TEMP_MAX (ШИМ),  
#endif
#ifndef CURRENT_LIMIT_STEP
#define CURRENT_LIMIT_STEP    (0U)
#endif
#ifndef TEMP_DEST
#define TEMP_DEST         (50U)
#endif

/*
#ifndef MOSFET_PIN
#define MOSFET_PIN            (D2)                          // пин MOSFET транзистора   (D2) - может быть использован для управления питанием матрицы/ленты
#endif*/
/*#ifndef ALARM_PIN                                        
#define ALARM_PIN             (D8)                         // пин состояния будильника (D0) - может быть использован для управления каким-либо внешним устройством на время работы будильника
#endif*/
#ifndef MOSFET_LEVEL
#define MOSFET_LEVEL          (HIGH)                        // логический уровень, в который будет установлен пин MOSFET_PIN, когда матрица включена - HIGH или LOW
#endif
#ifndef ALARM_LEVEL
#define ALARM_LEVEL           (HIGH)                        // логический уровень, в который будет установлен пин ALARM_PIN, когда "рассвет"/будильник включен
#endif

#ifndef WIDTH
#define WIDTH                 (16U)                         // ширина матрицы
#endif
#ifndef HEIGHT
#define HEIGHT                (16U)                         // высота матрицы
#endif

#ifndef COLOR_ORDER
#define COLOR_ORDER           (GRB)                         // порядок цветов на ленте. Если цвет отображается некорректно - меняйте. Начать можно с RGB
#endif

#ifndef MATRIX_TYPE
#define MATRIX_TYPE           (0U)                          // тип матрицы: 0 - зигзаг, 1 - параллельная
#endif
#ifndef CONNECTION_ANGLE
#define CONNECTION_ANGLE      (1U)                          // угол подключения: 0 - левый нижний, 1 - левый верхний, 2 - правый верхний, 3 - правый нижний
#endif
#ifndef STRIP_DIRECTION
#define STRIP_DIRECTION       (3U)                          // направление ленты из угла: 0 - вправо, 1 - вверх, 2 - влево, 3 - вниз
                                                            // при неправильной настройке матрицы вы получите предупреждение "Wrong matrix parameters! Set to default"
                                                            // шпаргалка по настройке матрицы здесь! https://alexgyver.ru/matrix_guide/
#endif

#define NUM_LEDS              (uint16_t)(WIDTH * HEIGHT)    // не менять и не переопределять, служебный!

#ifndef SEGMENTS
#define SEGMENTS              (1U)                          // диодов в одном "пикселе" (для создания матрицы из кусков ленты)
#endif

#ifndef NUMHOLD_TIME
#define NUMHOLD_TIME          (3000U)                       // время запоминания последней комбинации яркости/скорости/масштаба в мс
#endif

#ifndef BRIGHTNESS
#define BRIGHTNESS            (255U)                        // стандартная максимальная яркость (0-255)
#endif

#ifndef OFF_BRIGHTNESS
#define OFF_BRIGHTNESS          (2U)                        // яркость вывода текста в случае выключенной лампы
#endif

#ifndef CURRENT_LIMIT
#define CURRENT_LIMIT         (2000U)                       // лимит по току в миллиамперах, автоматически управляет яркостью (пожалей свой блок питания!) 0 - выключить лимит
#endif

#ifndef FADE_STEPTIME
#define FADE_STEPTIME         (50U)                         // default time between fade steps, ms (2 seconds with max steps)
#endif
#ifndef FADE_TIME
#define FADE_TIME             (2000U)                       // Default fade time, ms
#endif
#ifndef FADE_MININCREMENT
#define FADE_MININCREMENT     (3U)                          // Minimal increment for brightness fade
#endif
#ifndef FADE_MINCHANGEBRT
#define FADE_MINCHANGEBRT     (30U)                         // Minimal brightness for effects changer
#endif

#ifndef MAX_FPS
#define MAX_FPS               (60U)                         // Максимальное число обсчитываемых и выводимых кадров в секунду
#endif

#ifndef SPEED_ADJ
#define SPEED_ADJ (float)NUM_LEDS/256                         // Поправка скорости риал-тайм эффектов относительно размеров метрицы.
#endif

#define EFFECTS_RUN_TIMER   (uint16_t)(1000 / MAX_FPS)     // период обработки эффектов - при 10 это 10мс, т.е. 1000/10 = 100 раз в секунду, при 20 = 50 раз в секунду, желательно использовать диапазон 10...40

#ifndef DEFAULT_DEMO_TIMER
  #define DEFAULT_DEMO_TIMER  (60U)                         // интервал смены демо по-умолчанию
#endif


// настройки времени
#ifndef HTTPTIME_SYNC_INTERVAL
 #define HTTPTIME_SYNC_INTERVAL    (4)                           // интервал синхронизации времени по http, час
#endif

#ifndef CFG_AUTOSAVE_TIMEOUT
#define CFG_AUTOSAVE_TIMEOUT       (60*1000U)                   // таймаут сохранения конфигурации эффекта, по умолчанию - 60 секунд
#endif

#ifndef TEXT_OFFSET
#define TEXT_OFFSET           (4U)                          // высота, на которой бежит текст (от низа матрицы)
#endif
#ifndef LET_WIDTH
#define LET_WIDTH             (5U)                          // ширина буквы шрифта
#endif
#ifndef LET_HEIGHT
#define LET_HEIGHT            (8U)                          // высота буквы шрифта
#endif
#ifndef LET_SPACE
#define LET_SPACE             (1U)                          // пропуск между символами (кол-во пикселей)
#endif
#ifndef LETTER_COLOR
#define LETTER_COLOR          (CRGB::White)                 // цвет букв по умолчанию
#endif
#ifndef DEFAULT_TEXT_SPEED
#define DEFAULT_TEXT_SPEED    (100U)                        // скорость движения текста, в миллисекундах - меньше == быстрее
#endif
#ifndef FADETOBLACKVALUE
#define FADETOBLACKVALUE      (222U)                        // степень затенения фона под текстом, до 255, чем больше число - тем больше затенение.
#endif

// --- РАССВЕТ -------------------------
#ifndef DAWN_BRIGHT
#define DAWN_BRIGHT           (200U)                        // максимальная яркость рассвета (0-255)
#endif
#ifndef DAWN_TIMEOUT
#define DAWN_TIMEOUT          (1U)                          // сколько рассвет светит после времени будильника, минут
#endif
//#define PRINT_ALARM_TIME                                    // нужен ли вывод времени для будильника, если пустая строка в событии будильника

// ************* НАСТРОЙКА МАТРИЦЫ *****
#if (CONNECTION_ANGLE == 0 && STRIP_DIRECTION == 0)
#define _WIDTH WIDTH
#define THIS_X (MIRR_V ? (WIDTH - x - 1) : x)
#define THIS_Y (MIRR_H ? (HEIGHT - y - 1) : y)

#elif (CONNECTION_ANGLE == 0 && STRIP_DIRECTION == 1)
#define _WIDTH HEIGHT
#define ROTATED_MATRIX
#define THIS_X (MIRR_V ? (HEIGHT - y - 1) : y)
#define THIS_Y (MIRR_H ? (WIDTH - x - 1) : x)

#elif (CONNECTION_ANGLE == 1 && STRIP_DIRECTION == 0)
#define _WIDTH WIDTH
#define THIS_X (MIRR_V ? (WIDTH - x - 1) : x)
#define THIS_Y (MIRR_H ?  y : (HEIGHT - y - 1))

#elif (CONNECTION_ANGLE == 1 && STRIP_DIRECTION == 3)
#define _WIDTH HEIGHT
#define ROTATED_MATRIX
#define THIS_X (MIRR_V ? y : (HEIGHT - y - 1))
#define THIS_Y (MIRR_H ? (WIDTH - x - 1) : x)

#elif (CONNECTION_ANGLE == 2 && STRIP_DIRECTION == 2)
#define _WIDTH WIDTH
#define THIS_X (MIRR_V ?  x : (WIDTH - x - 1))
#define THIS_Y (MIRR_H ? y : (HEIGHT - y - 1))

#elif (CONNECTION_ANGLE == 2 && STRIP_DIRECTION == 3)
#define _WIDTH HEIGHT
#define ROTATED_MATRIX
#define THIS_X (MIRR_V ? y : (HEIGHT - y - 1))
#define THIS_Y (MIRR_H ?  x : (WIDTH - x - 1))

#elif (CONNECTION_ANGLE == 3 && STRIP_DIRECTION == 2)
#define _WIDTH WIDTH
#define THIS_X (MIRR_V ?  x : (WIDTH - x - 1))
#define THIS_Y (MIRR_H ? (HEIGHT - y - 1) : y)

#elif (CONNECTION_ANGLE == 3 && STRIP_DIRECTION == 1)
#define _WIDTH HEIGHT
#define ROTATED_MATRIX
#define THIS_X (MIRR_V ? (HEIGHT - y - 1) : y)
#define THIS_Y (MIRR_H ?  x : (WIDTH - x - 1))

#else
#define _WIDTH WIDTH
#define THIS_X x
#define THIS_Y y
#pragma message "Wrong matrix parameters! Set to default"

#endif

#ifdef TM1637_CLOCK
#ifndef TM_CLK_PIN
  #define TM_CLK_PIN 16   // D0
#endif
#ifndef TM_DIO_PIN
  #define TM_DIO_PIN 13   // D7
#endif
#ifndef TM_BRIGHTNESS
  #define TM_BRIGHTNESS 7U //яркость дисплея, 0..7
#endif
#ifndef TM_SHOW_BANNER
  #define TM_SHOW_BANNER 0
#endif
#endif
