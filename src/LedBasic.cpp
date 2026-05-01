#include "LedBasic.h"

// =============================================================
//  sin8 / cos8  LUT — 256 значений 0..255
// =============================================================
static const uint8_t PROGMEM _sin8_table[256] = {
    128, 131, 134, 137, 140, 143, 146, 149, 152, 155, 158, 162, 165, 167, 170, 173,
    176, 179, 182, 185, 188, 190, 193, 196, 198, 201, 203, 206, 208, 211, 213, 215,
    218, 220, 222, 224, 226, 228, 230, 232, 234, 235, 237, 238, 240, 241, 243, 244,
    245, 246, 247, 248, 249, 250, 250, 251, 252, 252, 253, 253, 253, 254, 254, 254,
    254, 254, 254, 254, 253, 253, 253, 252, 252, 251, 250, 250, 249, 248, 247, 246,
    245, 244, 243, 241, 240, 238, 237, 235, 234, 232, 230, 228, 226, 224, 222, 220,
    218, 215, 213, 211, 208, 206, 203, 201, 198, 196, 193, 190, 188, 185, 182, 179,
    176, 173, 170, 167, 165, 162, 158, 155, 152, 149, 146, 143, 140, 137, 134, 131,
    128, 124, 121, 118, 115, 112, 109, 106, 103, 100, 97, 93, 90, 88, 85, 82,
    79, 76, 73, 70, 67, 65, 62, 59, 57, 54, 52, 49, 47, 44, 42, 40,
    37, 35, 33, 31, 29, 27, 25, 23, 21, 20, 18, 17, 15, 14, 12, 11,
    10, 9, 8, 7, 6, 5, 5, 4, 3, 3, 2, 2, 2, 1, 1, 1,
    1, 1, 1, 1, 2, 2, 2, 3, 3, 4, 5, 5, 6, 7, 8, 9,
    10, 11, 12, 14, 15, 17, 18, 20, 21, 23, 25, 27, 29, 31, 33, 35,
    37, 40, 42, 44, 47, 49, 52, 54, 57, 59, 62, 65, 67, 70, 73, 76,
    79, 82, 85, 88, 90, 93, 97, 100, 103, 106, 109, 112, 115, 118, 121, 124};
uint8_t LedBasic::sin8(uint8_t a) { return pgm_read_byte(&_sin8_table[a]); }
uint8_t LedBasic::cos8(uint8_t a) { return pgm_read_byte(&_sin8_table[(uint8_t)(a + 64)]); }

// =============================================================
//  EXP8 — гамма 2.2: round(255*(x/255)^2.2)
// =============================================================
static const uint8_t _exp8_table[256] PROGMEM = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2,
    3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6,
    6, 7, 7, 7, 8, 8, 8, 9, 9, 9, 10, 10, 11, 11, 11, 12,
    12, 13, 13, 13, 14, 14, 15, 15, 16, 16, 17, 17, 18, 18, 19, 19,
    20, 20, 21, 22, 22, 23, 23, 24, 25, 25, 26, 26, 27, 28, 28, 29,
    30, 30, 31, 32, 33, 33, 34, 35, 35, 36, 37, 38, 39, 39, 40, 41,
    42, 43, 43, 44, 45, 46, 47, 48, 49, 49, 50, 51, 52, 53, 54, 55,
    56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71,
    73, 74, 75, 76, 77, 78, 79, 81, 82, 83, 84, 85, 87, 88, 89, 90,
    91, 93, 94, 95, 97, 98, 99, 100, 102, 103, 105, 106, 107, 109, 110, 111,
    113, 114, 116, 117, 119, 120, 121, 123, 124, 126, 127, 129, 130, 132, 133, 135,
    137, 138, 140, 141, 143, 145, 146, 148, 149, 151, 153, 154, 156, 158, 159, 161,
    163, 165, 166, 168, 170, 172, 173, 175, 177, 179, 181, 182, 184, 186, 188, 190,
    192, 194, 196, 197, 199, 201, 203, 205, 207, 209, 211, 213, 215, 217, 219, 221,
    223, 225, 227, 229, 231, 234, 236, 238, 240, 242, 244, 246, 248, 251, 253, 255};
