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

#include "main.h"
#include "effectmath.h"
#include "fontHEX.h"
#include "actions.hpp"
#include "alarm.h"

GAUGE *GAUGE::gauge = nullptr; // объект индикатора
ALARMTASK *ALARMTASK::alarmTask = nullptr; // объект будильника

LAMP::LAMP(LedFB &m) : mx(m), tmStringStepTime(DEFAULT_TEXT_SPEED), tmNewYearMessage(0), effects(&lampState, m){
  lampState.isOptPass = false; // введен ли пароль для опций
  lampState.isInitCompleted = false; // завершилась ли инициализация лампы
  lampState.isStringPrinting = false; // печатается ли прямо сейчас строка?
  lampState.isEffectsDisabledUntilText = false;
  lampState.isOffAfterText = false;
  lampState.dawnFlag = false; // флаг устанавливается будильником "рассвет"
//#ifdef MIC_EFFECTS
  lampState.isCalibrationRequest = false; // находимся ли в режиме калибровки микрофона
  lampState.micAnalyseDivider = 1; // анализ каждый раз
//#endif
  lampState.flags = 0; // сборосить все флаги состояния
  lampState.speedfactor = 1.0; // дефолтное значение
  lampState.brightness = 127;
}

void LAMP::lamp_init(const uint16_t curlimit)
{
  setcurLimit(curlimit);

  // moved into main.cpp
  //FastLED.addLeds<WS2812B, LAMP_PIN, COLOR_ORDER>(leds, num_leds).setCorrection(TypicalLEDStrip);
  //FastLED.addLeds<WS2812B, LAMP_PIN, COLOR_ORDER>(leds, num_leds).setCorrection(TypicalPixelString);
  //FastLED.addLeds<WS2812B, LAMP_PIN, COLOR_ORDER>(getUnsafeLedsArray(), num_leds);

  brightness(0, false);                          // начинаем с полностью потушеной матрицы 1-й яркости
  if (curlimit > 0){
    FastLED.setMaxPowerInVoltsAndMilliamps(5, curlimit); // установка максимального тока БП
  }
  FastLED.clearData();
  //FastLED.clear();                                            // очистка матрицы
  //FastLED.show(); // для ESP32 вызывает перезагрузку циклическую!!! Убираю, т.к. при 160Мгц вызывает бросок тока и яркости!!! Не включать и оставить как напоминание!

  // initialize fader instance
  LEDFader::getInstance()->setLamp(this);

  // GPIO's
  DynamicJsonDocument doc(512);
  if (!embuifs::deserializeFile(doc, FPSTR(TCONST_fcfg_gpio))) return;     // GPIO cfg is broken or missing
  // restore fet gpio
  fet_gpio = doc[FPSTR(TCONST_mosfet_gpio)] | static_cast<int>(GPIO_NUM_NC);
  fet_ll = doc[FPSTR(TCONST_mosfet_ll)];

  aux_gpio = doc[FPSTR(TCONST_aux_gpio)] | static_cast<int>(GPIO_NUM_NC);
  aux_ll = doc[FPSTR(TCONST_aux_ll)];
  // gpio that controls FET (for disabling matrix)
  if (fet_gpio > static_cast<int>(GPIO_NUM_NC)){
    pinMode(fet_gpio, OUTPUT);
    digitalWrite(fet_gpio, !fet_ll);
  }
  // gpio that controls AUX/Alarm pin
  if (aux_gpio > static_cast<int>(GPIO_NUM_NC)){
    pinMode(aux_gpio, OUTPUT);
    digitalWrite(aux_gpio, !aux_ll);
  }
}

void LAMP::handle()
{
#ifdef MIC_EFFECTS
  static unsigned long mic_check = 0; // = 40000; // пропускаю первые 40 секунд
  if(effects.worker && flags.isMicOn && (flags.ONflag || isMicCalibration()) && !isAlarm() && mic_check + MIC_POLLRATE < millis()){
    if(effects.worker->isMicOn() || isMicCalibration())
      micHandler();
    mic_check = millis();
  } else {
    // если микрофон не нужен, удаляем объект
    if (mw){ delete mw; mw = nullptr; }
  }
#endif

  // все что ниже, будет выполняться раз в 0.999 секундy
  static unsigned long wait_handlers;
  if (wait_handlers + 999U > millis())
      return;
  wait_handlers = millis();

  EVERY_N_SECONDS(15){
    lampState.freeHeap = ESP.getFreeHeap();
#ifdef ESP8266
    lampState.HeapFragmentation = ESP.getHeapFragmentation();
#else
    lampState.HeapFragmentation = 0;
#endif
    lampState.rssi = WiFi.RSSI();

#ifdef LAMP_DEBUG
    // fps counter
    LOG(printf_P, PSTR("Eff:%d, FPS: %u, FastLED FPS: %u\n"), effects.getEn(), avgfps, FastLED.getFPS());
#ifdef ESP8266

    LOG(printf_P, PSTR("MEM stat: %d, HF: %d, Time: %s\n"), lampState.freeHeap, lampState.HeapFragmentation, embui.timeProcessor.getFormattedShortTime().c_str());

#else
    LOG(printf_P, PSTR("MEM stat: %d, Time: %s\n"), lampState.freeHeap, embui.timeProcessor.getFormattedShortTime().c_str());
#endif
#endif
  }
#ifdef LAMP_DEBUG
  avgfps = (avgfps+fps) / 2;
#endif
  fps = 0; // сброс FPS раз в секунду


  // будильник обрабатываем раз в секунду
  //alarmWorker();

  if(lampState.isEffectsDisabledUntilText && !lampState.isStringPrinting) {
    setBrightness(0,false,false); // напечатали, можно гасить матрицу :)
    lampState.isEffectsDisabledUntilText = false;
  }

  // отложенное включение/выключение
  if(lampState.isOffAfterText && !lampState.isStringPrinting) {
    changePower(false);
    run_action(ra::off);
  }

  newYearMessageHandle();

  // обработчик событий (пока не выкину в планировщик)
  if (flags.isEventsHandled) {
    events.events_handle();
  }

  if(!lampState.isStringPrinting && !flags.ONflag && !LEDFader::getInstance()->running()){ // освобождать буфер только если не выводится строка, иначе держать его
    if(sledsbuff){
      delete sledsbuff;
      sledsbuff = nullptr;
    }
  }

}

