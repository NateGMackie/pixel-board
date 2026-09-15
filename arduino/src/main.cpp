#include <Arduino.h>
#include <esp_system.h>
#include <FastLED.h>

void showDisplay();
void printStatus();
void resetAnimation();
void renderAnimationFrame();

// -------------------------
// Display Configuration
// -------------------------

const int DISPLAY_WIDTH = 32;
const int DISPLAY_HEIGHT = 8;
const int LED_COUNT = DISPLAY_WIDTH * DISPLAY_HEIGHT;

#define DATA_PIN 17

CRGB physicalLeds[LED_COUNT];

// -------------------------
// Color
// -------------------------

struct Color
{
    int red;
    int green;
    int blue;
};

const Color BLACK   = {0, 0, 0};
const Color RED     = {255, 0, 0};
const Color GREEN   = {0, 255, 0};
const Color BLUE    = {0, 0, 255};
const Color WHITE   = {255, 255, 255};
const Color CYAN    = {0, 255, 255};
const Color MAGENTA = {255, 0, 255};
const Color YELLOW  = {255, 255, 0};
const Color ORANGE  = {188, 127, 25};

// -------------------------
// Frame Buffer
// -------------------------

Color leds[LED_COUNT];

// -------------------------
// Font Configuration
// -------------------------

const int FONT_WIDTH = 5;
const int FONT_HEIGHT = 7;
const int FONT_SPACING = 1;

struct Glyph
{
    char symbol;
    char pattern[FONT_HEIGHT][FONT_WIDTH + 1];
};

const Glyph FONT[] =
{
    // A
    {
        'A',
        {
            ".###.",
            "#...#",
            "#...#",
            "#####",
            "#...#",
            "#...#",
            "#...#"
        }
    },

    // B
    {
        'B',
        {
            "####.",
            "#...#",
            "#...#",
            "####.",
            "#...#",
            "#...#",
            "####."
        }
    },

    // C
    {
        'C',
        {
            ".####",
            "#....",
            "#....",
            "#....",
            "#....",
            "#....",
            ".####"
        }
    },

    // D
    {
        'D',
        {
            "###..",
            "#..#.",
            "#...#",
            "#...#",
            "#...#",
            "#..#.",
            "###.."
        }
    },

    // E
    {
        'E',
        {
            "#####",
            "#....",
            "#....",
            "####.",
            "#....",
            "#....",
            "#####"
        }
    },

    // F
    {
        'F',
        {
            "#####",
            "#....",
            "#....",
            "####.",
            "#....",
            "#....",
            "#...."
        }
    },

    // G
    {
        'G',
        {
            ".###.",
            "#...#",
            "#....",
            "#.###",
            "#...#",
            "#...#",
            ".###."
        }
    },

    // H
    {
        'H',
        {
            "#...#",
            "#...#",
            "#...#",
            "#####",
            "#...#",
            "#...#",
            "#...#"
        }
    },

    // I
    {
        'I',
        {
            "#####",
            "..#..",
            "..#..",
            "..#..",
            "..#..",
            "..#..",
            "#####"
        }
    },

    // J
    {
        'J',
        {
            "#####",
            "....#",
            "....#",
            "....#",
            "#...#",
            "#...#",
            ".###."
        }
    },

    // K
    {
        'K',
        {
            "#...#",
            "#..#.",
            "#.#..",
            "##...",
            "#.#..",
            "#..#.",
            "#...#"
        }
    },

    // L
    {
        'L',
        {
            "#....",
            "#....",
            "#....",
            "#....",
            "#....",
            "#....",
            "#####"
        }
    },

    // M
    {
        'M',
        {
            "#...#",
            "##.##",
            "#.#.#",
            "#.#.#",
            "#...#",
            "#...#",
            "#...#"
        }
    },

    // N
    {
        'N',
        {
            "#...#",
            "#...#",
            "##..#",
            "#.#.#",
            "#..##",
            "#...#",
            "#...#"
        }
    },

    // O
    {
        'O',
        {
            ".###.",
            "#...#",
            "#...#",
            "#...#",
            "#...#",
            "#...#",
            ".###."
        }
    },

    // P
    {
        'P',
        {
            "####.",
            "#...#",
            "#...#",
            "####.",
            "#....",
            "#....",
            "#...."
        }
    },

    // Q
    {
        'Q',
        {
            ".###.",
            "#...#",
            "#...#",
            "#...#",
            "#.#.#",
            "#..#.",
            ".##.#"
        }
    },

    // R
    {
        'R',
        {
            "####.",
            "#...#",
            "#...#",
            "####.",
            "#.#..",
            "#..#.",
            "#...#"
        }
    },

    // S
    {
        'S',
        {
            ".####",
            "#....",
            "#....",
            ".###.",
            "....#",
            "....#",
            "####."
        }
    },

    // T
    {
        'T',
        {
            "#####",
            "..#..",
            "..#..",
            "..#..",
            "..#..",
            "..#..",
            "..#.."
        }
    },

    // U
    {
        'U',
        {
            "#...#",
            "#...#",
            "#...#",
            "#...#",
            "#...#",
            "#...#",
            ".###."
        }
    },

    // V
    {
        'V',
        {
            "#...#",
            "#...#",
            "#...#",
            "#...#",
            "#...#",
            ".#.#.",
            "..#.."
        }
    },

    // W
    {
        'W',
        {
            "#...#",
            "#...#",
            "#...#",
            "#.#.#",
            "#.#.#",
            "#.#.#",
            ".#.#."
        }
    },

    // X
    {
        'X',
        {
            "#...#",
            "#...#",
            ".#.#.",
            "..#..",
            ".#.#.",
            "#...#",
            "#...#"
        }
    },

    // Y
    {
        'Y',
        {
            "#...#",
            "#...#",
            ".#.#.",
            "..#..",
            "..#..",
            "..#..",
            "..#.."
        }
    },

    // Z
    {
        'Z',
        {
            "#####",
            "....#",
            "...#.",
            "..#..",
            ".#...",
            "#....",
            "#####"
        }
    },

    // 0
    {
        '0',
        {
            ".###.",
            "#...#",
            "#..##",
            "#.#.#",
            "##..#",
            "#...#",
            ".###."
        }
    },

    // 1
    {
        '1',
        {
            "..#..",
            ".##..",
            "..#..",
            "..#..",
            "..#..",
            "..#..",
            ".###."
        }
    },

    // 2
    {
        '2',
        {
            ".###.",
            "#...#",
            "....#",
            "...#.",
            "..#..",
            ".#...",
            "#####"
        }
    },

    // 3
    {
        '3',
        {
            "####.",
            "....#",
            "....#",
            ".###.",
            "....#",
            "....#",
            "####."
        }
    },

    // 4
    {
        '4',
        {
            "...#.",
            "..##.",
            ".#.#.",
            "#..#.",
            "#####",
            "...#.",
            "...#."
        }
    },

    // 5
    {
        '5',
        {
            "#####",
            "#....",
            "#....",
            "####.",
            "....#",
            "....#",
            "####."
        }
    },

    // 6
    {
        '6',
        {
            ".###.",
            "#....",
            "#....",
            "####.",
            "#...#",
            "#...#",
            ".###."
        }
    },

    // 7
    {
        '7',
        {
            "#####",
            "....#",
            "...#.",
            "..#..",
            ".#...",
            ".#...",
            ".#..."
        }
    },

    // 8
    {
        '8',
        {
            ".###.",
            "#...#",
            "#...#",
            ".###.",
            "#...#",
            "#...#",
            ".###."
        }
    },

    // 9
    {
        '9',
        {
            ".###.",
            "#...#",
            "#...#",
            ".####",
            "....#",
            "....#",
            ".###."
        }
    },

    // Apostrophe
    {
        '\'',
        {
            "..#..",
            "..#..",
            ".#...",
            ".....",
            ".....",
            ".....",
            "....."
        }
    },

    // Colon
    {
        ':',
        {
            ".....",
            "..#..",
            "..#..",
            ".....",
            "..#..",
            "..#..",
            "....."
        }
    },

    // Exclamation mark
    {
        '!',
        {
            "..#..",
            "..#..",
            "..#..",
            "..#..",
            "..#..",
            ".....",
            "..#.."
        }
    },

    // Question mark
    {
        '?',
        {
            ".###.",
            "#...#",
            "....#",
            "...#.",
            "..#..",
            ".....",
            "..#.."
        }
    },

    // Space
    {
        ' ',
        {
            ".....",
            ".....",
            ".....",
            ".....",
            ".....",
            ".....",
            "....."
        }
    }
};