uint8_t LedBasic::exp8(uint8_t x) { return pgm_read_byte(&_exp8_table[x]); }

// =============================================================
//  Конструктор / деструктор
// =============================================================
LedBasic::LedBasic(uint16_t count, LBSetPixel setPixelCb, LBShow showCb, LBClear clearCb)
    : cbSetPixel(setPixelCb), cbShow(showCb), cbClear(clearCb),
      pixelBuf(nullptr),
      num_leds(count),
      bytecode(nullptr), bytecodeSize(0), isAllocated(false),
      codePtr(nullptr), codeEnd(nullptr),
      state(VM_STOPPED), waitTimer(0), speedPercent(100),
      sp(0), fp(0), lineCount(0)
{
    memset(vars, 0, sizeof(vars));
    pixelBuf = new uint8_t[(size_t)count * 3]();
}
LedBasic::~LedBasic()
{
    delete[] pixelBuf;
    freeMemory();
}

void LedBasic::freeMemory()
{
    if (isAllocated && bytecode)
    {
        free(bytecode);
        isAllocated = false;
    }
    bytecode = nullptr;
    bytecodeSize = 0;
    lineCount = 0;
    codePtr = codeEnd = nullptr;
}

// =============================================================
//  Загрузка
// =============================================================
bool LedBasic::loadBinMemory(const uint8_t *code)
{
    freeMemory();
    bytecode = (uint8_t *)code;
    isAllocated = false;
    uint16_t pos = 0;
    while (true)
    {
        uint8_t len = code[pos + 2];
        pos += 3 + len;
        if (code[pos] == T_EOF)
        {
            bytecodeSize = pos + 1;
            break;
        }
        if (pos > 4096)
        {
            bytecodeSize = pos;
            break;
        }
    }
    codeEnd = bytecode + bytecodeSize;
    buildLineIndex();
    linkBytecode();
    return true;
}

bool LedBasic::loadBinFile(const char *path)
{
    if (!LittleFS.begin())
        return false;
    File file = LittleFS.open(path, "r");
    if (!file)
        return false;
    size_t size = file.size();
    freeMemory();
    bytecode = (uint8_t *)malloc(size + 1);
    if (!bytecode)
    {
        file.close();
        return false;
    }
    file.read(bytecode, size);
    bytecode[size] = T_EOF;
    file.close();
    isAllocated = true;
    bytecodeSize = (uint16_t)(size + 1);
    codeEnd = bytecode + bytecodeSize;
    buildLineIndex();
    linkBytecode();
    return true;
}

// =============================================================
//  Индекс строк + линковка
// =============================================================
void LedBasic::buildLineIndex()
{
    lineCount = 0;
    uint8_t *p = bytecode;
    while (p + 3 <= codeEnd && lineCount < MAX_LINES)
    {
        uint8_t len = p[2];
        if (p + 3 + len > codeEnd)
            break;
        lineIndex[lineCount].number = ((uint16_t)p[0] << 8) | p[1];
        lineIndex[lineCount].bodyPtr = p + 3;
        lineCount++;
        p += 3 + len;
    }
    for (uint8_t i = 0; i < lineCount - 1; i++)
        for (uint8_t j = 0; j < lineCount - 1 - i; j++)
            if (lineIndex[j].number > lineIndex[j + 1].number)
            {
                LineEntry tmp = lineIndex[j];
                lineIndex[j] = lineIndex[j + 1];
                lineIndex[j + 1] = tmp;
            }
}

uint8_t *LedBasic::findLine(uint16_t num) const
{
    int lo = 0, hi = (int)lineCount - 1;
    while (lo <= hi)
    {
        int mid = (lo + hi) >> 1;
        if (lineIndex[mid].number == num)
            return lineIndex[mid].bodyPtr;
        else if (lineIndex[mid].number < num)
            lo = mid + 1;
        else
            hi = mid - 1;
    }
    return nullptr;
}

