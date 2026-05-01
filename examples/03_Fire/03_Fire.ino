// =============================================================
//  03_Fire.ino — эффект «Языки пламени»
//
//  8 независимых частиц огня. Каждая имеет позицию и яркость.
//  Цвет зависит от яркости (от горячего к холодному):
//    V > 200 → белое ядро
//    V > 160 → жёлтый
//    V > 100 → оранжевый
//    V >  40 → красный
//    иначе  → угасает (не рисуется)
//
//  Частицы поднимаются вверх по ленте и случайно остывают.
//  Угасшие или вышедшие за ленту — перерождаются снизу.
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
//  Скрипт: языки пламени (8 частиц)
//
//  Позиции: I, J, K, L, N, O, Q, R
//  Яркости: A, B, C, D, E, F, G, H
//
//  Передача параметров в подпрограмму 600:
//    V = яркость, P = позиция  (установить перед GOSUB)
//
//  Подпрограммы 700-770 — перерождение каждой частицы.
//  Граница ленты зашита как 59 (для 60 пикселей).
//  Для другой длины замени 59 на NUM_LEDS - 1.
// =============================================================
const char* scriptText =
    // --- начальные позиции (нижняя часть ленты) ---
    "10 I = RND 0 , 7\n"
    "11 J = RND 0 , 7\n"
    "12 K = RND 0 , 7\n"
    "13 L = RND 0 , 7\n"
    "14 N = RND 0 , 7\n"
    "15 O = RND 0 , 7\n"
    "16 Q = RND 0 , 7\n"
    "17 R = RND 0 , 7\n"

    // --- начальные яркости (горящие) ---
    "18 A = RND 180 , 255\n"
    "19 B = RND 180 , 255\n"
    "20 C = RND 180 , 255\n"
    "21 D = RND 180 , 255\n"
    "22 E = RND 180 , 255\n"
    "23 F = RND 180 , 255\n"
    "24 G = RND 180 , 255\n"
    "25 H = RND 180 , 255\n"

    // --- кадр: нарисовать 8 частиц ---
    "100 CLEAR\n"
    "110 V = A\n" "111 P = I\n" "112 GOSUB 600\n"
    "120 V = B\n" "121 P = J\n" "122 GOSUB 600\n"
    "130 V = C\n" "131 P = K\n" "132 GOSUB 600\n"
    "140 V = D\n" "141 P = L\n" "142 GOSUB 600\n"
    "150 V = E\n" "151 P = N\n" "152 GOSUB 600\n"
    "160 V = F\n" "161 P = O\n" "162 GOSUB 600\n"
    "170 V = G\n" "171 P = Q\n" "172 GOSUB 600\n"
    "180 V = H\n" "181 P = R\n" "182 GOSUB 600\n"

    // --- частицы поднимаются вверх ---
    "200 I = I + 1\n" "201 J = J + 1\n" "202 K = K + 1\n" "203 L = L + 1\n"
    "204 N = N + 1\n" "205 O = O + 1\n" "206 Q = Q + 1\n" "207 R = R + 1\n"

    // --- случайное остывание ---
    "210 A = A - RND 5 , 20\n"
    "211 B = B - RND 5 , 20\n"
    "212 C = C - RND 5 , 20\n"
    "213 D = D - RND 5 , 20\n"
    "214 E = E - RND 5 , 20\n"
    "215 F = F - RND 5 , 20\n"
    "216 G = G - RND 5 , 20\n"
    "217 H = H - RND 5 , 20\n"

    // --- перезапуск угасших (V<=0) или ушедших за ленту (>59) ---
    "220 IF A <= 0 THEN GOSUB 700\n" "221 IF I > 59 THEN GOSUB 700\n"
    "230 IF B <= 0 THEN GOSUB 710\n" "231 IF J > 59 THEN GOSUB 710\n"
    "240 IF C <= 0 THEN GOSUB 720\n" "241 IF K > 59 THEN GOSUB 720\n"
    "250 IF D <= 0 THEN GOSUB 730\n" "251 IF L > 59 THEN GOSUB 730\n"
    "260 IF E <= 0 THEN GOSUB 740\n" "261 IF N > 59 THEN GOSUB 740\n"
    "270 IF F <= 0 THEN GOSUB 750\n" "271 IF O > 59 THEN GOSUB 750\n"
    "280 IF G <= 0 THEN GOSUB 760\n" "281 IF Q > 59 THEN GOSUB 760\n"
    "290 IF H <= 0 THEN GOSUB 770\n" "291 IF R > 59 THEN GOSUB 770\n"
    "300 WAIT 30\n"
    "310 GOTO 100\n"

    // --- подпрограмма рисования (V=яркость, P=позиция) ---
    // Цепочка IF→RETURN от горячего к холодному (switch/case)
    "600 IF V <= 0 THEN RETURN\n"
    "601 IF P > 59  THEN RETURN\n"
    "602 IF P < 0   THEN RETURN\n"
    "610 IF V > 200 THEN SET P , 255 , 255 , 255\n"   // белое ядро
    "611 IF V > 200 THEN RETURN\n"
    "620 IF V > 160 THEN SET P , 255 , V , 0\n"       // жёлтый
    "621 IF V > 160 THEN RETURN\n"
    "630 IF V > 100 THEN SET P , 255 , V / 4 , 0\n"   // оранжевый
    "631 IF V > 100 THEN RETURN\n"
    "640 IF V > 40  THEN SET P , V , 0 , 0\n"         // красный
    "650 RETURN\n"

    // --- перерождение: новая позиция снизу + полная яркость ---
    "700 I = RND 0 , 7\n" "701 A = RND 200 , 255\n" "702 RETURN\n"
    "710 J = RND 0 , 7\n" "711 B = RND 200 , 255\n" "712 RETURN\n"
    "720 K = RND 0 , 7\n" "721 C = RND 200 , 255\n" "722 RETURN\n"
    "730 L = RND 0 , 7\n" "731 D = RND 200 , 255\n" "732 RETURN\n"
    "740 N = RND 0 , 7\n" "741 E = RND 200 , 255\n" "742 RETURN\n"
    "750 O = RND 0 , 7\n" "751 F = RND 200 , 255\n" "752 RETURN\n"
    "760 Q = RND 0 , 7\n" "761 G = RND 200 , 255\n" "762 RETURN\n"
    "770 R = RND 0 , 7\n" "771 H = RND 200 , 255\n" "772 RETURN\n";

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
