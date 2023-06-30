#pragma once
#include <string_view>

/** набор служебных текстовых констант (не для локализации)
 */
static const char TCONST_1609459200[] PROGMEM = "1609459200";
static const char TCONST_act[] PROGMEM = "act";
static const char TCONST_add_lamp_config[] PROGMEM = "add_lamp_config";
static const char TCONST_afS[] PROGMEM = "afS";
static const char TCONST_alarmPT[] PROGMEM = "alarmPT";
static const char TCONST_alarmP[] PROGMEM = "alarmP";
static const char TCONST_alarmSound[] PROGMEM = "alarmSound";
static const char TCONST_alarmT[] PROGMEM = "alarmT";
static const char TCONST_Alarm[] PROGMEM = "Alarm";
static const char TCONST_AUX[] PROGMEM = "AUX";
static const char TCONST_aux_gpio[] PROGMEM = "aux_gpio";             // AUX gpio
static const char TCONST_aux_ll[] PROGMEM = "aux_ll";                 // AUX logic level
static const char TCONST_bactList[] PROGMEM = "bactList";
static const char TCONST_blabel[] PROGMEM = "blabel";
static const char TCONST_bparam[] PROGMEM = "bparam";
static const char TCONST_bright[] PROGMEM = "bright";
static const char TCONST_Btn[] PROGMEM = "Btn";
static const char TCONST_buttList[] PROGMEM = "buttList";
static const char TCONST_butt_conf[] PROGMEM = "butt_conf";
static const char TCONST_clicks[] PROGMEM = "clicks";
static const char TCONST_CLmt[] PROGMEM = "CLmt"; // лимит тока
static const char TCONST_control[] PROGMEM = "control";
static const char TCONST_copy[] PROGMEM = "copy";
static const char TCONST_d1[] PROGMEM = "d1";
static const char TCONST_d2[] PROGMEM = "d2";
static const char TCONST_d3[] PROGMEM = "d3";
static const char TCONST_d4[] PROGMEM = "d4";
static const char TCONST_d5[] PROGMEM = "d5";
static const char TCONST_d6[] PROGMEM = "d6";
static const char TCONST_d7[] PROGMEM = "d7";
static const char TCONST_debug[] PROGMEM = "debug";
static const char TCONST_delall[] PROGMEM = "delall";
static const char TCONST_delCfg[] PROGMEM = "delCfg";
static const char TCONST_delete[] PROGMEM = "delete";
static const char TCONST_delfromlist[] PROGMEM = "delfromlist";
static const char TCONST_del_[] PROGMEM = "del*";
static const char TCONST_Demo[] PROGMEM = "Demo";
static const char TCONST_direct[] PROGMEM = "direct";
static const char TCONST_DRand[] PROGMEM = "DRand";
static const char TCONST_drawbuff[] PROGMEM = "drawbuff";
static const char TCONST_drawClear[] PROGMEM = "drawClear";
static const char TCONST_drawing[] PROGMEM = "drawing";
static const char TCONST_draw_dat[] PROGMEM = "draw_dat";
static const char TCONST_ds18b20[] PROGMEM = "ds18b20";
static const char TCONST_DTimer[] PROGMEM = "DTimer";
static const char TCONST_dynCtrl[] PROGMEM = "dynCtrl";
static const char TCONST_dynCtrl_[] PROGMEM = "dynCtrl*";
static const char TCONST_edit_lamp_config[] PROGMEM = "edit_lamp_config";
static const char TCONST_edit_text_config[] PROGMEM = "edit_text_config";
static const char TCONST_edit[] PROGMEM = "edit";
static const char TCONST_eff_config[] PROGMEM = "eff_config";
static const char TCONST_eff_fav[] PROGMEM = "eff_fav";
static const char TCONST_eff_fulllist_json[] PROGMEM = "/eff_fulllist.json"; // a json serialized full list of effects and it's names for WebUI drop-down list on effects management page
static const char TCONST_eff_index[] PROGMEM = "/eff_index.json";
static const char TCONST_eff_list_json_tmp[] PROGMEM = "/eff_list.json.tmp"; // a json serialized list of effects and it's names for WebUI drop-down list
static const char TCONST_eff_list_json[] PROGMEM = "/eff_list.json";         // a json serialized list of effects and it's names for WebUI drop-down list on main page
static const char TCONST_eff_next[] PROGMEM = "eff_next";
static const char TCONST_eff_prev[] PROGMEM = "eff_prev";
static const char TCONST_eff_sel[] PROGMEM = "eff_sel";
static const char TCONST_eff_ctrls[] PROGMEM = "eff_ctrls";
static const char TCONST_effects_config[] PROGMEM = "effects_config";
static const char TCONST_effects[] PROGMEM = "effects";
static const char TCONST_effHasMic[] PROGMEM = "effHasMic";
static const char TCONST_effListConf[] PROGMEM = "effListConf";
static const char TCONST_eff_run[] PROGMEM = "effrun";                       // switch running effect to effect_#
static const char TCONST_effname[] PROGMEM = "effname";
static const char TCONST_effSort[] PROGMEM = "effSort";
static const char TCONST_embui_get_[] PROGMEM = "embui/get/";
static const char TCONST_embui_pub_[] PROGMEM = "embui/pub/";
static const char TCONST_embui_[] PROGMEM = "embui/";
static const char TCONST_enabled[] PROGMEM = "enabled";
static const char TCONST_encoder[] PROGMEM = "encoder";
static const char TCONST_encTxtCol[] PROGMEM = "encTxtCol";
static const char TCONST_encTxtDel[] PROGMEM = "encTxtDel";
static const char TCONST_EncVGCol[] PROGMEM = "EncVGCol";
static const char TCONST_EncVG[] PROGMEM = "EncVG";
static const char TCONST_eqSetings[] PROGMEM = "eqSetings";
static const char TCONST_event[] PROGMEM = "event";
static const char TCONST_eventList[] PROGMEM = "eventList";
static const char TCONST_Events[] PROGMEM = "Events";
static const char TCONST_event_conf[] PROGMEM = "event_conf";
static const char TCONST_evList[] PROGMEM = "evList";
static const char TCONST_fcfg_gpio[] PROGMEM = "/gpio.json";
static const char TCONST_fcfg_ledstrip[] = "/ledstrip.json";
static const char TCONST_fileName2[] PROGMEM = "fileName2";
static const char TCONST_fileName[] PROGMEM = "fileName";
static const char TCONST_fill[] PROGMEM = "fill";
static const char TCONST_flags[] PROGMEM = "flags";
static const char TCONST_force[] PROGMEM = "force";
static const char TCONST_gbright[] PROGMEM = "gbright";
static const char TCONST_GBR[] PROGMEM = "GBR";
static const char TCONST_GlobBRI[] PROGMEM = "GlobBRI";
static const char TCONST_height[] PROGMEM = "height";
static const char TCONST_hflip[] = "hflip";
static const char TCONST_hold[] PROGMEM = "hold";
static const char TCONST_Host[] PROGMEM = "Host";
static const char TCONST_Ip[] PROGMEM = "Ip";
static const char TCONST_isClearing[] PROGMEM = "isClearing";
static const char TCONST_isFaderON[] PROGMEM = "isFaderON";
static const char TCONST_isOnMP3[] PROGMEM = "isOnMP3";
static const char TCONST_isPlayTime[] PROGMEM = "isPlayTime";
static const char TCONST_isShowOff[] PROGMEM = "isShowOff";
static const char TCONST_isStreamOn[] PROGMEM = "isStreamOn";
static const char TCONST_lamptext[] PROGMEM = "lamptext";
static const char TCONST_lamp_config[] PROGMEM = "lamp_config";
static const char TCONST_lcurve[] = "lcurve";
static const char TCONST_limitAlarmVolume[] PROGMEM = "limitAlarmVolume";
static const char TCONST_load[] PROGMEM = "load";
static const char TCONST_lV[] PROGMEM = "lV";
static const char TCONST_Mac[] PROGMEM = "Mac";
static const char TCONST_main[] PROGMEM = "main";
static const char TCONST_makeidx[] PROGMEM = "makeidx";
static const char TCONST_mapping[] PROGMEM = "mapping";
static const char TCONST_Memory[] PROGMEM = "Memory";
static const char TCONST_micNoise[] PROGMEM = "micNoise";
static const char TCONST_micnRdcLvl[] PROGMEM = "micnRdcLvl";
static const char TCONST_micScale[] PROGMEM = "micScale";
static const char TCONST_Mic[] PROGMEM = "Mic";
static const char TCONST_mic_cal[] PROGMEM = "mic_cal";
static const char TCONST_MIRR_H[] PROGMEM = "MIRR_H";
static const char TCONST_MIRR_V[] PROGMEM = "MIRR_V";
static const char TCONST_Mode[] PROGMEM = "Mode";
static const char TCONST_mode[] PROGMEM = "mode";
static const char TCONST_mosfet_gpio[] PROGMEM = "fet_gpio";             // MOSFET gpio
static const char TCONST_mosfet_ll[] PROGMEM = "fet_ll";                // MOSFET logic level
static const char TCONST_mp3count[] PROGMEM = "mp3count";
static const char TCONST_mp3volume[] PROGMEM = "mp3volume";
static const char TCONST_mp3rx[] PROGMEM = "mp3rx";
static const char TCONST_mp3tx[] PROGMEM = "mp3tx";
static const char TCONST_mp3_n5[] PROGMEM = "mp3_n5";
static const char TCONST_mp3_p5[] PROGMEM = "mp3_p5";
static const char TCONST_MQTTTopic[] PROGMEM = "MQTTTopic";
static const char TCONST_msg[] PROGMEM = "msg";
static const char TCONST_mx_gpio[] PROGMEM = "mx_gpio";
static const char TCONST_nofade[] PROGMEM = "nofade";
static const char TCONST_Normal[] PROGMEM = "Normal";
static const char TCONST_numInList[] PROGMEM = "numInList";
static const char TCONST_ny_period[] PROGMEM = "ny_period";
static const char TCONST_ny_unix[] PROGMEM = "ny_unix";
static const char TCONST_onetime[] PROGMEM = "onetime";
static const char TCONST_ONflag[] PROGMEM = "ONflag";
static const char TCONST_on[] PROGMEM = "on";
static const char TCONST_opt_pass[] PROGMEM = "opt_pass";
static const char TCONST_OTA[] PROGMEM = "OTA";
static const char TCONST_Other[] PROGMEM = "Other";
static const char TCONST_pFS[] PROGMEM = "pFS";
static const char TCONST_PINB[] PROGMEM = "PINB"; // пин кнопки
static const char TCONST_pin[] PROGMEM = "pin";
static const char TCONST_playEffect[] PROGMEM = "playEffect";
static const char TCONST_playMP3[] PROGMEM = "playMP3";
static const char TCONST_playName[] PROGMEM = "playName";
static const char TCONST_playTime[] PROGMEM = "playTime";
static const char TCONST_pMem[] PROGMEM = "pMem";
static const char TCONST_pRSSI[] PROGMEM = "pRSSI";
static const char TCONST_pTemp[] PROGMEM = "pTemp";
static const char TCONST_pTime[] PROGMEM = "pTime";
static const char TCONST_pUptime[] PROGMEM = "pUptime";
static const char TCONST_repeat[] PROGMEM = "repeat";
static const char TCONST_RGB[] PROGMEM = "RGB";
static const char TCONST_RSSI[] PROGMEM = "RSSI";
static const char TCONST_save[] PROGMEM = "save";
static const char TCONST_scale[] PROGMEM = "scale";
static const char TCONST_settings[] PROGMEM = "settings";
static const char TCONST_settings_ledstrip[] PROGMEM = "set_ledstrip";
static const char TCONST_settings_mic[] PROGMEM = "settings_mic";
static const char TCONST_settings_mp3[] PROGMEM = "settings_mp3";
static const char TCONST_settings_wifi[] PROGMEM = "settings_wifi";
static const char TCONST_set_gpio[] PROGMEM = "s_gpio";                    // set gpio action
static const char TCONST_set_butt[] PROGMEM = "set_butt";
static const char TCONST_set_effect[] PROGMEM = "set_effect";
static const char TCONST_set_enc[] PROGMEM = "set_enc";
static const char TCONST_set_event[] PROGMEM = "set_event";
static const char TCONST_set_mic[] PROGMEM = "set_mic";
static const char TCONST_set_mp3[] PROGMEM = "set_mp3";
static const char TCONST_set_mqtt[] PROGMEM = "set_mqtt";
static const char TCONST_set_opt_pass[] PROGMEM = "set_opt_pass";
static const char TCONST_set_other[] PROGMEM = "set_other";
static const char TCONST_sh_page[] PROGMEM = "sh_page";                    // show_page action for webui page selector
static const char TCONST_showName[] PROGMEM = "showName";
static const char TCONST_show_button[] PROGMEM = "show_button";
static const char TCONST_show_butt[] PROGMEM = "show_butt";
static const char TCONST_show_event[] PROGMEM = "show_event";
static const char TCONST_show_flags[] PROGMEM = "show_flags";
static const char TCONST_show_mp3[] PROGMEM = "show_mp3";
static const char TCONST_snake[] PROGMEM = "snake";
static const char TCONST_soundfile[] PROGMEM = "soundfile";
static const char TCONST_spdcf[] PROGMEM = "spdcf";
static const char TCONST_speed[] PROGMEM = "speed";
static const char TCONST_state[] PROGMEM = "state";
static const char TCONST_STA[] PROGMEM = "STA";
static const char TCONST_stopat[] PROGMEM = "stopat";
static const char TCONST_streaming[] PROGMEM = "streaming";
static const char TCONST_stream_type[] PROGMEM = "stream_type";
static const char TCONST_sT[] PROGMEM = "sT";
static const char TCONST_syslampFlags[] PROGMEM = "syslampFlags";
static const char TCONST_sysSettings[] PROGMEM = "sysSettings";
static const char TCONST_textsend[] PROGMEM = "textsend";
static const char TCONST_text_config[] PROGMEM = "text_config";
static const char TCONST_Time[] PROGMEM = "Time";
static const char TCONST_tm24[] PROGMEM = "tm24";
static const char TCONST_tm_clk[] PROGMEM = "tmclk";
static const char TCONST_tm_dio[] PROGMEM = "tmdio";
static const char TCONST_tmBrightOff[] PROGMEM = "tmBrightOff";
static const char TCONST_tmBrightOn[] PROGMEM = "tmBrightOn";
static const char TCONST_tmBright[] PROGMEM = "tmBright";
static const char TCONST_tmEvent[] PROGMEM = "tmEvent";
static const char TCONST_tmZero[] PROGMEM = "tmZero";
static const char TCONST_txtBfade[] PROGMEM = "txtBfade";
static const char TCONST_txtColor[] PROGMEM = "txtColor";
static const char TCONST_txtOf[] PROGMEM = "txtOf";
static const char TCONST_txtSpeed[] PROGMEM = "txtSpeed";
static const char TCONST_Universe[] PROGMEM = "Universe";
static const char TCONST_Uptime[] PROGMEM = "Uptime";
static const char TCONST_value[] PROGMEM = "value";
static const char TCONST_vertical[] = "vertical";
static const char TCONST_Version[] PROGMEM = "Version";
static const char TCONST_vflip[] = "vflip";
static const char TCONST_wcpass[] PROGMEM = "wcpass";
static const char TCONST_White[] PROGMEM = "White";
static const char TCONST_width[] PROGMEM = "width";
static const char TCONST_XLOAD[] PROGMEM = "xload";
static const char TCONST__5f9ea0[] PROGMEM = "#5f9ea0";
static const char TCONST__708090[] PROGMEM = "#708090";
static const char TCONST__backup_btn_[] PROGMEM = "/backup/btn/";
static const char TCONST__backup_evn_[] PROGMEM = "/backup/evn/";
static const char TCONST__backup_glb_[] PROGMEM = "/backup/glb/";
static const char TCONST__backup_idx[] PROGMEM = "/backup/idx";
static const char TCONST__backup_idx_[] PROGMEM = "/backup/idx/";
static const char TCONST__demo[] PROGMEM = "demo";
static const char TCONST__ffffff[] PROGMEM = "#ffffff";
static const char TCONST__tmplist_tmp[] PROGMEM = "/tmplist.tmp";
static const char TCONST__tmpqlist_tmp[] PROGMEM = "/tmpqlist.tmp";

