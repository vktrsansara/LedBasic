// =============================================================
//  07_AllEffects.ino — сборник эффектов с автосменой
//
//  10 эффектов, сменяющихся каждые 15 секунд:
//    1. Rainbow Sparkle  — радуга со случайными вспышками
//    2. Comet            — комета с хвостом
//    3. Fire             — языки пламени (8 частиц)
//    4. Fireworks        — фейерверк (5 искр)
//    5. Police           — мигалка (синий/красный)
//    6. Sunset           — закат с движущимся солнцем
//    7. Ants             — муравьиная тропа
//    8. Breath           — плавное дыхание (SIN8)
//    9. Plasma           — плазменный эффект (SIN8 + COS8)
//   10. Noise Fire       — огонь через 2D Value Noise
//
//  Вдохновлён: kitesurfer1404 / WS2812FX
//
//  Железо: лента WS2812B, ESP8266, DMA-пин GPIO3 (RX)
// =============================================================

#include <NeoPixelBus.h>
#include <NeoPixelBrightnessBus.h>
#include "LedBasic.h"

// --- настройки ---
#define NUM_LEDS        60
#define DATA_PIN         3
#define EFFECT_DURATION  15000   // мс на каждый эффект

using MyStrip = NeoPixelBrightnessBus<NeoGrbFeature, NeoEsp8266DmaWs2812xMethod>;
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
//  1. Rainbow Sparkle — радуга со случайными вспышками
// =============================================================
const char* FX_RAINBOW =
    "10 H = 0\n"
    "100 P = 0\n"
    "110 C = H + P * 256 / PIXEL\n"
    "120 SET_HSV P , C , 255 , 170\n"
    "130 S = RND 0 , 14\n"
    "140 IF S == 0 THEN SET P , 255 , 255 , 220\n"  // вспышка 1/15
    "150 P = P + 1\n"
    "160 IF P <= PIXEL THEN GOTO 110\n"
    "170 WAIT 25\n"
    "180 H = H + 4\n"
    "190 IF H > 255 THEN H = H - 256\n"
    "200 GOTO 100\n";

// =============================================================
//  2. Comet — комета с затухающим хвостом
// =============================================================
const char* FX_COMET =
    "10 H = 0\n"
    "20 P = 0\n"
    "30 CLEAR\n"
    "40 V = 160\n"
    "50 FOR T = 1 TO 8\n"
    "60   Q = P - T\n"
    "70   IF Q >= 0 THEN SET_HSV Q , H , 255 , V\n"
    "80   V = V * 3 / 4\n"          // хвост: яркость × 0.75
    "90 NEXT T\n"
    "100 SET P , 255 , 255 , 255\n"  // голова — белая
    "110 WAIT 25\n"
    "120 P = P + 1\n"
    "130 IF P <= PIXEL THEN GOTO 30\n"
    "140 CLEAR\n"
    "150 WAIT 200\n"
    "160 H = H + 40\n"
    "170 IF H > 255 THEN H = H - 256\n"
    "180 GOTO 20\n";

