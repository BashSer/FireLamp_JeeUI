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

#pragma once

#include "color_palette.h"
#include "effectworker.h"
#include "effectmath.h"

const byte maxDim = max(WIDTH, HEIGHT);
const byte minDim = min(WIDTH, HEIGHT);
const byte width_adj = (WIDTH < HEIGHT ? (HEIGHT - WIDTH) /2 : 0);
const byte height_adj = (HEIGHT < WIDTH ? (WIDTH - HEIGHT) /2: 0);

//-------------- Специально обученный пустой эффект :)
class EffectNone : public EffectCalc {
private:
    void load() override { FastLED.clear(); };
public:
    EffectNone(LedFB &framebuffer) : EffectCalc(framebuffer){}
    bool run() override {return true;};
};

//-------------- Эффект "Часы"
class EffectTime : public EffectCalc {
private:
    bool timeShiftDir; // направление сдвига
    float curTimePos; // текущая позиция вывода
    float color_idx; // индекс цвета
    CRGB hColor[1]; // цвет часов и минут
    CRGB mColor[1]; // цвет часов и минут
    uint32_t lastrun=0;     /**< счетчик времени для эффектов с "задержкой" */
    bool isMinute=false;

    bool timePrintRoutine();
    bool palleteTest();
    void load() override;
public:
    EffectTime(LedFB &framebuffer) : EffectCalc(framebuffer){}
    bool run() override;
};

/*
 ***** METABALLS / МЕТАСФЕРЫ *****
Metaballs proof of concept by Stefan Petrick 
https://gist.github.com/StefanPetrick/170fbf141390fafb9c0c76b8a0d34e54
*/
class EffectMetaBalls : public EffectCalc {
private:
	float speedFactor;
	const float hormap = (256 / WIDTH);
    const float vermap = (256 / HEIGHT);
	String setDynCtrl(UIControl*_val) override;
    void load() override;
public:
    EffectMetaBalls(LedFB &framebuffer) : EffectCalc(framebuffer){}
    bool run() override;
};

// ***** SINUSOID3 / СИНУСОИД3 *****
/*
  Sinusoid3 by Stefan Petrick (mod by Palpalych for GyverLamp 27/02/2020)
  read more about the concept: https://www.youtube.com/watch?v=mubH-w_gwdA
*/
class EffectSinusoid3 : public EffectCalc {
private:
	const uint8_t semiHeightMajor =  HEIGHT / 2 + (HEIGHT % 2);
	const uint8_t semiWidthMajor =  WIDTH / 2  + (WIDTH % 2);
	float e_s3_speed;
	float e_s3_size;
	uint8_t _scale;
	uint8_t type;
	
	String setDynCtrl(UIControl*_val) override;

public:
    EffectSinusoid3(LedFB &framebuffer) : EffectCalc(framebuffer){}
    bool run() override;
};



//----- Эффект "Прыгающие Мячики"
// перевод на субпиксельную графику kostyamat
class EffectBBalls : public EffectCalc {
    struct Ball {
        uint8_t color;              // прикручено при адаптации для разноцветных мячиков
        uint8_t brightness{156};
        int8_t x;                   // прикручено при адаптации для распределения мячиков по радиусу лампы
        float pos{0};               // The integer position of the dot on the strip (LED index) /yeah, integer.../
        float vimpact{0};           // As time goes on the impact velocity will change, so make an array to store those values
        float cor{0};               // Coefficient of Restitution (bounce damping)
        long unsigned tlast{millis()};      // The clock time of the last ground strike
        float shift{0};
    };

    uint8_t bballsNUM_BALLS{1};                            // Number of bouncing balls you want (recommend < 7, but 20 is fun in its own way) ... количество мячиков теперь задаётся бегунком, а не константой
    float bballsHi = 0.0;                               // An array of heights
    uint32_t bballsTCycle = 0;                        // The time since the last time the ball struck the ground
    float hue{0};
    bool halo = false;                                  // ореол
    uint8_t _scale=1;
    uint16_t _speed;
    std::vector<Ball> balls = std::vector<Ball>(bballsNUM_BALLS, Ball());

    bool bBallsRoutine();
    void load() override;
	String setDynCtrl(UIControl*_val) override;
public:
    EffectBBalls(LedFB &framebuffer) : EffectCalc(framebuffer){}
    bool run() override;
};

// ------------- Эффект "Пейнтбол" -------------
class EffectLightBalls : public EffectCalc {
private:
	#define BORDERTHICKNESS       (1U)   // глубина бордюра для размытия яркой частицы: 0U - без границы (резкие края); 1U - 1 пиксель (среднее размытие) ; 2U - 2 пикселя (глубокое размытие)
	const uint8_t paintWidth = WIDTH - BORDERTHICKNESS * 2;
	const uint8_t paintHeight = HEIGHT - BORDERTHICKNESS * 2;
	float speedFactor;
	
	String setDynCtrl(UIControl*_val) override;

public:
    EffectLightBalls(LedFB &framebuffer) : EffectCalc(framebuffer){}
    bool run() override;
};

// ------- Эффект "Пульс"
class EffectPulse : public EffectCalc {
    uint8_t pulse_hue;
    float pulse_step = 0;
    uint8_t centerX = random8(WIDTH - 5U) + 3U;
    uint8_t centerY = random8(HEIGHT - 5U) + 3U;
    uint8_t currentRadius = 4;
    float _pulse_hue = 0;
    uint8_t _pulse_hueall = 0;
    float speedFactor;
    String setDynCtrl(UIControl*_val) override;
public:
    EffectPulse(LedFB &framebuffer) : EffectCalc(framebuffer){}
    bool run() override;
};

// ------------- эффект "Блуждающий кубик" -------------
class EffectBall : public EffectCalc {
private:
    uint8_t ballSize;
    CRGB ballColor;
    float vectorB[2U];
    float coordB[2U];
	bool flag[2] = {true, true};
	float speedFactor;

	String setDynCtrl(UIControl*_val);
	
public:
    EffectBall(LedFB &framebuffer) : EffectCalc(framebuffer){}
    void load() override;
    bool run() override;
};

// -------- Эффект "Светлячки со шлейфом"
#define _AMOUNT 16U
class EffectLighterTracers : public EffectCalc {
private:
    uint8_t cnt = 1;
    float vector[_AMOUNT][2U];
    float coord[_AMOUNT][2U];
    int16_t ballColors[_AMOUNT];
    byte light[_AMOUNT];
    float speedFactor;
    bool lighterTracersRoutine();

public:
    EffectLighterTracers(LedFB &framebuffer) : EffectCalc(framebuffer){}
    void load() override;
    bool run() override;
    String setDynCtrl(UIControl*_val) override;
};

class EffectRainbow : public EffectCalc {
private:
    float hue; // вещественное для малых скоростей, нужно приведение к uint8_t по месту
    float twirlFactor;
    float micCoef;

    bool rainbowHorVertRoutine(bool isVertical);
    bool rainbowDiagonalRoutine();

public:
    EffectRainbow(LedFB &framebuffer) : EffectCalc(framebuffer){}
    bool run() override;
};

class EffectColors : public EffectCalc {
private:
    uint8_t ihue;
    uint8_t mode;
    uint8_t modeColor;

    bool colorsRoutine();
    //void setscl(const byte _scl) override;
    String setDynCtrl(UIControl*_val) override;
public:
    EffectColors(LedFB &framebuffer) : EffectCalc(framebuffer){}
    void load() override;
    bool run() override;
};

// ------ Эффект "Белая Лампа"
class EffectWhiteColorStripe : public EffectCalc {
private:
    uint8_t shift=0;
    bool whiteColorStripeRoutine();
    String setDynCtrl(UIControl*_val) override;
public:
    EffectWhiteColorStripe(LedFB &framebuffer) : EffectCalc(framebuffer){}
    bool run() override;
};

// ---- Эффект "Конфетти"
class EffectSparcles : public EffectCalc {
private:
    uint8_t eff = 1;
    bool sparklesRoutine();

public:
    EffectSparcles(LedFB &framebuffer) : EffectCalc(framebuffer){}
    bool run() override;
    String setDynCtrl(UIControl*_val) override;
};

// ========== Эффект "Эффектопад"
// совместное творчество юзеров форума https://community.alexgyver.ru/
class EffectEverythingFall : public EffectCalc {
private:
    byte heat[WIDTH][HEIGHT];

public:
    EffectEverythingFall(LedFB &framebuffer) : EffectCalc(framebuffer){}
    void load() override;
    bool run() override;
};

// ============= FIRE 2012 /  ОГОНЬ 2012 ===============
// based on FastLED example Fire2012WithPalette: https://github.com/FastLED/FastLED/blob/master/examples/Fire2012WithPalette/Fire2012WithPalette.ino
// v1.0 - Updating for GuverLamp v1.7 by SottNick 17.04.2020
/*
 * Эффект "Огонь 2012"
 */
class EffectFire2012 : public EffectCalc {
private:
#define NUMPALETTES 10

  // COOLING: How much does the air cool as it rises?
  // Less cooling = taller flames.  More cooling = shorter flames.
    uint8_t cooling = 80U; // 70
  // SPARKING: What chance (out of 255) is there that a new spark will be lit?
  // Higher chance = more roaring fire.  Lower chance = more flickery fire.
     uint8_t sparking = 90U; // 130
  // SMOOTHING; How much blending should be done between frames
  // Lower = more blending and smoother flames. Higher = less blending and flickery flames
    uint8_t _scale = 1;
    const uint8_t fireSmoothing = 60U; // 90
    uint8_t noise3d[NUM_LAYERS][WIDTH][HEIGHT];
    bool fire2012Routine();
    String setDynCtrl(UIControl*_val) override;
public:
    EffectFire2012(LedFB &framebuffer) : EffectCalc(framebuffer){}
    void load() override;
    bool run() override;
};

// ------------- класс Светлячки -------------
// нужен для некоторых эффектов
#define LIGHTERS_MAX    10
class EffectLighters : public EffectCalc {
protected:
struct Lighter {
    uint8_t color;
    uint8_t light;
    float spdX, spdY;
    float posX, posY;
};

    bool subPix = false;
    //uint16_t lightersIdx;
    float speedFactor{0.1};
    std::vector<Lighter> lighters;
private:
    String setDynCtrl(UIControl*_val) override;
public:
    EffectLighters(LedFB &framebuffer) : EffectCalc(framebuffer), lighters(std::vector<Lighter>(10)) {}
    void load() override;
    bool run() override;
};

// ------------- Эффект "New Матрица" ---------------
class EffectMatrix : public EffectLighters {
private:
    bool matrixRoutine();
    uint8_t _scale = 1;
    byte gluk = 1;
    uint8_t hue, _hue;
    bool randColor = false;
    bool white = false;
    float count{0};
    float _speed{1};
    String setDynCtrl(UIControl*_val) override;
public:
    EffectMatrix(LedFB &framebuffer) : EffectLighters(framebuffer){}
    void load() override;
    bool run() override;
};