/** набор служебных текстовых констант (HTTP/MQTT запросы) */
static const char CMD_ON[] PROGMEM = "ON";                    // Без параметров - возвращает 1/0 в зависимости вкл/выкл лампа, принимает параметр - 1/0 (вкл/выкл лампу)
static const char CMD_OFF[] PROGMEM = "OFF";                  // Без параметров - возвращает 1/0 в зависимости выкл/вкл лампа, принимает параметр - 1/0 (выкл/вкл лампу)
static const char CMD_G_BRIGHT[] PROGMEM = "G_BRIGHT";        // Без параметров - возвращает 1/0 в зависимости вкл/выкл глобальная яркость, принимает параметр - 1/0 (вкл/выкл глобальная яркость)
static const char CMD_G_BRTPCT[] PROGMEM = "G_BRTPCT";        // Без параметров - возвращает общую яркость в процентах, принимает параметр - 0-100 выставить общую яркость в процентах
static const char CMD_DEMO[] PROGMEM = "DEMO";                // Без параметров - возвращает 1/0 в зависимости вкл/выкл глобальная яркость, принимает параметр - 1/0 (вкл/выкл глобальная яркость)
static const char CMD_PLAYER[] PROGMEM = "PLAYER";            // Без параметров - возвращает 1/0 в зависимости вкл/выкл плеер, принимает параметр - 1/0 (вкл/выкл плеер)
static const char CMD_MP3_SOUND[] PROGMEM = "MP3_SOUND";      // Без параметров - возвращает 1/0 в зависимости вкл/выкл проигрывание MP3, принимает параметр - 1/0 (вкл/выкл проигрывание MP3)
static const char CMD_MP3_PREV[] PROGMEM = "MP3_PREV";        // Без параметров - переключает трек на 1 назад, принимает числовой параметр, на сколько треков вернуть назад
static const char CMD_MP3_NEXT[] PROGMEM = "MP3_NEXT";        // Без параметров - переключает трек на 1 назад, принимает числовой параметр, на сколько треков вернуть назад
static const char CMD_MP3_VOLUME[] PROGMEM = "MP3_VOL";       // принимает числовой параметр - установить громкость mp3 плеера
static const char CMD_MIC[] PROGMEM = "MIC";                  // Без параметров - возвращает 1/0 в зависимости вкл/выкл микрофон, принимает параметр - 1/0 (вкл/выкл микрофон)
static const char CMD_EFFECT[] PROGMEM = "EFFECT";            // Без параметров - возвращает номер текущего эффекта, принимает числовой параметр - включить заданный эффект
static const char CMD_WARNING[] PROGMEM = "WARNING";          // Без параметров - мигает желтым 5 раз (1 раз в сек), принимает параметры /cmd?warning=[16777215,5000,500,1] - неблокирующий вывод мигалки поверх эффекта (выдача предупреждений), первое число - цвет, второе - полный период, третье - полупериод мигания, четвертое - тип уведомления (0 - цвет, 1-2 - цвет и счетчик, 3 - счетчик)
static const char CMD_EFF_CONFIG[] PROGMEM = "EFF_CONFIG";    // Без параметров - возвращает ВСЕ настройки текущего эффекта, с параметрами - не реализовано пока
static const char CMD_CONTROL[] PROGMEM = "CONTROL";          // Только с параметрами - принимает числовой параметр (номер контрола) и возвращает его текущие настройки, также принимает массив /cmd?control=[N,val] где - N - номер контрола, val - нужное значение
static const char CMD_RGB[] PROGMEM = "RGB";                  // RGB = r,g,b т.е. к примеру для красного 255,0,0
static const char CMD_LIST[] PROGMEM = "LIST";                // Без параметров - возвращает полный список эффектов (их номера)
static const char CMD_SHOWLIST[] PROGMEM = "SHOWLIST";        // Без параметров - возвращает список отображаемых эффектов (их номера)
static const char CMD_DEMOLIST[] PROGMEM = "DEMOLIST";        // Без параметров - возвращает список отображаемых эффектов (их номера) в DEMO режиме
static const char CMD_EFF_NAME[] PROGMEM = "EFF_NAME";        // Без параметров - возвращает из конфига имя текущего эффекта, принимает числовой параметр (номер эффекта) и возвращает имя этого эффекта
static const char CMD_EFF_ONAME[] PROGMEM = "EFF_ONAME";      // Без параметров - возвращает из внутренней памяти имя текущего эффекта, принимает числовой параметр (номер эффекта) и возвращает имя этого эффекта
static const char CMD_MOVE_NEXT[] PROGMEM = "MOVE_NEXT";      // Без параметров - переключает на следующий эффект
static const char CMD_MOVE_PREV[] PROGMEM = "MOVE_PREV";      // Без параметров - переключает на предыдущий эффект
static const char CMD_MOVE_RND[] PROGMEM = "MOVE_RND";        // Без параметров - переключает на рандомный эффект
static const char CMD_REBOOT[] PROGMEM = "REBOOT";            // Без параметров - перезагружает лампу
static const char CMD_ALARM[] PROGMEM = "ALARM";              // Без параметров - возвращает 1/0 в зависимости включен ли сейчас будильник рассвет, принимает параметры "cmd?alarm={alarmP=N,alarmT=N,msg="Message",lV=N, afS=N, sT=N}", где alarmP длительность рассвета, alarmT - светить после рассвета, msg - сообщение, lV - уровень громкости будильника, afS - стандартная ли мелодия будильника, sT - мелодия будильника (от 0 до 7), все комманды указывать не обязательно
static const char CMD_MATRIX[] PROGMEM = "MATRIX";            // Возвращает размер матрицы в формате [X, Y]
static const char CMD_MSG[] PROGMEM = "MSG";                  // Только с параметрами - выводит сообщение на лампу
static const char CMD_DRAW[] PROGMEM = "DRAW";                // Только с параметрами =[цвет,X,Y] - закрашивает пиксель указанным цветом (если включено рисование)
static const char CMD_INC_CONTROL[] PROGMEM = "INC_CONTROL";  // Только с параметрами =[N,val] - увеличивает\уменьшает контрол N на значение val
static const char CMD_FILL_MATRIX[] PROGMEM = "FILL_MATRIX";  // Только с параметрами - закрашивает матрицу указанным цветом (если включено рисование)
static const char CMD_AUX_ON[] PROGMEM = "AUX_ON";            // Включает AUX пин
static const char CMD_AUX_OFF[] PROGMEM = "AUX_OFF";          // Выключает AUX пин
static const char CMD_AUX_TOGGLE[] PROGMEM = "AUX_TOGGLE";    // Переключает AUX пин
static const char CMD_DRAWING[] PROGMEM = "DRAWING";          // Без параметров - возвращает 1/0 в зависимости вкл/выкл рисование, принимает параметр - 1/0 (вкл/выкл рисование)