// =============================================================
//  3. Fire — языки пламени (8 частиц)
//  Позиции: I,J,K,L,N,O,Q,R  Яркости: A,B,C,D,E,F,G,H
//  Подпрограмма 600: рисует частицу (V=яркость, P=позиция)
//  Подпрограммы 700-770: перерождение каждой частицы
// =============================================================
const char* FX_FIRE =
    "10 I=RND 0,7\n"  "11 J=RND 0,7\n"  "12 K=RND 0,7\n"  "13 L=RND 0,7\n"
    "14 N=RND 0,7\n"  "15 O=RND 0,7\n"  "16 Q=RND 0,7\n"  "17 R=RND 0,7\n"
    "18 A=RND 180,255\n" "19 B=RND 180,255\n"
    "20 C=RND 180,255\n" "21 D=RND 180,255\n"
    "22 E=RND 180,255\n" "23 F=RND 180,255\n"
    "24 G=RND 180,255\n" "25 H=RND 180,255\n"
    "100 CLEAR\n"
    "110 V=A\n" "111 P=I\n" "112 GOSUB 600\n"
    "120 V=B\n" "121 P=J\n" "122 GOSUB 600\n"
    "130 V=C\n" "131 P=K\n" "132 GOSUB 600\n"
    "140 V=D\n" "141 P=L\n" "142 GOSUB 600\n"
    "150 V=E\n" "151 P=N\n" "152 GOSUB 600\n"
    "160 V=F\n" "161 P=O\n" "162 GOSUB 600\n"
    "170 V=G\n" "171 P=Q\n" "172 GOSUB 600\n"
    "180 V=H\n" "181 P=R\n" "182 GOSUB 600\n"
    "200 I=I+1\n" "201 J=J+1\n" "202 K=K+1\n" "203 L=L+1\n"
    "204 N=N+1\n" "205 O=O+1\n" "206 Q=Q+1\n" "207 R=R+1\n"
    "210 A=A-RND 5,20\n" "211 B=B-RND 5,20\n"
    "212 C=C-RND 5,20\n" "213 D=D-RND 5,20\n"
    "214 E=E-RND 5,20\n" "215 F=F-RND 5,20\n"
    "216 G=G-RND 5,20\n" "217 H=H-RND 5,20\n"
    "220 IF A<=0 THEN GOSUB 700\n" "221 IF I>59 THEN GOSUB 700\n"
    "230 IF B<=0 THEN GOSUB 710\n" "231 IF J>59 THEN GOSUB 710\n"
    "240 IF C<=0 THEN GOSUB 720\n" "241 IF K>59 THEN GOSUB 720\n"
    "250 IF D<=0 THEN GOSUB 730\n" "251 IF L>59 THEN GOSUB 730\n"
    "260 IF E<=0 THEN GOSUB 740\n" "261 IF N>59 THEN GOSUB 740\n"
    "270 IF F<=0 THEN GOSUB 750\n" "271 IF O>59 THEN GOSUB 750\n"
    "280 IF G<=0 THEN GOSUB 760\n" "281 IF Q>59 THEN GOSUB 760\n"
    "290 IF H<=0 THEN GOSUB 770\n" "291 IF R>59 THEN GOSUB 770\n"
    "300 WAIT 30\n" "310 GOTO 100\n"
    // рисование: белый→жёлтый→оранжевый→красный
    "600 IF V<=0 THEN RETURN\n" "601 IF P>59 THEN RETURN\n" "602 IF P<0 THEN RETURN\n"
    "610 IF V>200 THEN SET P,255,255,255\n" "611 IF V>200 THEN RETURN\n"
    "620 IF V>160 THEN SET P,255,V,0\n"    "621 IF V>160 THEN RETURN\n"
    "630 IF V>100 THEN SET P,255,V/4,0\n"  "631 IF V>100 THEN RETURN\n"
    "640 IF V>40  THEN SET P,V,0,0\n"      "650 RETURN\n"
    // перерождение
    "700 I=RND 0,7\n" "701 A=RND 200,255\n" "702 RETURN\n"
    "710 J=RND 0,7\n" "711 B=RND 200,255\n" "712 RETURN\n"
    "720 K=RND 0,7\n" "721 C=RND 200,255\n" "722 RETURN\n"
    "730 L=RND 0,7\n" "731 D=RND 200,255\n" "732 RETURN\n"
    "740 N=RND 0,7\n" "741 E=RND 200,255\n" "742 RETURN\n"
    "750 O=RND 0,7\n" "751 F=RND 200,255\n" "752 RETURN\n"
    "760 Q=RND 0,7\n" "761 G=RND 200,255\n" "762 RETURN\n"
    "770 R=RND 0,7\n" "771 H=RND 200,255\n" "772 RETURN\n";