// -------------------------
// Graphics Functions
// -------------------------

void clearDisplay()
{
    for (int i = 0; i < LED_COUNT; i++)
    {
        leds[i] = BLACK;
    }
}

int getIndex(int x, int y)
{
    if (x % 2 == 0)
    {
        // Even column: top to bottom.
        return x * DISPLAY_HEIGHT + y;
    }
    else
    {
        // Odd column: bottom to top.
        return x * DISPLAY_HEIGHT + (DISPLAY_HEIGHT - 1 - y);
    }
}

void setPixel(int x, int y, Color color)
{
    if (x < 0 || x >= DISPLAY_WIDTH ||
        y < 0 || y >= DISPLAY_HEIGHT)
    {
        return;
    }

    int index = getIndex(x, y);
    leds[index] = color;
}

void drawHorizontalLine(int startX, int y, int length, Color color)
{
    for (int x = startX; x < startX + length; x++)
    {
        setPixel(x, y, color);
    }
}

void drawVerticalLine(int x, int startY, int length, Color color)
{
    for (int y = startY; y < startY + length; y++)
    {
        setPixel(x, y, color);
    }
}

void drawSolidSquare()
{
    int startX = 0;
    int startY = 0;
    int length = 16;
    int width = 16;

    for (int y = startY; y < startY + width; y++)
    {
        drawHorizontalLine(startX, y, length, WHITE);
    }
}

void drawOutlineSquare()
{
    int startX = 2;
    int startY = 2;
    int width = 12;
    int height = 12;

    drawHorizontalLine(startX, startY, width, RED);
    drawVerticalLine(startX, startY, height, RED);
    drawVerticalLine(startX + width - 1, startY, height, RED);
    drawHorizontalLine(startX, startY + height - 1, width, RED);
}

void drawHeart()
{
    drawHorizontalLine(4, 3, 2, MAGENTA);
    drawHorizontalLine(9, 3, 2, MAGENTA);
    drawHorizontalLine(3, 4, 4, MAGENTA);
    drawHorizontalLine(8, 4, 4, MAGENTA);

    drawHorizontalLine(2, 5, 2, MAGENTA);
    drawHorizontalLine(4, 5, 1, WHITE);
    drawHorizontalLine(5, 5, 8, MAGENTA);
    drawHorizontalLine(2, 6, 1, MAGENTA);
    drawHorizontalLine(3, 6, 1, WHITE);
    drawHorizontalLine(4, 6, 9, MAGENTA);
    drawHorizontalLine(2, 7, 11, MAGENTA);
    drawHorizontalLine(3, 8, 9, MAGENTA);
    drawHorizontalLine(4, 9, 7, MAGENTA);
    drawHorizontalLine(5, 10, 5, MAGENTA);
    drawHorizontalLine(6, 11, 3, MAGENTA);
    drawHorizontalLine(7, 12, 1, MAGENTA);
}

const char (*getCharacterPattern(char letter))[FONT_WIDTH + 1]
{
    const int fontSize = sizeof(FONT) / sizeof(FONT[0]);

    for (int i = 0; i < fontSize; i++)
    {
        if (FONT[i].symbol == letter)
        {
            return FONT[i].pattern;
        }
    }

    return nullptr;
}

void drawCharacter(char letter, int startX, int startY, Color color)
{
    const char (*character)[FONT_WIDTH + 1] =
        getCharacterPattern(letter);

    if (character == nullptr)
    {
        return;
    }

    for (int y = 0; y < FONT_HEIGHT; y++)
    {
        for (int x = 0; x < FONT_WIDTH; x++)
        {
            if (character[y][x] == '#')
            {
                setPixel(startX + x, startY + y, color);
            }
        }
    }
}

void drawText(
    const char text[],
    int startX,
    int startY,
    Color color)
{
    int x = startX;

    for (int i = 0; text[i] != '\0'; i++)
    {
        drawCharacter(text[i], x, startY, color);
        x += FONT_WIDTH + FONT_SPACING;
    }
}

int getTextWidth(const char text[])
{
    int length = 0;

    while (text[length] != '\0')
    {
        length++;
    }

    return length * (FONT_WIDTH + FONT_SPACING);
}

void drawIdleAnimation()
{
    const Color colors[] =
    {
        RED,
        GREEN,
        BLUE,
        MAGENTA,
        CYAN,
        YELLOW,
        WHITE
    };

    const int colorCount = sizeof(colors) / sizeof(colors[0]);

    for (int c = 0; c < colorCount; c++)
    {
        for (int y = 0; y < DISPLAY_HEIGHT; y++)
        {
            for (int x = -4; x < DISPLAY_WIDTH; x++)
            {
                clearDisplay();
                drawHorizontalLine(x, y, 5, colors[c]);
                showDisplay();
                delay(500);
            }
        }
    }

    clearDisplay();
    showDisplay();
}

void showDisplay()
{
    for (int i = 0; i < LED_COUNT; i++)
    {
        physicalLeds[i] = CRGB(
            leds[i].red,
            leds[i].green,
            leds[i].blue
        );
    }

    FastLED.show();
}

// -------------------------
// Marquee Configuration
// -------------------------

const int MESSAGE_BUFFER_SIZE = 100;
const int COMMAND_BUFFER_SIZE = 256;
const int FRAME_ROW_HEX_LENGTH = DISPLAY_WIDTH * 6;

enum AnimationEffect
{
    EFFECT_STILL,
    EFFECT_SCROLL,
    EFFECT_WIPE,
    EFFECT_BLINK
};

enum AnimationDirection
{
    DIRECTION_LEFT,
    DIRECTION_RIGHT,
    DIRECTION_UP,
    DIRECTION_DOWN
};

enum ContentMode
{
    MODE_TEXT,
    MODE_DRAWING,
    MODE_PRESET
};

enum PresetId
{
    PRESET_SOLID,
    PRESET_CLOCK,
    PRESET_RAIN,
    PRESET_FIRE
};

enum FirePalette
{
    FIRE_CLASSIC,
    FIRE_BLUE,
    FIRE_PURPLE
};

char message[MESSAGE_BUFFER_SIZE] = "HELLO WORLD!";
char inputBuffer[COMMAND_BUFFER_SIZE];

int inputIndex = 0;

Color messageColor = WHITE;
const int TEXT_Y = 0;
int animationSpeed = 10;
unsigned long blinkOnMs = 450;
unsigned long blinkOffMs = 350;

AnimationEffect animationEffect = EFFECT_SCROLL;
AnimationDirection animationDirection = DIRECTION_LEFT;
ContentMode contentMode = MODE_TEXT;
PresetId activePreset = PRESET_SOLID;
bool animationPaused = false;

int textOffset = DISPLAY_WIDTH;
int textOffsetY = TEXT_Y;
int textWidth = 0;
int wipeProgress = 0;
bool blinkVisible = true;

unsigned long lastAnimationTime = 0;
unsigned long lastBlinkTime = 0;