void LedBasic::linkBytecode()
{
    uint8_t *p = bytecode;
    while (p < codeEnd)
    {
        uint8_t b = *p;
        if (b == T_EOF)
            break;

        if ((b == CMD_GOTO || b == CMD_GOSUB) && p + 3 < codeEnd && p[1] == T_NUM)
        {
            uint16_t lineNum = ((uint16_t)p[2] << 8) | p[3];
            uint8_t *dest = findLine(lineNum);
            if (dest)
            {
                uint16_t addr = (uint16_t)(dest - bytecode);
                p[0] = (b == CMD_GOTO) ? CMD_GOTO_DIRECT : CMD_GOSUB_DIRECT;
                p[1] = (addr >> 8) & 0xFF;
                p[2] = addr & 0xFF;
                p += 4;
                continue;
            }
        }
        p++;
        if (b == T_NUM)
        {
            p += 2;
            continue;
        }
        if (b == T_VAR)
        {
            p += 1;
            continue;
        }
        if (b == CMD_GOTO_DIRECT || b == CMD_GOSUB_DIRECT)
        {
            p += 2;
            continue;
        }
    }
    LB_LOGLN("[LedBasic] linkBytecode OK");
}

// =============================================================
//  Управление
// =============================================================
void LedBasic::play()
{
    if (!bytecode || lineCount == 0)
        return;
    memset(vars, 0, sizeof(vars));
    if (pixelBuf) memset(pixelBuf, 0, (size_t)num_leds * 3);
    codePtr = lineIndex[0].bodyPtr;
    sp = 0;
    fp = 0;
    state = VM_RUNNING;
    LB_LOG("[LedBasic] play() ptr=%p\n", codePtr);
}

void LedBasic::stop()
{
    state = VM_STOPPED;
    if (pixelBuf) memset(pixelBuf, 0, (size_t)num_leds * 3);
    cbClear();
}
bool LedBasic::isRunning() const { return state != VM_STOPPED; }

// =============================================================
//  skipToEOL — осознанный пропуск токенов до T_EOL
// =============================================================
void LedBasic::skipToEOL()
{
    while (codePtr < codeEnd)
    {
        uint8_t tok = *codePtr;
        if (tok == T_EOL)
            return;
        codePtr++;
        if (tok == T_NUM)
        {
            codePtr += 2;
        }
        else if (tok == T_VAR)
        {
            codePtr += 1;
        }
        else if (tok == CMD_GOTO_DIRECT || tok == CMD_GOSUB_DIRECT)
        {
            codePtr += 2;
        }
    }
}

// =============================================================
//  Вычислитель выражений
// =============================================================
int16_t LedBasic::evalExpr() { return evalAddSub(); }

int16_t LedBasic::evalAddSub()
{
    int16_t left = evalMulDiv();
    for (;;)
    {
        uint8_t op = *codePtr;
        if (op != OP_PLUS && op != OP_MINUS && op != OP_AND && op != OP_OR)
            break;
        codePtr++;
        int16_t right = evalMulDiv();
        if (op == OP_PLUS)
            left += right;
        if (op == OP_MINUS)
            left -= right;
        if (op == OP_AND)
            left &= right;
        if (op == OP_OR)
            left |= right;
    }
    return left;
}

int16_t LedBasic::evalMulDiv()
{
    int16_t left = evalUnary();
    for (;;)
    {
        uint8_t op = *codePtr;
        if (op != OP_MUL && op != OP_DIV && op != OP_MOD)
            break;
        codePtr++;
        int16_t right = evalUnary();
        if (op == OP_MUL)
            left = left * right;
        if (op == OP_DIV)
            left = right ? left / right : 0;
        if (op == OP_MOD)
            left = right ? left % right : 0;
    }
    return left;
}

int16_t LedBasic::evalUnary()
{
    if (*codePtr == OP_MINUS)
    {
        codePtr++;
        return -evalPrimary();
    }
    return evalPrimary();
}