// =============================================================
//  4. Fireworks — фейерверк (5 искр)
//  Позиции: F,G,H,I,J  Яркости: A,B,C,D,E
//  X — оттенок, записывается при рождении каждой искры
// =============================================================
const char* FX_FIREWORKS =
    "10 GOSUB 900\n"
    "100 CLEAR\n"
    "110 V=A\n" "111 W=F\n" "112 GOSUB 800\n"
    "120 V=B\n" "121 W=G\n" "122 GOSUB 800\n"
    "130 V=C\n" "131 W=H\n" "132 GOSUB 800\n"
    "140 V=D\n" "141 W=I\n" "142 GOSUB 800\n"
    "150 V=E\n" "151 W=J\n" "152 GOSUB 800\n"
    "160 WAIT 25\n"
    "200 A=A-6\n" "201 B=B-7\n" "202 C=C-5\n" "203 D=D-8\n" "204 E=E-6\n"
    "210 IF A<=0 THEN GOSUB 910\n" "220 IF B<=0 THEN GOSUB 920\n"
    "230 IF C<=0 THEN GOSUB 930\n" "240 IF D<=0 THEN GOSUB 940\n"
    "250 IF E<=0 THEN GOSUB 950\n"
    "260 GOTO 100\n"
    "800 IF V<=0 THEN RETURN\n" "801 IF W<0 THEN RETURN\n" "802 IF W>PIXEL THEN RETURN\n"
    "810 IF V>180 THEN SET W,255,255,255\n" "811 IF V>180 THEN RETURN\n"
    "820 SET_HSV W,X,255,V\n" "830 RETURN\n"
    "900 GOSUB 910\n" "901 GOSUB 920\n" "902 GOSUB 930\n"
    "903 GOSUB 940\n" "904 GOSUB 950\n" "905 RETURN\n"
    "910 F=RND 0,PIXEL\n" "911 X=RND 0,255\n" "912 A=RND 200,255\n" "913 RETURN\n"
    "920 G=RND 0,PIXEL\n" "921 X=RND 0,255\n" "922 B=RND 200,255\n" "923 RETURN\n"
    "930 H=RND 0,PIXEL\n" "931 X=RND 0,255\n" "932 C=RND 200,255\n" "933 RETURN\n"
    "940 I=RND 0,PIXEL\n" "941 X=RND 0,255\n" "942 D=RND 200,255\n" "943 RETURN\n"
    "950 J=RND 0,PIXEL\n" "951 X=RND 0,255\n" "952 E=RND 200,255\n" "953 RETURN\n";

// =============================================================
//  5. Police — мигалка (синий/красный)
// =============================================================
const char* FX_POLICE =
    "10 Z = 0\n"
    "20 FOR P = 0 TO PIXEL\n"
    "30   IF P <= PIXEL / 2 THEN GOSUB 200\n"
    "40   IF P > PIXEL / 2  THEN GOSUB 300\n"
    "50 NEXT P\n"
    "60 WAIT 120\n"
    "70 Z = Z + 1\n"
    "80 IF Z > 1 THEN Z = 0\n"
    "90 GOTO 20\n"
    "200 IF Z == 0 THEN SET P , 0 , 0 , 255\n"
    "201 IF Z == 1 THEN SET P , 255 , 0 , 0\n"
    "202 RETURN\n"
    "300 IF Z == 0 THEN SET P , 255 , 0 , 0\n"
    "301 IF Z == 1 THEN SET P , 0 , 0 , 255\n"
    "302 RETURN\n";