void LAMP::effectsTick(){
  uint32_t _begin = millis();

  // проверяем, нужно ли обсчитывать новый кадр, что само по себе тупо, если уж этот метод был вызван
  if (effects.worker && (flags.ONflag || LEDFader::getInstance()->running()) && !isAlarm() && !isRGB()) {
    if(!lampState.isEffectsDisabledUntilText){  // если не выводится текст
      // if  there is a sledsbuff defined, than swap content with current mx buff, 'cause effects runner expect it to be intact from the last run
      if (sledsbuff)
        mx.swap(std::move(*sledsbuff));

      // посчитать текущий эффект (сохранить кадр в sledsbuff буфер, если был обсчет и до этого не было создано sleds буфера
      // ппц... копия будет создаваться ВСЕГДА, даже если оверлей не нужен Ж()
      if(effects.worker->run()) {
        if(!sledsbuff)
          sledsbuff = new LedFB(mx);    // create buffer clone
        else
          *sledsbuff = mx;               // copy mx buffer

      }
    }
  }
#if defined(USE_STREAMING) && defined(EXT_STREAM_BUFFER)
    if(!streambuff.empty()){
    uint8_t mi;
    for(uint16_t i=0; i<streambuff.size() && i<mx.size(); i++){
      mi = streambuff[i].r > streambuff[i].g ? streambuff[i].r : streambuff[i].g;
      mi = mi > streambuff[i].b ? mi : streambuff[i].b;
      if(mi>=5) {
        mx.at(i) = streambuff[i];
      } else if(mi && mi<5) {
        EffectMath::setLedsNscale8(i, map(mi,1,4,128,10)); // 5 градаций прозрачности, где 0 - полностью прозрачный
      }
    }
  }
#endif

  if(drawbuff){
    uint8_t mi;
    for(uint16_t i=0; i<mx.size(); i++){
      mi = drawbuff->at(i).r > drawbuff->at(i).g ? drawbuff->at(i).r : drawbuff->at(i).g;
      mi = mi > drawbuff->at(i).b ? mi : drawbuff->at(i).b;
      if(mi>=5) {
        mx.at(i) = drawbuff->at(i);
      } else if(mi) {
        mx.at(i).nscale8(map(mi,1,4,128,10)); // 5 градаций прозрачности, где 0 - полностью прозрачный
      }
    }
  }

  if(isRGB()) { // режим заливки цветом
    mx.fill(rgbColor);
  }

  if(isWarning()) {
    warningHelper(); // вывод предупреждения
  }

  if (isAlarm() || lampState.isStringPrinting) { // isWarning() || 
    doPrintStringToLamp(); // обработчик печати строки
  }

  GAUGE::GetGaugeInstance()->GaugeMix((GAUGETYPE)flags.GaugeType);

  // это жесть...
  if (isRGB() || isWarning() || isAlarm() || lampState.isEffectsDisabledUntilText || LEDFader::getInstance()->running() || (effects.worker ? effects.worker->status() : 1) || lampState.isStringPrinting) {
    // выводим 1 кадр на матрицу только если есть текст или эффект
    effectsTimer(T_FRAME_ENABLE, _begin);
  } else if(isLampOn()) {
    // иначе перезапускаем этот же метод бесконечно
    effectsTimer(T_ENABLE);
  }
}

/*
 * вывод готового кадра на матрицу,
 * и перезапуск эффект-процессора
 */
void LAMP::frameShow(const uint32_t ticktime){
  if ( !LEDFader::getInstance()->running() && !isLampOn() && !isAlarm() ) return;

  FastLED.show();

  // откладываем пересчет эффекта на время для желаемого FPS, либо
  // на минимальный интервал в следующем loop()
  int32_t delay = (ticktime + EFFECTS_RUN_TIMER) - millis();
  if (delay < LED_SHOW_DELAY) delay = LED_SHOW_DELAY;

  effectsTimer(T_ENABLE, delay);
  ++fps;
}

void LAMP::changePower() {changePower(!flags.ONflag);}

void LAMP::changePower(bool flag) // флаг включения/выключения меняем через один метод
{
  ALARMTASK::stopAlarm();            // любая активность в интерфейсе - отключаем будильник
  if (flag == flags.ONflag) return;  // пропускаем холостые вызовы
  LOG(print, F("Lamp powering ")); LOG(println, flag ? F("On"): F("Off"));
  flags.ONflag = flag;

  if(mode == LAMPMODE::MODE_OTA)
    mode = LAMPMODE::MODE_NORMAL;

  if (flag){
#ifdef USE_STREAMING
    if (flags.isStream)
      Led_Stream::newStreamObj((STREAM_TYPE)embui.param(FPSTR(TCONST_stream_type)).toInt());
    if(!flags.isDirect || !flags.isStream)
#endif
    effectsTimer(T_ENABLE);
    if(mode == LAMPMODE::MODE_DEMO)
      demoTimer(T_ENABLE);
  } else  {
#ifdef USE_STREAMING
    Led_Stream::clearStreamObj();
#endif
    if(flags.isFaderON && !lampState.isOffAfterText){
      LEDFader::getInstance()->fadelight(0, FADE_TIME, std::bind(&LAMP::effectsTimer, this, SCHEDULER::T_DISABLE, 0));  // гасим эффект-процессор
    }
    else {
      brightness(0);
      effectsTimer(SCHEDULER::T_DISABLE);
    }
    lampState.isOffAfterText = false;
    lampState.isStringPrinting = false;
    demoTimer(T_DISABLE);     // гасим Демо-таймер
  }

  // установка сигнала в пин, управляющий MOSFET транзистором, соответственно состоянию вкл/выкл матрицы
  if (fet_gpio > static_cast<int>(GPIO_NUM_NC)){
    Task *_t = new Task(flags.isFaderON && !flags.ONflag ? 5*TASK_SECOND : 50, TASK_ONCE, // для выключения - отложенное переключение мосфета 5 секунд
      [this](){ digitalWrite(fet_gpio, (flags.ONflag ? fet_ll : !fet_ll)); },
      &ts, false, nullptr, nullptr, true);
    _t->enableDelayed();
  }

#ifdef DS18B20
    // восстанавливаем значение тока после включения. Так как значение 0 не работает в ограничителе тока по перегреву, 
    // то если ограничение тока установлено в 0, устанвливаем вместо него рассчетный максимум в 15.36А на 256 диодов (бред конечно, но нужно же хоть какое-то значение больше 0).
    setcurLimit(embui.param(FPSTR(TCONST_CLmt)).toInt() == 0 ? (mx.size() * 60) : embui.param(FPSTR(TCONST_CLmt)).toInt());
#endif
    FastLED.setMaxPowerInVoltsAndMilliamps(5, curLimit); // установка максимального тока БП, более чем актуально))). Проверил, без этого куска - ограничение по току не работает :)
}

#ifdef MP3PLAYER
void LAMP::playEffect(bool isPlayName, EFFSWITCH action){
  if(mp3!=nullptr && mp3->isOn() && effects.getEn()>0 && (flags.playEffect || ((isLampOn() || millis()>5000) && flags.playMP3 && action!=EFFSWITCH::SW_NEXT_DEMO && action!=EFFSWITCH::SW_RND))){
    LOG(printf_P, PSTR("playEffect soundfile:%s, effect:%d, delayed:%d\n"), effects.getSoundfile().c_str(), effects.getEn(), (flags.playName && !flags.playMP3));
    if(!flags.playMP3 || (flags.playEffect && action!=EFFSWITCH::SW_NEXT_DEMO && action!=EFFSWITCH::SW_RND)) // для mp3-плеера есть отдельное управление
      mp3->playEffect(effects.getEn(), effects.getSoundfile(), (isPlayName && mp3!=nullptr && mp3->isOn() && !flags.playMP3)); // влияние на отложенное воспроизведение, но не для MP3-плеера
  } else {
    mp3->setCurEffect(effects.getEn());
  }
}
#endif  // MP3PLAYER