int16_t LedBasic::evalPrimary()
{
    uint8_t tok = fetch();
    if (tok == T_NUM)
        return fetchI16();
    if (tok == T_VAR)
        return vars[fetch()];
    if (tok == OP_RND)
    {
        int16_t lo = evalExpr();
        codePtr++;
        int16_t hi = evalExpr();
        if (lo > hi)
        {
            int16_t t = lo;
            lo = hi;
            hi = t;
        }
        return (int16_t)random(lo, hi + 1);
    }
    if (tok == OP_ABS)
    {
        int16_t v = evalExpr();
        return v < 0 ? -v : v;
    }
    if (tok == OP_MIN)
    {
        int16_t a = evalExpr();
        codePtr++;
        int16_t b = evalExpr();
        return a < b ? a : b;
    }
    if (tok == OP_MAX)
    {
        int16_t a = evalExpr();
        codePtr++;
        int16_t b = evalExpr();
        return a > b ? a : b;
    }
    if (tok == OP_SIN8)
    {
        return (int16_t)sin8((uint8_t)evalExpr());
    }
    if (tok == OP_COS8)
    {
        return (int16_t)cos8((uint8_t)evalExpr());
    }
    if (tok == OP_NOISE)
    {
        int16_t x = evalExpr();
        codePtr++;
        int16_t y = evalExpr();
        return (int16_t)inoise8((uint16_t)x, (uint16_t)y);
    }
    if (tok == OP_PIXEL)
    {
        return (int16_t)num_leds - 1;
    }
    if (tok == OP_EXP8)
    {
        int16_t v = evalExpr();
        return (int16_t)exp8((uint8_t)clamp8(v));
    }
    if (tok == OP_CONSTRAIN)
    {
        int16_t v = evalExpr();
        codePtr++;
        int16_t lo = evalExpr();
        codePtr++;
        int16_t hi = evalExpr();
        return v < lo ? lo : v > hi ? hi
                                    : v;
    }
    if (tok == OP_MAP)
    {
        int16_t val = evalExpr();
        codePtr++;
        int16_t i0 = evalExpr();
        codePtr++;
        int16_t i1 = evalExpr();
        codePtr++;
        int16_t o0 = evalExpr();
        codePtr++;
        int16_t o1 = evalExpr();
        if (i1 == i0)
            return o0;
        return (int16_t)((int32_t)(val - i0) * (o1 - o0) / (i1 - i0) + o0);
    }
    if (tok == T_LPAREN)
    {
        int16_t v = evalExpr();
        if (*codePtr == T_RPAREN)
            codePtr++;
        return v;
    }
    return 0;
}

// =============================================================
//  Хелпер: запись пикселя + обновление буфера кадра
// =============================================================
IRAM_ATTR void LedBasic::doSetPixel(uint16_t pos, uint8_t r, uint8_t g, uint8_t b)
{
    if (pixelBuf)
    {
        uint16_t idx = pos * 3;
        pixelBuf[idx]     = r;
        pixelBuf[idx + 1] = g;
        pixelBuf[idx + 2] = b;
    }
    cbSetPixel(pos, r, g, b);
}

// =============================================================
//  Вспомогательный макрос для SET_HSV (используется дважды)
// =============================================================
#define DO_SET_HSV()                                \
    {                                               \
        int16_t _pos = evalExpr();                  \
        codePtr++;                                  \
        uint8_t _h = (uint8_t)evalExpr();           \
        codePtr++;                                  \
        uint8_t _s = (uint8_t)clamp8(evalExpr());   \
        codePtr++;                                  \
        uint8_t _v = (uint8_t)clamp8(evalExpr());   \
        if (_pos >= 0 && _pos < num_leds)           \
        {                                           \
            uint8_t _r, _g, _b;                     \
            hsv2rgb_fast(_h, _s, _v, _r, _g, _b);   \
            doSetPixel((uint16_t)_pos, _r, _g, _b); \
        }                                           \
    }

#define DO_SET()                                                              \
    {                                                                         \
        int16_t _pos = evalExpr();                                            \
        codePtr++;                                                            \
        int16_t _r = evalExpr();                                              \
        codePtr++;                                                            \
        int16_t _g = evalExpr();                                              \
        codePtr++;                                                            \
        int16_t _b = evalExpr();                                              \
        if (_pos >= 0 && _pos < num_leds)                                     \
            doSetPixel((uint16_t)_pos, clamp8(_r), clamp8(_g), clamp8(_b));   \
    }

