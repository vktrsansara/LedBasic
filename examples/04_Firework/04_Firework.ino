// =============================================================
//  04_Firework.ino — эффект «Фейерверк»
//
//  5 независимых искр одновременно вспыхивают и угасают
//  на случайных позициях. Каждая искра:
//    - начинается белой вспышкой (V > 180)
//    - затем затухает своим уникальным цветом
//    - при угасании перерождается в новом месте
//
//  Вдохновлён: kitesurfer1404 / WS2812FX
//
//  Железо: лента WS2812B, ESP8266, DMA-пин GPIO3 (RX)
// =============================================================

#include <NeoPixelBus.h>
#include "LedBasic.h"

// --- настройки ---
#define NUM_LEDS  60
#define DATA_PIN   3

using MyStrip = NeoPixelBus<NeoGrbFeature, NeoEsp8266DmaWs2812xMethod>;
MyStrip strip(NUM_LEDS, DATA_PIN);

LedBasic basic(
    NUM_LEDS,
    [](uint16_t pos, uint8_t r, uint8_t g, uint8_t b) {
        strip.SetPixelColor(pos, RgbColor(r, g, b));
    },
    []{ strip.Show(); },
    []{ strip.ClearTo(RgbColor(0, 0, 0)); strip.Show(); }
);

// =============================================================
//  Скрипт: 5 искр фейерверка
//
//  Позиции искр:  F, G, H, I, J
//  Яркости искр:  A, B, C, D, E
//  X  — оттенок (записывается при рождении каждой искры)
//
//  Передача параметров в подпрограмму 800:
//    V = яркость, W = позиция (перед GOSUB 800)
//
//  Подпрограммы 900/910-950 — инициализация и перерождение.
// =============================================================
const char* scriptText =
    // запустить все 5 искр
    "10 GOSUB 900\n"

    // --- основной цикл ---
    "100 CLEAR\n"
    "110 V = A\n" "111 W = F\n" "112 GOSUB 800\n"   // искра 1
    "120 V = B\n" "121 W = G\n" "122 GOSUB 800\n"   // искра 2
    "130 V = C\n" "131 W = H\n" "132 GOSUB 800\n"   // искра 3
    "140 V = D\n" "141 W = I\n" "142 GOSUB 800\n"   // искра 4
    "150 V = E\n" "151 W = J\n" "152 GOSUB 800\n"   // искра 5
    "160 WAIT 25\n"

    // угасание яркостей (разная скорость — разные ритмы)
    "200 A = A - 6\n"
    "201 B = B - 7\n"
    "202 C = C - 5\n"
    "203 D = D - 8\n"
    "204 E = E - 6\n"

    // перерождение угасших
    "210 IF A <= 0 THEN GOSUB 910\n"
    "220 IF B <= 0 THEN GOSUB 920\n"
    "230 IF C <= 0 THEN GOSUB 930\n"
    "240 IF D <= 0 THEN GOSUB 940\n"
    "250 IF E <= 0 THEN GOSUB 950\n"
    "260 GOTO 100\n"

    // --- подпрограмма рисования искры ---
    // V > 180: белая вспышка в момент рождения
    // V <= 180: затухание своим цветом X
    "800 IF V <= 0 THEN RETURN\n"
    "801 IF W < 0   THEN RETURN\n"
    "802 IF W > PIXEL THEN RETURN\n"
    "810 IF V > 180 THEN SET W , 255 , 255 , 255\n"  // белая вспышка
    "811 IF V > 180 THEN RETURN\n"
    "820 SET_HSV W , X , 255 , V\n"                  // затухание своим цветом
    "830 RETURN\n"

    // --- инициализация всех 5 искр ---
    "900 GOSUB 910\n"
    "901 GOSUB 920\n"
    "902 GOSUB 930\n"
    "903 GOSUB 940\n"
    "904 GOSUB 950\n"
    "905 RETURN\n"

    // рождение каждой искры: случайная позиция, цвет, яркость
    "910 F = RND 0 , PIXEL\n" "911 X = RND 0 , 255\n" "912 A = RND 200 , 255\n" "913 RETURN\n"
    "920 G = RND 0 , PIXEL\n" "921 X = RND 0 , 255\n" "922 B = RND 200 , 255\n" "923 RETURN\n"
    "930 H = RND 0 , PIXEL\n" "931 X = RND 0 , 255\n" "932 C = RND 200 , 255\n" "933 RETURN\n"
    "940 I = RND 0 , PIXEL\n" "941 X = RND 0 , 255\n" "942 D = RND 200 , 255\n" "943 RETURN\n"
    "950 J = RND 0 , PIXEL\n" "951 X = RND 0 , 255\n" "952 E = RND 200 , 255\n" "953 RETURN\n";

// =============================================================
void setup() {
    Serial.end();
    strip.Begin();
    strip.Show();
    basic.compileFromText(scriptText);
    basic.play();
}

void loop() {
    basic.tick();
}