int brightness = 2;
Color solidPresetColor = {255, 102, 0};
Color clockPresetColor = {255, 102, 0};
Color rainPresetColor = {0, 255, 102};
bool clockUse24Hour = false;
bool clockLeadingZero = false;
bool clockTimeValid = false;
unsigned long clockSyncMillis = 0;
unsigned long clockSyncUnixSeconds = 0;
int clockUtcOffsetMinutes = 0;
int lastRenderedClockMinute = -1;
int rainSpeed = 8;
int rainDensity = 45;
int rainTrailLength = 4;
bool rainColumnActive[DISPLAY_WIDTH];
int rainColumnHead[DISPLAY_WIDTH];
int rainColumnDelay[DISPLAY_WIDTH];
FirePalette firePalette = FIRE_CLASSIC;
int fireSpeed = 10;
int fireIntensity = 60;
uint8_t fireHeat[DISPLAY_HEIGHT][DISPLAY_WIDTH];
unsigned long lastAutonomousTime = 0;

Color activeDrawingFrame[LED_COUNT];
Color stagingDrawingFrame[LED_COUNT];
bool stagingRowsReceived[DISPLAY_HEIGHT];
bool receivingFrame = false;

const char *getEffectName()
{
    switch (animationEffect)
    {
        case EFFECT_STILL:
            return "STILL";
        case EFFECT_WIPE:
            return "WIPE";
        case EFFECT_BLINK:
            return "BLINK";
        default:
            return "SCROLL";
    }
}

const char *getDirectionName()
{
    switch (animationDirection)
    {
        case DIRECTION_RIGHT:
            return "RIGHT";
        case DIRECTION_UP:
            return "UP";
        case DIRECTION_DOWN:
            return "DOWN";
        default:
            return "LEFT";
    }
}

const char *getContentModeName()
{
    if (contentMode == MODE_DRAWING)
    {
        return "DRAWING";
    }

    if (contentMode == MODE_PRESET)
    {
        return "PRESET";
    }

    return "TEXT";
}

const char *getPresetName()
{
    switch (activePreset)
    {
        case PRESET_RAIN:
            return "RAIN";
        case PRESET_FIRE:
            return "FIRE";
        case PRESET_CLOCK:
            return "CLOCK";
        case PRESET_SOLID:
        default:
            return "SOLID";
    }
}

const char *getFirePaletteName()
{
    switch (firePalette)
    {
        case FIRE_BLUE:
            return "BLUE";
        case FIRE_PURPLE:
            return "PURPLE";
        case FIRE_CLASSIC:
        default:
            return "CLASSIC";
    }
}

bool isAutonomousPreset()
{
    return activePreset == PRESET_RAIN || activePreset == PRESET_FIRE;
}

Color scaleColor(Color color, uint8_t scale)
{
    return {
        (color.red * scale) / 255,
        (color.green * scale) / 255,
        (color.blue * scale) / 255
    };
}

Color blendColor(Color start, Color end, uint8_t amount)
{
    return {
        start.red + ((end.red - start.red) * amount) / 255,
        start.green + ((end.green - start.green) * amount) / 255,
        start.blue + ((end.blue - start.blue) * amount) / 255
    };
}

unsigned long getAnimationInterval()
{
    return 1000UL / animationSpeed;
}

unsigned long getAutonomousInterval()
{
    int speed = activePreset == PRESET_FIRE ? fireSpeed : rainSpeed;

    return 1000UL / constrain(speed, 1, 20);
}

int getCenteredTextX()
{
    return (DISPLAY_WIDTH - textWidth) / 2;
}

int getLogicalIndex(int x, int y)
{
    return y * DISPLAY_WIDTH + x;
}

void clearDrawingFrame(Color frame[])
{
    for (int i = 0; i < LED_COUNT; i++)
    {
        frame[i] = BLACK;
    }
}

void clearStagingRows()
{
    for (int y = 0; y < DISPLAY_HEIGHT; y++)
    {
        stagingRowsReceived[y] = false;
    }
}

void displayActiveDrawingFrame()
{
    clearDisplay();

    for (int y = 0; y < DISPLAY_HEIGHT; y++)
    {
        for (int x = 0; x < DISPLAY_WIDTH; x++)
        {
            setPixel(x, y, activeDrawingFrame[getLogicalIndex(x, y)]);
        }
    }

    showDisplay();
}

void drawDrawingFrame(int startX, int startY)
{
    for (int y = 0; y < DISPLAY_HEIGHT; y++)
    {
        for (int x = 0; x < DISPLAY_WIDTH; x++)
        {
            setPixel(
                startX + x,
                startY + y,
                activeDrawingFrame[getLogicalIndex(x, y)]
            );
        }
    }
}

int getContentWidth()
{
    return contentMode == MODE_TEXT ? textWidth : DISPLAY_WIDTH;
}

int getContentHeight()
{
    return contentMode == MODE_TEXT ? FONT_HEIGHT : DISPLAY_HEIGHT;
}

int getCenteredContentX()
{
    return contentMode == MODE_TEXT ? getCenteredTextX() : 0;
}

int getCenteredContentY()
{
    return contentMode == MODE_TEXT ? TEXT_Y : 0;
}

unsigned long getCurrentClockUnixSeconds()
{
    unsigned long elapsedSeconds =
        (millis() - clockSyncMillis) / 1000UL;

    return clockSyncUnixSeconds + elapsedSeconds;
}

int getCurrentClockMinute()
{
    if (!clockTimeValid)
    {
        return -1;
    }

    long localSeconds =
        (long)(getCurrentClockUnixSeconds() % 86400UL) +
        (long)clockUtcOffsetMinutes * 60L;

    while (localSeconds < 0)
    {
        localSeconds += 86400L;
    }

    localSeconds %= 86400L;

    return localSeconds / 60;
}

void formatClockText(char output[], size_t outputSize)
{
    if (!clockTimeValid)
    {
        strncpy(output, "--:--", outputSize - 1);
        output[outputSize - 1] = '\0';
        return;
    }

    int minuteOfDay = getCurrentClockMinute();
    int hours = minuteOfDay / 60;
    int minutes = minuteOfDay % 60;

    if (!clockUse24Hour)
    {
        hours %= 12;

        if (hours == 0)
        {
            hours = 12;
        }
    }

    if (clockLeadingZero)
    {
        snprintf(output, outputSize, "%02d:%02d", hours, minutes);
    }
    else
    {
        snprintf(output, outputSize, "%d:%02d", hours, minutes);
    }
}

int getClockTextWidth()
{
    char clockText[6];
    formatClockText(clockText, sizeof(clockText));

    return getTextWidth(clockText);
}

void drawClockPresetFrame(int startX, int startY)
{
    char clockText[6];
    formatClockText(clockText, sizeof(clockText));

    int x = startX + (DISPLAY_WIDTH - getTextWidth(clockText)) / 2;
    int y = startY + (DISPLAY_HEIGHT - FONT_HEIGHT) / 2;

    drawText(clockText, x, y, clockPresetColor);
}

void initializeRainPreset()
{
    for (int x = 0; x < DISPLAY_WIDTH; x++)
    {
        rainColumnActive[x] = false;
        rainColumnHead[x] = -1;
        rainColumnDelay[x] = random(0, DISPLAY_HEIGHT);
    }
}

void initializeFirePreset()
{
    for (int y = 0; y < DISPLAY_HEIGHT; y++)
    {
        for (int x = 0; x < DISPLAY_WIDTH; x++)
        {
            fireHeat[y][x] = 0;
        }
    }
}

void renderDigitalRainFrame()
{
    clearDisplay();

    for (int x = 0; x < DISPLAY_WIDTH; x++)
    {
        if (!rainColumnActive[x])
        {
            continue;
        }

        for (int offset = 0; offset < rainTrailLength; offset++)
        {
            int y = rainColumnHead[x] - offset;

            if (y < 0 || y >= DISPLAY_HEIGHT)
            {
                continue;
            }

            Color color = offset == 0
                ? blendColor(rainPresetColor, WHITE, 140)
                : scaleColor(
                    rainPresetColor,
                    constrain(255 - (offset * 190 / rainTrailLength), 35, 255)
                );

            setPixel(x, y, color);
        }
    }

    showDisplay();
}