// =============================================================
//  Главный цикл VM
// =============================================================
void LedBasic::tick()
{
    if (state == VM_STOPPED)
        return;

    if (state == VM_WAITING)
    {
        if ((int32_t)(millis() - waitTimer) >= 0)
            state = VM_RUNNING;
        else
            return;
    }

    int budget = 1000;
    while (state == VM_RUNNING && budget-- > 0)
    {
        if (codePtr >= codeEnd)
        {
            state = VM_STOPPED;
            return;
        }

        uint8_t opcode = fetch();

        switch (opcode)
        {

        case T_EOF:
            state = VM_STOPPED;
            return;

        case T_EOL:
            if (codePtr + 2 >= codeEnd)
                state = VM_STOPPED;
            else
                codePtr += 3;
            break;

        case T_VAR:
        {
            uint8_t idx = fetch();
            codePtr++;
            vars[idx] = evalExpr();
            break;
        }

        case CMD_SET:
            DO_SET();
            break;

        case CMD_SET_HSV:
            DO_SET_HSV();
            break;

        case CMD_CLEAR:
            if (pixelBuf) memset(pixelBuf, 0, (size_t)num_leds * 3);
            cbClear();
            break;

        case CMD_FILL:
        {
            int16_t r = evalExpr();
            codePtr++;
            int16_t g = evalExpr();
            codePtr++;
            int16_t b = evalExpr();
            uint8_t rb = clamp8(r), gb = clamp8(g), bb = clamp8(b);
            for (uint16_t i = 0; i < num_leds; i++)
                doSetPixel(i, rb, gb, bb);
            break;
        }

        case CMD_SHOW:
            cbShow();
            break;

        case CMD_WAIT:
            cbShow();
            waitTimer = millis() + applySpeed(evalExpr());
            state = VM_WAITING;
            return;

        case CMD_DELAY:
            waitTimer = millis() + applySpeed(evalExpr());
            state = VM_WAITING;
            return;

        case CMD_BRIGHT:
            evalExpr();
            break;

        case CMD_FADE:
        {
            int16_t pos = evalExpr();
            codePtr++;
            int16_t amt = evalExpr();
            if (pos >= 0 && pos < (int16_t)num_leds && pixelBuf)
            {
                uint16_t idx = (uint16_t)pos * 3;
                uint8_t r = pixelBuf[idx];
                uint8_t g = pixelBuf[idx + 1];
                uint8_t b = pixelBuf[idx + 2];
                uint8_t a = clamp8(amt);
                doSetPixel((uint16_t)pos,
                    (r >= a) ? (uint8_t)(r - a) : 0,
                    (g >= a) ? (uint8_t)(g - a) : 0,
                    (b >= a) ? (uint8_t)(b - a) : 0);
            }
            break;
        }

        case CMD_MIRROR:
        {
            if (pixelBuf)
            {
                uint16_t half = num_leds / 2;
                for (uint16_t i = 0; i < half; i++)
                {
                    uint16_t src = i * 3;
                    uint16_t mirrorPos = num_leds - 1 - i;
                    doSetPixel(mirrorPos,
                        pixelBuf[src],
                        pixelBuf[src + 1],
                        pixelBuf[src + 2]);
                }
            }
            break;
        }

        case CMD_GOTO_DIRECT:
        {
            uint16_t addr = fetchU16();
            codePtr = bytecode + addr;
            break;
        }

        case CMD_GOSUB_DIRECT:
        {
            uint16_t addr = fetchU16();
            if (sp < CALL_DEPTH)
            {
                callStack[sp++] = codePtr;
                codePtr = bytecode + addr;
            }
            break;
        }

        case CMD_GOTO:
        {
            uint8_t *dest = findLine((uint16_t)evalExpr());
            if (dest)
                codePtr = dest;
            else
                state = VM_STOPPED;
            break;
        }

        case CMD_GOSUB:
        {
            uint8_t *dest = findLine((uint16_t)evalExpr());
            if (dest && sp < CALL_DEPTH)
            {
                callStack[sp++] = codePtr;
                codePtr = dest;
            }
            break;
        }

        case CMD_RETURN:
            if (sp > 0)
                codePtr = callStack[--sp];
            break;

        case CMD_IF:
        {
            int16_t left = evalExpr();
            uint8_t op = fetch();
            int16_t right = evalExpr();
            if (*codePtr == CMD_THEN)
                codePtr++;

            bool cond = false;
            switch (op)
            {
            case OP_EQ:
                cond = (left == right);
                break;
            case OP_NEQ:
                cond = (left != right);
                break;
            case OP_GT:
                cond = (left > right);
                break;
            case OP_LT:
                cond = (left < right);
                break;
            case OP_GE:
                cond = (left >= right);
                break;
            case OP_LE:
                cond = (left <= right);
                break;
            }

            if (cond)
            {
                uint8_t thenOp = fetch();
                switch (thenOp)
                {
                case CMD_SET:
                    DO_SET();
                    break;
                case CMD_SET_HSV:
                    DO_SET_HSV();
                    break;
                case T_VAR:
                {
                    uint8_t idx = fetch();
                    codePtr++;
                    vars[idx] = evalExpr();
                    break;
                }
                case CMD_GOTO_DIRECT:
                {
                    uint16_t addr = fetchU16();
                    skipToEOL();
                    codePtr = bytecode + addr;
                    goto next_iter;
                }
                case CMD_GOSUB_DIRECT:
                {
                    uint16_t addr = fetchU16();
                    skipToEOL();
                    if (sp < CALL_DEPTH)
                    {
                        callStack[sp++] = codePtr;
                        codePtr = bytecode + addr;
                    }
                    goto next_iter;
                }
                case CMD_GOTO:
                {
                    uint8_t *dest = findLine((uint16_t)evalExpr());
                    if (dest)
                    {
                        skipToEOL();
                        codePtr = dest;
                        goto next_iter;
                    }
                    break;
                }
                case CMD_GOSUB:
                {
                    uint8_t *dest = findLine((uint16_t)evalExpr());
                    if (dest && sp < CALL_DEPTH)
                    {
                        skipToEOL();
                        callStack[sp++] = codePtr;
                        codePtr = dest;
                        goto next_iter;
                    }
                    break;
                }
                case CMD_RETURN:
                    if (sp > 0)
                    {
                        skipToEOL();
                        codePtr = callStack[--sp];
                        goto next_iter;
                    }
                    break;
                case CMD_FADE:
                {
                    int16_t pos = evalExpr();
                    codePtr++;
                    int16_t amt = evalExpr();
                    if (pos >= 0 && pos < (int16_t)num_leds && pixelBuf)
                    {
                        uint16_t idx = (uint16_t)pos * 3;
                        uint8_t a = clamp8(amt);
                        doSetPixel((uint16_t)pos,
                            (pixelBuf[idx]   >= a) ? (uint8_t)(pixelBuf[idx]   - a) : 0,
                            (pixelBuf[idx+1] >= a) ? (uint8_t)(pixelBuf[idx+1] - a) : 0,
                            (pixelBuf[idx+2] >= a) ? (uint8_t)(pixelBuf[idx+2] - a) : 0);
                    }
                    break;
                }
                case CMD_MIRROR:
                    if (pixelBuf)
                    {
                        uint16_t half = num_leds / 2;
                        for (uint16_t i = 0; i < half; i++)
                        {
                            uint16_t src = i * 3;
                            doSetPixel(num_leds - 1 - i,
                                pixelBuf[src], pixelBuf[src+1], pixelBuf[src+2]);
                        }
                    }
                    break;
                }
            }
            skipToEOL();
            break;
        }

        case CMD_FOR:
        {
            if (fp >= FOR_DEPTH)
            {
                skipToEOL();
                break;
            }
            codePtr++;
            uint8_t varIdx = fetch();
            codePtr++;
            vars[varIdx] = evalExpr();
            codePtr++;
            int16_t endVal = evalExpr();
            int16_t stepVal = 1;
            if (*codePtr == CMD_STEP)
            {
                codePtr++;
                stepVal = evalExpr();
            }
            skipToEOL();
            codePtr++;
            codePtr += 3;
            forStack[fp] = {varIdx, endVal, stepVal, codePtr};
            fp++;
            break;
        }

        case CMD_NEXT:
        {
            codePtr++;
            uint8_t varIdx = fetch();
            if (fp == 0)
                break;
            uint8_t fi = fp - 1;
            vars[varIdx] += forStack[fi].step;
            bool cont = (forStack[fi].step > 0)
                            ? (vars[varIdx] <= forStack[fi].limit)
                            : (vars[varIdx] >= forStack[fi].limit);
            if (cont)
                codePtr = forStack[fi].bodyPtr;
            else
                fp--;
            break;
        }

        default:
            skipToEOL();
            break;
        }
    next_iter:;
    }
}