void LAMP::startRGB(CRGB &val){
  rgbColor = val;
  storedMode = ((mode == LAMPMODE::MODE_RGBLAMP) ? storedMode: mode);
  mode = LAMPMODE::MODE_RGBLAMP;
  demoTimer(T_DISABLE);     // гасим Демо-таймер
  effectsTimer(T_ENABLE);
}

void LAMP::stopRGB(){
  if (mode != LAMPMODE::MODE_RGBLAMP) return;

  setBrightness(getLampBrightness(), false, false);
  mode = (storedMode != LAMPMODE::MODE_RGBLAMP ? storedMode : LAMPMODE::MODE_NORMAL); // возвращаем предыдущий режим
  if(mode==LAMPMODE::MODE_DEMO)
    demoTimer(T_ENABLE);     // вернуть демо-таймер
  if (flags.ONflag)
    effectsTimer(T_ENABLE);
}


/*
 * запускаем режим "ДЕМО"
 */
void LAMP::startDemoMode(uint8_t tmout)
{
  LOG(println,F("Demo mode"));
  if(!isLampOn()) run_action(ra::on);       // "включаем" лампу
  if(mode == LAMPMODE::MODE_DEMO) return;   // уже и так в "демо" режиме, выходим
  
  storedEffect = ((static_cast<EFF_ENUM>(effects.getEn()%256) == EFF_ENUM::EFF_WHITE_COLOR) ? storedEffect : effects.getEn()); // сохраняем предыдущий эффект, если только это не белая лампа
  mode = LAMPMODE::MODE_DEMO;
  demoTimer(T_ENABLE, tmout);
  sendString(String(F("- Demo ON -")).c_str(), CRGB::Green, false);
}

void LAMP::storeEffect()
{
  storedEffect = ((static_cast<EFF_ENUM>(effects.getEn()%256) == EFF_ENUM::EFF_WHITE_COLOR) ? storedEffect : effects.getEn()); // сохраняем предыдущий эффект, если только это не белая лампа
  storedBright = getLampBrightness();
  lampState.isMicOn = false;
  LOG(printf_P, PSTR("storeEffect() %d,%d\n"),storedEffect,storedBright);
}

void LAMP::restoreStored()
{
  LOG(printf_P, PSTR("restoreStored() %d,%d\n"),storedEffect,storedBright);
  if(storedBright)
    setLampBrightness(storedBright);
  lampState.isMicOn = flags.isMicOn;
  if (static_cast<EFF_ENUM>(storedEffect) != EFF_NONE) {    // ничего не должно происходить, включаемся на текущем :), текущий всегда определен...
    Task *_t = new Task(3 * TASK_SECOND, TASK_ONCE, [this](){ run_action( ra::eff_switch, storedEffect); }, &ts, false, nullptr, nullptr, true);
    _t->enableDelayed();
  } else if(static_cast<EFF_ENUM>(effects.getEn()%256) == EFF_NONE) { // если по каким-то причинам текущий пустой, то выбираем рандомный
    Task *_t = new Task(3 * TASK_SECOND, TASK_ONCE, [this](){ run_action(ra::eff_rnd); }, &ts, false, nullptr, nullptr, true);
    _t->enableDelayed();
  }
}

void LAMP::startNormalMode(bool forceOff)
{
  LOG(println,F("Normal mode"));
  if(forceOff)
    flags.ONflag=false;
  mode = LAMPMODE::MODE_NORMAL;
  demoTimer(T_DISABLE);
  restoreStored();
}

typedef enum {FIRSTSYMB=1,LASTSYMB=2} SYMBPOS;

bool LAMP::fillStringManual(const char* text,  const CRGB &letterColor, bool stopText, bool isInverse, int32_t pos, int8_t letSpace, int8_t txtOffset, int8_t letWidth, int8_t letHeight)
{
  static int32_t offset = mx.cfg.vmirror() ? 0 : mx.cfg.w();
  uint8_t bcount = 0;

  if(pos)
    offset = (mx.cfg.vmirror() ? 0 + pos : mx.cfg.w() - pos);

  if (!text || !strlen(text))
  {
    offset = (mx.cfg.vmirror() ? 0 : mx.cfg.w());
    return true;
  }

  uint16_t i = 0, j = 0;
  uint8_t flSymb = SYMBPOS::FIRSTSYMB; // маркер первого символа строки
  while (text[i] != '\0')
  {
    if(text[i+1] == '\0')
      flSymb|=SYMBPOS::LASTSYMB; // маркер последнего символа строки
    if ((uint8_t)text[i] > 191)  // работаем с UTF8 после префикса
    {
      bcount = (uint8_t)text[i]; // кол-во октетов для UTF-8
      i++;
    }
    else
    {
      if(!mx.cfg.vmirror())
        drawLetter(bcount, text[i], offset + (int16_t)j * (letWidth + letSpace), letterColor, letSpace, txtOffset, isInverse, letWidth, letHeight, flSymb);
      else
        drawLetter(bcount, text[i], offset - (int16_t)j * (letWidth + letSpace), letterColor, letSpace, txtOffset, isInverse, letWidth, letHeight, flSymb);
      i++;
      j++;
      bcount = 0;
      flSymb &= (0xFF^SYMBPOS::FIRSTSYMB); // сбросить маркер первого символа строки
    }
  }

  if(!stopText)
    (mx.cfg.vmirror() ? offset++ : offset--);
  if ((!mx.cfg.vmirror() && offset < (int32_t)(-j * (letWidth + letSpace))) || (mx.cfg.vmirror() && offset > (int32_t)(j * (letWidth + letSpace))+(signed)mx.cfg.w()))       // строка убежала
  {
    offset = (mx.cfg.vmirror() ? 0 : mx.cfg.w());
    return true;
  }
  if(pos) // если задана позиция, то считаем что уже отобразили
  {
    offset = (mx.cfg.vmirror() ? 0 : mx.cfg.w());
    return true;
  }

  return false;
}

