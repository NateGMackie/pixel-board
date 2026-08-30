#include <Arduino.h>
#include <FastLED.h>

void showDisplay();
void printStatus();

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

char message[MESSAGE_BUFFER_SIZE] = "HELLO WORLD!";
char inputBuffer[MESSAGE_BUFFER_SIZE];

int inputIndex = 0;

Color messageColor = WHITE;
const int TEXT_Y = 1;
unsigned long scrollInterval = 100;

int textOffset = DISPLAY_WIDTH;
int textWidth = 0;

unsigned long lastScrollTime = 0;

int brightness = 2;

void printStatus()
{
    Serial.print("STATUS:MESSAGE=");
    Serial.print(message);

    Serial.print(";SPEED=");
    Serial.print(scrollInterval);

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
    Serial.println("SPEED:<milliseconds>");
    Serial.println("COLOR:<red>,<green>,<blue>");
    Serial.println("BRIGHTNESS:<0-255>");
    Serial.println("STATUS");
    Serial.println("HELP");
}

void updateMarquee()
{
    unsigned long currentTime = millis();

    if (currentTime - lastScrollTime >= scrollInterval)
    {
        lastScrollTime = currentTime;

        clearDisplay();

        drawText(
            message,
            textOffset,
            TEXT_Y,
            messageColor
        );

        showDisplay();

        textOffset--;

        if (textOffset < -textWidth)
        {
            textOffset = DISPLAY_WIDTH;
        }
    }
}

void setMarqueeMessage(const char newMessage[])
{
    strcpy(message, newMessage);

    textWidth = getTextWidth(message);
    textOffset = DISPLAY_WIDTH;

    Serial.print("New message: ");
    Serial.println(message);
}

void processCommand(const char command[])
{
    if (strncmp(command, "MESSAGE:", 8) == 0)
    {
        setMarqueeMessage(command + 8);
    }
    else if (strncmp(command, "SPEED:", 6) == 0)
    {
        int newSpeed = atoi(command + 6);

        if (newSpeed > 0)
        {
            scrollInterval = newSpeed;

            Serial.print("New speed: ");
            Serial.println(scrollInterval);
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

                Serial.print("\b \b");
            }

            continue;
        }

        Serial.print(incomingCharacter);

        if (incomingCharacter == '\n')
        {
            inputBuffer[inputIndex] = '\0';

            if (inputIndex > 0)
            {
                processCommand(inputBuffer);
            }

            inputIndex = 0;
        }
        else if (inputIndex < MESSAGE_BUFFER_SIZE - 1)
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

    FastLED.addLeds<WS2812B, DATA_PIN, GRB>(
        physicalLeds,
        LED_COUNT
    );

    FastLED.setBrightness(brightness);

    clearDisplay();
    showDisplay();

    textWidth = getTextWidth(message);

    Serial.println("Enter a marquee message:");
}

void loop()
{
    checkSerialInput();
    updateMarquee();
}