// ------------- звездопад/метель -------------
class EffectStarFall : public EffectLighters {
private:
    uint8_t _scale = 1;
    uint8_t effId = 1;
    bool isNew = true;
    float fade;
    float _speed{1};
    bool snowStormStarfallRoutine();
    String setDynCtrl(UIControl*_val) override;

public:
    EffectStarFall(LedFB &framebuffer) : EffectLighters(framebuffer){}
    void load() override;
    bool run() override;
};

// ----------- Эффекты "Лава, Зебра, etc"
class Effect3DNoise : public EffectCalc {
private:
    void fillNoiseLED();
    void fillnoise8();

    uint8_t ihue;
    bool colorLoop;
	bool blurIm;
    float _speed;             // speed is set dynamically once we've started up
    float _scale;             // scale is set dynamically once we've started up
    float x;
    float y;
    float z;
    #if (WIDTH > HEIGHT)
    uint8_t noise[2*HEIGHT][WIDTH];
    #else
    uint8_t noise[2*WIDTH][HEIGHT];
    #endif

public:
    Effect3DNoise(LedFB &framebuffer) : EffectCalc(framebuffer){}
    void load() override;
    bool run() override;
    String setDynCtrl(UIControl*_val) override;
};

// ***** Эффект "Спираль"     ****
/*
 * Aurora: https://github.com/pixelmatix/aurora
 * https://github.com/pixelmatix/aurora/blob/sm3.0-64x64/PatternSpiro.h
 * Copyright (c) 2014 Jason Coon
 * Неполная адаптация SottNick
 */
class EffectSpiro : public EffectCalc {
private:
  const uint8_t spiroradiusx = WIDTH /4; //((!WIDTH & 1) ? (WIDTH -1) : WIDTH) / 4;
  const uint8_t spiroradiusy = HEIGHT /4;//(!(HEIGHT & 1) ? (HEIGHT-1) : HEIGHT) / 4;

  const uint8_t spirocenterX = WIDTH /2; //(!(WIDTH & 1) ? (WIDTH -1) : WIDTH) / 2;
  const uint8_t spirocenterY = HEIGHT /2; //(!(HEIGHT & 1) ? (HEIGHT-1) : HEIGHT) / 2;

  const uint8_t spirominx = spirocenterX - spiroradiusx;
  const uint8_t spiromaxx = spirocenterX + spiroradiusx - (WIDTH%2 == 0 ? 1:0);// + 1;
  const uint8_t spirominy = spirocenterY - spiroradiusy;
  const uint8_t spiromaxy = spirocenterY + spiroradiusy - (HEIGHT%2 == 0 ? 1:0); // + 1;

  bool spiroincrement = false;
  bool spirohandledChange = false;
  float spirohueoffset = 0;
  uint8_t spirocount = 1;
  float spirotheta1 = 0;
  float spirotheta2 = 0;
  uint8_t internalCnt = 0;
  float speedFactor;

  String setDynCtrl(UIControl*_val) override;

public:
    EffectSpiro(LedFB &framebuffer) : EffectCalc(framebuffer){}
    void load() override;
    bool run() override;
};

// ============= ЭФФЕКТ ПРИЗМАТА ===============
// Prismata Loading Animation
class EffectPrismata : public EffectCalc {
private:
    byte spirohueoffset = 0;
    uint8_t fadelvl=1;
	float speedFactor;
    
    String setDynCtrl(UIControl*_val) override;
public:
    EffectPrismata(LedFB &framebuffer) : EffectCalc(framebuffer){}
    void load() override;
    bool run() override;
};

// ============= ЭФФЕКТ СТАЯ ===============
// Адаптация от (c) SottNick
class EffectFlock : public EffectCalc {
private:
  Boid boids[AVAILABLE_BOID_COUNT];
  Boid predator;
  PVector wind;
  float speedFactor;

  bool predatorPresent;
  float hueoffset;

  bool flockRoutine();
  String setDynCtrl(UIControl*_val) override;
  //void setspd(const byte _spd) override;
public:
    EffectFlock(LedFB &framebuffer) : EffectCalc(framebuffer){}
    void load() override;
    bool run() override;
};

// ***** RAINBOW COMET / РАДУЖНАЯ КОМЕТА *****
// ***** Парящий огонь, Кровавые Небеса, Радужный Змей и т.п.
// базис (c) Stefan Petrick
#define COMET_NOISE_LAYERS  1
class EffectComet : public EffectCalc {
    byte hue, hue2;
    uint8_t eNs_noisesmooth = 200;
    uint8_t count;
    uint8_t speedy{100};
    float spiral, spiral2;
    float _speed{1};
    uint8_t _scale = 4;     // effect switcher
    uint8_t effId = 1;      // 2, 1-6
    uint8_t colorId;        // 3, 1-255
    uint8_t smooth = 1;     // 4, 1-12
    uint8_t blur{10};           // 5, 1-64
    // 3D Noise map
    Noise3dMap noise3d;

    const uint8_t e_centerX = (fb.cfg.w() / 2) - ((fb.cfg.w() - 1) & 0x01);
    const uint8_t e_centerY = (fb.cfg.h() / 2) - ((fb.cfg.h() - 1) & 0x01);


    void drawFillRect2_fast(int8_t x1, int8_t y1, int8_t x2, int8_t y2, CRGB color);
    void moveFractionalNoise(bool direction, int8_t amplitude, float shift = 0);

    bool rainbowCometRoutine();
    bool rainbowComet3Routine();
    bool firelineRoutine();
    bool fractfireRoutine();
    bool flsnakeRoutine();
    bool smokeRoutine();
    String setDynCtrl(UIControl*_val) override;

public:
    EffectComet(LedFB &framebuffer) :  EffectCalc(framebuffer), noise3d(COMET_NOISE_LAYERS, framebuffer.cfg.w(), framebuffer.cfg.w()) {}
    void load() override;
    bool run() override;
};

// ============= SWIRL /  ВОДОВОРОТ ===============
// https://gist.github.com/kriegsman/5adca44e14ad025e6d3b
// Copyright (c) 2014 Mark Kriegsman
class EffectSwirl : public EffectCalc {
private:
    bool swirlRoutine();

public:
    EffectSwirl(LedFB &framebuffer) : EffectCalc(framebuffer){}
    void load() override;
    bool run() override;
};

// ============= DRIFT / ДРИФТ ===============
// v1.0 - Updating for GuverLamp v1.7 by SottNick 12.04.2020
// v1.1 - +dither, +phase shifting by PalPalych 12.04.2020
// https://github.com/pixelmatix/aurora/blob/master/PatternIncrementalDrift.h
class EffectDrift : public EffectCalc {
private:
	const byte maxDim_steps = 256 / max(WIDTH, HEIGHT);
	uint8_t dri_phase;
	float _dri_speed;
	uint8_t _dri_delta;
	byte driftType = 0;

	String setDynCtrl(UIControl*_val) override;
	bool incrementalDriftRoutine();
	bool incrementalDriftRoutine2();

public:
    EffectDrift(LedFB &framebuffer) : EffectCalc(framebuffer){}
    void load() override;
    bool run() override;
};

// ------------------------------ ЭФФЕКТ МЕРЦАНИЕ ----------------------
// (c) SottNick
class EffectTwinkles : public EffectCalc {
private:
  uint8_t thue = 0U;
  uint8_t tnum;
  CRGB ledsbuff[num_leds];
  float speedFactor;
  bool twinklesRoutine();
  String setDynCtrl(UIControl*_val) override;
  //void setscl(const byte _scl) override;
public:
    EffectTwinkles(LedFB &framebuffer) : EffectCalc(framebuffer){}
    void load() override;
    void setup();
    bool run() override;
};

class EffectWaves : public EffectCalc {
private:
  float whue;
  float waveTheta;
  uint8_t _scale=1;
  float speedFactor;
  bool wavesRoutine();
  String setDynCtrl(UIControl*_val) override;
public:
    EffectWaves(LedFB &framebuffer) : EffectCalc(framebuffer){}
    void load() override;
    bool run() override;
};

// ============= RADAR / РАДАР ===============
// Aurora : https://github.com/pixelmatix/aurora/blob/master/PatternRadar.h
// Copyright(c) 2014 Jason Coon
class EffectRadar : public EffectCalc {
private:
    float eff_offset; 
    float eff_theta;  // глобальная переменная угла для работы эффектов
    bool subPix = false;
    byte hue;
    const float width_adj_f = (float)(WIDTH < HEIGHT ? (HEIGHT - WIDTH) / 2. : 0);
    const float height_adj_f= (float)(HEIGHT < WIDTH ? (WIDTH - HEIGHT) / 2. : 0);
    bool radarRoutine();
    String setDynCtrl(UIControl *_val) override;

public:
    EffectRadar(LedFB &framebuffer) : EffectCalc(framebuffer){}
    void load() override;
    bool run() override;
};

class EffectFire2018 : public EffectCalc {
#define FIRE_NUM_LAYERS     2
private:
  //const uint8_t centreY = fb.cfg.h() / 2 + (fb.cfg.h() % 2);
  //const uint8_t centreX = fb.cfg.w() / 2 + (fb.cfg.w() % 2);
  bool isLinSpeed = true;
/*
  uint32_t noise32_x[FIRE_NUM_LAYERS];
  uint32_t noise32_y[FIRE_NUM_LAYERS];
  uint32_t noise32_z[FIRE_NUM_LAYERS];
  uint32_t scale32_x[FIRE_NUM_LAYERS];
  uint32_t scale32_y[FIRE_NUM_LAYERS];*/
  // use vector of vectors to take benefit of swap operations
  std::vector<std::vector<uint8_t>> fire18heat;
  Noise3dMap noise;

  String setDynCtrl(UIControl*_val) override;

public:
    EffectFire2018(LedFB &framebuffer) : 
        EffectCalc(framebuffer),
        fire18heat(std::vector<std::vector<uint8_t>>(framebuffer.cfg.h(), std::vector<uint8_t>(framebuffer.cfg.w()))),
        noise(FIRE_NUM_LAYERS, framebuffer.cfg.w(), framebuffer.cfg.h()) {}
    bool run() override;
};

// -------------- Эффект "Кодовый замок"
// (c) SottNick
class EffectRingsLock : public EffectCalc {
private:
  uint8_t ringWidth; // максимальне количество пикселей в кольце (толщина кольца) от 1 до height / 2 + 1
  uint8_t ringNb; // количество колец от 2 до height
  uint8_t downRingHue, upRingHue; // количество пикселей в нижнем (downRingHue) и верхнем (upRingHue) кольцах