void LAMP::drawLetter(uint8_t bcount, uint16_t letter, int16_t offset,  const CRGB &letterColor, uint8_t letSpace, int8_t txtOffset, bool isInverse, int8_t letWidth, int8_t letHeight, uint8_t flSymb)
{
  int16_t start_pos = 0, finish_pos = letWidth + letSpace;

  if (offset < (int16_t)-letWidth || offset > (int16_t)mx.cfg.w())
  {
    return;
  }
  if (offset < 0)
  {
    start_pos = (uint16_t)-offset;
  }
  if (offset > (int16_t)(mx.cfg.w() - letWidth))
  {
    finish_pos = (uint16_t)(mx.cfg.w() - offset);
  }

  if(flSymb){
      if(flSymb&SYMBPOS::FIRSTSYMB){ // битовое &
        start_pos--; // c 0 для самого первого символа
      }
      if(flSymb&SYMBPOS::LASTSYMB && !letSpace){ // битовое &
        finish_pos++; // доп. ряд погашенных символов для последнего символа
      }
  }

  //LOG(printf_P, PSTR("%d %d\n"), start_pos, finish_pos);

  for (int16_t i = start_pos; i < finish_pos; i++)
  {
    uint8_t thisByte;

    if((i<0) || (finish_pos - i <= letSpace) || ((letWidth - 1 - i)<0))
      thisByte = 0x00;
    else
    {
      thisByte = getFont(bcount, letter, i);
    }

    for (uint16_t j = 0; j < letHeight + 1; j++) // +1 доп. пиксель сверху
    {
      bool thisBit = thisByte & (1 << (letHeight - 1 - j));

      // рисуем столбец (i - горизонтальная позиция, j - вертикальная)
      if (offset + i >= 0 && offset + i < (int)mx.cfg.w() && txtOffset + j >= 0 && txtOffset + j < (int)mx.cfg.h()) {
        if (thisBit) {
          if(!isInverse)
            mx.pixel(offset + i, txtOffset + j) = letterColor;
          else
            //EffectMath::setLedsfadeToBlackBy(getPixelNumber(offset + i, txtOffset + j), (isWarning() && lampState.warnType==2) ? 0 : (isWarning() && lampState.warnType==1) ? 255 : getBFade());
            mx.pixel(offset + i, txtOffset + j).fadeToBlackBy((isWarning() && lampState.warnType==2) ? 0 : (isWarning() && lampState.warnType==1) ? 255 : getBFade());
        } else {
          if(isInverse)
            mx.pixel(offset + i, txtOffset + j) = letterColor;
          else
            //EffectMath::setLedsfadeToBlackBy(getPixelNumber(offset + i, txtOffset + j), (isWarning() && lampState.warnType==2) ? 0 : (isWarning() && lampState.warnType==1) ? 255 : getBFade());
            mx.pixel(offset + i, txtOffset + j).fadeToBlackBy((isWarning() && lampState.warnType==2) ? 0 : (isWarning() && lampState.warnType==1) ? 255 : getBFade());
        }
      }
    }
  }
}

uint8_t LAMP::getFont(uint8_t bcount, uint8_t asciiCode, uint8_t row)       // интерпретатор кода символа в массиве fontHEX (для Arduino IDE 1.8.* и выше)
{
  asciiCode = asciiCode - '0' + 16;                         // перевод код символа из таблицы ASCII в номер согласно нумерации массива

  // if (asciiCode <= 90)                                      // печатаемые символы и английские буквы
  // {
  //   return pgm_read_byte(&fontHEX[asciiCode][row]);
  // }
  // else if (asciiCode >= 112 && asciiCode <= 159)
  // {
  //   return pgm_read_byte(&fontHEX[asciiCode - 17][row]);
  // }
  // else if (asciiCode >= 96 && asciiCode <= 111)
  // {
  //   return pgm_read_byte(&fontHEX[asciiCode + 47][row]);
  // }

  if (asciiCode <= 94) {
    return pgm_read_byte(&(fontHEX[asciiCode][row]));     // для английских букв и символов
  } else if ((bcount == 209) && asciiCode == 116) {         // є
    return pgm_read_byte(&(fontHEX[162][row])); 
  } else if ((bcount == 209) && asciiCode == 118) {        // і
    return pgm_read_byte(&(fontHEX[73][row])); 
  } else if ((bcount == 209) && asciiCode == 119) {         // ї
    return pgm_read_byte(&(fontHEX[163][row])); 
  } else if ((bcount == 208) && asciiCode == 100) {        // Є
    return pgm_read_byte(&(fontHEX[160][row])); 
  } else if ((bcount == 208) && asciiCode == 102) {         // І
    return pgm_read_byte(&(fontHEX[41][row])); 
  } else if ((bcount == 208) && asciiCode == 103) {        // Ї
    return pgm_read_byte(&(fontHEX[161][row])); 
  } else if ((bcount == 208) && asciiCode == 97) {         // Ё
    return pgm_read_byte(&(fontHEX[100][row])); 
  } else if ((bcount == 209) && asciiCode == 113) {        // ё
    return pgm_read_byte(&(fontHEX[132][row])); 
  } else if ((bcount == 208 || bcount == 209) && asciiCode >= 112 && asciiCode <= 159) {      // русские символы
    return pgm_read_byte(&(fontHEX[asciiCode - 17][row]));
  } else if ((bcount == 208 || bcount == 209) && asciiCode >= 96 && asciiCode <= 111) {
    return pgm_read_byte(&(fontHEX[asciiCode + 47][row]));
  } else if ((bcount == 194) && asciiCode == 144) {                                          // Знак градуса '°'
    return pgm_read_byte(&(fontHEX[159][row]));
  }

  return 0;
}

void LAMP::sendString(const char* text){
  String tmpStr = embui.param(FPSTR(TCONST_txtColor));
  tmpStr.replace(F("#"),F("0x"));
  CRGB::HTMLColorCode color = (CRGB::HTMLColorCode)strtol(tmpStr.c_str(), NULL, 0);
  sendString(text, color);
}

void LAMP::sendString(const char* text, CRGB letterColor, bool forcePrint, bool clearQueue){
  if (!isLampOn() && forcePrint){
      disableEffectsUntilText(); // будем выводить текст, при выкюченной матрице
      setOffAfterText();
      changePower(true);
      setBrightness(OFF_BRIGHTNESS, false, false); // выводить будем минимальной яркостью в OFF_BRIGHTNESS пункта
      sendStringToLamp(text, letterColor, forcePrint, clearQueue);
  } else {
      sendStringToLamp(text, letterColor, forcePrint, clearQueue);
  }
}

String &LAMP::prepareText(String &source){
  source.replace(F("%TM"), embui.timeProcessor.getFormattedShortTime());
  source.replace(F("%IP"), WiFi.localIP().toString());
  source.replace(F("%EN"), effects.getEffectName());
  const tm *tm = localtime(embui.timeProcessor.now());
  char buffer[11]; //"xx.xx.xxxx"
  sprintf_P(buffer,PSTR("%02d.%02d.%04d"),tm->tm_mday,tm->tm_mon+1,tm->tm_year+ TM_BASE_YEAR);
  source.replace(F("%DT"), buffer);
#ifdef LAMP_DEBUG  
  if(!source.isEmpty() && effects.getCurrent()!=EFF_ENUM::EFF_TIME && !isWarning()) // спам эффекта часы и предупреждений убираем костыльным способом :)
    LOG(println, source.c_str()); // вывести в лог строку, которая после преобразований получилась
#endif
  return source;  
}

void LAMP::sendStringToLampDirect(const char* text, CRGB letterColor, bool forcePrint, bool clearQueue, int8_t textOffset, int16_t fixedPos)
{
    String storage = text;
    prepareText(storage);
    doPrintStringToLamp(storage.c_str(), letterColor, textOffset, fixedPos); // отправляем
}

