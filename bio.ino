#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include "pitches.h"

// leds in each strip
#define NUMPIXELS 3

// pins
const int bttn_pin1 = 2;
const int bttn_pin2 = 3;
const int bttn_pin3 = 4;
const int bttn_pin4 = 5;
const int lstrip_pin1 = 9;
const int lstrip_pin2 = 8;
const int lstrip_pin3 = 7;
const int lstrip_pin4 = 6;
const int buzzer_pin = 10;

// neopixel strips
Adafruit_NeoPixel lstrip1 = Adafruit_NeoPixel(NUMPIXELS, lstrip_pin1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel lstrip2 = Adafruit_NeoPixel(NUMPIXELS, lstrip_pin2, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel lstrip3 = Adafruit_NeoPixel(NUMPIXELS, lstrip_pin3, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel lstrip4 = Adafruit_NeoPixel(NUMPIXELS, lstrip_pin4, NEO_GRB + NEO_KHZ800);

// lcd display (16x2)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// colors
uint32_t dry_powders = lstrip1.Color(20, 15, 0);
uint32_t cultured_meat = lstrip1.Color(20, 0, 0);
uint32_t hydrogenotrophs = lstrip1.Color(0, 0, 20);
uint32_t spirulina = lstrip1.Color(0, 20, 0);
uint32_t reset = lstrip1.Color(0, 0, 0);

// button press turn on current strip
int current_strip_index = 1;
const int button_delay = 300;

// Recipe structure (who needs oop?)
struct Recipe {
    uint32_t color1;
    uint32_t color2;
    uint32_t color3;
    uint32_t color4;
    const char* food;
};

// All food recipies
Recipe recipes[] = {
    {cultured_meat, cultured_meat, cultured_meat, cultured_meat, "Steak"},
    {cultured_meat, cultured_meat, spirulina, spirulina, "Chicken"},
    {cultured_meat, spirulina, spirulina, spirulina, "Lamb"},
    {cultured_meat, cultured_meat, cultured_meat, spirulina, "Fish"},
    {dry_powders, cultured_meat, cultured_meat, hydrogenotrophs, "Burger"},
    {dry_powders, cultured_meat, spirulina, spirulina, "Hotdog"},
    {dry_powders, cultured_meat, cultured_meat, spirulina, "Beef Stroganoff"},
    {dry_powders, dry_powders, spirulina, spirulina, "Syrniki"},
    {dry_powders, dry_powders, dry_powders, spirulina, "Waffles"},
    {dry_powders, dry_powders, cultured_meat, spirulina, "Pizza"},
    {dry_powders, dry_powders, dry_powders, dry_powders, "Rice"},
    {dry_powders, dry_powders, cultured_meat, hydrogenotrophs, "Dumplings"},
    {spirulina, spirulina, spirulina, spirulina, "Noodles"},
    {dry_powders, cultured_meat, hydrogenotrophs, spirulina, "Sushi"},
    {dry_powders, dry_powders, dry_powders, hydrogenotrophs, "Tacos"},
    {dry_powders, cultured_meat, hydrogenotrophs, hydrogenotrophs, "Quesadillas"},
    {hydrogenotrophs, hydrogenotrophs, hydrogenotrophs, spirulina, "Caesar Salad"},
    {dry_powders, spirulina, spirulina, spirulina, "Mac' and Cheese"},
    {dry_powders, hydrogenotrophs, hydrogenotrophs, hydrogenotrophs, "Vegetable Salad"},
    {dry_powders, dry_powders, spirulina, spirulina, "Falafel"},
    {dry_powders, dry_powders, dry_powders, hydrogenotrophs, "Bolognese"},
    {dry_powders, dry_powders, dry_powders, spirulina, "Bread"},
    {dry_powders, dry_powders, dry_powders, spirulina, "Chicken Soup"},
    {dry_powders, dry_powders, hydrogenotrophs, hydrogenotrophs, "Miso Soup"},
    {hydrogenotrophs, hydrogenotrophs, hydrogenotrophs, hydrogenotrophs, "Bean Soup"}
};

void setup()
{
    // buttons and leds
    pinMode(bttn_pin1, INPUT_PULLUP);
    pinMode(bttn_pin2, INPUT_PULLUP);
    pinMode(bttn_pin3, INPUT_PULLUP);
    pinMode(bttn_pin4, INPUT_PULLUP);

    // init neopixel strips
    lstrip1.begin();
    lstrip2.begin();
    lstrip3.begin();
    lstrip4.begin();

    // reset led strips
    lstrip_reset_all();

    // init lcd display
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
}

void loop()
{
    bool win = false;
    if (digitalRead(bttn_pin1) == LOW) {
        lstrip_fill_color(dry_powders, current_strip_index);
        delay(button_delay);
        current_strip_index++;
    } else if (digitalRead(bttn_pin2) == LOW) {
        lstrip_fill_color(cultured_meat, current_strip_index);
        delay(button_delay);
        current_strip_index++;
    } else if (digitalRead(bttn_pin3) == LOW) {
        lstrip_fill_color(hydrogenotrophs, current_strip_index);
        delay(button_delay);
        current_strip_index++;
    } else if (digitalRead(bttn_pin4) == LOW) {
        lstrip_fill_color(spirulina, current_strip_index);
        delay(button_delay);
        current_strip_index++;
    }

    if (current_strip_index > 4) {
        for (const Recipe& recipe : recipes) {
            if (lstrip1.getPixelColor(0) == recipe.color1 && lstrip2.getPixelColor(0) == recipe.color2 &&
                lstrip3.getPixelColor(0) == recipe.color3 && lstrip4.getPixelColor(0) == recipe.color4) {
                lcd.print(recipe.food);
                win_sound();
                win = true;
                delay(1000);
                lcd.clear();
                break;
            }
        }
        if (!win) {
            lcd.print("Invalid Recipe!");
            lose_sound();
            delay(1000);
            lcd.clear();
        }
        lstrip_reset_all();
        current_strip_index = 1;
    }
}

// reset all leds
void lstrip_reset_all()
{
    lstrip_fill_color(reset, 1);
    lstrip_fill_color(reset, 2);
    lstrip_fill_color(reset, 3);
    lstrip_fill_color(reset, 4);
}

// fill entire strip with color
void lstrip_fill_color(uint32_t color, int strip_index)
{
    Adafruit_NeoPixel* strip;
    // Select strip based on the strip_index
    switch (strip_index) {
        case 1: strip = &lstrip1; break;
        case 2: strip = &lstrip2; break;
        case 3: strip = &lstrip3; break;
        case 4: strip = &lstrip4; break;
        default: return;
    }

    // Fill the specified strip with the given color
    for (int i = 0; i < NUMPIXELS; ++i) {
        strip->setPixelColor(i, color);
        strip->show();
    }
}

void win_sound()
{
    tone(buzzer_pin, NOTE_C4);
    delay(100);
    noTone(buzzer_pin);
    delay(130);

    tone(buzzer_pin, NOTE_C4);
    delay(120);
    noTone(buzzer_pin);

    tone(buzzer_pin, NOTE_G4);
    delay(300);
    noTone(buzzer_pin);
}

void lose_sound()
{
    tone(buzzer_pin, NOTE_C4);
    delay(80);
    noTone(buzzer_pin);
    delay(50);

    tone(buzzer_pin, NOTE_A2);
    delay(130);
    noTone(buzzer_pin);

    tone(buzzer_pin, NOTE_A1);
    delay(550);
    noTone(buzzer_pin);
}