  uint8_t ringColor[HEIGHT]; // начальный оттенок каждого кольца (оттенка из палитры) 0-255
  uint8_t huePos[HEIGHT]; // местоположение начального оттенка кольца 0-WIDTH-1
  uint8_t shiftHueDir[HEIGHT]; // 4 бита на ringHueShift, 4 на ringHueShift2
  ////ringHueShift[ringsCount]; // шаг градиета оттенка внутри кольца -8 - +8 случайное число
  ////ringHueShift2[ringsCount]; // обычная скорость переливания оттенка всего кольца -8 - +8 случайное число
  uint8_t currentRing; // кольцо, которое в настоящий момент нужно провернуть
  uint8_t stepCount; // оставшееся количество шагов, на которое нужно провернуть активное кольцо - случайное от WIDTH/5 до WIDTH-3
  void ringsSet();
  bool ringsRoutine();
  String setDynCtrl(UIControl*_val) override;
public:
    EffectRingsLock(LedFB &framebuffer) : EffectCalc(framebuffer){}
    void load() override;
    bool run() override;
};

// ------------------------------ ЭФФЕКТ КУБИК 2D ----------------------
// (c) SottNick
// refactored by Vortigont
class EffectCube2d : public EffectCalc {
private:
  bool classic = false;
  uint8_t sizeX, sizeY; // размеры ячеек по горизонтали / вертикали
  uint8_t cntX, cntY; // количество ячеек по горизонтали / вертикали
  uint8_t fieldX, fieldY; // размер всего поля блоков по горизонтали / вертикали (в том числе 1 дополнительная пустая дорожка-разделитель с какой-то из сторон)
  uint8_t currentStep;
  uint8_t pauseSteps; // осталось шагов паузы
  uint8_t shiftSteps; // всего шагов сдвига
  bool direction = false; // направление вращения в текущем цикле (вертикаль/горизонталь)
  std::vector<int8_t> moveItems;     // индекс перемещаемого элемента
  std::vector< std::vector<uint8_t> > storage;
  int8_t globalShiftX, globalShiftY;
  uint8_t gX, gY;
  bool seamlessX = true;

  LedFB ledbuff;        // виртуальй холст

  void swapBuff();
  void cubesize();
  bool cube2dRoutine();
  bool cube2dClassicRoutine();
  void cube2dmoveCols(uint8_t moveItem, bool movedirection);
  void cube2dmoveRows(uint8_t moveItem, bool movedirection);
  String setDynCtrl(UIControl*_val) override;

public:
    EffectCube2d(LedFB &framebuffer) : EffectCalc(framebuffer), sizeX(5), sizeY(5), ledbuff(1,1)  { cubesize(); moveItems = std::vector<int8_t>(direction ? cntX : cntY, 0); }
    void load() override;
    bool run() override;
};

// ------ Эффекты "Пикассо"
// (c) obliterator
#define PICASSO_MIN_PARTICLES   6U
#define PICASSO_MAX_PARTICLES   20U
class EffectPicasso : public EffectCalc {
private:
    struct Particle {
        float position_x{0}, position_y{0};
        float speed_x{0}, speed_y{0};
        CHSV color;
        uint8_t hue_next = 0;
        int8_t hue_step = 0;
    };
    uint8_t effId=0;
    uint8_t pidx = 0;
	float speedFactor{1};
    std::vector<Particle> particles{std::vector<Particle>(PICASSO_MIN_PARTICLES, Particle())};

    void generate(bool reset = false);
    void position();
    bool picassoRoutine();
    bool metaBallsRoutine();
    GradientPaletteList palettes;
public:
    EffectPicasso(LedFB &framebuffer);
    virtual ~EffectPicasso(){}
    bool run() override;
    String setDynCtrl(UIControl*_val) override;
};

// ------ Эффект "Прыгуны" (c) obliterator
#define LEAPERS_MIN     5
#define LEAPERS_MAX     20
class EffectLeapers : public EffectCalc {
    struct Leaper {
        float x{0}, y{0};
        float xd{0}, yd{0};
        byte color;
    };
private:
    unsigned numParticles = 0;
    uint8_t _rv{0};
	float speedFactor{1};
    std::vector<Leaper> leapers{std::vector<Leaper>(LEAPERS_MIN, Leaper())};

    void generate();
    void restart_leaper(Leaper &l);
    void move_leaper(Leaper &l);
	String setDynCtrl(UIControl*_val) override;
public:
    EffectLeapers(LedFB &framebuffer) : EffectCalc(framebuffer){}
	void load() override;
    bool run() override;

};

// ------ Эффект "Лавовая Лампа"
// (c) obliterator
#define LIQLAMP_MASS_MIN    10
#define LIQLAMP_MASS_MAX    50
#define LIQLAMP_MIN_PARTICLES   10
#define LIQLAMP_MAX_PARTICLES   20
class EffectLiquidLamp : public EffectCalc {
    struct Particle{
        float position_x = 0;
        float position_y = 0;
        float speed_x = 0;
        float speed_y = 0;
        float rad = 0;
        float hot = 0;
        float spf = 0;
        int mass = 0;
        unsigned mx = 0;
        unsigned sc = 0;
        unsigned tr = 0;
    };

    uint8_t pidx = 0;
    bool physic_on = 1;
    unsigned filter = 0;
	float speedFactor = 1.0;
    GradientPaletteList palettes;

    std::vector<Particle> particles{std::vector<Particle>(LIQLAMP_MIN_PARTICLES, Particle())};
    // todo: allocate/deallocate those arrays dynamically only if needed by filter settings
    uint8_t buff[WIDTH][HEIGHT];
    float buff2[WIDTH][HEIGHT];

    void generate(bool reset = false);
    void position();
    void physic();
    bool routine();

public:
    EffectLiquidLamp(LedFB &framebuffer);
    virtual ~EffectLiquidLamp() {};
    void load() override { generate(true); };
    bool run() override {return routine();};
    String setDynCtrl(UIControl*_val) override;
};

// ------- Эффект "Вихри"
// Based on Aurora : https://github.com/pixelmatix/aurora/blob/master/PatternFlowField.h
// Copyright(c) 2014 Jason Coon
//адаптация SottNick
class EffectWhirl : public EffectCalc {
private:
    float ff_x;
    float ff_y;
    float ff_z;
    float hue;
    Boid boids[AVAILABLE_BOID_COUNT];
    uint8_t micPick = 0;
	float speedFactor;

    const uint8_t ff_speed = 1; // чем выше этот параметр, тем короче переходы (градиенты) между цветами. 1 - это самое красивое
    const uint8_t ff_scale = 26; // чем больше этот параметр, тем больше "языков пламени" или как-то так. 26 - это норм

    bool whirlRoutine();
    String setDynCtrl(UIControl*_val) override;
public:
    EffectWhirl(LedFB &framebuffer) : EffectCalc(framebuffer){}
    void load() override;
    bool run() override;
};

// ------------- Эффект "Блики на воде Цвета"
// Идея SottNick
// переписал на программные блики + паттерны - (c) kostyamat
// Генератор бликов (c) stepko
class EffectAquarium : public EffectCalc {
private:

struct Drop{
    uint8_t posX, posY;
    float radius;
};

    CRGBPalette16 currentPalette;
    const uint8_t _scale = 25;
    const uint8_t _speed = 3;

    float hue = 0.;
    uint16_t x{0}, y{0}, z{0};
    Noise3dMap noise;

    inline uint8_t maxRadius(){return fb.cfg.w() + fb.cfg.h();};
    std::vector<Drop> drops;
    uint8_t satur;
    uint8_t glare = 0;
    uint8_t iconIdx = 0;
	float speedFactor{1};

    void nGlare(uint8_t bri);
    void nDrops(uint8_t bri);
    void fillNoiseLED();

public:
    EffectAquarium(LedFB &framebuffer) : EffectCalc(framebuffer),
        noise(1, framebuffer.cfg.w(), framebuffer.cfg.h()),
        drops(std::vector<Drop>((fb.cfg.h() + fb.cfg.w()) / 6)) {}

    void load() override;
    String setDynCtrl(UIControl*_val) override;
    bool run() override;
};


// ----------- Эффект "Звезды"
// (c) SottNick
#define STARS_NUM (5)
class EffectStar : public EffectCalc {
private:
    float driftx;
    float drifty;
    float cangle;
    float sangle;
    float radius2;
    uint8_t stars_count;
    float color[STARS_NUM] ;                        // цвет звезды
    uint8_t points[STARS_NUM] ;                       // количество углов в звезде
    unsigned int cntdelay[STARS_NUM] ;                   // задержка пуска звезды относительно счётчика
    float counter = 0;                                // счетчик для реализации смещений, наростания и т.д.
	float _speedFactor{1};
	float _speed;
    bool setup = true;
    uint8_t micPick = 0;
    const uint8_t spirocenterX = WIDTH / 2;
    const uint8_t spirocenterY = HEIGHT / 2;
    void drawStar(float xlocl, float ylocl, float biggy, float little, int16_t points, float dangle, uint8_t koler);
	String setDynCtrl(UIControl*_val) override;

public:
    EffectStar(LedFB &framebuffer) : EffectCalc(framebuffer){}
    void load() override;
    bool run() override;
};

//---------- Эффект "Фейерверк"
// адаптация и переписал - kostyamat
// https://gist.github.com/jasoncoon/0cccc5ba7ab108c0a373
// vortigont переписал за kostyamat'ом

#define MIN_RCKTS   2U
#define MAX_RCKTS   8U     // максимальное количество снарядов
#define NUM_SPARKS  16U    // количество разлитающихся петард (частей снаряда)

struct DotsStore {
    accum88 gBurstx;
    accum88 gBursty;
    saccum78 gBurstxv;
    saccum78 gBurstyv;
    CRGB gBurstcolor;
    bool gSkyburst = false;
};

struct Dot {    // класс для создания снарядов и петард

  static constexpr saccum78 gGravity = 10;
  static constexpr fract8  gBounce = 127;
  static constexpr fract8  gDrag = 255;

  byte    show{0};
  byte    theType{0};
  accum88 x{0}, y{0};
  saccum78 xv{0}, yv{0};
  accum88 r{0};
  CRGB color{CRGB::Black};
  uint16_t cntdown{0};

  //Dot()
  void Move(DotsStore &store, bool flashing);
  void GroundLaunch(DotsStore &store);
  void Skyburst( accum88 basex, accum88 basey, saccum78 basedv, CRGB& basecolor, uint8_t dim);
  //CRGB &piXY(LedFB &leds, byte x, byte y);

  int16_t scale15by8_local( int16_t i, fract8 _scale ){ return (i * _scale / 256); };
};

class EffectFireworks : public EffectCalc {
//#define MODEL_BORDER (fb.cfg.w() - 4U)  
//#define MODEL_WIDTH  (2*MODEL_BORDER + fb.cfg.w()) // не трогать, - матиматика
//#define MODEL_HEIGHT (2*MODEL_BORDER + fb.cfg.h()) // -//-
//#define PIXEL_X_OFFSET ((MODEL_WIDTH  - fb.cfg.w() ) / 2) // -//-
//#define PIXEL_Y_OFFSET ((MODEL_HEIGHT - fb.cfg.h()) / 2) // -//-