void advanceDigitalRain()
{
    for (int x = 0; x < DISPLAY_WIDTH; x++)
    {
        if (rainColumnActive[x])
        {
            rainColumnHead[x]++;

            if (rainColumnHead[x] - rainTrailLength > DISPLAY_HEIGHT)
            {
                rainColumnActive[x] = false;
                rainColumnDelay[x] = random(0, 8);
            }

            continue;
        }

        if (rainColumnDelay[x] > 0)
        {
            rainColumnDelay[x]--;
            continue;
        }

        if (random(0, 100) < rainDensity)
        {
            rainColumnActive[x] = true;
            rainColumnHead[x] = 0;
        }
        else
        {
            rainColumnDelay[x] = random(1, 6);
        }
    }
}

Color getFirePaletteColor(uint8_t heat)
{
    if (heat < 8)
    {
        return BLACK;
    }

    if (firePalette == FIRE_BLUE)
    {
        if (heat < 110)
        {
            return blendColor({0, 0, 20}, {0, 64, 255}, heat * 255 / 110);
        }

        if (heat < 210)
        {
            return blendColor({0, 64, 255}, {0, 255, 255}, (heat - 110) * 255 / 100);
        }

        return blendColor({0, 255, 255}, {216, 247, 255}, (heat - 210) * 255 / 45);
    }

    if (firePalette == FIRE_PURPLE)
    {
        if (heat < 110)
        {
            return blendColor({16, 0, 24}, {106, 0, 168}, heat * 255 / 110);
        }

        if (heat < 210)
        {
            return blendColor({106, 0, 168}, {255, 43, 214}, (heat - 110) * 255 / 100);
        }

        return blendColor({255, 43, 214}, {255, 211, 247}, (heat - 210) * 255 / 45);
    }

    if (heat < 96)
    {
        return blendColor({18, 0, 0}, {204, 24, 0}, heat * 255 / 96);
    }

    if (heat < 190)
    {
        return blendColor({204, 24, 0}, {255, 174, 0}, (heat - 96) * 255 / 94);
    }

    return blendColor({255, 174, 0}, {255, 240, 184}, (heat - 190) * 255 / 65);
}

void renderDigitalFireFrame()
{
    clearDisplay();

    for (int y = 0; y < DISPLAY_HEIGHT; y++)
    {
        for (int x = 0; x < DISPLAY_WIDTH; x++)
        {
            setPixel(x, y, getFirePaletteColor(fireHeat[y][x]));
        }
    }

    showDisplay();
}

void advanceDigitalFire()
{
    for (int y = 0; y < DISPLAY_HEIGHT; y++)
    {
        for (int x = 0; x < DISPLAY_WIDTH; x++)
        {
            int cooling = random(18, 52);
            fireHeat[y][x] = cooling > fireHeat[y][x]
                ? 0
                : fireHeat[y][x] - cooling;
        }
    }

    for (int y = 0; y < DISPLAY_HEIGHT - 1; y++)
    {
        for (int x = 0; x < DISPLAY_WIDTH; x++)
        {
            int below = fireHeat[y + 1][x];
            int belowLeft = fireHeat[y + 1][x > 0 ? x - 1 : x];
            int belowRight = fireHeat[y + 1][x < DISPLAY_WIDTH - 1 ? x + 1 : x];

            fireHeat[y][x] = (below * 2 + belowLeft + belowRight) / 4;
        }
    }

    int bottom = DISPLAY_HEIGHT - 1;

    for (int x = 0; x < DISPLAY_WIDTH; x++)
    {
        if (random(0, 100) < fireIntensity)
        {
            fireHeat[bottom][x] = constrain(
                fireHeat[bottom][x] + random(120, 256),
                0,
                255
            );
        }
    }
}

void drawActiveContent(int startX, int startY)
{
    if (contentMode == MODE_DRAWING)
    {
        drawDrawingFrame(startX, startY);
    }
    else if (contentMode == MODE_PRESET)
    {
        if (activePreset == PRESET_CLOCK)
        {
            drawClockPresetFrame(startX, startY);
        }
        else if (activePreset == PRESET_SOLID)
        {
            for (int y = 0; y < DISPLAY_HEIGHT; y++)
            {
                for (int x = 0; x < DISPLAY_WIDTH; x++)
                {
                    setPixel(startX + x, startY + y, solidPresetColor);
                }
            }
        }
    }
    else
    {
        drawText(message, startX, startY, messageColor);
    }
}

void renderSolidColorPreset()
{
    clearDisplay();

    for (int y = 0; y < DISPLAY_HEIGHT; y++)
    {
        for (int x = 0; x < DISPLAY_WIDTH; x++)
        {
            setPixel(x, y, solidPresetColor);
        }
    }

    showDisplay();
}

void renderActivePreset()
{
    switch (activePreset)
    {
        case PRESET_RAIN:
            renderDigitalRainFrame();
            break;
        case PRESET_FIRE:
            renderDigitalFireFrame();
            break;
        case PRESET_CLOCK:
            renderAnimationFrame();
            break;
        case PRESET_SOLID:
        default:
            renderAnimationFrame();
            break;
    }
}

bool setActivePreset(const char value[])
{
    if (strcmp(value, "SOLID") == 0)
    {
        activePreset = PRESET_SOLID;
        Serial.println("PRESET:SOLID");

        if (contentMode == MODE_PRESET)
        {
            resetAnimation();
        }

        return true;
    }

    if (strcmp(value, "CLOCK") == 0)
    {
        activePreset = PRESET_CLOCK;
        lastRenderedClockMinute = -2;
        Serial.println("PRESET:CLOCK");

        if (contentMode == MODE_PRESET)
        {
            resetAnimation();
        }

        return true;
    }

    if (strcmp(value, "RAIN") == 0)
    {
        activePreset = PRESET_RAIN;
        initializeRainPreset();
        lastAutonomousTime = millis();
        Serial.println("PRESET:RAIN");

        if (contentMode == MODE_PRESET)
        {
            renderActivePreset();
        }

        return true;
    }

    if (strcmp(value, "FIRE") == 0)
    {
        activePreset = PRESET_FIRE;
        initializeFirePreset();
        lastAutonomousTime = millis();
        Serial.println("PRESET:FIRE");

        if (contentMode == MODE_PRESET)
        {
            renderActivePreset();
        }

        return true;
    }

    return false;
}

bool parseRgbParameter(
    const char value[],
    Color *targetColor)
{
    int red;
    int green;
    int blue;

    if (sscanf(value, "%d,%d,%d", &red, &green, &blue) != 3)
    {
        return false;
    }

    if (red < 0 || red > 255 ||
        green < 0 || green > 255 ||
        blue < 0 || blue > 255)
    {
        return false;
    }

    *targetColor = {red, green, blue};
    return true;
}

bool parseIntegerParameter(
    const char value[],
    int minimum,
    int maximum,
    int *targetValue)
{
    char *end = nullptr;
    long parsed = strtol(value, &end, 10);

    if (*end != '\0' || parsed < minimum || parsed > maximum)
    {
        return false;
    }

    *targetValue = (int)parsed;
    return true;
}

