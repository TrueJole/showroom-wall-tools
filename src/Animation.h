#pragma once

#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Screen setup
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define SCREEN_WIDTH 128 // OLED display width, in pixels

// Declaration for SSD1306 display connected using software SPI (default case):
#define OLED_MOSI 4 // DATA
#define OLED_CLK 5
#define OLED_DC 6
#define OLED_CS 7
#define OLED_RESET 0

class Display {
    public:
    Display()
    : display(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS)
    {};

    void drawPixel(int16_t x, int16_t y, uint16_t color) {
        display.drawPixel(x, y, color);
    }

    //normal setup
    void animationSetup(uint8_t rotation = 0) {
        // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
        if (!display.begin(SSD1306_SWITCHCAPVCC)) {
            Serial.println("Display - Allocation failed!");
            for (;;); // Don't proceed, loop forever
        }
        display.setRotation(rotation);
        display.display();
        Serial.println("Display - Setup done!");

    }

    //setup with capability of showing image immediatly after start
    void animationSetup(const unsigned char *bitmap, uint8_t color, uint8_t rotation, int spriteWidth, int spriteHeight) {
        // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
        if (!display.begin(SSD1306_SWITCHCAPVCC)) {
            Serial.println("Display - Allocation failed!");
            for (;;); // Don't proceed, loop forever
        }
        display.setRotation(rotation);
        display.clearDisplay();
        display.drawBitmap(0, 0, bitmap, spriteWidth, spriteHeight, color);
        display.display();
        Serial.println("Display - Setup done!");
    }

    //draws a singular image(bitmap) on screen
    void drawImage(const unsigned char *bitmap, uint8_t color, uint8_t rotation, int spriteWidth, int spriteHeight) {
        display.clearDisplay(); // Clear the display buffer
        display.setRotation(rotation);
        display.drawBitmap(0, 0, bitmap, spriteWidth, spriteHeight, color);
        display.display();
    }

    //loops once throug a specified area of a bitmap, printing the images stored on screen
    void drawAnimation(const unsigned char *bitmap_, uint8_t fps, uint8_t startFrame_, uint8_t endFrame_, int numberData_, uint8_t color_, uint8_t rotation_, int spriteWidth_, int spriteHeight_) {
        frameCounter = startFrame_;
        bitmap = bitmap_;
        duration = (u64_t) 1000 / fps;
        startFrame = startFrame_;
        endFrame = endFrame_;
        numberData = numberData_;
        color = color_;
        rotation = rotation_;
        spriteWidth = spriteWidth_;
        spriteHeight = spriteHeight_;
        play = true;
    }

    void displayLoopBegin() {
        if (play && millis() - timestamp >= duration) {
            frameCounter++;
            if (frameCounter > endFrame) {
                frameCounter = startFrame;
            }
            timestamp = millis();
            display.clearDisplay(); // Clear the display buffer
            display.setRotation(rotation);
            display.drawBitmap(0, 0, &bitmap[frameCounter * numberData], spriteWidth, spriteHeight, color);
            endLoop = true;
        }
    } 

    void displayLoopEnd() {
        if (endLoop) {
            endLoop = false;
            display.display();
        }
    } 

    
    private:
        Adafruit_SSD1306 display;
        u64_t timestamp;

        uint8_t startFrame;
        uint8_t endFrame;

        uint8_t duration;
        uint8_t rotation;
        const unsigned char *bitmap;
        int numberData;
        int spriteWidth;
        int spriteHeight;
        uint8_t color;
        bool play = false;
        bool endLoop = false;

        int frameCounter;
};