    DotsStore store;
    byte dim;
    uint8_t valDim;
    //uint8_t cnt;
    bool flashing = false;
    bool fireworksRoutine();
    void sparkGen();
    std::vector<Dot> gDot;
    std::vector<Dot> gSparks;
    //Dot gDot[MAX_RCKTS];
    //Dot gSparks[NUM_SPARKS];
    String setDynCtrl(UIControl*_val) override;
    void draw(Dot &d);
    int16_t _model_w(){ return 2*(fb.cfg.w() - 4) + fb.cfg.w(); };  // как далеко за экран может вылетить снаряд, если снаряд вылетает за экран, то всышка белого света (не особо логично)
    int16_t _model_h(){ return 2*(fb.cfg.h() - 4) + fb.cfg.h(); };
    int16_t _x_offset(){ return (_model_w()-fb.cfg.w())/2; };
    int16_t _y_offset(){ return (_model_h()-fb.cfg.h())/2; };
    void _screenscale(accum88 a, byte N, byte &screen, byte &screenerr);

public:
    EffectFireworks(LedFB &framebuffer) : EffectCalc(framebuffer), gDot(std::vector<Dot>(MIN_RCKTS)), gSparks(std::vector<Dot>(NUM_SPARKS)) {}
    //void load() override;
    bool run() override;
};

// ------------ Эффект "Тихий Океан"
//  "Pacifica" перенос кода kostyamat
//  Gentle, blue-green ocean waves.
//  December 2019, Mark Kriegsman and Mary Corey March.
//  For Dan.
// https://raw.githubusercontent.com/FastLED/FastLED/master/examples/Pacifica/Pacifica.ino
class EffectPacific : public EffectCalc {
private:
	uint32_t speedFactor = 100;
	void pacifica_one_layer(const TProgmemRGBPalette16& p, uint16_t cistart, uint16_t wavescale, uint8_t bri, uint16_t ioff);
	void pacifica_deepen_colors();
	void pacifica_add_whitecaps();
	String setDynCtrl(UIControl*_val) override;

public:
    EffectPacific(LedFB &framebuffer) : EffectCalc(framebuffer){}
    //void load() override;
    bool run() override;
};

#ifdef MIC_EFFECTS
//----- Эффект "Осциллограф" 
// (c) kostyamat
class EffectOsc : public EffectCalc {
private:
    byte oscHV;
    byte oscilLimit;
#ifdef ESP32
    float pointer{2048};
#else
    float pointer{512};
#endif
    CRGB color;
    float div{1};
    byte gain;
    float y[2] = {0., 0.};
    String setDynCtrl(UIControl*_val) override;
    

public:
    EffectOsc(LedFB &framebuffer) : EffectCalc(framebuffer){}
    bool run() override;
};
#endif

// ------ Эффект "Вышиванка" 
// (с) проект Aurora "Munch"
class EffectMunch : public EffectCalc {
private:
    byte count = 0;
    int8_t dir = 1;
    byte flip = 0;
    byte generation = 0;
    byte mic[2];
    byte rand;
    bool flag = false;
    uint8_t minDimLocal = maxDim > 32 ? 32 : 16;

    String setDynCtrl(UIControl*_val) override;
    bool munchRoutine();

public:
    EffectMunch(LedFB &framebuffer) : EffectCalc(framebuffer){}
    void load() override;
    bool run() override;
};

// ------ Эффект "Цветной шум" 
// (с) https://gist.github.com/StefanPetrick/c856b6d681ec3122e5551403aabfcc68
class EffectNoise : public EffectCalc {
private:

    uint8_t CentreX =  (WIDTH / 2) - 1;
    uint8_t CentreY = (HEIGHT / 2) - 1;
    uint32_t x[NUM_LAYERS];
    uint32_t y[NUM_LAYERS];
    uint32_t z[NUM_LAYERS];
    uint32_t scale_x[NUM_LAYERS];
    uint32_t scale_y[NUM_LAYERS];
    uint8_t  noise[NUM_LAYERS][WIDTH][HEIGHT];
	uint8_t speedFactor;
    bool type = false;

	String setDynCtrl(UIControl*_val) override;

public:
    EffectNoise(LedFB &framebuffer) : EffectCalc(framebuffer){}
    void load() override;
    bool run() override;
};

// ---- Эффект "Мотыльки"
// (с) SottNick, https://community.alexgyver.ru/threads/wifi-lampa-budilnik-obsuzhdenie-proekta.1411/post-49262
class EffectButterfly : public EffectCalc {
private:
    float butterflysPosX[BUTTERFLY_MAX_COUNT];
    float butterflysPosY[BUTTERFLY_MAX_COUNT];
    float butterflysSpeedX[BUTTERFLY_MAX_COUNT];
    float butterflysSpeedY[BUTTERFLY_MAX_COUNT];
    float butterflysTurn[BUTTERFLY_MAX_COUNT];
    uint8_t butterflysColor[BUTTERFLY_MAX_COUNT];
    uint8_t butterflysBrightness[BUTTERFLY_MAX_COUNT];
    uint8_t deltaValue;
    uint8_t deltaHue;
    uint8_t hue;
    uint8_t hue2;
    byte step = 0;
    byte csum = 0;
    uint8_t cnt;
    bool wings = false;
    bool isColored = true;
	float speedFactor;
	String setDynCtrl(UIControl*_val) override;

public:
    EffectButterfly(LedFB &framebuffer) : EffectCalc(framebuffer){}
    void load() override;
    bool run() override;
};

// ---- Эффект "Тени"
// https://github.com/vvip-68/GyverPanelWiFi/blob/master/firmware/GyverPanelWiFi_v1.02/effects.ino
class EffectShadows : public EffectCalc {
private:
    uint16_t sPseudotime = 0;
    uint16_t sLastMillis = 0;
    uint16_t sHue16 = 0;

public:
    EffectShadows(LedFB &framebuffer) : EffectCalc(framebuffer){}
    bool run() override;
};

// ---- Эффект "Узоры"
// (c) kostyamat (Kostyantyn Matviyevskyy) 2020
// переделано kDn
// идея отсюда https://github.com/vvip-68/GyverPanelWiFi/
#define PATTERNS_BUFFSIZE   20
class EffectPatterns : public EffectCalc {
private:
    int8_t patternIdx = -1;
    int8_t lineIdx = 0;
    bool _subpixel = false;
    bool _sinMove = false;
    int8_t _speed = 1, _scale = 1;
    bool dir = false;
    byte csum = 0;
    byte _bri = 255U;
    byte buff[20 * 20];
    //LedFB buff(PATTERNS_BUFFSIZE, PATTERNS_BUFFSIZE);
    float xsin, ysin;
    unsigned long lastrun2;
    byte _sc = 0;
    float _speedX, _speedY;

    CHSV colorMR[12] = {
        CHSV(0, 0, 0),              // 0 - Black
        CHSV(HUE_RED, 255, 255),    // 1 - Red
        CHSV(HUE_GREEN , 255, 255),  // 2 - Green
        CHSV(HUE_BLUE, 255, 255),   // 3 - Blue
        CHSV(HUE_YELLOW, 255, 255), // 4 - Yellow
        CHSV(0, 0, 220),            // 5 - White
        CHSV(0, 255, 255),              // 6 - плавно меняеться в цикле (фон)
        CHSV(0, 255, 255),              // 7 - цвет равен 6 но +64
        CHSV(HUE_ORANGE, 255, 255),
        CHSV(HUE_PINK, 255, 255),
        CHSV(HUE_PURPLE, 255, 255),
        CHSV(HUE_AQUA, 255, 255),
    };

    String setDynCtrl(UIControl*_val) override;
    void drawPicture_XY();
    bool patternsRoutine();

public:
    EffectPatterns(LedFB &framebuffer) : EffectCalc(framebuffer){}
    void load() override;
    bool run() override;
};


// ***************************** "Стрелки" *****************************
// взято отсюда https://github.com/vvip-68/GyverPanelWiFi/
class EffectArrows : public EffectCalc {
private:
    float arrow_x[4], arrow_y[4], stop_x[4], stop_y[4];
    byte arrow_direction;             // 0x01 - слева направо; 0x02 - снизу вверх; 0х04 - справа налево; 0х08 - сверху вниз
    byte arrow_mode, arrow_mode_orig; // 0 - по очереди все варианты
                                      // 1 - по очереди от края до края экрана;
                                      // 2 - одновременно по горизонтали навстречу к ентру, затем одновременно по вертикали навстречу к центру
                                      // 3 - одновременно все к центру
                                      // 4 - по два (горизонталь / вертикаль) все от своего края к противоположному, стрелки смещены от центра на 1/3
                                      // 5 - одновременно все от своего края к противоположному, стрелки смещены от центра на 1/3
    bool arrow_complete, arrow_change_mode;
    byte arrow_hue[4];
    byte arrow_play_mode_count[6];      // Сколько раз проигрывать полностью каждый режим если вариант 0 - текущий счетчик
    byte arrow_play_mode_count_orig[6]; // Сколько раз проигрывать полностью каждый режим если вариант 0 - исходные настройки
    uint8_t _scale;
    float speedFactor;
    void arrowSetupForMode(byte mode, bool change);
    void arrowSetup_mode1();
    void arrowSetup_mode2();
    //void arrowSetup_mode3(;)
    void arrowSetup_mode4();

    String setDynCtrl(UIControl*_val) override;
public:
    EffectArrows(LedFB &framebuffer) : EffectCalc(framebuffer){}
    void load() override;
    bool run() override;
};

// ------ Эффект "Дикие шарики"
// (с) https://gist.github.com/bonjurroughs/9c107fa5f428fb01d484#file-noise-balls
class EffectNBals : public EffectCalc {
private:
    uint8_t lastSecond = 99;
    uint16_t speedy;// speed is set dynamically once we've started up
    uint16_t _scale;
    byte beat1, beat2 = 0;
    byte balls = 4;
    void balls_timer();
    void blur();
    bool nballsRoutine();

    String setDynCtrl(UIControl*_val) override;

public:
    EffectNBals(LedFB &framebuffer) : EffectCalc(framebuffer){}
    bool run() override;
};

// ------ Эффект "Притяжение"
// project Aurora
// доведено до ума - kostyamat
class EffectAttract : public EffectCalc {
private:
    const uint8_t spirocenterX = WIDTH / 2;
    const uint8_t spirocenterY = HEIGHT / 2;
    float speedFactor;
    float mass;    // Mass, tied to size
    float G;       // Gravitational Constant
    uint8_t _mass = 127;
    uint8_t _energy = 127;
    static const uint8_t count = HEIGHT *2 - WIDTH /2;
    bool loadingFlag = true;
    byte csum = 0;
    //Boid boids[AVAILABLE_BOID_COUNT];
    Boid boids[count];
    PVector location;   // Location
    String setDynCtrl(UIControl*_val) override;
    void setup();


