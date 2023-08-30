/*
Copyright © 2023 Emil Muratov (https://github.com/vortigont/FireLamp_JeeUI)
Copyright © 2020 Dmytro Korniienko (https://github.com/DmytroKorniienko/FireLamp_EmbUI)
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

#include "ledfb.hpp"
//#include "log.h"

// Timings from FastLED chipsets.h
// WS2812@800kHz - 250ns, 625ns, 375ns
// время "отправки" кадра в матрицу, мс. где 1.5 эмпирический коэффициент
// #define FastLED_SHOW_TIME = WIDTH * HEIGHT * 24 * (0.250 + 0.625) / 1000 * 1.5

// An object ref I'll use to access LED device
OverlayEngine *display = nullptr;

// compatibility buff reference
LedFB *mx = nullptr;

// Kostyamat's "invisible" pixel :) current effects code can't live w/o it
static CRGB blackhole;

// copy via assignment
LedFB& LedFB::operator=(LedFB const& rhs){
    fb = rhs.fb;
    cfg=rhs.cfg;
    return *this;
}

LedFB::LedFB(LedFB&& rhs) noexcept : fb(std::move(rhs.fb)), cfg(rhs.cfg){
    cled = rhs.cled;
    if (rhs.cled){  // steal cled pointer, if set
        rhs.cled = nullptr;
    }
    _reset_cled();      // if we moved data from rhs, than need to reset cled controller
    //LOG(printf, "Move Constructing: %u From: %u\n", reinterpret_cast<size_t>(fb.data()), reinterpret_cast<size_t>(rhs.fb.data()));
};

LedFB& LedFB::operator=(LedFB&& rhs){
    fb = std::move(rhs.fb);
    cfg=rhs.cfg;

    if (cled && rhs.cled && (cled != rhs.cled)){
        /* oops... we are moving from a buff binded to some other cled controller
        * since there is no method to detach active controller from CFastLED
        * than let's use a dirty WA - bind a blackhole to our cled and steal rhs's ptr
        */
        cled->setLeds(&blackhole, 1);
    }

    if (rhs.cled){ cled = rhs.cled; rhs.cled = nullptr; }   // steal a pointer from rhs
    _reset_cled();      // if we moved data from rhs, than need to reset cled controller
    //LOG(printf, "Move assign: %u from: %u\n", reinterpret_cast<size_t>(fb.data()), reinterpret_cast<size_t>(rhs.fb.data()));
    // : fb(std::move(rhs.fb)), cfg(rhs.cfg){ LOG(printf, "Move Constructing: %u From: %u\n", reinterpret_cast<size_t>(&fb), reinterpret_cast<size_t>(&rhs.fb)); };
    return *this;
}

LedFB::~LedFB(){
    if (cled){
        /* oops... somehow we ended up in destructor with binded cled controller
        * since there is no method to detach active controller from CFastLED
        * than let's use a dirty WA - bind a blackhole to cled untill some other buffer
        * regains control
        */
       cled->setLeds(&blackhole, 1);
    }
}

// matrix transformation
size_t LedFB::transpose(unsigned x, unsigned y) const {
    unsigned idx = y*cfg.w()+x;
    if ( cfg.vertical() ){
        // verticaly ordered stripes
        bool ivm{cfg.hmirror()}, ihm{cfg.vmirror()};                // reverse mirror
        bool virtual_mirror = (cfg.snake() && x%2) ? !ihm : ihm;    // for snake-shaped strip, invert vertical odd columns
        size_t xx = virtual_mirror ? cfg.w() - idx/cfg.h()-1 : idx/cfg.h();
        size_t yy = ivm ? cfg.h()-idx%cfg.h()-1 : idx%cfg.h();
        return yy * cfg.w() + xx;
    } else {
        // horizontaly ordered stripes
        bool virtual_mirror = (cfg.snake() && y%2) ? !cfg.hmirror() : cfg.hmirror(); // for snake-shaped displays, invert horizontal odd rows
        size_t yy = cfg.vmirror() ? cfg.h() - idx/cfg.w()-1 : idx/cfg.w();
        size_t xx = virtual_mirror ? cfg.w()-idx%cfg.w()-1 : idx%cfg.w();
        return yy * cfg.w() + xx;
    }
}

CRGB& LedFB::at(size_t i){ return i < fb.size() ? fb.at(i) : blackhole; };