// =============================================================
//  6. Sunset — закат с движущимся солнцем
//  D = |X - P| (ручной ABS), цвет по расстоянию от солнца
// =============================================================
const char* FX_SUNSET =
    "10 X = 0\n"           // позиция солнца
    "20 B = 255\n"         // яркость солнца
    "30 W = 70\n"          // яркость неба
    "100 P = 0\n"
    "110 D = X - P\n"
    "120 IF D < 0 THEN D = P - X\n"    // D = |X - P|
    "130 IF D == 0 THEN SET P , B , B , B\n"           "131 IF D == 0 THEN GOTO 220\n"
    "140 IF D == 1 THEN SET P , B , B / 4 * 3 , 0\n"  "141 IF D == 1 THEN GOTO 220\n"
    "150 IF D == 2 THEN SET P , B , B / 3 , 0\n"      "151 IF D == 2 THEN GOTO 220\n"
    "160 IF D == 3 THEN SET P , B , B / 6 , 0\n"      "161 IF D == 3 THEN GOTO 220\n"
    "170 IF D == 4 THEN SET P , B / 2 , 0 , 0\n"      "171 IF D == 4 THEN GOTO 220\n"
    "180 SET P , W / 4 , 0 , W\n"
    "220 P = P + 1\n"
    "230 IF P <= PIXEL THEN GOTO 110\n"
    "240 WAIT 160\n"
    "250 X = X + 1\n"
    "260 IF X > PIXEL * 3 / 4 THEN B = B - 8\n"
    "270 IF B < 0 THEN B = 0\n"
    "280 IF X > PIXEL * 2 / 3 THEN W = W - 2\n"
    "290 IF W < 3 THEN W = 3\n"
    "300 IF X <= PIXEL THEN GOTO 100\n"
    "400 P = 0\n"
    "410 SET P , W / 4 , 0 , W\n"
    "420 P = P + 1\n"
    "430 IF P <= PIXEL THEN GOTO 410\n"
    "440 WAIT 80\n"
    "450 W = W - 3\n"
    "460 IF W > 0 THEN GOTO 400\n"
    "470 CLEAR\n"
    "480 WAIT 1200\n"
    "490 GOTO 10\n";

// =============================================================
//  7. Ants — муравьиная тропа
//  Паттерн (I + Z) % 8: 0..3 светлый, 4..7 тёмный
// =============================================================
const char* FX_ANTS =
    "10 Z = 0\n"
    "11 H = 160\n"
    "20 FOR I = 0 TO PIXEL\n"
    "30   R = I + Z\n"
    "40   R = R % 8\n"
    "50   IF R < 4 THEN SET_HSV I , H , 255 , 200\n"
    "60   IF R >= 4 THEN SET I , 12 , 12 , 12\n"
    "70 NEXT I\n"
    "80 WAIT 80\n"
    "90 Z = Z + 1\n"
    "100 IF Z > 7 THEN Z = 0\n"
    "110 H = H + 1\n"
    "120 IF H > 255 THEN H = 0\n"
    "130 GOTO 20\n";

// =============================================================
//  8. Breath — плавное дыхание (синусоида SIN8)
// =============================================================
const char* FX_BREATH =
    "10 T = 0\n"
    "20 V = SIN8 T\n"           // яркость: 0..255 по синусу
    "30 FILL 180 , 60 , V\n"    // розово-красный, яркость V
    "40 WAIT 14\n"
    "50 T = T + 2\n"
    "60 IF T > 255 THEN T = T - 256\n"
    "70 GOTO 20\n";

