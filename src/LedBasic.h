#ifndef LEDBASIC_H
#define LEDBASIC_H

#include <Arduino.h>
#include <LittleFS.h>

// #define LEDBASIC_DEBUG

#ifdef LEDBASIC_DEBUG
#define LB_LOG(...) Serial.printf(__VA_ARGS__)
#define LB_LOGLN(msg) Serial.println(msg)
#else
#define LB_LOG(...) \
    do              \
    {               \
    } while (0)
#define LB_LOGLN(msg) \
    do                \
    {                 \
    } while (0)
#endif

enum Token : uint8_t
{
    T_EOF = 0xFF,
    T_EOL = 0x01,
    T_COMMA = 0x02,
    T_LPAREN = 0x03,
    T_RPAREN = 0x04,

    CMD_FOR = 0x10,
    CMD_TO = 0x11,
    CMD_STEP = 0x12,
    CMD_NEXT = 0x13,
    CMD_IF = 0x14,
    CMD_THEN = 0x15,
    CMD_GOTO = 0x16,
    CMD_GOSUB = 0x17,
    CMD_RETURN = 0x18,

    CMD_CLEAR = 0x20,
    CMD_FILL = 0x21,
    CMD_SET = 0x22,
    CMD_SET_HSV = 0x23,
    CMD_WAIT = 0x24,
    CMD_DELAY = 0x25,
    CMD_SHOW = 0x26,
    CMD_BRIGHT = 0x27,
    CMD_FADE   = 0x28,
    CMD_MIRROR = 0x29,

    T_VAR = 0x30,
    T_NUM = 0x31,

    OP_ASSIGN = 0x40,
    OP_EQ = 0x41,
    OP_NEQ = 0x42,
    OP_GT = 0x43,
    OP_LT = 0x44,
    OP_GE = 0x45,
    OP_LE = 0x46,
    OP_PLUS = 0x50,
    OP_MINUS = 0x51,
    OP_MUL = 0x52,
    OP_DIV = 0x53,
    OP_MOD = 0x54,
    OP_AND = 0x55,
    OP_OR = 0x56,
    OP_RND = 0x60,
    OP_ABS = 0x61,
    OP_MIN = 0x62,
    OP_MAX = 0x63,
    OP_SIN8 = 0x64,
    OP_COS8 = 0x65,
    OP_PIXEL = 0x66,
    OP_NOISE = 0x67,
    OP_MAP = 0x68,
    OP_CONSTRAIN = 0x69,
    OP_EXP8 = 0x6A,

    CMD_GOTO_DIRECT = 0xF0,
    CMD_GOSUB_DIRECT = 0xF1,
};

enum VMState : uint8_t
{
    VM_STOPPED,
    VM_RUNNING,
    VM_WAITING
};

#include <functional>
using LBSetPixel = std::function<void(uint16_t pos, uint8_t r, uint8_t g, uint8_t b)>;
using LBShow = std::function<void()>;
using LBClear = std::function<void()>;

class LedBasic
{
public:
    LedBasic(uint16_t count, LBSetPixel setPixelCb, LBShow showCb, LBClear clearCb);
    ~LedBasic();

    bool loadBinMemory(const uint8_t *code);
    bool loadBinFile(const char *path);
    bool compileFromText(const char *scriptText);
    bool compileBasToBin(const char *basPath, const char *binPath);

    void play();
    void stop();
    void tick();
    bool isRunning() const;

    void setSpeed(uint16_t percent) { speedPercent = (uint16_t)constrain((int)percent, 1, 1000); }
    uint16_t getSpeed() const { return speedPercent; }

private:
    LBSetPixel cbSetPixel;
    LBShow cbShow;
    LBClear cbClear;
    uint8_t *pixelBuf;
    uint16_t num_leds;

    uint8_t *bytecode;
    uint16_t bytecodeSize;
    bool isAllocated;

    uint8_t *codePtr;
    uint8_t *codeEnd;

    VMState state;
    uint32_t waitTimer;
    uint16_t speedPercent;
    int16_t vars[26];

    static const uint8_t CALL_DEPTH = 10;
    uint8_t *callStack[CALL_DEPTH];
    uint8_t sp;

    static const uint8_t FOR_DEPTH = 8;
    struct ForLoop
    {
        uint8_t varIndex;
        int16_t limit;
        int16_t step;
        uint8_t *bodyPtr;
    } forStack[FOR_DEPTH];
    uint8_t fp;

    static const uint8_t MAX_LINES = 128;
    struct LineEntry
    {
        uint16_t number;
        uint8_t *bodyPtr;
    };
    LineEntry lineIndex[MAX_LINES];
    uint8_t lineCount;

    void freeMemory();
    void buildLineIndex();
    void linkBytecode();

    IRAM_ATTR void doSetPixel(uint16_t pos, uint8_t r, uint8_t g, uint8_t b);

    IRAM_ATTR inline uint8_t fetch() { return *codePtr++; }
    IRAM_ATTR inline int16_t fetchI16()
    {
        uint8_t h = *codePtr++, l = *codePtr++;
        return (int16_t)((h << 8) | l);
    }
    IRAM_ATTR inline uint16_t fetchU16()
    {
        uint8_t h = *codePtr++, l = *codePtr++;
        return ((uint16_t)h << 8) | l;
    }

    int16_t evalExpr();
    int16_t evalAddSub();
    int16_t evalMulDiv();
    int16_t evalUnary();
    int16_t evalPrimary();

    uint8_t *findLine(uint16_t lineNumber) const;

    void skipToEOL();

    uint32_t applySpeed(int16_t ms) const
    {
        if (ms <= 0)
            return 0;
        return (uint32_t)ms * 100UL / speedPercent;
    }

    IRAM_ATTR static inline uint8_t clamp8(int16_t v)
    {
        if (v <= 0)
            return 0;
        if (v >= 255)
            return 255;
        return (uint8_t)v;
    }

    IRAM_ATTR static inline void hsv2rgb_fast(uint8_t h, uint8_t s, uint8_t v,
                                              uint8_t &r, uint8_t &g, uint8_t &b)
    {
        uint8_t offset = h % 85;
        uint8_t up = (uint16_t)offset * 3;
        uint8_t down = 255 - up;
        if (h < 85)
        {
            r = down;
            g = up;
            b = 0;
        }
        else if (h < 170)
        {
            r = 0;
            g = down;
            b = up;
        }
        else
        {
            r = up;
            g = 0;
            b = down;
        }

        if (s < 255)
        {
            if (s == 0)
            {
                r = g = b = 255;
            }
            else
            {
                r = 255 - (((uint16_t)(255 - r) * s) >> 8);
                g = 255 - (((uint16_t)(255 - g) * s) >> 8);
                b = 255 - (((uint16_t)(255 - b) * s) >> 8);
            }
        }

        if (v < 255)
        {
            r = ((uint16_t)r * v) >> 8;
            g = ((uint16_t)g * v) >> 8;
            b = ((uint16_t)b * v) >> 8;
        }
    }

    static uint8_t sin8(uint8_t angle);
    static uint8_t cos8(uint8_t angle);
    static uint8_t inoise8(uint16_t x, uint16_t y);
    static uint8_t exp8(uint8_t x);

    uint16_t tokenizeLine(const char *line, uint8_t *buffer);
};

#endif