CRGB& LedFB::at(unsigned x, unsigned y){
    return ( x >= cfg.w() || y >= cfg.h() ) ? blackhole : at(transpose(x,y));
};

void LedFB::fill(const CRGB &color){ fb.assign(fb.size(), color); };

void LedFB::clear(){ fill(CRGB::Black); };

bool LedFB::bind(CLEDController *pLed){
    if (!pLed) return false;  // some empty pointer

    /* since there is no method to unbind CRGB buffer from CLEDController,
    than if there is a pointer already exist we refuse to rebind to a new cled.
    It's up to user to deal with it
    */
    if (cled && (cled !=pLed)) return false;

    cled = pLed;
    _reset_cled();
    return true;
}

bool LedFB::swap(LedFB& rhs){
    if (rhs.fb.size() != fb.size()) return false;   // won't swap buffers of different size
    std::swap(fb, rhs.fb);
    // update CLEDControllers
    _reset_cled();
    rhs._reset_cled();
    return true;
}

void LedFB::resize(uint16_t w, uint16_t h){
    cfg._w = w; cfg._h = h;
    fb.reserve(w*h);
    _reset_cled();
    clear();
};

void LedFB::rebind(LedFB &rhs){
    //LOG(println, "LedFB rebind");
    std::swap(cled, rhs.cled);  // swap pointers, if any
    _reset_cled();
    rhs._reset_cled();
}

// *** OverlayEngine implementation ***

OverlayEngine::OverlayEngine(int gpio){
    wsstrip = new(std::nothrow) ESP32RMT_WS2812B<COLOR_ORDER>(gpio);
}

bool OverlayEngine::makeCanvas(Mtrx_cfg &cfg){
    if (cled) return false; // this function is not idempotent, so refuse to mess with existing controller

    if (!canvas) canvas = std::make_unique<LedFB>(cfg);

    if (wsstrip && canvas){
        // attach buffer to RMT engine
        cled = &FastLED.addLeds(wsstrip, canvas->data(), canvas->size());
        // hook framebuffer to contoller
        canvas->bind(cled);
        show();
        return true;
    }

    return false;   // somethign went either wrong or already been setup 
}

void OverlayEngine::show(){
    if (!overlay.expired() && canvas->persistent && !backbuff)    // check if I need to switch to back buff due to canvas persistency and overlay data present 
        _switch_to_bb();

    // check if back-buffer is present but no longer needed (if no overlay present or canvas is not persistent anymore)
    if (backbuff && (overlay.expired() || !canvas->persistent)){
        //LOG(println, "BB release");
        canvas->rebind(*backbuff.get());
        backbuff.release();
    }

    _ovr_overlap(); // apply overlay to either canvas or back buffer, if bb is present
    FastLED.show();
};

void OverlayEngine::clear(){
    if (!canvas) return;
    canvas->clear();
    if (backbuff)
        backbuff->clear();
    FastLED.show();
}

std::shared_ptr<LedFB> OverlayEngine::getOverlay(){
    auto p = overlay.lock();
    if (!p){
        // no overlay exist at the moment
        p = std::make_shared<LedFB>(canvas->cfg);
        overlay = p;
    }
    return p;
}

void OverlayEngine::_ovr_overlap(){
    if (overlay.expired()) return;     // no overlay data exist

    auto ovr = overlay.lock();

    if (canvas->size() != ovr->size()) return;  // a safe-check for buffer sizes

    auto ovr_iterator = ovr->begin();

    if (backbuff.get()){
        auto bb_iterator = backbuff->begin();
        // fill BackBuffer with either canvas or overlay based on keycolor
        for (auto i = canvas->begin(); i != canvas->end(); ++i ){
            *bb_iterator = (*ovr_iterator == _transparent_color) ? *i : *ovr_iterator;
            ++ovr_iterator;
            ++bb_iterator;
        }
    } else {
        // apply all non key-color pixels to canvas
        for (auto i = canvas->begin(); i != canvas->end(); ++i ){
            if (*ovr_iterator != _transparent_color)
                *i = *ovr_iterator;
            ++ovr_iterator;
        }
    }   
}

void OverlayEngine::_switch_to_bb(){
    //LOG(println, "Switch to BB");
    backbuff = std::make_unique<LedFB>(*canvas);
    backbuff->rebind(*canvas);    // switch backend binding
}