// =============================================================
//  9. Plasma — плазменный эффект (SIN8 + COS8)
//
//  Для каждого пикселя вычисляем оттенок из трёх синусоид
//  по разным осям и фазам. Сумма/3 = плавный цветной узор.
//
//  X, Y — координаты пикселя в пространстве 16×... матрицы
//  (работает и на линейной ленте: Y=0 для всех)
// =============================================================
const char* FX_PLASMA =
    "10 T = 0\n"
    "20 FOR I = 0 TO PIXEL\n"
    "30   X = I % 16\n"          // X-координата (0..15)
    "40   Y = I / 16\n"          // Y-координата (строка матрицы)
    "50   X = X * 16\n"          // масштабировать до 0..240
    "60   Y = Y * 16\n"
    "70   A = X + T\n"           // первая волна: горизонтальная
    "80   A = A % 256\n"
    "90   H = SIN8 A\n"
    "100  B = Y + 256 - T\n"     // вторая волна: вертикальная, обратная фаза
    "110  B = B % 256\n"
    "120  S = COS8 B\n"
    "130  H = H + S\n"
    "140  C = X + Y + T\n"       // третья волна: диагональная
    "150  C = C % 256\n"
    "160  S = SIN8 C\n"
    "170  H = H + S\n"
    "180  H = H / 3\n"           // среднее трёх волн → оттенок
    "190  SET_HSV I , H , 255 , 255\n"
    "200 NEXT I\n"
    "210 WAIT 25\n"
    "220 T = T + 3\n"
    "230 IF T > 255 THEN T = T - 256\n"
    "240 GOTO 20\n";

// =============================================================
//  10. Noise Fire — органичный огонь через 2D Value Noise
//
//  NOISE x , t — двумерный шум: плавное псевдослучайное поле.
//  x = позиция пикселя (пространство), t = время.
//  Результат 0..255: плавно меняется и в пространстве, и во времени.
//
//  Оттенок H = V/4 даёт диапазон красный→жёлтый (0..63 HSV),
//  яркость = сам шум — тёмные участки выглядят как угли.
// =============================================================
const char* FX_NOISE =
    "10 T = 0\n"
    "20 FOR I = 0 TO PIXEL\n"
    "30   X = I * 32\n"          // масштаб пространства (32 = 1/8 диапазона)
    "40   V = NOISE X , T\n"     // шум: 0..255, плавно меняется
    "50   H = V / 4\n"           // оттенок: 0..63 = красный→жёлтый
    "60   SET_HSV I , H , 255 , V\n"
    "70 NEXT I\n"
    "80 WAIT 20\n"
    "90 T = T + 10\n"            // скорость анимации
    "100 GOTO 20\n";

// =============================================================
//  Таблица эффектов
// =============================================================
struct Effect { const char* script; const char* name; };

const Effect EFFECTS[] = {
    { FX_RAINBOW,   "Rainbow Sparkle" },
    { FX_COMET,     "Comet"           },
    { FX_FIRE,      "Fire"            },
    { FX_FIREWORKS, "Fireworks"       },
    { FX_POLICE,    "Police"          },
    { FX_SUNSET,    "Sunset"          },
    { FX_ANTS,      "Ants"            },
    { FX_BREATH,    "Breath"          },
    { FX_PLASMA,    "Plasma"          },
    { FX_NOISE,     "Noise Fire"      },
};
const uint8_t EFFECT_COUNT = sizeof(EFFECTS) / sizeof(EFFECTS[0]);

uint8_t currentEffect = 0;

// =============================================================
void loadEffect(uint8_t idx) {
    Serial.printf("→ [%lu ms] %d/%d: %s\n",
                  millis(), idx + 1, EFFECT_COUNT, EFFECTS[idx].name);
    basic.stop();
    if (basic.compileFromText(EFFECTS[idx].script)) {
        basic.play();
    } else {
        Serial.println("  ОШИБКА компиляции!");
    }
}

void setup() {
    Serial.begin(115200);
    delay(300);
    Serial.printf("\n--- LedBasic %d effects demo ---\n", EFFECT_COUNT);

    strip.Begin();
    strip.SetBrightness(25);
    strip.Show();

    loadEffect(currentEffect);
    basic.setSpeed(100);
}

void loop() {
    basic.tick();

    static uint32_t lastSwitch = 0;
    if (millis() - lastSwitch >= EFFECT_DURATION) {
        lastSwitch = millis();
        currentEffect = (currentEffect + 1) % EFFECT_COUNT;
        loadEffect(currentEffect);
    }
}
