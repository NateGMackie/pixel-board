#include <Arduino.h>
#include <esp_system.h>
#include <FastLED.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include <WiFi.h>

#include "generated_web_app.h"

void showDisplay();
void printStatus();
void resetAnimation();
void renderAnimationFrame();
void processCommand(const char command[]);
void sendProtocolLine(const String &line);

// -------------------------
// Display Configuration
// -------------------------

const int DISPLAY_WIDTH = 32;
const int DISPLAY_HEIGHT = 8;
const int LED_COUNT = DISPLAY_WIDTH * DISPLAY_HEIGHT;

#define DATA_PIN 17

const char PIXEL_BOARD_AP_PASSWORD[] = "PixelBoard123";

CRGB physicalLeds[LED_COUNT];
WebServer httpServer(80);
WebSocketsServer webSocket(81);

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

void sendProtocolLine(const String &line)
{
    Serial.println(line);
    String payload = line;
    webSocket.broadcastTXT(payload);
}

void sendProtocolLine(const char line[])
{
    sendProtocolLine(String(line));
}

String getAccessPointSsid()
{
    uint64_t mac = ESP.getEfuseMac();
    char suffix[5];

    snprintf(suffix, sizeof(suffix), "%04X", (uint16_t)(mac & 0xFFFF));

    return String("PixelBoard-") + suffix;
}

void handleWebAppRequest()
{
    httpServer.sendHeader("Content-Encoding", "gzip");
    httpServer.sendHeader("Cache-Control", "no-cache");
    httpServer.send_P(
        200,
        "text/html",
        (const char *)WEB_APP_GZ,
        WEB_APP_GZ_LEN
    );
}

void handleNotFound()
{
    httpServer.send(404, "text/plain", "Not found");
}

void handleWebSocketEvent(
    uint8_t clientNumber,
    WStype_t type,
    uint8_t *payload,
    size_t length)
{
    if (type == WStype_CONNECTED)
    {
        webSocket.sendTXT(clientNumber, "PIXEL_BOARD:CONNECTED");
        printStatus();
        return;
    }

    if (type == WStype_TEXT)
    {
        if (length == 0 || length >= COMMAND_BUFFER_SIZE)
        {
            webSocket.sendTXT(clientNumber, "ERROR:COMMAND_TOO_LONG");
            return;
        }

        char command[COMMAND_BUFFER_SIZE];

        memcpy(command, payload, length);
        command[length] = '\0';

        for (size_t i = 0; command[i] != '\0'; i++)
        {
            if (command[i] >= 'a' && command[i] <= 'z')
            {
                command[i] = command[i] - 'a' + 'A';
            }
        }

        processCommand(command);
        return;
    }

    if (type == WStype_BIN)
    {
        webSocket.sendTXT(clientNumber, "ERROR:BINARY_UNSUPPORTED");
    }
}