// =============================================================
//  КОМПИЛЯТОР
// =============================================================
struct Keyword
{
    const char *word;
    Token tok;
};
static const Keyword KEYWORDS[] = {
    {"FOR", CMD_FOR},
    {"TO", CMD_TO},
    {"STEP", CMD_STEP},
    {"NEXT", CMD_NEXT},
    {"IF", CMD_IF},
    {"THEN", CMD_THEN},
    {"GOTO", CMD_GOTO},
    {"GOSUB", CMD_GOSUB},
    {"RETURN", CMD_RETURN},
    {"CLEAR", CMD_CLEAR},
    {"FILL", CMD_FILL},
    {"SET_HSV", CMD_SET_HSV},
    {"SET", CMD_SET},
    {"WAIT", CMD_WAIT},
    {"DELAY", CMD_DELAY},
    {"SHOW", CMD_SHOW},
    {"BRIGHT", CMD_BRIGHT},
    {"FADE",   CMD_FADE},
    {"MIRROR", CMD_MIRROR},
    {"NOISE",  OP_NOISE},
    {"CONSTRAIN", OP_CONSTRAIN},
    {"RND", OP_RND},
    {"ABS", OP_ABS},
    {"MIN", OP_MIN},
    {"MAX", OP_MAX},
    {"SIN8", OP_SIN8},
    {"COS8", OP_COS8},
    {"EXP8", OP_EXP8},
    {"MAP", OP_MAP},
    {"PIXEL", OP_PIXEL},
    {"AND", OP_AND},
    {"OR", OP_OR},
    {"==", OP_EQ},
    {"!=", OP_NEQ},
    {">=", OP_GE},
    {"<=", OP_LE},
    {">", OP_GT},
    {"<", OP_LT},
    {"=", OP_ASSIGN},
    {"+", OP_PLUS},
    {"-", OP_MINUS},
    {"*", OP_MUL},
    {"/", OP_DIV},
    {"%", OP_MOD},
    {",", T_COMMA},
};
static const int KW_COUNT = sizeof(KEYWORDS) / sizeof(KEYWORDS[0]);