void LAMP::sendStringToLamp(const char* text, CRGB letterColor, bool forcePrint, bool clearQueue, int8_t textOffset, int16_t fixedPos)
{
  if((!flags.ONflag && !forcePrint) || (isAlarm() && !forcePrint)) return; // если выключена, или если будильник, но не задан принудительный вывод - то на выход
  if(textOffset==-128) textOffset=this->txtOffset;

  if(text==nullptr){ // текст пустой
    if(!lampState.isStringPrinting){ // ничего сейчас не печатается
      if(!docArrMessages){ // массив пустой
        return; // на выход
      }
      else { // есть что печатать
        JsonArray arr = (*docArrMessages).as<JsonArray>(); // используем имеющийся
        JsonObject var=arr[0]; // извлекаем очередной
        if(!var.isNull()){
          String storage = var[F("s")];
          prepareText(storage);
          doPrintStringToLamp(storage.c_str(), (var[F("c")].as<unsigned long>()), (var[F("o")].as<int>()), (var[F("f")].as<int>())); // отправляем
#ifdef MP3PLAYER
          String tmpStr = var[F("s")];
          if(mp3!=nullptr && ((mp3->isOn() && isLampOn()) || isAlarm()) && flags.playTime && tmpStr.indexOf(String(F("%TM")))>=0)
            if(FastLED.getBrightness()!=OFF_BRIGHTNESS)
              mp3->playTime(embui.timeProcessor.getHours(), embui.timeProcessor.getMinutes(), (TIME_SOUND_TYPE)flags.playTime);
#endif
        }
        if(arr.size()>0)
          arr.remove(0); // удаляем отправленный
        if(!arr.size()){ // очередь опустела, освобождаем массив
          delete docArrMessages;
          docArrMessages = nullptr;
        }
      }
    } else {
        // текст на входе пустой, идет печать
        return; // на выход
    }
  } else { // текст не пустой
    if(clearQueue){
      LOG(println, F("Clear message queue"));
      if(docArrMessages){ // очистить очередь, освободить память
          delete docArrMessages;
          docArrMessages = nullptr;
      }
      lampState.isStringPrinting = false; // сбросить текущий вывод строки
    }

    if(!lampState.isStringPrinting){ // ничего сейчас не печатается
      String storage = text;
      prepareText(storage);
      doPrintStringToLamp(storage.c_str(), letterColor, textOffset, fixedPos); // отправляем
#ifdef MP3PLAYER
      String tmpStr = text;
      if(mp3!=nullptr && ((mp3->isOn() && isLampOn()) || isAlarm()) && flags.playTime && tmpStr.indexOf(String(F("%TM")))>=0)
        if(FastLED.getBrightness()!=OFF_BRIGHTNESS)
          mp3->playTime(embui.timeProcessor.getHours(), embui.timeProcessor.getMinutes(), (TIME_SOUND_TYPE)flags.playTime);
#endif
    } else { // идет печать, помещаем в очередь
      JsonArray arr; // добавляем в очередь

      if(docArrMessages){
        arr = (*docArrMessages).as<JsonArray>(); // используем имеющийся
      } else {
        docArrMessages = new DynamicJsonDocument(512);
        arr = (*docArrMessages).to<JsonArray>(); // создаем новый
      }

      for (size_t i = 0; i < arr.size(); i++)
      {
        if((arr[i])[F("s")]==text
          && (arr[i])[F("c")]==((unsigned long)letterColor.r<<16)+((unsigned long)letterColor.g<<8)+(unsigned long)letterColor.b
          && (arr[i])[F("o")]==textOffset
          && (arr[i])[F("f")]==fixedPos
        ){
          LOG(println, F("Duplicate string skipped"));
          //LOG(println, (*docArrMessages).as<String>());
          return;
        }
      }

      JsonObject var = arr.createNestedObject();
      var[F("s")]=text;
      var[F("c")]=((unsigned long)letterColor.r<<16)+((unsigned long)letterColor.g<<8)+(unsigned long)letterColor.b;
      var[F("o")]=textOffset;
      var[F("f")]=fixedPos;

      String tmp; // Тут шаманство, чтобы не ломало JSON
      serializeJson((*docArrMessages), tmp);
      deserializeJson((*docArrMessages), tmp);

      LOG(print, F("Array: "));
      LOG(println, (*docArrMessages).as<String>());
    }
  }
}

void LAMP::doPrintStringToLamp(const char* text,  CRGB letterColor, const int8_t textOffset, const int16_t fixedPos)
{
  static String toPrint;
  static CRGB _letterColor;

  if(!lampState.isStringPrinting){
    toPrint.clear();
    fillStringManual(nullptr, CRGB::Black);
  }

  lampState.isStringPrinting = true;
  int8_t offs=(textOffset==-128?txtOffset:textOffset);

  if(text!=nullptr && text[0]!='\0'){
    toPrint.concat(text);
    _letterColor = letterColor;
  }

  if(toPrint.length()==0) {
    lampState.isStringPrinting = false;
    return; // нечего печатать
  } else {
    lampState.isStringPrinting = true;
  }

  if(tmStringStepTime.isReadyManual()){
    if(!fillStringManual(toPrint.c_str(), _letterColor, false, isAlarm() || (isWarning() && lampState.warnType<2), fixedPos, (fixedPos? 0 : LET_SPACE), offs) && (!isWarning() || (isWarning() && fixedPos))){ // смещаем
      tmStringStepTime.reset();
    }
    else {
      lampState.isStringPrinting = false;
      toPrint.clear(); // все напечатали
      sendStringToLamp(); // получаем новую порцию
    }
  } else {
    if((!isWarning() || (isWarning() && fixedPos)))
      fillStringManual(toPrint.c_str(), _letterColor, true, isAlarm() || (isWarning() && lampState.warnType<2), fixedPos, (fixedPos? 0 : LET_SPACE), offs);
  }
}

void LAMP::newYearMessageHandle()
{
  if(!tmNewYearMessage.isReady())
    return;

    char strMessage[256]; // буфер
    time_t calc = NEWYEAR_UNIXDATETIME - embui.timeProcessor.getUnixTime();

    if(calc<0) {
      sprintf_P(strMessage, NY_MDG_STRING2, localtime(embui.timeProcessor.now())->tm_year+ TM_BASE_YEAR);
    } else if(calc<300){
      sprintf_P(strMessage, NY_MDG_STRING1, (int)calc, String(FPSTR(TINTF_0C1)).c_str());
    } else if(calc/60<60){
      uint16_t calcT=calc/(60*60); // минуты
      uint8_t calcN=calcT%10; // остаток от деления на 10
      String str;
      if(calcN>=2 && calcN<=4) {
        str = FPSTR(TINTF_0CC); // минуты
      } else if(calcN==1) {
        str = FPSTR(TINTF_0CD); // минута
      } else {
        str = FPSTR(TINTF_0C2); // минут
      }
      sprintf_P(strMessage, NY_MDG_STRING1, calcT, str.c_str());
    } else if(calc/(60*60)<60){
	    uint16_t calcT=calc/(60*60); // часы
      uint8_t calcN=calcT%10; // остаток от деления на 10
      String str;
      if(calcN>=2 && calcN<=4) {
        str = FPSTR(TINTF_0C7); // часа
      } else if(calcN==1) {
        str = FPSTR(TINTF_0C8); // час
      } else {
        str = FPSTR(TINTF_0C3); // часов
      }
      sprintf_P(strMessage, NY_MDG_STRING1, calcT, str.c_str());
    } else {
      uint16_t calcT=calc/(60*60*24); // дни
      uint8_t calcN=calcT%10; // остаток от деления на 10
      String str;
      if(calcT>=11 && calcT<=20)
        str = FPSTR(TINTF_0C4);
      else if(calcN>=2 && calcN<=4)
        str = FPSTR(TINTF_0C5);
      else if(calc!=11 && calcN==1)
        str = FPSTR(TINTF_0C6);
      else
        str = FPSTR(TINTF_0C4);
      sprintf_P(strMessage, NY_MDG_STRING1, calcT, str.c_str());
    }

    LOG(printf_P, PSTR("Prepared message: %s\n"), strMessage);
    sendStringToLamp(strMessage, LETTER_COLOR);
}