bool setPresetParameter(const char command[])
{
    if (strncmp(command, "COLOR:", 6) == 0)
    {
        Color color;

        if (!parseRgbParameter(command + 6, &color))
        {
            Serial.println("PRESET_PARAM:ERROR:COLOR");
            return true;
        }

        solidPresetColor = color;
        Serial.println("PRESET_PARAM:COLOR:OK");

        if (contentMode == MODE_PRESET && activePreset == PRESET_SOLID)
        {
            renderAnimationFrame();
        }

        return true;
    }

    if (strncmp(command, "CLOCK_COLOR:", 12) == 0)
    {
        Color color;

        if (!parseRgbParameter(command + 12, &color))
        {
            Serial.println("PRESET_PARAM:ERROR:CLOCK_COLOR");
            return true;
        }

        clockPresetColor = color;
        Serial.println("PRESET_PARAM:CLOCK_COLOR:OK");

        if (contentMode == MODE_PRESET && activePreset == PRESET_CLOCK)
        {
            renderAnimationFrame();
        }

        return true;
    }

    if (strncmp(command, "CLOCK_FORMAT:", 13) == 0)
    {
        if (strcmp(command + 13, "12") == 0)
        {
            clockUse24Hour = false;
        }
        else if (strcmp(command + 13, "24") == 0)
        {
            clockUse24Hour = true;
        }
        else
        {
            Serial.println("PRESET_PARAM:ERROR:CLOCK_FORMAT");
            return true;
        }

        lastRenderedClockMinute = -2;
        Serial.println("PRESET_PARAM:CLOCK_FORMAT:OK");

        if (contentMode == MODE_PRESET && activePreset == PRESET_CLOCK)
        {
            renderAnimationFrame();
        }

        return true;
    }

    if (strncmp(command, "CLOCK_LEADING_ZERO:", 19) == 0)
    {
        if (strcmp(command + 19, "1") == 0)
        {
            clockLeadingZero = true;
        }
        else if (strcmp(command + 19, "0") == 0)
        {
            clockLeadingZero = false;
        }
        else
        {
            Serial.println("PRESET_PARAM:ERROR:CLOCK_LEADING_ZERO");
            return true;
        }

        lastRenderedClockMinute = -2;
        Serial.println("PRESET_PARAM:CLOCK_LEADING_ZERO:OK");

        if (contentMode == MODE_PRESET && activePreset == PRESET_CLOCK)
        {
            renderAnimationFrame();
        }

        return true;
    }

    if (strncmp(command, "RAIN_COLOR:", 11) == 0)
    {
        Color color;

        if (!parseRgbParameter(command + 11, &color))
        {
            Serial.println("PRESET_PARAM:ERROR:RAIN_COLOR");
            return true;
        }

        rainPresetColor = color;
        Serial.println("PRESET_PARAM:RAIN_COLOR:OK");

        if (contentMode == MODE_PRESET && activePreset == PRESET_RAIN)
        {
            renderActivePreset();
        }

        return true;
    }

    if (strncmp(command, "RAIN_SPEED:", 11) == 0)
    {
        if (!parseIntegerParameter(command + 11, 1, 20, &rainSpeed))
        {
            Serial.println("PRESET_PARAM:ERROR:RAIN_SPEED");
            return true;
        }

        lastAutonomousTime = millis();
        Serial.println("PRESET_PARAM:RAIN_SPEED:OK");
        return true;
    }

    if (strncmp(command, "RAIN_DENSITY:", 13) == 0)
    {
        if (!parseIntegerParameter(command + 13, 1, 100, &rainDensity))
        {
            Serial.println("PRESET_PARAM:ERROR:RAIN_DENSITY");
            return true;
        }

        Serial.println("PRESET_PARAM:RAIN_DENSITY:OK");
        return true;
    }

    if (strncmp(command, "RAIN_TRAIL:", 11) == 0)
    {
        if (!parseIntegerParameter(command + 11, 2, 7, &rainTrailLength))
        {
            Serial.println("PRESET_PARAM:ERROR:RAIN_TRAIL");
            return true;
        }

        Serial.println("PRESET_PARAM:RAIN_TRAIL:OK");

        if (contentMode == MODE_PRESET && activePreset == PRESET_RAIN)
        {
            renderActivePreset();
        }

        return true;
    }

    if (strncmp(command, "FIRE_PALETTE:", 13) == 0)
    {
        if (strcmp(command + 13, "CLASSIC") == 0)
        {
            firePalette = FIRE_CLASSIC;
        }
        else if (strcmp(command + 13, "BLUE") == 0)
        {
            firePalette = FIRE_BLUE;
        }
        else if (strcmp(command + 13, "PURPLE") == 0)
        {
            firePalette = FIRE_PURPLE;
        }
        else
        {
            Serial.println("PRESET_PARAM:ERROR:FIRE_PALETTE");
            return true;
        }

        Serial.println("PRESET_PARAM:FIRE_PALETTE:OK");

        if (contentMode == MODE_PRESET && activePreset == PRESET_FIRE)
        {
            renderActivePreset();
        }

        return true;
    }

    if (strncmp(command, "FIRE_SPEED:", 11) == 0)
    {
        if (!parseIntegerParameter(command + 11, 1, 20, &fireSpeed))
        {
            Serial.println("PRESET_PARAM:ERROR:FIRE_SPEED");
            return true;
        }

        lastAutonomousTime = millis();
        Serial.println("PRESET_PARAM:FIRE_SPEED:OK");
        return true;
    }

    if (strncmp(command, "FIRE_INTENSITY:", 15) == 0)
    {
        if (!parseIntegerParameter(command + 15, 1, 100, &fireIntensity))
        {
            Serial.println("PRESET_PARAM:ERROR:FIRE_INTENSITY");
            return true;
        }

        Serial.println("PRESET_PARAM:FIRE_INTENSITY:OK");
        return true;
    }

    return false;
}

bool setClockTime(const char command[])
{
    const char *separator = strchr(command, ':');

    if (separator == nullptr || separator == command || *(separator + 1) == '\0')
    {
        return false;
    }

    char timestampText[16];
    size_t timestampLength = separator - command;

    if (timestampLength >= sizeof(timestampText))
    {
        return false;
    }

    strncpy(timestampText, command, timestampLength);
    timestampText[timestampLength] = '\0';

    char *timestampEnd = nullptr;
    unsigned long unixSeconds = strtoul(timestampText, &timestampEnd, 10);

    if (*timestampEnd != '\0' || unixSeconds == 0)
    {
        return false;
    }

    char *offsetEnd = nullptr;
    long offsetMinutes = strtol(separator + 1, &offsetEnd, 10);

    if (*offsetEnd != '\0' || offsetMinutes < -840 || offsetMinutes > 840)
    {
        return false;
    }

    clockSyncUnixSeconds = unixSeconds;
    clockSyncMillis = millis();
    clockUtcOffsetMinutes = (int)offsetMinutes;
    clockTimeValid = true;
    lastRenderedClockMinute = getCurrentClockMinute();

    Serial.println("CLOCK_TIME:OK");

    if (contentMode == MODE_PRESET && activePreset == PRESET_CLOCK)
    {
        renderAnimationFrame();
    }

    return true;
}

bool isHexCharacter(char character)
{
    return
        (character >= '0' && character <= '9') ||
        (character >= 'A' && character <= 'F');
}

int hexValue(char character)
{
    if (character >= '0' && character <= '9')
    {
        return character - '0';
    }

    return character - 'A' + 10;
}

Color parseHexColor(const char hex[])
{
    return {
        hexValue(hex[0]) * 16 + hexValue(hex[1]),
        hexValue(hex[2]) * 16 + hexValue(hex[3]),
        hexValue(hex[4]) * 16 + hexValue(hex[5])
    };
}

void beginFrameTransfer(const char dimensions[])
{
    if (strcmp(dimensions, "32X8") != 0)
    {
        Serial.println("FRAME:ERROR:DIMENSIONS");
        receivingFrame = false;
        return;
    }

    clearDrawingFrame(stagingDrawingFrame);
    clearStagingRows();
    receivingFrame = true;
    Serial.println("FRAME:READY");
}