uint16_t LedBasic::tokenizeLine(const char *lineStr, uint8_t *buf)
{
    uint16_t pos = 0;
    const char *p = lineStr;
    while (*p == ' ' || *p == '\t')
        p++;
    if (!*p || *p == '\'' || *p == '\r' || *p == '\n')
        return 0;
    if (!isdigit((unsigned char)*p))
        return 0;
    int lineNum = atoi(p);
    if (lineNum <= 0)
        return 0;
    buf[pos++] = (lineNum >> 8) & 0xFF;
    buf[pos++] = lineNum & 0xFF;
    uint16_t lenIdx = pos++;
    while (isdigit((unsigned char)*p))
        p++;
    while (*p && *p != '\r' && *p != '\n')
    {
        if (*p == ' ' || *p == '\t')
        {
            p++;
            continue;
        }
        if (*p == '\'')
            break;
        bool found = false;
        for (int k = 0; k < KW_COUNT; k++)
        {
            size_t kwLen = strlen(KEYWORDS[k].word);
            if (strncmp(p, KEYWORDS[k].word, kwLen) == 0)
            {
                char after = p[kwLen];
                if (isalpha((unsigned char)KEYWORDS[k].word[0]) && (isalnum((unsigned char)after) || after == '_'))
                    continue;
                buf[pos++] = (uint8_t)KEYWORDS[k].tok;
                p += kwLen;
                found = true;
                break;
            }
        }
        if (found)
            continue;
        if (isupper((unsigned char)*p) && !isalnum((unsigned char)*(p + 1)))
        {
            buf[pos++] = T_VAR;
            buf[pos++] = (uint8_t)(*p - 'A');
            p++;
            continue;
        }
        if (isdigit((unsigned char)*p))
        {
            int16_t val = (int16_t)atoi(p);
            while (isdigit((unsigned char)*p))
                p++;
            buf[pos++] = T_NUM;
            buf[pos++] = (val >> 8) & 0xFF;
            buf[pos++] = val & 0xFF;
            continue;
        }
        if (*p == '(')
        {
            buf[pos++] = T_LPAREN;
            p++;
            continue;
        }
        if (*p == ')')
        {
            buf[pos++] = T_RPAREN;
            p++;
            continue;
        }
        LB_LOG("[LedBasic] Unknown char: '%c'\n", *p);
        p++;
    }
    buf[pos++] = T_EOL;
    buf[lenIdx] = (uint8_t)(pos - 3);
    return pos;
}