// при вызове - вывозит на лампу текущее время
void LAMP::showTimeOnScreen(const char *value, bool force)
{
  DynamicJsonDocument doc(512);
  String buf(value);
  buf.replace("'","\"");
  deserializeJson(doc,buf);
  bool isShowOff = doc[FPSTR(TCONST_isShowOff)];
  bool isPlayTime = doc[FPSTR(TCONST_isPlayTime)];

  const tm* t = localtime(embui.timeProcessor.now());
  if(t->tm_sec && !force)
    return;

  LOG(printf_P, PSTR("showTime: %02d:%02d, evenWhenOff=%d, PlayTime=%d\n"), t->tm_hour,t->tm_min, isShowOff, isPlayTime);

  time_t tm = t->tm_hour * 60 + t->tm_min;
  String time = isPlayTime ? String(F("%TM")) : embui.timeProcessor.getFormattedShortTime();

  CRGB color;
  if(!(tm%60)){
    color = CRGB::Red;
  } else if(!(tm%30)){
    color = CRGB::Green;
  } else {
    color =  CRGB::Blue;
  }
#ifdef MP3PLAYER
  if(!isLampOn() && isPlayTime && mp3){ // произносить время
    mp3->setIsOn(true, false);
    mp3->playTime(embui.timeProcessor.getHours(), embui.timeProcessor.getMinutes(), (TIME_SOUND_TYPE)flags.playTime);
  }
#endif
  sendString(time.c_str(), color, isShowOff);  // выводить ли время при выключенной лампе
}

#ifdef MIC_EFFECTS
void LAMP::micHandler()
{
  static uint8_t counter=0;
  if(effects.getEn()==EFF_ENUM::EFF_NONE)
    return;
  if(!mw && !lampState.isCalibrationRequest && lampState.micAnalyseDivider){ // обычный режим
    //mw = new(std::nothrow) MicWorker(lampState.mic_scale,lampState.mic_noise,!counter);
    mw = new(std::nothrow) MicWorker(lampState.mic_scale,lampState.mic_noise,true);   // создаем полноценный объект и держим в памяти

    if(!mw) {
      return; // не удалось выделить память, на выход
    }
    
    lampState.samp_freq = mw->process(lampState.noise_reduce); // возвращаемое значение - частота семплирования
    lampState.last_min_peak = mw->getMinPeak();
    lampState.last_max_peak = mw->getMaxPeak();
    lampState.cur_val = mw->getCurVal();

    if(!counter) // раз на N измерений берем частоту, т.к. это требует обсчетов
      lampState.last_freq = mw->analyse(); // возвращаемое значение - частота главной гармоники
    if(lampState.micAnalyseDivider)
      counter = (counter+1)%(0x01<<(lampState.micAnalyseDivider-1)); // как часто выполнять анализ
    else
      counter = 1; // при micAnalyseDivider == 0 - отключено

    // EVERY_N_SECONDS(1){
    //   LOG(println, counter);
    // }

    //LOG(println, last_freq);
    //mw->debug();

    //delete mw;    // не удаляем, пока пользуемся
    //mw = nullptr;
  } else if(lampState.isCalibrationRequest) {
    if(!mw){ // калибровка начало
      mw = new(std::nothrow) MicWorker();
      if(!mw) return;   // was not able to alloc mem
    }
    mw->calibrate();
    if(!mw->isCaliblation()){ // калибровка конец
      lampState.mic_noise = mw->getNoise();
      lampState.mic_scale = mw->getScale();
      lampState.isCalibrationRequest = false; // завершили
      delete mw;
      mw = nullptr;

      DynamicJsonDocument doc(512);
      JsonObject obj = doc.to<JsonObject>();

      //remote_action(RA::RA_MIC, NULL);
      CALL_INTF_OBJ(show_settings_mic);
    }
  }
}

void LAMP::setMicOnOff(bool val) {
    flags.isMicOn = val;
    lampState.isMicOn = val;
    if(effects.getEn()==EFF_NONE || !effects.worker) return;

    unsigned foundc7 = 0;
    LList<std::shared_ptr<UIControl>>&controls = effects.getControls();
    if(val){
        for(unsigned i=3; i<controls.size(); i++) {
            if(controls[i]->getId()==7 && controls[i]->getName().startsWith(FPSTR(TINTF_020))==1){
                effects.worker->setDynCtrl(controls[i].get());
                return;
            } else if(controls[i]->getId()==7) {
                foundc7 = i;
            }
        }
    }

    UIControl ctrl(7,(CONTROL_TYPE)18,String(FPSTR(TINTF_020)), val ? "1" : "0", "0", "1", "1");
    effects.worker->setDynCtrl(&ctrl);
    if(foundc7){ // был найден 7 контрол, но не микрофон
        effects.worker->setDynCtrl(controls[foundc7].get());
    }
}
#endif  // MIC_EFFECTS

/*
 * Change global brightness with or without fade effect
 * fade applied in non-blocking way
 * FastLED dim8 function applied internaly for natural brightness controll
 * @param uint8_t _brt - target brigtness level 0-255
 * @param bool fade - use fade effect on brightness change
 */
void LAMP::setBrightness(const uint8_t _brt, const bool fade, const bool natural){
    LOG(printf_P, PSTR("setBrightness(): %u\n"), _brt);
    if (fade) {
        LEDFader::getInstance()->fadelight(_brt);
    } else {
        brightness(_brt, natural);
    }
}

/*
 * Get current brightness
 * FastLED brighten8 function applied internaly for natural brightness compensation
 * @param bool natural - return compensated or absolute brightness
 */
uint8_t LAMP::getBrightness(const bool natural){
    return (natural ? brighten8_raw(FastLED.getBrightness()) : FastLED.getBrightness());
}


/*
 * Set global brightness
 * @param bool natural
 */
void LAMP::brightness(const uint8_t _brt, bool natural){
    uint8_t _cur = natural ? brighten8_video(FastLED.getBrightness()) : FastLED.getBrightness();
    if ( _cur == _brt) return;

    if (_brt) {
      FastLED.setBrightness(natural ? dim8_video(_brt) : _brt);
    } else {
      FastLED.setBrightness(1); // 8266 may crash if brightness is set to zero, need triage
      FastLED.clear();
    }
    FastLED.show();
}

uint8_t LAMP::lampBrightnesspct(uint8_t brt){
  if (brt >=100)
    setLampBrightness(255);
  else
    setLampBrightness(brt * 255 / 100);
  return brt;
}

/*
 * переключатель эффектов для других методов,
 * может использовать фейдер, выбирать случайный эффект для демо
 * @param EFFSWITCH action - вид переключения (пред, след, случ.)
 * @param fade - переключаться через фейдер или сразу
 */