void storeFrameRow(const char command[])
{
    if (!receivingFrame)
    {
        Serial.println("ROW:ERROR:NO_FRAME");
        return;
    }

    const char *rowText = command + 4;
    const char *separator = strchr(rowText, ':');

    if (separator == nullptr || separator == rowText)
    {
        Serial.println("ROW:ERROR:FORMAT");
        return;
    }

    for (const char *character = rowText; character < separator; character++)
    {
        if (*character < '0' || *character > '9')
        {
            Serial.println("ROW:ERROR:INDEX");
            return;
        }
    }

    int row = atoi(rowText);

    if (row < 0 || row >= DISPLAY_HEIGHT)
    {
        Serial.println("ROW:ERROR:INDEX");
        return;
    }

    const char *hex = separator + 1;

    if (strlen(hex) != FRAME_ROW_HEX_LENGTH)
    {
        Serial.println("ROW:ERROR:LENGTH");
        return;
    }

    for (int i = 0; i < FRAME_ROW_HEX_LENGTH; i++)
    {
        if (!isHexCharacter(hex[i]))
        {
            Serial.println("ROW:ERROR:HEX");
            return;
        }
    }

    for (int x = 0; x < DISPLAY_WIDTH; x++)
    {
        stagingDrawingFrame[getLogicalIndex(x, row)] =
            parseHexColor(hex + x * 6);
    }

    stagingRowsReceived[row] = true;

    Serial.print("ROW:");
    Serial.print(row);
    Serial.println(":OK");
}

void endFrameTransfer()
{
    if (!receivingFrame)
    {
        Serial.println("FRAME:ERROR:NO_FRAME");
        return;
    }

    for (int row = 0; row < DISPLAY_HEIGHT; row++)
    {
        if (!stagingRowsReceived[row])
        {
            Serial.println("FRAME:ERROR:INCOMPLETE");
            return;
        }
    }

    for (int i = 0; i < LED_COUNT; i++)
    {
        activeDrawingFrame[i] = stagingDrawingFrame[i];
    }

    receivingFrame = false;
    Serial.println("FRAME:STORED");
}

bool setContentMode(const char value[])
{
    if (strcmp(value, "TEXT") == 0)
    {
        contentMode = MODE_TEXT;
        resetAnimation();
        Serial.println("MODE:TEXT");
        return true;
    }

    if (strcmp(value, "DRAWING") == 0)
    {
        contentMode = MODE_DRAWING;
        resetAnimation();
        Serial.println("MODE:DRAWING");
        return true;
    }

    if (strcmp(value, "PRESET") == 0)
    {
        contentMode = MODE_PRESET;
        resetAnimation();
        Serial.println("MODE:PRESET");
        return true;
    }

    return false;
}

void maskWipeFrame()
{
    int limit;

    if (animationDirection == DIRECTION_LEFT)
    {
        limit = constrain(wipeProgress, 0, DISPLAY_WIDTH);

        for (int y = 0; y < DISPLAY_HEIGHT; y++)
        {
            for (int x = limit; x < DISPLAY_WIDTH; x++)
            {
                setPixel(x, y, BLACK);
            }
        }
    }
    else if (animationDirection == DIRECTION_RIGHT)
    {
        limit = constrain(wipeProgress, 0, DISPLAY_WIDTH);

        for (int y = 0; y < DISPLAY_HEIGHT; y++)
        {
            for (int x = 0; x < DISPLAY_WIDTH - limit; x++)
            {
                setPixel(x, y, BLACK);
            }
        }
    }
    else if (animationDirection == DIRECTION_UP)
    {
        limit = constrain(wipeProgress, 0, DISPLAY_HEIGHT);

        for (int y = limit; y < DISPLAY_HEIGHT; y++)
        {
            for (int x = 0; x < DISPLAY_WIDTH; x++)
            {
                setPixel(x, y, BLACK);
            }
        }
    }
    else
    {
        limit = constrain(wipeProgress, 0, DISPLAY_HEIGHT);

        for (int y = 0; y < DISPLAY_HEIGHT - limit; y++)
        {
            for (int x = 0; x < DISPLAY_WIDTH; x++)
            {
                setPixel(x, y, BLACK);
            }
        }
    }
}

void renderAnimationFrame()
{
    if (contentMode == MODE_PRESET && isAutonomousPreset())
    {
        renderActivePreset();
        return;
    }

    clearDisplay();

    if (animationEffect == EFFECT_SCROLL)
    {
        drawActiveContent(textOffset, textOffsetY);
    }
    else if (animationEffect == EFFECT_BLINK)
    {
        if (blinkVisible)
        {
            drawActiveContent(
                getCenteredContentX(),
                getCenteredContentY()
            );
        }
    }
    else
    {
        drawActiveContent(
            getCenteredContentX(),
            getCenteredContentY()
        );

        if (animationEffect == EFFECT_WIPE)
        {
            maskWipeFrame();
        }
    }

    showDisplay();
}

void resetAnimation()
{
    if (contentMode == MODE_PRESET && isAutonomousPreset())
    {
        lastAutonomousTime = millis();
        renderActivePreset();
        return;
    }

    textWidth = getTextWidth(message);
    int contentWidth = getContentWidth();
    int contentHeight = getContentHeight();

    wipeProgress = 0;
    blinkVisible = true;

    if (animationDirection == DIRECTION_LEFT)
    {
        textOffset = DISPLAY_WIDTH;
        textOffsetY = getCenteredContentY();
    }
    else if (animationDirection == DIRECTION_RIGHT)
    {
        textOffset = -contentWidth;
        textOffsetY = getCenteredContentY();
    }
    else if (animationDirection == DIRECTION_UP)
    {
        textOffset = getCenteredContentX();
        textOffsetY = DISPLAY_HEIGHT;
    }
    else
    {
        textOffset = getCenteredContentX();
        textOffsetY = -contentHeight;
    }

    lastAnimationTime = millis();
    lastBlinkTime = lastAnimationTime;
    renderAnimationFrame();
}

void printStatus()
{
    Serial.print("STATUS:MATRIX=");
    Serial.print(DISPLAY_WIDTH);
    Serial.print("x");
    Serial.print(DISPLAY_HEIGHT);

    Serial.print(";MESSAGE=");
    Serial.print(message);

    Serial.print(";EFFECT=");
    Serial.print(getEffectName());

    Serial.print(";DIRECTION=");
    Serial.print(getDirectionName());

    Serial.print(";MODE=");
    Serial.print(getContentModeName());

    Serial.print(";PRESET=");
    Serial.print(getPresetName());

    Serial.print(";PRESET_COLOR=");
    Serial.print(solidPresetColor.red);
    Serial.print(",");
    Serial.print(solidPresetColor.green);
    Serial.print(",");
    Serial.print(solidPresetColor.blue);

    Serial.print(";CLOCK_COLOR=");
    Serial.print(clockPresetColor.red);
    Serial.print(",");
    Serial.print(clockPresetColor.green);
    Serial.print(",");
    Serial.print(clockPresetColor.blue);

    Serial.print(";CLOCK_FORMAT=");
    Serial.print(clockUse24Hour ? 24 : 12);

    Serial.print(";CLOCK_LEADING_ZERO=");
    Serial.print(clockLeadingZero ? 1 : 0);

    Serial.print(";CLOCK_VALID=");
    Serial.print(clockTimeValid ? 1 : 0);

    Serial.print(";RAIN_COLOR=");
    Serial.print(rainPresetColor.red);
    Serial.print(",");
    Serial.print(rainPresetColor.green);
    Serial.print(",");
    Serial.print(rainPresetColor.blue);

    Serial.print(";RAIN_SPEED=");
    Serial.print(rainSpeed);

    Serial.print(";RAIN_DENSITY=");
    Serial.print(rainDensity);

    Serial.print(";RAIN_TRAIL=");
    Serial.print(rainTrailLength);

    Serial.print(";FIRE_PALETTE=");
    Serial.print(getFirePaletteName());

    Serial.print(";FIRE_SPEED=");
    Serial.print(fireSpeed);

    Serial.print(";FIRE_INTENSITY=");
    Serial.print(fireIntensity);

    Serial.print(";SPEED=");
    Serial.print(animationSpeed);

    Serial.print(";BLINK_ON=");
    Serial.print(blinkOnMs);

    Serial.print(";BLINK_OFF=");
    Serial.print(blinkOffMs);

    Serial.print(";PAUSED=");
    Serial.print(animationPaused ? 1 : 0);

    Serial.print(";COLOR=");
    Serial.print(messageColor.red);
    Serial.print(",");
    Serial.print(messageColor.green);
    Serial.print(",");
    Serial.print(messageColor.blue);

    Serial.print(";BRIGHTNESS=");
    Serial.println(brightness);
}