    PVector attract(Boid m) {
        PVector force = location - m.location;   // Calculate direction of force
        float d = force.mag();                              // Distance between objects
        d = constrain(d, 5.0f, 32.0f);                        // Limiting the distance to eliminate "extreme" results for very close or very far objects
        force.normalize();                                  // Normalize vector (distance doesn't matter here, we just want this vector for direction)
        float strength = (G * mass * m.mass) / (d * d);      // Calculate gravitional force magnitude
        force *= strength;                                  // Get force vector --> magnitude * direction
        return force;
    }

public:
    EffectAttract(LedFB &framebuffer) : EffectCalc(framebuffer) {
        location = PVector(spirocenterX, spirocenterY);
        mass = 10;
        G = .5;
    }
    void load() override;
    bool run() override;
};

//------------ Эффект "Змейки"
// вариант субпикселя и поведения от kDn
class EffectSnake : public EffectCalc {
private:
    float hue;
    float speedFactor;
    int snakeCount;
    bool subPix = false;
    bool onecolor = false;
    enum Direction
{
  UP,
  DOWN,
  LEFT,
  RIGHT
};

struct Pixel
{
    float x;
    float y;
};

CRGB colors[SNAKE_LENGTH];
struct Snake
{
  float internal_counter = 0.0;
  float internal_speedf = 1.0;
  Pixel pixels[SNAKE_LENGTH];

  Direction direction;

  void newDirection()
  {
    switch (direction)
    {
    case UP:
    case DOWN:
      direction = random(0, 2) == 1 ? RIGHT : LEFT;
      break;

    case LEFT:
    case RIGHT:
      direction = random(0, 2) == 1 ? DOWN : UP;

    default:
      break;
    }
  };

  void shuffleDown(float speedy, bool subpix)
  {
    internal_counter+=speedy*internal_speedf;

    if(internal_counter>1.0){
        for (byte i = (byte)SNAKE_LENGTH - 1; i > 0; i--)
        {
            if(subpix)
                pixels[i] = pixels[i - 1];
            else {
                pixels[i].x = (uint8_t)pixels[i - 1].x;
                pixels[i].y = (uint8_t)pixels[i - 1].y;
            }
        }
        double f;
        internal_counter=modf(internal_counter, &f);
    }
  }

  void reset()
  {
    direction = UP;
    for (int i = 0; i < (int)SNAKE_LENGTH; i++)
    {
      pixels[i].x = 0;
      pixels[i].y = 0;
    }
  }

  void move(float speedy)
  {
    float inc = speedy*internal_speedf;

    switch (direction)
    {
    case UP:
      pixels[0].y = pixels[0].y >= HEIGHT ? inc : (pixels[0].y + inc);
      break;
    case LEFT:
      pixels[0].x = pixels[0].x >= WIDTH ? inc : (pixels[0].x + inc);
      break;
    case DOWN:
      pixels[0].y = pixels[0].y <= 0 ? HEIGHT - inc : pixels[0].y - inc;
      break;
    case RIGHT:
      pixels[0].x = pixels[0].x <= 0 ? WIDTH - inc : pixels[0].x - inc;
      break;
    }
  }

  void draw(CRGB colors[SNAKE_LENGTH], int snakenb, bool subpix, LedFB &fb, bool isDebug=false);
};

    Snake snakes[MAX_SNAKES];
    String setDynCtrl(UIControl*_val) override;
public:
    EffectSnake(LedFB &framebuffer) : EffectCalc(framebuffer){}
    void load() override;
    bool run() override;
};

//------------ Эффект "Nexus"
// База паттерн "Змейка" из проекта Аврора, 
// перенос и переписан - kostyamat
#define NEXUS_MIN   5
#define NEXUS_MAX (WIDTH)
class EffectNexus: public EffectCalc {
  struct Nexus{
    float posX{0};
    float posY{0};
    int8_t direct{0};           // направление точки 
    CRGB color{CRGB::Black};    // цвет точки
    float accel{0};             // персональное ускорение каждой точки
  };

    bool white = false;
    byte type = 1;
    uint8_t _scale = 1;
    bool randColor = false;
    //float windProgress{1};
	float speedFactor{1};
    std::vector<Nexus> nxdots{std::vector<Nexus>(NEXUS_MIN, Nexus())};

    void reload();
    void resetDot(Nexus &nx);
    String setDynCtrl(UIControl*_val) override;

  public:
    EffectNexus(LedFB &framebuffer) : EffectCalc(framebuffer){}
    bool run() override;
    void load() override;
};

// ----------- Эфеект "Змеиный Остров"
// (c) Сотнег
// База https://community.alexgyver.ru/threads/wifi-lampa-budilnik-obsuzhdenie-proekta.1411/post-53132
// адаптация и доработки kostyamat
class EffectTest : public EffectCalc {
private:
//#define MAX_SNAKES    (WIDTH * 2)          // максимальное количество червяков
    uint8_t SnakeNum;                        // выбранное количество червяков
    long  snakeLast[MAX_SNAKES] ;            // тут будет траектория тела червяка
    float snakePosX[MAX_SNAKES];             // тут будет позиция головы
    float snakePosY[MAX_SNAKES];             // тут будет позиция головы
    float snakeSpeedX[MAX_SNAKES];           // тут будет скорость червяка
    float snakeSpeedY[MAX_SNAKES];           // тут будет дробная часть позиции головы
    //float snakeTurn[MAX_SNAKES];           //не пригодилось пока что
    uint8_t snakeColor[MAX_SNAKES];          // тут будет начальный цвет червяка
    uint8_t snakeDirect[MAX_SNAKES];         //тут будет направление червяка
	float speedFactor;

    String setDynCtrl(UIControl*_val) override;
    void regen();

public:
    EffectTest(LedFB &framebuffer) : EffectCalc(framebuffer){}
    void load() override;
    bool run() override;
};

// ----------- Эфеект "Попкорн"
// (C) Aaron Gotwalt (Soulmate)
// адаптация и доработки kostyamat
#define POPCORN_ROCKETS 10
class EffectPopcorn : public EffectCalc {
private:
    uint8_t numRockets = POPCORN_ROCKETS;
    bool blurred = false;
    bool revCol = false;
    //bool tiltDirec;
    float speedFactor;
    float center = (float)WIDTH / 2.;

    typedef struct
    {
        float x, y, xd, yd;
        byte hue;
    } Rocket;

    std::vector<Rocket> rockets{std::vector<Rocket>(POPCORN_ROCKETS, Rocket())};

    void restart_rocket(uint8_t r);
    void reload();

    String setDynCtrl(UIControl*_val) override;
    //void setscl(const byte _scl) override; // перегрузка для масштаба

public:
    EffectPopcorn(LedFB &framebuffer) : EffectCalc(framebuffer){}
    void load() override;
    bool run() override;
};

//-------- Эффект "Детские сны"
// (c) Stepko https://editor.soulmatelights.com/gallery/505
#define WAVES_AMOUNT WIDTH
class EffectSmokeballs: public EffectCalc {
  private:
    struct Wave {
        uint16_t reg;
        uint16_t pos;
        float sSpeed;
        uint8_t maxMin;
        uint8_t waveColors;
    };
    uint8_t _scale = 1;
    //uint16_t reg[WAVES_AMOUNT];
    //uint16_t pos[WAVES_AMOUNT];
    //float sSpeed[WAVES_AMOUNT];
    //uint8_t maxMin[WAVES_AMOUNT];
    //uint8_t waveColors[WAVES_AMOUNT];
    float speedFactor = 0.1;
    std::array<Wave, WAVES_AMOUNT> waves;

    void shiftUp();
    void regen();
    String setDynCtrl(UIControl*_val) override;
  public:
    EffectSmokeballs(LedFB &framebuffer) : EffectCalc(framebuffer){}
    void load() override;
    bool run() override;
};

// ----------- Эффект "Ёлки-Палки"
// "Cell" (C) Elliott Kember из примеров программы Soulmate
// Spaider и Spruce (c) stepko
class EffectCell: public EffectCalc {
  private:
    const uint8_t Lines = 5;
	const bool glitch = abs((int)WIDTH-(int)HEIGHT) >= minDim/4;
	const byte density = 50;
    uint8_t Scale = 6;
    uint8_t _scale = 1;
    int16_t offsetX = 0;
    int16_t offsetY = 0;
    float x;
    uint8_t effId = 1;
    uint8_t hue;
    int16_t a;

	float speedFactor;
    void cell();
    void spider();
    void spruce();
    void vals();

  public:
    EffectCell(LedFB &framebuffer) : EffectCalc(framebuffer){}
    bool run() override;
    String setDynCtrl(UIControl*_val) override;
};

// ---------- Эффект "Тикси Ленд"
// (c)  Martin Kleppe @aemkei, https://github.com/owenmcateer/tixy.land-display
class EffectTLand: public EffectCalc {
  private:
    bool isSeq = false;
    byte animation = 0;
    bool ishue;
    bool ishue2;
    byte hue = 0;
    byte hue2 = 128;
    byte shift = 0;
    byte fine = 1;
    double t;
    void processFrame(LedFB &fb, double t, double x, double y);
    float code(double i, double x, double y);
    String setDynCtrl(UIControl*_val);
  public:
    EffectTLand(LedFB &framebuffer) : EffectCalc(framebuffer){}
    bool run() override;
};

// ----------- Эффект "Осцилятор"
// (c) Сотнег (SottNick)
class EffectOscilator: public EffectCalc {
  private:
    uint8_t hue, hue2;                                 // постепенный сдвиг оттенка или какой-нибудь другой цикличный счётчик
    uint8_t deltaHue, deltaHue2;                       // ещё пара таких же, когда нужно много
    uint8_t step;                                      // какой-нибудь счётчик кадров или последовательностей операций
    uint8_t deltaValue;  
    unsigned long timer;

    class oscillatingCell {
        public:
        byte red;
        byte blue;
        byte green;
        byte color;
    };

    oscillatingCell oscillatingWorld[WIDTH][HEIGHT];

    void drawPixelXYFseamless(float x, float y, CRGB color);
    int redNeighbours(uint8_t x, uint8_t y);
    int blueNeighbours(uint8_t x, uint8_t y);
    int greenNeighbours(uint8_t x, uint8_t y);
    void setCellColors(uint8_t x, uint8_t y);
    //String setDynCtrl(UIControl*_val) override;
  public:
    EffectOscilator(LedFB &framebuffer) : EffectCalc(framebuffer){}
    bool run() override;
    void load() override;
};