void startWirelessServices()
{
    String ssid = getAccessPointSsid();

    WiFi.mode(WIFI_AP);
    bool apStarted = WiFi.softAP(ssid.c_str(), PIXEL_BOARD_AP_PASSWORD);

    Serial.print("Pixel Board AP SSID: ");
    Serial.println(ssid);
    Serial.print("Pixel Board AP IP: ");
    Serial.println(WiFi.softAPIP());
    Serial.println("Pixel Board URL: http://192.168.4.1/");
    Serial.print("Pixel Board AP started: ");
    Serial.println(apStarted ? "yes" : "no");

    httpServer.on("/", HTTP_GET, handleWebAppRequest);
    httpServer.onNotFound(handleNotFound);
    httpServer.begin();
    Serial.println("HTTP server started: yes");

    webSocket.begin();
    webSocket.onEvent(handleWebSocketEvent);
    Serial.println("WebSocket server started: yes");
}

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
        sendProtocolLine("PRESET:SOLID");

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
        sendProtocolLine("PRESET:CLOCK");

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
        sendProtocolLine("PRESET:RAIN");

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
        sendProtocolLine("PRESET:FIRE");

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
            sendProtocolLine("PRESET_PARAM:ERROR:COLOR");
            return true;
        }

        solidPresetColor = color;
        sendProtocolLine("PRESET_PARAM:COLOR:OK");

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
            sendProtocolLine("PRESET_PARAM:ERROR:CLOCK_COLOR");
            return true;
        }

        clockPresetColor = color;
        sendProtocolLine("PRESET_PARAM:CLOCK_COLOR:OK");

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
            sendProtocolLine("PRESET_PARAM:ERROR:CLOCK_FORMAT");
            return true;
        }

        lastRenderedClockMinute = -2;
        sendProtocolLine("PRESET_PARAM:CLOCK_FORMAT:OK");

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
            sendProtocolLine("PRESET_PARAM:ERROR:CLOCK_LEADING_ZERO");
            return true;
        }

        lastRenderedClockMinute = -2;
        sendProtocolLine("PRESET_PARAM:CLOCK_LEADING_ZERO:OK");

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
            sendProtocolLine("PRESET_PARAM:ERROR:RAIN_COLOR");
            return true;
        }

        rainPresetColor = color;
        sendProtocolLine("PRESET_PARAM:RAIN_COLOR:OK");

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
            sendProtocolLine("PRESET_PARAM:ERROR:RAIN_SPEED");
            return true;
        }

        lastAutonomousTime = millis();
        sendProtocolLine("PRESET_PARAM:RAIN_SPEED:OK");
        return true;
    }

    if (strncmp(command, "RAIN_DENSITY:", 13) == 0)
    {
        if (!parseIntegerParameter(command + 13, 1, 100, &rainDensity))
        {
            sendProtocolLine("PRESET_PARAM:ERROR:RAIN_DENSITY");
            return true;
        }

        sendProtocolLine("PRESET_PARAM:RAIN_DENSITY:OK");
        return true;
    }

    if (strncmp(command, "RAIN_TRAIL:", 11) == 0)
    {
        if (!parseIntegerParameter(command + 11, 2, 7, &rainTrailLength))
        {
            sendProtocolLine("PRESET_PARAM:ERROR:RAIN_TRAIL");
            return true;
        }

        sendProtocolLine("PRESET_PARAM:RAIN_TRAIL:OK");

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
            sendProtocolLine("PRESET_PARAM:ERROR:FIRE_PALETTE");
            return true;
        }

        sendProtocolLine("PRESET_PARAM:FIRE_PALETTE:OK");

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
            sendProtocolLine("PRESET_PARAM:ERROR:FIRE_SPEED");
            return true;
        }

        lastAutonomousTime = millis();
        sendProtocolLine("PRESET_PARAM:FIRE_SPEED:OK");
        return true;
    }

    if (strncmp(command, "FIRE_INTENSITY:", 15) == 0)
    {
        if (!parseIntegerParameter(command + 15, 1, 100, &fireIntensity))
        {
            sendProtocolLine("PRESET_PARAM:ERROR:FIRE_INTENSITY");
            return true;
        }

        sendProtocolLine("PRESET_PARAM:FIRE_INTENSITY:OK");
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

    sendProtocolLine("CLOCK_TIME:OK");

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
        sendProtocolLine("FRAME:ERROR:DIMENSIONS");
        receivingFrame = false;
        return;
    }

    clearDrawingFrame(stagingDrawingFrame);
    clearStagingRows();
    receivingFrame = true;
    sendProtocolLine("FRAME:READY");
}

void storeFrameRow(const char command[])
{
    if (!receivingFrame)
    {
        sendProtocolLine("ROW:ERROR:NO_FRAME");
        return;
    }

    const char *rowText = command + 4;
    const char *separator = strchr(rowText, ':');

    if (separator == nullptr || separator == rowText)
    {
        sendProtocolLine("ROW:ERROR:FORMAT");
        return;
    }

    for (const char *character = rowText; character < separator; character++)
    {
        if (*character < '0' || *character > '9')
        {
            sendProtocolLine("ROW:ERROR:INDEX");
            return;
        }
    }

    int row = atoi(rowText);

    if (row < 0 || row >= DISPLAY_HEIGHT)
    {
        sendProtocolLine("ROW:ERROR:INDEX");
        return;
    }

    const char *hex = separator + 1;

    if (strlen(hex) != FRAME_ROW_HEX_LENGTH)
    {
        sendProtocolLine("ROW:ERROR:LENGTH");
        return;
    }

    for (int i = 0; i < FRAME_ROW_HEX_LENGTH; i++)
    {
        if (!isHexCharacter(hex[i]))
        {
            sendProtocolLine("ROW:ERROR:HEX");
            return;
        }
    }

    for (int x = 0; x < DISPLAY_WIDTH; x++)
    {
        stagingDrawingFrame[getLogicalIndex(x, row)] =
            parseHexColor(hex + x * 6);
    }

    stagingRowsReceived[row] = true;

    sendProtocolLine(String("ROW:") + row + ":OK");
}