void printHelp()
{
    Serial.println("Available commands:");
    Serial.println("MESSAGE:<text>");
    Serial.println("MODE:TEXT|DRAWING|PRESET");
    Serial.println("PRESET:SOLID|CLOCK|RAIN|FIRE");
    Serial.println("PRESET_PARAM:COLOR:<red>,<green>,<blue>");
    Serial.println("PRESET_PARAM:CLOCK_COLOR:<red>,<green>,<blue>");
    Serial.println("PRESET_PARAM:CLOCK_FORMAT:12|24");
    Serial.println("PRESET_PARAM:CLOCK_LEADING_ZERO:0|1");
    Serial.println("PRESET_PARAM:RAIN_COLOR:<red>,<green>,<blue>");
    Serial.println("PRESET_PARAM:RAIN_SPEED:<1-20>");
    Serial.println("PRESET_PARAM:RAIN_DENSITY:<1-100>");
    Serial.println("PRESET_PARAM:RAIN_TRAIL:<2-7>");
    Serial.println("PRESET_PARAM:FIRE_PALETTE:CLASSIC|BLUE|PURPLE");
    Serial.println("PRESET_PARAM:FIRE_SPEED:<1-20>");
    Serial.println("PRESET_PARAM:FIRE_INTENSITY:<1-100>");
    Serial.println("CLOCK_TIME:<unix seconds>:<offset minutes east of UTC>");
    Serial.println("EFFECT:STILL|SCROLL|WIPE|BLINK");
    Serial.println("DIRECTION:LEFT|RIGHT|UP|DOWN");
    Serial.println("SPEED:<pixels per second, 1-30>");
    Serial.println("BLINK_ON:<milliseconds>");
    Serial.println("BLINK_OFF:<milliseconds>");
    Serial.println("PAUSED:0|1");
    Serial.println("COLOR:<red>,<green>,<blue>");
    Serial.println("BRIGHTNESS:<0-255>");
    Serial.println("FRAME_BEGIN:32x8");
    Serial.println("ROW:<0-7>:<192 hex characters>");
    Serial.println("FRAME_END");
    Serial.println("RESET");
    Serial.println("STATUS");
    Serial.println("HELP");
}

void advanceScroll()
{
    int contentWidth = getContentWidth();
    int contentHeight = getContentHeight();

    if (animationDirection == DIRECTION_LEFT)
    {
        textOffset--;

        if (textOffset < -contentWidth)
        {
            textOffset = DISPLAY_WIDTH;
        }
    }
    else if (animationDirection == DIRECTION_RIGHT)
    {
        textOffset++;

        if (textOffset > DISPLAY_WIDTH)
        {
            textOffset = -contentWidth;
        }
    }
    else if (animationDirection == DIRECTION_UP)
    {
        textOffsetY--;

        if (textOffsetY < -contentHeight)
        {
            textOffsetY = DISPLAY_HEIGHT;
        }
    }
    else
    {
        textOffsetY++;

        if (textOffsetY > DISPLAY_HEIGHT)
        {
            textOffsetY = -contentHeight;
        }
    }
}

void advanceWipe()
{
    wipeProgress++;

    int resetLimit =
        animationDirection == DIRECTION_LEFT ||
        animationDirection == DIRECTION_RIGHT
            ? DISPLAY_WIDTH + 8
            : DISPLAY_HEIGHT + 6;

    if (wipeProgress > resetLimit)
    {
        wipeProgress = 0;
    }
}

void updateAnimation()
{
    if (contentMode == MODE_PRESET && isAutonomousPreset())
    {
        unsigned long currentTime = millis();
        unsigned long interval = getAutonomousInterval();

        if (currentTime - lastAutonomousTime < interval)
        {
            return;
        }

        lastAutonomousTime += interval;

        if (currentTime - lastAutonomousTime > interval * 4)
        {
            lastAutonomousTime = currentTime;
        }

        if (activePreset == PRESET_RAIN)
        {
            advanceDigitalRain();
        }
        else
        {
            advanceDigitalFire();
        }

        renderActivePreset();
        return;
    }

    if (contentMode == MODE_PRESET &&
        activePreset == PRESET_CLOCK &&
        clockTimeValid &&
        !animationPaused)
    {
        int currentClockMinute = getCurrentClockMinute();

        if (currentClockMinute != lastRenderedClockMinute)
        {
            lastRenderedClockMinute = currentClockMinute;
            renderAnimationFrame();
        }
    }

    if (animationPaused ||
        animationEffect == EFFECT_STILL)
    {
        return;
    }

    unsigned long currentTime = millis();

    if (animationEffect == EFFECT_BLINK)
    {
        unsigned long blinkDuration = blinkVisible ? blinkOnMs : blinkOffMs;

        if (currentTime - lastBlinkTime >= blinkDuration)
        {
            lastBlinkTime = currentTime;
            blinkVisible = !blinkVisible;
            renderAnimationFrame();
        }

        return;
    }

    unsigned long interval = getAnimationInterval();

    if (currentTime - lastAnimationTime < interval)
    {
        return;
    }

    // Advance the schedule by the intended interval so loop overhead does not
    // slowly make the physical board drift behind the browser preview.
    lastAnimationTime += interval;

    if (currentTime - lastAnimationTime > interval * 4)
    {
        lastAnimationTime = currentTime;
    }

    if (animationEffect == EFFECT_WIPE)
    {
        advanceWipe();
    }
    else
    {
        advanceScroll();
    }

    renderAnimationFrame();
}

void setMarqueeMessage(const char newMessage[])
{
    strncpy(message, newMessage, MESSAGE_BUFFER_SIZE - 1);
    message[MESSAGE_BUFFER_SIZE - 1] = '\0';
    contentMode = MODE_TEXT;
    resetAnimation();

    Serial.print("New message: ");
    Serial.println(message);
}

bool setAnimationEffect(const char value[])
{
    if (strcmp(value, "STILL") == 0)
    {
        animationEffect = EFFECT_STILL;
    }
    else if (strcmp(value, "SCROLL") == 0)
    {
        animationEffect = EFFECT_SCROLL;
    }
    else if (strcmp(value, "WIPE") == 0)
    {
        animationEffect = EFFECT_WIPE;
    }
    else if (strcmp(value, "BLINK") == 0)
    {
        animationEffect = EFFECT_BLINK;
    }
    else
    {
        return false;
    }

    resetAnimation();
    return true;
}

bool setAnimationDirection(const char value[])
{
    if (strcmp(value, "LEFT") == 0)
    {
        animationDirection = DIRECTION_LEFT;
    }
    else if (strcmp(value, "RIGHT") == 0)
    {
        animationDirection = DIRECTION_RIGHT;
    }
    else if (strcmp(value, "UP") == 0)
    {
        animationDirection = DIRECTION_UP;
    }
    else if (strcmp(value, "DOWN") == 0)
    {
        animationDirection = DIRECTION_DOWN;
    }
    else
    {
        return false;
    }

    resetAnimation();
    return true;
}