bool LedBasic::compileFromText(const char *scriptText)
{
    freeMemory();
    const size_t TMPBUF = 4096;
    uint8_t *tmp = (uint8_t *)malloc(TMPBUF);
    if (!tmp)
        return false;
    uint16_t wp = 0;
    char *src = strdup(scriptText);
    if (!src)
    {
        free(tmp);
        return false;
    }
    char *line = strtok(src, "\n");
    while (line && wp < TMPBUF - 64)
    {
        uint16_t n = tokenizeLine(line, &tmp[wp]);
        wp += n;
        line = strtok(nullptr, "\n");
    }
    tmp[wp++] = T_EOF;
    free(src);
    bytecode = (uint8_t *)malloc(wp);
    if (!bytecode)
    {
        free(tmp);
        return false;
    }
    memcpy(bytecode, tmp, wp);
    free(tmp);
    isAllocated = true;
    bytecodeSize = wp;
    codeEnd = bytecode + bytecodeSize;
    buildLineIndex();
    linkBytecode();
    LB_LOG("[LedBasic] compiled: %u bytes, %u lines\n", wp, lineCount);
    return true;
}

bool LedBasic::compileBasToBin(const char *basPath, const char *binPath)
{
    if (!LittleFS.begin())
        return false;
    File fIn = LittleFS.open(basPath, "r");
    File fOut = LittleFS.open(binPath, "w");
    if (!fIn || !fOut)
    {
        fIn.close();
        fOut.close();
        return false;
    }
    uint8_t buf[256];
    while (fIn.available())
    {
        String ln = fIn.readStringUntil('\n');
        uint16_t n = tokenizeLine(ln.c_str(), buf);
        if (n > 0)
            fOut.write(buf, n);
    }
    uint8_t eof = T_EOF;
    fOut.write(&eof, 1);
    fIn.close();
    fOut.close();
    return true;
}

// =============================================================
//  2D Value Noise
// =============================================================
uint8_t LedBasic::inoise8(uint16_t x, uint16_t y)
{
    uint8_t xi = x >> 8, yi = y >> 8, xf = x & 0xFF, yf = y & 0xFF;
    auto ease = [](uint8_t u) -> uint8_t
    {
        uint16_t uu = u;
        return (uint8_t)((uu * uu * (765 - 2 * uu)) >> 16);
    };
    uint8_t u = ease(xf), v = ease(yf);
    auto hash = [](uint8_t i, uint8_t j) -> uint8_t
    {
        uint32_t s = ((uint32_t)i * 0x9E3779B9) + ((uint32_t)j * 0x85EBCA6B);
        s ^= s >> 13;
        s *= 0xC2B2AE35;
        s ^= s >> 16;
        return s & 0xFF;
    };
    uint8_t h00 = hash(xi, yi), h10 = hash(xi + 1, yi);
    uint8_t h01 = hash(xi, yi + 1), h11 = hash(xi + 1, yi + 1);
    int16_t nx0 = h00 + (((int16_t)h10 - h00) * u >> 8);
    int16_t nx1 = h01 + (((int16_t)h11 - h01) * u >> 8);
    return nx0 + (((int16_t)nx1 - nx0) * v >> 8);
}