void LAMP::switcheffect(EFFSWITCH action, bool fade, uint16_t effnb, bool skip) {
#ifdef MIC_EFFECTS
    lampState.setMicAnalyseDivider(1); // восстановить делитель, при любой активности (поскольку эффекты могут его перенастраивать под себя)
#endif

#ifdef ENCODER
 exitSettings();
#endif

  if (!skip) {
    if(isRGB()){ // выход из этого режима, при любой попытки перехода по эффектам
      stopRGB();
    }
    uint16_t next_eff_num = effnb;
    switch (action) {
    case EFFSWITCH::SW_NEXT :
        next_eff_num = effects.getNext();
        break;
    case EFFSWITCH::SW_NEXT_DEMO :
        next_eff_num = effects.getByCnt(1);
        break;
    case EFFSWITCH::SW_PREV :
        next_eff_num = effects.getPrev();
        break;
    case EFFSWITCH::SW_SPECIFIC :
        next_eff_num = effnb;
        break;
    case EFFSWITCH::SW_RND :
        next_eff_num = effects.getByCnt(random(0, effects.getEffectsListSize()));
        break;
    default:
        return;
    }
    LOG(printf_P, PSTR("switcheffect() act=%d, fade=%d, effnb=%d\n"), action, fade, next_eff_num);
    // тухнем "вниз" только на включенной лампе
    if (fade && flags.ONflag) {
      effects.preloadEffCtrls(next_eff_num);    // preload controls for next effect
      // запускаем фейдер и уходим на второй круг переключения
      LEDFader::getInstance()->fadelight( myLamp.getLampBrightness() < 2*FADE_MINCHANGEBRT ? 0 : FADE_MINCHANGEBRT, FADE_TIME, std::bind(&LAMP::switcheffect, this, action, fade, next_eff_num, true));
      return;
    } else {
      // do direct switch to effect
      effects.directMoveBy(next_eff_num);
    }
  } else {
    LOG(printf_P, PSTR("switcheffect() postfade act=%d, fade=%d, effnb=%d\n"), action, fade, effnb ? effnb : effects.getSelected());
  }

  if(flags.isEffClearing || !effects.getEn()){ // для EFF_NONE или для случая когда включена опция - чистим матрицу
    mx.clear();
    FastLED.show();
  }

  // move to 'selected' only if lamp is On and fader is in effect (i.e. it's a second call after fade),
  // otherwise it's been switched already
  if (fade && flags.ONflag)
    effects.moveSelected();

  bool isShowName = (mode==LAMPMODE::MODE_DEMO && flags.showName);
#ifdef MP3PLAYER
  bool isPlayName = (isShowName && flags.playName && !flags.playMP3 && effects.getEn()>0);
#endif

  // show effects's name on screen and play name over speaker (if set)
  if(isShowName){
    sendStringToLamp(String(F("%EN")).c_str(), CRGB::Green);
#ifdef MP3PLAYER
    if(isPlayName && mp3!=nullptr && mp3->isOn()) // воспроизведение 
      mp3->playName(effects.getEn());
#endif
  }

#ifdef MP3PLAYER
  playEffect(isPlayName, action); // воспроизведение звука, с проверкой текущего состояния
#endif

  bool natural = true;

  if(effects.worker && flags.ONflag && !lampState.isEffectsDisabledUntilText){
    if(!sledsbuff){ // todo: WHY we need this clone here???
      sledsbuff = new LedFB(mx);  // clone existing frambuffer
    }
  }
  setBrightness(getLampBrightness(), fade, natural);
  LOG(println, F("eof switcheffect"));
}

/*
 * включает/выключает режим "демо"
 * @param SCHEDULER enable/disable/reset - вкл/выкл/сброс
 */
void LAMP::demoTimer(SCHEDULER action, uint8_t tmout){
  switch (action)
  {
  case SCHEDULER::T_DISABLE :
    delete demoTask;
    demoTask = nullptr;
    break;
  case SCHEDULER::T_ENABLE :
    if (!tmout && demoTask){
      delete demoTask;
      demoTask = nullptr;
      return;
    }
    if(demoTask){
      demoTask->setInterval(tmout * TASK_SECOND);
      return;
    }
    demoTask = new Task(tmout * TASK_SECOND, TASK_FOREVER, [](){run_action(ra::demo_next);}, &ts, false);    
    demoTask->enableDelayed();
    break;
  case SCHEDULER::T_RESET :
    if (isAlarm())
      ALARMTASK::stopAlarm(); // тут же сбросим и будильник
    if (mode==LAMPMODE::MODE_DEMO && demoTask)
      demoTask->restartDelayed();
    break;
  default:
    return;
  }
}

/*
 * включает/выключает таймер обработки эффектов
 * @param SCHEDULER enable/disable/reset - вкл/выкл/сброс
 */
void LAMP::effectsTimer(SCHEDULER action, uint32_t _begin) {
//  LOG.printf_P(PSTR("effectsTimer: %u\n"), action);
  switch (action)
  {
  case SCHEDULER::T_DISABLE :
    if(effectsTask){
      delete effectsTask;
      effectsTask = nullptr;
    }
    break;
  case SCHEDULER::T_ENABLE :
    if(!effectsTask){
      effectsTask = new Task(_begin?_begin:EFFECTS_RUN_TIMER, TASK_ONCE, [this](){effectsTick();}, &ts, false);
    } else {
      effectsTask->set(_begin?_begin:EFFECTS_RUN_TIMER, TASK_ONCE, [this](){effectsTick();});
    }
    effectsTask->restartDelayed();
    break;
  case SCHEDULER::T_FRAME_ENABLE :
    if(!effectsTask){
      effectsTask = new Task(LED_SHOW_DELAY, TASK_ONCE, [this, _begin](){frameShow(_begin);}, &ts, false);
    } else {
      effectsTask->set(LED_SHOW_DELAY, TASK_ONCE, [this, _begin](){frameShow(_begin);});
    }
    effectsTask->restartDelayed();
    break;
  case SCHEDULER::T_RESET :
    if (effectsTask)
      effectsTask->restartDelayed();
    break;
  default:
    return;
  }
}

//-----------------------------

// ------------- мигающий цвет (не эффект! используется для отображения краткосрочного предупреждения; неблокирующий код, рисует поверх эффекта!) -------------
void LAMP::warningHelper(){
  if(lampState.isWarning) {
    if(!warningTask)
      return;
    String msg = warningTask->getData();

    uint16_t cnt = warningTask->getWarn_duration()/(warningTask->getWarn_blinkHalfPeriod()*2);
    uint8_t xPos = (mx.cfg.w()+LET_WIDTH*(cnt>99?3:cnt>9?2:1))/2;    
    switch(lampState.warnType){
      case 0: mx.fill(warningTask->getWarn_color());
        break;
      case 1: {
        mx.fill(warningTask->getWarn_color());
        if (!isPrintingNow())
          sendStringToLamp(msg.isEmpty() ? String(cnt).c_str() : msg.c_str(), warningTask->getWarn_color(), true, false, -128, xPos);
        break;
      }
      case 2: {
        mx.fill(warningTask->getWarn_color());
        if (!isPrintingNow())
          sendStringToLamp(msg.isEmpty() ? String(cnt).c_str() : msg.c_str(), -warningTask->getWarn_color(), true, false, -128, xPos);
        break;
      }
      case 3: {
        if (!isPrintingNow())
          //sendStringToLamp(String(cnt).c_str(), cnt%2?warn_color:-warn_color, true, false, -128, xPos);
          sendStringToLamp(msg.isEmpty() ? String(cnt).c_str() : msg.c_str(), warningTask->getWarn_color(), true, false, -128, xPos);
        break;
      }
      default: break;
    }
  }
}

