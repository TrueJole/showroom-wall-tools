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

    void enableSnow() {
        snow = true;
    }

    void disableSnow() {
        snow = false;
    }

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

        addSnowFlakes();

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

        addSnowFlakes();

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

    void loop() {
        if (play && millis() - timestamp >= duration) {
            frameCounter++;
            if (frameCounter > endFrame) {
                frameCounter = startFrame;
            }
            timestamp = millis();
            display.clearDisplay(); // Clear the display buffer
            display.setRotation(rotation);
            display.drawBitmap(0, 0, &bitmap[frameCounter * numberData], spriteWidth, spriteHeight, color);

            if (snow) {
                updateSnowflakePosition();
            }

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


        // SNOW STUFF
        bool snow = false;
        struct flakeCoords
        {
            int x;
            int y;
            uint8_t type;
        };

        const int GOAL_FLAKE_NUM = 50;

        flakeCoords flakeData[50];

        int totalFlakeAmount = 0;
        int numberFlakes;
        clock_t TARGET_TIME = 20000;

        // 012345678   
        //    # #       0
        //  #  #  #     1
        //   # # #      2
        // #   #   #    3
        //  ### ###     4
        // #   #   #    5
        //   # # #      6
        //  #  #  #     7
        //    # #       8

        int pixelOffset[28][2] = {
        {3, 0}, {5, 0}, {1, 1}, {4, 1}, {7, 1}, {2, 2}, {4, 2}, {6, 2}, {0, 3}, {4, 3}, {8, 3}, {1, 4}, {2, 4}, {3, 4}, {5, 4}, {6, 4}, {7, 4}, {0, 5}, {4, 5}, {8, 5}, 
        {2, 6}, {4, 6}, {6, 6}, {1, 7}, {4, 7}, {7, 7}, {3, 8}, {5, 8}
        };


        void drawSnowFlake(int xPos, int yPos, uint8_t flakeType) {
            switch (flakeType) {
                case 0:
                case 5:
                case 7:
                case 1:   // medium snowflake, filled
                    display.drawPixel(xPos, yPos, SSD1306_INVERSE);             //    # 
                    display.drawPixel(xPos - 1, yPos + 1, SSD1306_INVERSE);     //   ###
                    display.drawPixel(xPos, yPos + 1, SSD1306_INVERSE);         //    #
                    display.drawPixel(xPos + 1, yPos + 1, SSD1306_INVERSE);     
                    display.drawPixel(xPos, yPos + 2, SSD1306_INVERSE);      
                    break;
                case 2:
                case 6:
                case 8:
                case 3:   // medium snowflake, empty
                    display.drawPixel(xPos, yPos, SSD1306_INVERSE);             //    # 
                    display.drawPixel(xPos - 1, yPos + 1, SSD1306_INVERSE);     //   # #
                    display.drawPixel(xPos + 1, yPos + 1, SSD1306_INVERSE);     //    #
                    display.drawPixel(xPos, yPos + 2, SSD1306_INVERSE);
                    break;
                case 4:   // large snowflake
                    for (int i = 0; i < 28; i++) {
                        display.drawPixel(xPos + pixelOffset[i][0], yPos + pixelOffset[i][1], SSD1306_INVERSE);
                    }
                    break;
            }

        }

        void addSnowFlakes() {
            numberFlakes = 50;
            
            for (int j = 0; j < numberFlakes; j++) {
                flakeData[j].x = rand()%129;
                flakeData[j].y = -rand()%64;
                flakeData[j].type = rand()%9;
                drawSnowFlake(flakeData[j].x, flakeData[j].y, flakeData[j].type);
            }
        }

        void updateSnowflakePosition() {
            for (int k = 49; k > 0; k--) {
                int K = k - 1;
                //update y
                int flakeSpeed = rand()%7+3;
                flakeData[K].y += flakeSpeed;
                //update x
                int horizontalFlakeOffset = rand()%5-2;
                flakeData[K].x += horizontalFlakeOffset;
                if (flakeData[K].x < 0 || flakeData[K].x > 128) {
                    flakeData[K].x -= horizontalFlakeOffset;
                }
                drawSnowFlake(flakeData[K].x, flakeData[K].y, flakeData[K].type);
                    if (flakeData[K].y > 64) {
                    flakeData[K].y = -1;
                }
            }
        }
        
};