void endFrameTransfer()
{
    if (!receivingFrame)
    {
        sendProtocolLine("FRAME:ERROR:NO_FRAME");
        return;
    }

    for (int row = 0; row < DISPLAY_HEIGHT; row++)
    {
        if (!stagingRowsReceived[row])
        {
            sendProtocolLine("FRAME:ERROR:INCOMPLETE");
            return;
        }
    }

    for (int i = 0; i < LED_COUNT; i++)
    {
        activeDrawingFrame[i] = stagingDrawingFrame[i];
    }

    receivingFrame = false;
    sendProtocolLine("FRAME:STORED");
}

bool setContentMode(const char value[])
{
    if (strcmp(value, "TEXT") == 0)
    {
        contentMode = MODE_TEXT;
        resetAnimation();
        sendProtocolLine("MODE:TEXT");
        return true;
    }

    if (strcmp(value, "DRAWING") == 0)
    {
        contentMode = MODE_DRAWING;
        resetAnimation();
        sendProtocolLine("MODE:DRAWING");
        return true;
    }

    if (strcmp(value, "PRESET") == 0)
    {
        contentMode = MODE_PRESET;
        resetAnimation();
        sendProtocolLine("MODE:PRESET");
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
    String status = "STATUS:MATRIX=";

    status += DISPLAY_WIDTH;
    status += "x";
    status += DISPLAY_HEIGHT;
    status += ";MESSAGE=";
    status += message;
    status += ";EFFECT=";
    status += getEffectName();
    status += ";DIRECTION=";
    status += getDirectionName();
    status += ";MODE=";
    status += getContentModeName();
    status += ";PRESET=";
    status += getPresetName();
    status += ";PRESET_COLOR=";
    status += solidPresetColor.red;
    status += ",";
    status += solidPresetColor.green;
    status += ",";
    status += solidPresetColor.blue;
    status += ";CLOCK_COLOR=";
    status += clockPresetColor.red;
    status += ",";
    status += clockPresetColor.green;
    status += ",";
    status += clockPresetColor.blue;
    status += ";CLOCK_FORMAT=";
    status += (clockUse24Hour ? 24 : 12);
    status += ";CLOCK_LEADING_ZERO=";
    status += (clockLeadingZero ? 1 : 0);
    status += ";CLOCK_VALID=";
    status += (clockTimeValid ? 1 : 0);
    status += ";RAIN_COLOR=";
    status += rainPresetColor.red;
    status += ",";
    status += rainPresetColor.green;
    status += ",";
    status += rainPresetColor.blue;
    status += ";RAIN_SPEED=";
    status += rainSpeed;
    status += ";RAIN_DENSITY=";
    status += rainDensity;
    status += ";RAIN_TRAIL=";
    status += rainTrailLength;
    status += ";FIRE_PALETTE=";
    status += getFirePaletteName();
    status += ";FIRE_SPEED=";
    status += fireSpeed;
    status += ";FIRE_INTENSITY=";
    status += fireIntensity;
    status += ";SPEED=";
    status += animationSpeed;
    status += ";BLINK_ON=";
    status += blinkOnMs;
    status += ";BLINK_OFF=";
    status += blinkOffMs;
    status += ";PAUSED=";
    status += (animationPaused ? 1 : 0);
    status += ";COLOR=";
    status += messageColor.red;
    status += ",";
    status += messageColor.green;
    status += ",";
    status += messageColor.blue;
    status += ";BRIGHTNESS=";
    status += brightness;

    sendProtocolLine(status);
}

void printHelp()
{
    sendProtocolLine("Available commands:");
    sendProtocolLine("MESSAGE:<text>");
    sendProtocolLine("MODE:TEXT|DRAWING|PRESET");
    sendProtocolLine("PRESET:SOLID|CLOCK|RAIN|FIRE");
    sendProtocolLine("PRESET_PARAM:COLOR:<red>,<green>,<blue>");
    sendProtocolLine("PRESET_PARAM:CLOCK_COLOR:<red>,<green>,<blue>");
    sendProtocolLine("PRESET_PARAM:CLOCK_FORMAT:12|24");
    sendProtocolLine("PRESET_PARAM:CLOCK_LEADING_ZERO:0|1");
    sendProtocolLine("PRESET_PARAM:RAIN_COLOR:<red>,<green>,<blue>");
    sendProtocolLine("PRESET_PARAM:RAIN_SPEED:<1-20>");
    sendProtocolLine("PRESET_PARAM:RAIN_DENSITY:<1-100>");
    sendProtocolLine("PRESET_PARAM:RAIN_TRAIL:<2-7>");
    sendProtocolLine("PRESET_PARAM:FIRE_PALETTE:CLASSIC|BLUE|PURPLE");
    sendProtocolLine("PRESET_PARAM:FIRE_SPEED:<1-20>");
    sendProtocolLine("PRESET_PARAM:FIRE_INTENSITY:<1-100>");
    sendProtocolLine("CLOCK_TIME:<unix seconds>:<offset minutes east of UTC>");
    sendProtocolLine("EFFECT:STILL|SCROLL|WIPE|BLINK");
    sendProtocolLine("DIRECTION:LEFT|RIGHT|UP|DOWN");
    sendProtocolLine("SPEED:<pixels per second, 1-30>");
    sendProtocolLine("BLINK_ON:<milliseconds>");
    sendProtocolLine("BLINK_OFF:<milliseconds>");
    sendProtocolLine("PAUSED:0|1");
    sendProtocolLine("COLOR:<red>,<green>,<blue>");
    sendProtocolLine("BRIGHTNESS:<0-255>");
    sendProtocolLine("FRAME_BEGIN:32x8");
    sendProtocolLine("ROW:<0-7>:<192 hex characters>");
    sendProtocolLine("FRAME_END");
    sendProtocolLine("RESET");
    sendProtocolLine("STATUS");
    sendProtocolLine("HELP");
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

    sendProtocolLine(String("New message: ") + message);
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
            sendProtocolLine("Use EFFECT:STILL|SCROLL|WIPE|BLINK");
        }
    }
    else if (strncmp(command, "DIRECTION:", 10) == 0)
    {
        if (!setAnimationDirection(command + 10))
        {
            sendProtocolLine("Use DIRECTION:LEFT|RIGHT|UP|DOWN");
        }
    }
    else if (strncmp(command, "MODE:", 5) == 0)
    {
        if (!setContentMode(command + 5))
        {
            sendProtocolLine("Use MODE:TEXT|DRAWING|PRESET");
        }
    }
    else if (strncmp(command, "PRESET_PARAM:", 13) == 0)
    {
        if (!setPresetParameter(command + 13))
        {
            sendProtocolLine("Use PRESET_PARAM:COLOR|CLOCK|RAIN|FIRE setting");
        }
    }
    else if (strncmp(command, "PRESET:", 7) == 0)
    {
        if (!setActivePreset(command + 7))
        {
            sendProtocolLine("Use PRESET:SOLID|CLOCK|RAIN|FIRE");
        }
    }
    else if (strncmp(command, "CLOCK_TIME:", 11) == 0)
    {
        if (!setClockTime(command + 11))
        {
            sendProtocolLine("Use CLOCK_TIME:unixSeconds:offsetMinutesEastOfUtc");
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

            sendProtocolLine(String("New speed: ") + animationSpeed + " pixels per second");
        }
        else
        {
            sendProtocolLine("Speed must be 1-30 pixels per second.");
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
            sendProtocolLine("Blink-on duration must be 100-1200 milliseconds.");
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
            sendProtocolLine("Blink-off duration must be 100-1200 milliseconds.");
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
            sendProtocolLine("Use PAUSED:0 or PAUSED:1");
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

                sendProtocolLine(
                    String("New color: ") +
                    red +
                    ", " +
                    green +
                    ", " +
                    blue
                );

                renderAnimationFrame();
            }
            else
            {
                sendProtocolLine("Color values must be 0-255.");
            }
        }
        else
        {
            sendProtocolLine("Use COLOR:red,green,blue");
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

            sendProtocolLine(String("New brightness: ") + brightness);
        }
        else
        {
            sendProtocolLine("Brightness must be 0-255.");
        }
    }
    else if (strcmp(command, "STATUS") == 0)
    {
        printStatus();
    }
    else if (strcmp(command, "RESET") == 0)
    {
        resetAnimation();
        sendProtocolLine("Animation reset.");
    }
    else if (strcmp(command, "HELP") == 0)
    {
        printHelp();
    }
    else
    {
        sendProtocolLine("Unknown command.");
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
    startWirelessServices();

    Serial.println("PIXEL BOARD FIRMWARE V2");
}

void loop()
{
    httpServer.handleClient();
    webSocket.loop();
    checkSerialInput();
    updateAnimation();
}