void processCommand(const char command[])
{
    if (strncmp(command, "MESSAGE:", 8) == 0)
    {
        setMarqueeMessage(command + 8);
    }
    else if (strncmp(command, "EFFECT:", 7) == 0)
    {
        if (!setAnimationEffect(command + 7))
        {
            Serial.println("Use EFFECT:STILL|SCROLL|WIPE|BLINK");
        }
    }
    else if (strncmp(command, "DIRECTION:", 10) == 0)
    {
        if (!setAnimationDirection(command + 10))
        {
            Serial.println("Use DIRECTION:LEFT|RIGHT|UP|DOWN");
        }
    }
    else if (strncmp(command, "MODE:", 5) == 0)
    {
        if (!setContentMode(command + 5))
        {
            Serial.println("Use MODE:TEXT|DRAWING|PRESET");
        }
    }
    else if (strncmp(command, "PRESET_PARAM:", 13) == 0)
    {
        if (!setPresetParameter(command + 13))
        {
            Serial.println("Use PRESET_PARAM:COLOR|CLOCK|RAIN|FIRE setting");
        }
    }
    else if (strncmp(command, "PRESET:", 7) == 0)
    {
        if (!setActivePreset(command + 7))
        {
            Serial.println("Use PRESET:SOLID|CLOCK|RAIN|FIRE");
        }
    }
    else if (strncmp(command, "CLOCK_TIME:", 11) == 0)
    {
        if (!setClockTime(command + 11))
        {
            Serial.println("Use CLOCK_TIME:unixSeconds:offsetMinutesEastOfUtc");
        }
    }
    else if (strncmp(command, "FRAME_BEGIN:", 12) == 0)
    {
        beginFrameTransfer(command + 12);
    }
    else if (strncmp(command, "ROW:", 4) == 0)
    {
        storeFrameRow(command);
    }
    else if (strcmp(command, "FRAME_END") == 0)
    {
        endFrameTransfer();
    }
    else if (strncmp(command, "SPEED:", 6) == 0)
    {
        int newSpeed = atoi(command + 6);

        if (newSpeed >= 1 && newSpeed <= 30)
        {
            animationSpeed = newSpeed;
            resetAnimation();

            Serial.print("New speed: ");
            Serial.print(animationSpeed);
            Serial.println(" pixels per second");
        }
        else
        {
            Serial.println("Speed must be 1-30 pixels per second.");
        }
    }
    else if (strncmp(command, "BLINK_ON:", 9) == 0)
    {
        unsigned long newDuration = strtoul(command + 9, nullptr, 10);

        if (newDuration >= 100 && newDuration <= 1200)
        {
            blinkOnMs = newDuration;
            lastBlinkTime = millis();
        }
        else
        {
            Serial.println("Blink-on duration must be 100-1200 milliseconds.");
        }
    }
    else if (strncmp(command, "BLINK_OFF:", 10) == 0)
    {
        unsigned long newDuration = strtoul(command + 10, nullptr, 10);

        if (newDuration >= 100 && newDuration <= 1200)
        {
            blinkOffMs = newDuration;
            lastBlinkTime = millis();
        }
        else
        {
            Serial.println("Blink-off duration must be 100-1200 milliseconds.");
        }
    }
    else if (strncmp(command, "PAUSED:", 7) == 0)
    {
        if (strcmp(command + 7, "1") == 0)
        {
            animationPaused = true;
        }
        else if (strcmp(command + 7, "0") == 0)
        {
            animationPaused = false;
            resetAnimation();
        }
        else
        {
            Serial.println("Use PAUSED:0 or PAUSED:1");
        }
    }
    else if (strncmp(command, "COLOR:", 6) == 0)
    {
        int red;
        int green;
        int blue;

        if (sscanf(command + 6, "%d,%d,%d", &red, &green, &blue) == 3)
        {
            if (red >= 0 && red <= 255 &&
                green >= 0 && green <= 255 &&
                blue >= 0 && blue <= 255)
            {
                messageColor = {red, green, blue};

                Serial.print("New color: ");
                Serial.print(red);
                Serial.print(", ");
                Serial.print(green);
                Serial.print(", ");
                Serial.println(blue);

                renderAnimationFrame();
            }
            else
            {
                Serial.println("Color values must be 0-255.");
            }
        }
        else
        {
            Serial.println("Use COLOR:red,green,blue");
        }
    }
    else if (strncmp(command, "BRIGHTNESS:", 11) == 0)
    {
        int newBrightness = atoi(command + 11);

        if (newBrightness >= 0 && newBrightness <= 255)
        {
            brightness = newBrightness;
            FastLED.setBrightness(brightness);
            showDisplay();

            Serial.print("New brightness: ");
            Serial.println(brightness);
        }
        else
        {
            Serial.println("Brightness must be 0-255.");
        }
    }
    else if (strcmp(command, "STATUS") == 0)
    {
        printStatus();
    }
    else if (strcmp(command, "RESET") == 0)
    {
        resetAnimation();
        Serial.println("Animation reset.");
    }
    else if (strcmp(command, "HELP") == 0)
    {
        printHelp();
    }
    else
    {
        Serial.println("Unknown command.");
    }
}

void checkSerialInput()
{
    while (Serial.available() > 0)
    {
        char incomingCharacter = Serial.read();

        if (incomingCharacter >= 'a' && incomingCharacter <= 'z')
        {
            incomingCharacter = incomingCharacter - 'a' + 'A';
        }

        if (incomingCharacter == '\r')
        {
            continue;
        }

        if (incomingCharacter == '\b' || incomingCharacter == 127)
        {
            if (inputIndex > 0)
            {
                inputIndex--;

            }

            continue;
        }

        if (incomingCharacter == '\n')
        {
            inputBuffer[inputIndex] = '\0';

            if (inputIndex > 0)
            {
                processCommand(inputBuffer);
            }

            inputIndex = 0;
        }
        else if (inputIndex < COMMAND_BUFFER_SIZE - 1)
        {
            inputBuffer[inputIndex] = incomingCharacter;
            inputIndex++;
        }
    }
}

// -------------------------
// Debug Functions
// -------------------------

char getColorSymbol(Color color)
{
    if (
        color.red == 0 &&
        color.green == 0 &&
        color.blue == 0
    )
    {
        return '.';
    }

    if (
        color.red == 255 &&
        color.green == 0 &&
        color.blue == 0
    )
    {
        return 'R';
    }

    if (
        color.red == 0 &&
        color.green == 255 &&
        color.blue == 0
    )
    {
        return 'G';
    }

    if (
        color.red == 0 &&
        color.green == 0 &&
        color.blue == 255
    )
    {
        return 'B';
    }

    return '?';
}

void printFrameBuffer()
{
    for (int y = 0; y < DISPLAY_HEIGHT; y++)
    {
        for (int x = 0; x < DISPLAY_WIDTH; x++)
        {
            int index = getIndex(x, y);
            Color color = leds[index];

            Serial.print(getColorSymbol(color));
        }

        Serial.println();
    }
}

void printPixel(int index)
{
    Serial.print("Pixel ");
    Serial.print(index);
    Serial.print(": (");

    Serial.print(leds[index].red);
    Serial.print(", ");

    Serial.print(leds[index].green);
    Serial.print(", ");

    Serial.print(leds[index].blue);

    Serial.println(")");
}

// -------------------------
// Arduino
// -------------------------

void setup()
{
    Serial.begin(115200);
    randomSeed(esp_random());

    FastLED.addLeds<WS2812B, DATA_PIN, GRB>(
        physicalLeds,
        LED_COUNT
    );

    FastLED.setBrightness(brightness);

    clearDrawingFrame(activeDrawingFrame);
    clearDrawingFrame(stagingDrawingFrame);
    clearStagingRows();
    initializeRainPreset();
    initializeFirePreset();

    clearDisplay();
    showDisplay();

    resetAnimation();

    Serial.println("PIXEL BOARD FIRMWARE V2");
}

void loop()
{
    checkSerialInput();
    updateAnimation();
}