//------------ Эффект "Шторм" 
// (с) kostyamat 1.12.2020
#define DROP_CNT  (WIDTH*3)

class EffectWrain: public EffectCalc {
  private:

    struct Drop {
        float posX{0};
        float posY{0};
        uint8_t color{0};    // цвет капли
        float accell{0};     // персональное ускорение каждой капли
        uint8_t bri{0};      // яркость капли
    };

    static const uint8_t cloudHeight = HEIGHT / 5 + 1;
    float dotChaos;         // сила ветра
    int8_t dotDirect;       // направление ветра 
    bool clouds = false;
    bool storm = false;
    bool white = false;
    uint8_t _scale=1;
    byte type = 1;
    bool _flash;
    bool randColor = false;
    float windProgress = 0;
    float speedFactor = 0.5;
    uint32_t timer = 0;
    std::array<uint8_t, WIDTH * cloudHeight> _noise;
    std::vector<Drop> drops {std::vector<Drop>(DROP_CNT)};

    void reload();
    String setDynCtrl(UIControl*_val) override;
    bool Lightning(uint16_t chanse);
    void Clouds(bool flash);

  public:
    EffectWrain(LedFB &framebuffer) : EffectCalc(framebuffer){}
    bool run() override;
    void load() override;
};

//-------- по мотивам Эффектов Particle System -------------------------
// https://github.com/fuse314/arduino-particle-sys
// https://github.com/giladaya/arduino-particle-sys
// https://www.youtube.com/watch?v=S6novCRlHV8&t=51s
//при попытке вытащить из этой библиотеки только минимально необходимое выяснилось, что там очередной (третий) вариант реализации субпиксельной графики.
//ну его нафиг. лучше будет повторить визуал имеющимися в прошивке средствами.

// ============= ЭФФЕКТ Фея/Источник ===============
// (c) SottNick

#define trackingOBJECT_MAX_COUNT    (WIDTH * 3)  // максимальное количество отслеживаемых объектов (очень влияет на расход памяти)
#define enlargedOBJECT_MAX_COUNT    (WIDTH * 3) // максимальное количество сложных отслеживаемых объектов (меньше, чем trackingOBJECT_MAX_COUNT)

class EffectFairy : public EffectCalc {
private:
    float   trackingObjectPosX[trackingOBJECT_MAX_COUNT];
    float   trackingObjectPosY[trackingOBJECT_MAX_COUNT];
    float   trackingObjectSpeedX[trackingOBJECT_MAX_COUNT];
    float   trackingObjectSpeedY[trackingOBJECT_MAX_COUNT];
    float   trackingObjectShift[trackingOBJECT_MAX_COUNT];
    uint8_t trackingObjectHue[trackingOBJECT_MAX_COUNT];
    float   trackingObjectState[trackingOBJECT_MAX_COUNT];
    bool    trackingObjectIsShift[trackingOBJECT_MAX_COUNT];
    uint8_t enlargedObjectNUM;                                       // используемое в эффекте количество объектов

    Boid boids[2];

    uint8_t hue;
    uint8_t hue2;
    uint8_t step;
    uint8_t deltaValue;
    uint8_t deltaHue;
    uint8_t deltaHue2;
    float speedFactor;
    byte type = false;
    byte blur;
    uint8_t _video = 255;
    uint8_t gain;

    void particlesUpdate(uint8_t i);
    void fairyEmit(uint8_t i);
    void fountEmit(uint8_t i);
    bool fairy();
    void fount();
    //void setscl(const byte _scl) override; // перегрузка для масштаба
    String setDynCtrl(UIControl*_val) override;

public:
    EffectFairy(LedFB &framebuffer) : EffectCalc(framebuffer){}
    void load() override;
    bool run() override;
};

// ---------- Эффект "Бульбулятор"
// "Circles" (C) Elliott Kember https://editor.soulmatelights.com/gallery/11
// адаптация и переделка - kostyamat
#define NUMBER_OF_CIRCLES (num_leds / 16U)
class EffectCircles : public EffectCalc {
private:
    byte color;
    byte count;
    float speedFactor;
    byte _video = 255;
    byte gain;
    class Circle
    {
    public:
        //uint16_t offset;
        int16_t centerX;
        int16_t centerY;
        byte hue;
        float bpm = random(0, 255);

        void move() {
            centerX = random(0, WIDTH-1);
            centerY = random(0, HEIGHT-1);
        }
        
        void reset() {
            centerX = random(0, WIDTH-1);
            centerY = random(0, HEIGHT-1);
            hue = random(0, 255);
        }

        float radius() {
            float radius = EffectMath::fmap(triwave8(bpm), 0, 254, 0, 5); //beatsin16(bpm, 0, 500, 0, offset) / 100.0;
            return radius;
        }
    };

    Circle circles[NUMBER_OF_CIRCLES] = {};

    void drawCircle(LedFB &fb, Circle circle);
    String setDynCtrl(UIControl*_val) override;

public:
    EffectCircles(LedFB &framebuffer) : EffectCalc(framebuffer){}
    void load() override;
    bool run() override;

}; 

// ----------- Эффект "Бенгальские Огни"
// (c) stepko https://wokwi.com/arduino/projects/289797125785520649
// 06.02.2021
class EffectBengalL : public EffectCalc {
private:
    #define sparksNum  WIDTH*4

    float sparksPos[2][sparksNum];
    float sparksSpeed[2][sparksNum];
    byte sparksColor[sparksNum];
    float sparksSat[sparksNum];
    float sparksFade[sparksNum];
    uint8_t gPos[2];

    bool centerRun = true;
    byte period = 10;
    byte _x = WIDTH/2;
    byte _y = HEIGHT/2;
    float speedFactor;

    void regen(byte id);
    void phisics(byte id);
    String setDynCtrl(UIControl*_val) override;


public:
    EffectBengalL(LedFB &framebuffer) : EffectCalc(framebuffer){}
    void load() override;
    bool run() override;
};

// ----------- Эффект "Шары"
// (c) stepko and kostyamat https://wokwi.com/arduino/projects/289839434049782281
// 07.02.2021
class EffectBalls : public EffectCalc {
private:
#if WIDTH >= HEIGHT
    #define ballsAmount WIDTH
#else
    #define ballsAmount HEIGHT
#endif
    float ball[ballsAmount][4]; //0-PosY 1-PosX 2-SpeedY 3-SpeedX
    float radius[ballsAmount];
    bool rrad[ballsAmount];
    byte color[ballsAmount];
    const float radiusMax = (float)ballsAmount /5;

    float speedFactor;
    //void setspd(const byte _spd) override;
    String setDynCtrl(UIControl*_val) override;


public:
    EffectBalls(LedFB &framebuffer) : EffectCalc(framebuffer){}
    void load() override;
    bool run() override;
};

// ---------- Эффект-игра "Лабиринт"
class EffectMaze : public EffectCalc {
private:
    const uint8_t _mwidth, _mheight;
    const uint16_t maxSolves;
    std::vector<uint8_t> maze;
    int8_t playerPos[2];
    uint32_t labTimer;
    bool mazeMode = false;
    bool mazeStarted = false;
    uint8_t hue;
    CRGB color;
    CRGB playerColor = CRGB::White;

    bool loadingFlag = true;
    bool gameOverFlag = false;
    bool gameDemo = true;
    bool gamePaused = false;
    bool track = random8(0,2);  // будет ли трек игрока
    uint8_t buttons;

    unsigned long timer = millis(), gameTimer = 200;         // Таймер скорости игр

    void newGameMaze();
    void buttonsTickMaze();
    void movePlayer(int8_t nowX, int8_t nowY, int8_t prevX, int8_t prevY);
    void demoMaze();
    bool checkPath(int8_t x, int8_t y);
    void CarveMaze(std::vector<uint8_t> &maze, int width, int height, int x, int y);
    void GenerateMaze(std::vector<uint8_t> &maze, int width, int height);
    void SolveMaze(std::vector<uint8_t> &maze, int width, int height);

    bool checkButtons(){ return (buttons != 4); }

    String setDynCtrl(UIControl*_val) override;
    //void setspd(const byte _spd) override; // перегрузка для скорости
public:
    EffectMaze(LedFB &framebuffer);
    bool run() override;

}; 

// --------- Эффект "Вьющийся Цвет"
// (c) Stepko https://wokwi.com/arduino/projects/283705656027906572
class EffectFrizzles : public EffectCalc {
private:
    float _speed;
    float _scale;
    //String setDynCtrl(UIControl*_val) override;

public:
    EffectFrizzles(LedFB &framebuffer) : EffectCalc(framebuffer){}
    //void load() override;
    bool run() override;
};

// --------- Эффект "Северное Сияние"
// (c) kostyamat 05.02.2021
// идеи подсмотрены тут https://www.reddit.com/r/FastLED/comments/jyly1e/challenge_fastled_sketch_that_fits_entirely_in_a/

// Палитры, специально созданные под этот эффект, огромная благодарность @Stepko
static const TProgmemRGBPalette16 GreenAuroraColors_p FL_PROGMEM ={0x000000, 0x003300, 0x006600, 0x009900, 0x00cc00,0x00ff00, 0x33ff00, 0x66ff00, 0x99ff00,0xccff00, 0xffff00, 0xffcc00, 0xff9900, 0xff6600, 0xff3300, 0xff0000};
static const TProgmemRGBPalette16 BlueAuroraColors_p FL_PROGMEM ={0x000000, 0x000033, 0x000066, 0x000099, 0x0000cc,0x0000ff, 0x3300ff, 0x6600ff, 0x9900ff,0xcc00ff, 0xff00ff, 0xff33ff, 0xff66ff, 0xff99ff, 0xffccff, 0xffffff};
static const TProgmemRGBPalette16 NeonAuroraColors_p FL_PROGMEM ={0x000000, 0x003333, 0x006666, 0x009999, 0x00cccc,0x00ffff, 0x33ffff, 0x66ffff, 0x99ffff,0xccffff, 0xffffff, 0xffccff, 0xff99ff, 0xff66ff, 0xff33ff, 0xff00ff};

class EffectPolarL : public EffectCalc {
private:
    const byte numpalettes = 14;
    unsigned long timer = 0;
    float adjastHeight = 1;
    uint16_t adjScale = 1;
    byte pal = 0;
    uint16_t _scale = 30;
    byte flag = 0;
	byte _speed = 16;

    //void setscl(const byte _scl) override;
    String setDynCtrl(UIControl*_val) override;
    void palettemap(std::vector<PGMPalette*> &_pals, const uint8_t _val, const uint8_t _min, const uint8_t _max) override;
    void palettesload() override;

public:
    EffectPolarL(LedFB &framebuffer) : EffectCalc(framebuffer){}
    void load() override;
    bool run() override;
};

