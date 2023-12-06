__[CHANGELOG](/CHANGELOG.md)__ | [![PlatformIO CI](https://github.com/vortigont/FireLamp_JeeUI/actions/workflows/pio_build.yml/badge.svg)](https://github.com/vortigont/FireLamp_JeeUI/actions/workflows/pio_build.yml)

## Огненная лампа FireLamp_EmbUI
Деполитизированный форк проекта "огненной" лампы [FireLamp_EmbUI](https://github.com/DmytroKorniienko/FireLamp_EmbUI).

<details>
  <summary>Project Manifest</summary>

Проект был пересобран из старых форков и архивов репозитория пользователей и участников разработки на момент примерно 2021 года. В [исходном](https://github.com/DmytroKorniienko/FireLamp_EmbUI) проекте был полностью вырезан русский язык, задним числом переписана история разработки в Git и удалена соотвествующая тема [форума](https://community.alexgyver.ru/threads/wifi-lampa-budilnik-obsuzhdenie-proekta.1411/). Данный форк это не срез исходного репозитория, хотя сохранил часть общей истории до определенного момента.

</details>

Обсуждение и поддержка данной прошивки идет [на форуме](https://community.alexgyver.ru/threads/wifi-lampa-budilnik-obsuzhdenie-proshivki-firelamp_embui.7257/)


## Содержание
- [WiKi проекта](#WiKi-проекта)
- [Концепт](#Концепт)
- [ESP8266 vs ESP32](#esp8266-vs-esp32)
- [Распиновка](#pinouts)

<a name="WiKi-проекта"></a>
### WiKi проекта
Актуальную документацию по проекту можно найти в [WiKi](https://github.com/vortigont/EmbUI/wiki)

<a name="Концепт"></a>
### Концепт
Исходный проект был значительно переписан и реструктурирован. В разработке основное внимание уделяется структуре кода, декомпозиции и объектоориентированности даже в ущерб функциональнсти и "фичам". Основная идея развития - вся конфигурация и управление периферией должна быть максимально доступна во время работы кода без необходимости пересборки всего проекта. Параметры в конфигурационных файлах и опции сборки сводятся к минимуму, упраздняются и заменяются настройками через WebUI.

<a name="esp8266-vs-esp32"></a>
### ESP8266 vs ESP32
Разработка ведется под контроллеры семейства **esp32**. ESP8266 морально устарел, было принято решение не тратить время на поддержку совместимости и уделить больше времени и ресурсов актуальной платформе.
Последний стабильно работающий на контроллерах 8266 выпуск проекта - версия [v3.1.1](../../releases/tag/v3.1.1), исправления будут аккумулироваться в ветке [legacy](../../tree/legacy).

<a name="pinouts"></a>
## Распиновка

<details>
  <summary>ESP32 30PIN:</summary>

  ![ESP32 30PIN](https://github.com/BashSer/FireLamp_JeeUI/blob/5835c8861ec4f3d6f49d37cb4d475c02c7989ea1/esp32-30pin.png)

${\color{green}GREEN}$ - OK to use.

${\color{orange}ORANGE}$ - OK to use, but you need to pay attention because they may have unexpected behaviour, mainly at boot.

${\color{red}RED}$ - NOT recommended to use as inputs or outputs.

|GPIO |Input |Output |Notes |
|-|-|-|-|
|0|$${\color{orange}Pulled\ up}$$|$${\color{orange}OK}$$|Outputs PWM signal at boot|
|1|$${\color{red}TX\ Pin}$$|$${\color{orange}OK}$$|Debug output at boot|
|2|$${\color{green}OK}$$|$${\color{green}OK}$$|Connected to onboard LED|
|3|$${\color{orange}OK}$$|$${\color{red}RX\ Pin}$$|HIGH at boot|
|6, 7, 8, 9, 10, 11|$${\color{red}\-}$$|$${\color{red}\-}$$|Connected to integrated SPI flash. |
|12|$${\color{orange}OK}$$|$${\color{green}OK}$$||
|5, 13, 14|$${\color{green}OK}$$|$${\color{green}OK}$$|Outputs PWM signal at boot|
|4, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33|$${\color{green}OK}$$|$${\color{green}OK}$$||
|34, 35|$${\color{green}OK}$$|||
|36, 37, 38, 39|$${\color{green}OK}$$||Input only|

</details>

<details>
  <summary>ESP32 C3 Dual USB(not working ?):</summary>
  
  ![ESP32 C3 Dual USB](https://github.com/BashSer/FireLamp_JeeUI/blob/a99665cae5eccdcfe502ad324d2508c22c02d9fe/esp32-c3-dual-usb.png)
  
</details>