void LAMP::showWarning(
  const CRGB &color,                                        /* цвет вспышки                                                 */
  uint32_t duration,                                        /* продолжительность отображения предупреждения (общее время)   */
  uint16_t blinkHalfPeriod,                                 /* продолжительность одной вспышки в миллисекундах (полупериод) */
  uint8_t warnType,                                         /* тип предупреждения 0...3                                     */
  bool forcerestart,                                        /* перезапускать, если пришло повторное событие предупреждения  */
  const String &msg)                                        /* сообщение для вывода на матрицу                              */
{
  CRGB warn_color = CRGB::Black;
  uint32_t warn_duration = 1000;
  uint16_t warn_blinkHalfPeriod = 500;

  if(warningTask && !forcerestart){ // вытянуть данные из предыдущего таска, если таск существует и не перезапуск
    warn_color = warningTask->getWarn_color();
    warn_duration = warningTask->getWarn_duration();
    warn_blinkHalfPeriod = warningTask->getWarn_blinkHalfPeriod();
  }

  if(forcerestart || !warningTask){ // перезапуск или таск не существует - инициализация из параметров
    warn_color = color;
    warn_duration = duration;
    warn_blinkHalfPeriod = blinkHalfPeriod;
    lampState.isWarning = true;
    lampState.warnType = warnType;
  }

  if(!forcerestart && warnType<2)
    lampState.isWarning=!lampState.isWarning;
  if(warn_duration>warn_blinkHalfPeriod)
    warn_duration-=warn_blinkHalfPeriod;
  else
    warn_duration=0;
  if(warn_duration){
    if(warningTask){
      warningTask->cancel();
    }

    warningTask = new WarningTask(warn_color, warn_duration, warn_blinkHalfPeriod, msg.c_str(), blinkHalfPeriod, TASK_ONCE,
      [this](){
        WarningTask *cur = (WarningTask *)ts.getCurrentTask();
        showWarning(cur->getWarn_color(),cur->getWarn_duration(),cur->getWarn_blinkHalfPeriod(),(uint8_t)lampState.warnType, !lampState.isWarning, cur->getData());
      },
      &ts, false, nullptr, nullptr, true);
    warningTask->enableDelayed();
  } else {
    lampState.isWarning = false;
    if(warningTask)
      warningTask->cancel();
    warningTask = nullptr;
  }
}

void LAMP::setDraw(bool flag){
    flags.isDraw=flag;
// #if defined(USE_STREAMING) && !defined(EXT_STREAM_BUFFER)
//         if (flag && ledStream) {
//             flags.isStream=false;
//             remote_action();                // TODO: сделать нужный RA для стрима и рисования
//         }
// #endif
    setDrawBuff(flag);
}

void LAMP::setDrawBuff(bool active) {
  if (active){
    if (!drawbuff)
      drawbuff = new LedFB(mx.cfg);   // create a buff with same layout as main FB
    return;
  }

  delete drawbuff;
  drawbuff = nullptr;
}

void LAMP::fillDrawBuf(CRGB &color) {
  if(drawbuff) drawbuff->fill(color);
}

#ifdef EMBUI_USE_MQTT
void LAMP::setmqtt_int(int val) {
    if (!val && tmqtt_pub){
      delete tmqtt_pub;
      tmqtt_pub = nullptr;
      return;
    }

    if(tmqtt_pub){
        tmqtt_pub->setInterval(val);
        return;
    }

    extern void sendData();
    tmqtt_pub = new Task(val * TASK_SECOND, TASK_FOREVER, [this](){ if(embui.isMQTTconected()) sendData(); }, &ts, true);
}
#endif

#ifdef EXT_STREAM_BUFFER
void LAMP::setStreamBuff(bool active) {
    if(!active){
        if (!streambuff.empty()) {
            streambuff.resize(0);
            streambuff.shrink_to_fit();
        }
    } else if(streambuff.empty()){
        streambuff.resize(mx.size());
    }
}
#endif


/*  LEDFader class implementation */

void LEDFader::fadelight(const uint8_t _targetbrightness, const uint32_t _duration, std::function<void()> callback){
  if (!lmp) return;
  //LOG(printf_P, PSTR("FDR lamp_getbr:%d, fled_getbr:%d, fled_vid%d\n"), lmp->getBrightness(), FastLED.getBrightness(), brighten8_video(FastLED.getBrightness()));

  // will take previous value of fader target value as current lamp brightness,
  // during fadedown lamp's configured brightness does no change, so we can't rely on it
  _brt = _tgtbrt;
  if (_brt == _targetbrightness) {
    // no need to fade, already same brightness
    if (callback) callback();
    return;
  }
  _tgtbrt = _targetbrightness;
  _cb = callback;
  int _steps = (abs(_tgtbrt - _brt) > FADE_MININCREMENT * _duration / FADE_STEPTIME) ? _duration / FADE_STEPTIME : abs(_tgtbrt - _brt)/FADE_MININCREMENT;
  if (_steps < 3) {   // no need to fade for such small difference
    lmp->brightness(_tgtbrt);
    LOG(printf_P, PSTR("Fast fade to %d->%d\n"), _brt, _tgtbrt);
    if (runner) abort();
    if (callback) callback();
    return;
  }
  _brtincrement = (_tgtbrt - _brt) / _steps;

  if (runner){
    runner->setIterations(_steps);
    runner->restartDelayed();
  } else {
    runner = new Task((unsigned long)FADE_STEPTIME,
      _steps,
      [this](){ _brt += _brtincrement; lmp->brightness(_brt); /* LOG(printf_P, PSTR("fd brt %d/%d, glbr:%d, gbr:%d, vid:%d, vid2:%d\n"), _brt, _brtincrement, lmp->getLampBrightness(), lmp->getBrightness(), brighten8_video(FastLED.getBrightness()), brighten8_video(brighten8_video(FastLED.getBrightness()))  ); */ },
      &ts,
      true,
      nullptr,
      [this](){
          lmp->brightness(_tgtbrt);
          LOG(printf_P, PSTR("Fading to %d done\n"), _tgtbrt);
          // use new task for callback, 'cause effect switching will immiatetly respawn new fader from callback
          // so need to release a task instance
          if(_cb) { new Task(FADE_STEPTIME, TASK_ONCE, [this](){ if (_cb) { _cb(); _cb = nullptr; } }, &ts, true, nullptr, nullptr, true ); }
          runner = nullptr;
      },
      true);
  }

  LOG(printf_P, PSTR("Fading l:%d(led:%d)->%d, in %d steps, inc %d\n"), lmp->getBrightness(), _brt, _targetbrightness, _steps, _brtincrement);
}

void LEDFader::abort(){
  if (!runner) return;
  runner->abort();
  delete runner;
  runner = nullptr;
  LOG(println,F("Fader aborted"));
}