// --------- Эффект "Космо-Гонщик"
// (c) Stepko + kostyamat https://editor.soulmatelights.com/my-patterns/655
class EffectRacer: public EffectCalc {
private:
    float posX = random(0, WIDTH-1);
    float posY = random(0, HEIGHT-1);
    uint8_t aimX = random(0, WIDTH)-1;
    uint8_t aimY = random(0, HEIGHT-1);
    float radius = 0;
    byte hue = millis()>>1; //random(0, 255);
    CRGB color;
    float speedFactor;
    float addRadius;
    float angle;
    byte starPoints = random(3, 7);

    const float _speed = (float)num_leds / 256; // Нормализация скорости для разных размеров матриц
    const float _addRadius = (float)num_leds / 4000;   // Нормализация скорости увеличения радиуса круга для разных матриц


    void aimChange();
    void drawStarF(float x, float y, float biggy, float little, int16_t points, float dangle, CRGB color);
    //void setspd(const byte _spd) override;
    String setDynCtrl(UIControl*_val) override;

public:
    EffectRacer(LedFB &framebuffer) : EffectCalc(framebuffer){}
    void load() override;
    bool run() override;
};

// ----------------- Эффект "Магма"
// (c) Сотнег (SottNick) 2021
// адаптация и доводка до ума - kostyamat
#define MAGMA_MIN_OBJ   (WIDTH/2)
#define MAGMA_MAX_OBJ   (WIDTH*3)
class EffectMagma: public EffectCalc {
private:

    struct Magma {
        float posX{0}, posY{0};
        float speedX{0};
        float shift{0};
        uint8_t hue{0};
    };

    float ff_y{0}, ff_z{0};         // большие счётчики
    //control magma bursts
    const byte deltaValue = 6U;     // 2-12 
    const byte deltaHue = 8U;       // высота языков пламени должна уменьшаться не так быстро, как ширина
    const float gravity = 0.1;
    uint8_t step = WIDTH;
    float speedFactor{0.1};
    std::array<uint8_t, HEIGHT> shiftHue;
    std::vector<Magma> particles{std::vector<Magma>(WIDTH, Magma())};

    void palettesload();
    void regen();
    void leapersMove_leaper(Magma &l);
    void leapersRestart_leaper(Magma &l);
    String setDynCtrl(UIControl*_val) override;

public:
    EffectMagma(LedFB &framebuffer) : EffectCalc(framebuffer){}
    void load() override;
    bool run() override;
};

// ------------- Эффект "Флаги"
// (c) Stepko + kostyamat
// 17.03.21
// https://editor.soulmatelights.com/gallery/739-flags
class EffectFlags: public EffectCalc {
private:
    const float DEVIATOR = 512. / WIDTH;
    float counter;
    uint8_t flag = 0;
    uint8_t _flag;
    uint8_t count;
    uint8_t _speed; // 1 - 16
    const uint8_t CHANGE_FLAG = 30; // >= 10 Autochange

    uint8_t thisVal;
    uint8_t thisMax;

    //Germany
    void germany(uint8_t i)
    {
        for (uint8_t j = 0; j < HEIGHT; j++)
        {
            fb.pixel(i, j) += 
            (j < thisMax - HEIGHT / 4) ? CHSV(68, 255, thisVal) : (j < thisMax + HEIGHT / 4) ? CHSV(0, 255, thisVal)
            : CHSV(0, 0, thisVal / 2.5);
        }
    }

    //Ukraine
    void ukraine(uint8_t i)
    {
        for (uint8_t j = 0; j < HEIGHT; j++)
        {
            fb.pixel(i, j) += 
            (j < thisMax) ? CHSV(50, 255, thisVal) : CHSV(150, 255, thisVal);
        }
    }

    //Belarus
    void belarus(uint8_t i)
    {
        for (uint8_t j = 0; j < HEIGHT; j++)
        {
            fb.pixel(i, j) += 
            (j < thisMax - HEIGHT / 4) ? CHSV(0, 224, thisVal) : (j < thisMax + HEIGHT / 4) ? CHSV(0, 0, thisVal)
            : CHSV(0, 224, thisVal);
        }
    }

    //Russia
    void russia(uint8_t i)
    {
        for (uint8_t j = 0; j < HEIGHT; j++)
        {
            fb.pixel(i, j) += 
            (j < thisMax - HEIGHT / 4) ? CHSV(0, 255, thisVal) : (j < thisMax + HEIGHT / 4) ? CHSV(150, 255, thisVal)
            : CHSV(0, 0, thisVal);
        }
    }

    //Poland
    void poland(uint8_t i)
    {
        for (uint8_t j = 0; j < HEIGHT; j++)
        {
            fb.pixel(i, j) += 
            (j < thisMax + 1) ? CHSV(248, 214, (float)thisVal * 0.83) : CHSV(25, 3, (float)thisVal * 0.91);
        }
    }

    //The USA
    void usa(uint8_t i)
    {
        for (uint8_t j = 0; j < HEIGHT; j++)
        {
            fb.pixel(i, j) +=
            ((i <= WIDTH / 2) && (j + thisMax > HEIGHT - 1 + HEIGHT / 16)) ? 
            ((i % 2 && ((int)j - HEIGHT / 16 + thisMax) % 2) ? 
            CHSV(160, 0, thisVal) : CHSV(160, 255, thisVal)) 
            : ((j + 1 + thisMax) % 6 < 3 ? CHSV(0, 0, thisVal) : CHSV(0, 255, thisVal));
        }
    }

    //Italy
    void italy(uint8_t i)
    {
        for (uint8_t j = 0; j < HEIGHT; j++)
        {
            fb.pixel(i, j) += 
            (i < WIDTH / 3) ? CHSV(90, 255, thisVal) : (i < WIDTH - 1 - WIDTH / 3) ? CHSV(0, 0, thisVal)
            : CHSV(0, 255, thisVal);
        }
    }

    //France
    void france(uint8_t i)
    {
        for (uint8_t j = 0; j < HEIGHT; j++)
        {
            fb.pixel(i, j) += 
            (i < WIDTH / 3) ? CHSV(160, 255, thisVal) : (i < WIDTH - 1 - WIDTH / 3) ? CHSV(0, 0, thisVal)
            : CHSV(0, 255, thisVal);
        }
    }

    //UK
    void uk(uint8_t i)
    {
        for (uint8_t j = 0; j < HEIGHT; j++)
        {
            fb.pixel(i, j) += 
            (
                (
                    (i > WIDTH / 2 + 1 || i < WIDTH / 2 - 2) && ((i - (int)(j + thisMax - (HEIGHT * 2 - WIDTH) / 2) > -2) && (i - (j + thisMax - (HEIGHT * 2 - WIDTH) / 2) < 2))
                )
                    ||
                (
                    (i > WIDTH / 2 + 1 || i < WIDTH / 2 - 2) && ( (((int)WIDTH - 1 - i - ((int)j + thisMax - (int)(HEIGHT * 2 - WIDTH) / 2) > -2) && (WIDTH - 1 - i - (int)(j + thisMax - (HEIGHT * 2 - WIDTH) / 2) < 2)) )
                )
            || 
            (WIDTH / 2 - i == 0) || (WIDTH / 2 - 1 - i == 0) 
            || 
            ((HEIGHT - (j + thisMax)) == 0) || ((HEIGHT - 1 - (j + thisMax)) == 0)) ? 
            CHSV(0, 255, thisVal) 
            : 
            (((i - (int)(j + thisMax - (HEIGHT * 2 - WIDTH) / 2) > -4) 
            && (i - (j + thisMax - (HEIGHT * 2 - WIDTH) / 2) < 4)) 
            || 
            (((int)WIDTH - 1 - i - (int)(j + thisMax - (HEIGHT * 2 - WIDTH) / 2) > -4) 
            && (WIDTH - 1 - i - (int)(j + thisMax - (HEIGHT * 2 - WIDTH) / 2) < 4)) 
            || (WIDTH / 2 + 1 - i == 0) || (WIDTH / 2 - 2 - i == 0) 
            || (HEIGHT + 1 - (j + thisMax) == 0) || (HEIGHT - 2 - (int)(j + thisMax) == 0)) ? 
            CHSV(0, 0, thisVal)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  : CHSV(150, 255, thisVal);
        }
    }

    //Spain
    void spain(uint8_t i)
    {
        for (uint8_t j = 0; j < HEIGHT; j++)
        {
            fb.pixel(i, j) += 
            (j < thisMax - HEIGHT / 3) ? 
            CHSV(250, 224, (float)thisVal * 0.68) : (j < thisMax + HEIGHT / 3) ? CHSV(64, 255, (float)thisVal * 0.98)
            : CHSV(250, 224, (float)thisVal * 0.68);
        }
    }


    void changeFlags();
    String setDynCtrl(UIControl*_val) override;

public:
    EffectFlags(LedFB &framebuffer) : EffectCalc(framebuffer){}
    //void load () override;
    bool run() override;
};

// --------------------- Эффект "Звездный Десант"
// Starship Troopers https://editor.soulmatelights.com/gallery/839-starship-troopers
// Based on (c) stepko`s codes https://editor.soulmatelights.com/gallery/639-space-ships
// reworked (c) kostyamat (subpixel, shift speed control, etc) 08.04.2021
class EffectStarShips: public EffectCalc {
private:
    byte _scale = 8;
    const byte DIR_CHARGE = 2; // Chance to change direction 1-5
    uint16_t chance = 4096;

    byte dir = 3;
    int8_t _dir = 0;
    byte count = 0;
    uint8_t _fade = 20;

	float speedFactor = 0.5;

    void draw(float x, float y, CRGB color);
    String setDynCtrl(UIControl*_val) override;

public:
    EffectStarShips(LedFB &framebuffer) : EffectCalc(framebuffer){}
    bool run() override;
    void load() override;
};
#ifdef MIC_EFFECTS
/* -------------- эффект "VU-meter"
    (c) G6EJD, https://www.youtube.com/watch?v=OStljy_sUVg&t=0s
    reworked by s-marley https://github.com/s-marley/ESP32_FFT_VU
    adopted for FireLamp_jeeUI by kostyamat, kDn
    reworked and updated (c) kostyamat 24.04.2021
*/
class EffectVU: public EffectCalc {
private:
    CRGBPalette16 gradPal[5] = { 
    purple_gp,    rainbowsherbet_gp, 
    redyellow_gp, Colorfull_gp, es_ocean_breeze_068_gp
    };
    uint8_t NUM_BANDS = WIDTH;
    uint8_t BAR_WIDTH =  (WIDTH  / (NUM_BANDS - 1));
    uint8_t calcArray = 1; // уменьшение частоты пересчета массива
    uint8_t colorTimer = 0;
    const uint8_t colorDev = 256/TOP;
    // Sampling and FFT stuff
    float peak[WIDTH];              // The length of these arrays must be >= NUM_BANDS
    float oldBarHeights[WIDTH];
    float bandValues[WIDTH];

    float samp_freq;
    double last_freq = 0;
    uint8_t last_min_peak, last_max_peak;
    float maxVal;
    float threshold;
    byte tickCounter;
    byte colorType;

    float amplitude = 1.0;
    int effId = 0;
    bool type = false;
    bool colorShifting = false;
    const float speedFactorVertical = (float)HEIGHT / 16;
    bool averaging = true;

    String setDynCtrl(UIControl*_val) override;
    void horizontalColoredBars(uint8_t band, float barHeight, uint8_t type = 0, uint8_t colorShift = 0);
    void paletteBars(uint8_t band, float barHeight, CRGBPalette16& palette, uint8_t colorShift = 0);
    void verticalColoredBars(uint8_t band, float barHeight, uint8_t type = 0, uint8_t colorShift = 0);
    void centerBars(uint8_t band, float barHeight, CRGBPalette16& palette, uint8_t colorShift = 0);
    void whitePeak(uint8_t band);
    void outrunPeak(uint8_t band, CRGBPalette16& palette, uint8_t colorShift = 0);
    void waterfall(uint8_t band, uint8_t barHeight);

public:
    EffectVU(LedFB &framebuffer) : EffectCalc(framebuffer){}
    bool run() override;
    void load() override;
};
#endif

// ----------- Эффект "Огненная лампа"
// https://editor.soulmatelights.com/gallery/546-fire
// (c) Stepko 17.06.21
// sparks (c) kostyamat 10.01.2022 https://editor.soulmatelights.com/gallery/1619-fire-with-sparks
class EffectFire2021 : public EffectCalc {
private:
    byte _pal = 8;
    byte _scale = 32;
	byte speedFactor;
    uint32_t t;
    bool withSparks = false;

    const uint8_t sparksCount = WIDTH / 4;
    const uint8_t spacer = HEIGHT/4;

    class Spark {
    private:
        CRGB color;
        uint8_t Bri;
        uint8_t Hue;
        float x, y, speedy = 1;
    
    public:
        void addXY(float nx, float ny, LedFB &fb) {
            EffectMath::drawPixelXYF(x, y, 0, fb);
            x += nx;
            y += ny * speedy;
        }

        float getY() { return y; }

        void reset(LedFB &fb) {
            uint32_t peak = 0;
            speedy = (float)random(5, 30) / 10;
            y = random((HEIGHT/4) * 5, (HEIGHT /2) * 5) / 5;
            for (uint8_t i=0; i < WIDTH; i++) {
                if (fb.pixel(i, y).getLuma() > peak){
                    peak = fb.pixel(i, y).getLuma();
                    x = i;
                }
            }
            color = fb.pixel(x, y);
        }

        void draw(LedFB &fb) {
            color.fadeLightBy(256 / (HEIGHT));
            EffectMath::drawPixelXYF(x, y, color, fb);
        }
    }; 

    std::vector<Spark> sparks{std::vector<Spark>(sparksCount, Spark())};

    String setDynCtrl(UIControl*_val) override;
    void palettesload() override;

public:
    EffectFire2021(LedFB &framebuffer) : EffectCalc(framebuffer){}
    void load() override;
    bool run() override;
};

// ----------- Эффект "Пятнашки"
// https://editor.soulmatelights.com/gallery/1471-puzzles-subpixel
// (c) Stepko 10.12.21
class EffectPuzzles : public EffectCalc {
private:
private:
    byte psizeX = 4;
    byte psizeY = 4;
    uint8_t pcols, prows;
    float speedFactor = 0.1;
    std::vector< std::vector<uint8_t> > puzzle;
    byte color;
    byte z_dot[2];
    byte step;
    int8_t move[2] = {0, 0};
    float shift[2] = { 0, 0 };
    bool XorY;

    void draw_square(byte x1, byte y1, byte x2, byte y2, byte col);
    void draw_squareF(float x1, float y1, float x2, float y2, byte col);
    void regen();

    String setDynCtrl(UIControl*_val) override;

public:
    EffectPuzzles(LedFB &framebuffer) : EffectCalc(framebuffer){ regen(); }
    void load() override;
    bool run() override;
};

// ============= Эффект Цветные драже ===============
// (c) SottNick
//по мотивам визуала эффекта by Yaroslaw Turbin 14.12.2020
//https://vk.com/ldirko программный код которого он запретил брать
class EffectPile : public EffectCalc {
private:
    uint8_t pcnt = 0U, _scale;
    String setDynCtrl(UIControl*_val) override;

public:
    EffectPile(LedFB &framebuffer) : EffectCalc(framebuffer){}
    void load() override;
    bool run() override;
};

// ============= Эффект ДНК ===============
// (c) Stepko
// https://editor.soulmatelights.com/gallery/1520-dna
//по мотивам визуала эффекта by Yaroslaw Turbin
//https://vk.com/ldirko программный код которого он запретил брать
class EffectDNA : public EffectCalc {
private:
    float a = (256.0 / (float)WIDTH);
    float t = 0.0;
    float speedFactor = 0.5;
    bool flag = true; 
    bool bals = false;
    uint8_t type = 0, _type = 1, _scale = 16;
    
    String setDynCtrl(UIControl*_val) override;

public:
    EffectDNA(LedFB &framebuffer) : EffectCalc(framebuffer){}
    //void load() override;
    bool run() override;
};

// ----------- Эффект "Дым"
// based on cod by @Stepko (c) 23/12/2021
class EffectSmoker : public EffectCalc {
private:
    byte color, saturation;
    byte _scale = 30, glitch;
	float speedFactor;
    float t;

    String setDynCtrl(UIControl*_val) override;

public:
    EffectSmoker(LedFB &framebuffer) : EffectCalc(framebuffer){}

    bool run() override;
};

// ----------- Эффект "Мираж"
// based on cod by @Stepko (c) 23/12/2021
class EffectMirage : public EffectCalc {
private:
    const float div = 10.;
    const uint16_t width = (WIDTH - 1) * div, height = HEIGHT * div;
    uint16_t _speed;
    byte color;
    bool colorShift = false;
    byte buff[WIDTH + 2][HEIGHT + 2];
    bool a = true;
	float speedFactor;


    String setDynCtrl(UIControl*_val) override;
    void drawDot(float x, float y, byte a);
    void blur();

public:
    EffectMirage(LedFB &framebuffer) : EffectCalc(framebuffer){}
    bool run() override;
};

// -------------------- Эффект "Акварель"
// (c) kostyamat 26.12.2021
// https://editor.soulmatelights.com/gallery/1587-oil
#define BLOT_SIZE WIDTH/2
#define BLOT_COUNT 1U
class EffectWcolor : public EffectCalc {
private:
    float speedFactor;
    uint8_t bCounts = {BLOT_COUNT};
    uint8_t blur;
    bool mode = false;
    float t;

    class Blot {
    private:
        byte hue, sat;
        float bri;
        int x0, y0;
        float x[BLOT_SIZE]; 
        float y[BLOT_SIZE];

    public:

        void appendXY(float nx, float ny) {
            for (byte i = 0; i < BLOT_SIZE; i++) {
                x[i] += nx;
                y[i] += ny; 
            }
        }
        

        void reset(byte num, byte Counts) {     // wtf??? num and Counts are unused
            x0 = random(-5, WIDTH - 5);
            float y0 = EffectMath::randomf(-1, HEIGHT+1);
            uint8_t dy;
            for (uint8_t i = 0; i < BLOT_SIZE; i++) {
                bool f = random(0,2);
                dy = random(0, 2); 
                x[i] = x0 + i;
                if (f)
                y[i] = float((i ? y[i-1] : y0) + dy);
                else 
                y[i] = float((i ? y[i-1] : y0) - dy);
            }
            hue = random(0, 256);
            sat = random(160, 256);
            bri = random(128, 256);
            
        }

        double getY() {
            double result = y[0];
            for (uint8_t i = 1; i < BLOT_SIZE; i++) {
                if (y[i] > result) result = y[i];
            }
            return result;
        }

        void drawing(LedFB &fb) {
            for (uint8_t i = 0; i < BLOT_SIZE; i++) {
                byte bright = constrain((float)bri / fb.cfg.h() * (y[i] + fb.cfg.h() - y0), 32, 255);
                if (y[i] > -0.1)
                    EffectMath::drawPixelXYF(x[i], y[i], CHSV(hue, sat, bright), fb, 0);
            }
        }

    };


    std::vector<Blot> blots = std::vector<Blot>(BLOT_COUNT);

    String setDynCtrl(UIControl*_val) override;

public:
    EffectWcolor(LedFB &framebuffer) : EffectCalc(framebuffer){}
    void load() override;
    bool run() override;
};

// ----------- Эффект "Неопалимая купина"
//RadialFire
// (c) Stepko and Sutaburosu https://editor.soulmatelights.com/gallery/1570-radialfire
//23/12/21
class EffectRadialFire : public EffectCalc {
private:
    const int8_t maximum = max(WIDTH, HEIGHT);
    const int8_t centre = (max(WIDTH, HEIGHT) / 2);
    const uint8_t X = WIDTH > HEIGHT ? 0: (WIDTH - HEIGHT) /2; 
    const uint8_t Y = WIDTH < HEIGHT ? 0: (HEIGHT - WIDTH) /2;
    std::vector<std::vector<float>> XY_angle = std::vector<std::vector<float>>(max(WIDTH, HEIGHT), std::vector<float>(max(WIDTH, HEIGHT), 0));
    std::vector<std::vector<float>> XY_radius = std::vector<std::vector<float>>(max(WIDTH, HEIGHT), std::vector<float>(max(WIDTH, HEIGHT), 0));
    float t;
    float speedFactor;
    uint8_t _scale;
    bool mode = false;


    String setDynCtrl(UIControl*_val) override;
    void palettesload();

public:
    EffectRadialFire(LedFB &framebuffer) : EffectCalc(framebuffer){}
    void load() override;
    bool run() override;
};

class EffectSplashBals : public EffectCalc {
private:
    uint8_t count = 3;
    uint8_t hue;
    const uint8_t dev = 5;
    const float R = (float)num_leds/128;

    struct Ball{
        float x1{0}, y1{0};
        float x2{0}, y2{0};
        uint8_t iniX1{0}, iniX2{0};
        uint8_t iniY1{0}, iniY2{0};
    };

    std::array<Ball, 3> balls;

    float speedFactor{1};

    float dist(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);

    String setDynCtrl(UIControl*_val) override;

public:
    EffectSplashBals(LedFB &framebuffer) : EffectCalc(framebuffer){}
    void load() override;
    bool run() override;
};

/* Эффект "Цветение"
т.к. эффект отсутсвует, ставим сюда настроечную таблицу для проверки ориентации матрицы
*/
class EffectMira : public EffectCalc {
public:
    EffectMira(LedFB &framebuffer) : EffectCalc(framebuffer){}
    void load() override;
    bool run() override { return false; };
};
