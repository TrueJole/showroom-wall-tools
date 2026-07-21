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

    void setPlay(bool state) {
        play = state;
    }

    void enableSnow() {
        snow = true;
    }

    void disableSnow() {
        snow = false;
    }

    void setColor(uint8_t color_) {
        color = color_;
    }

    void setBitmap(const unsigned char *bitmap_, size_t spriteWidth_, size_t spriteHeight_, size_t bytesPerBitmap_) {
        bitmap = bitmap_;
        spriteWidth = spriteWidth_;
        spriteHeight = spriteHeight_;
        bytesPerBitmap = bytesPerBitmap_;
    }

    void setRotation(uint8_t rotation_) {
        if (rotation < 0 || rotation > 3)
            return;
        rotation = rotation_;
        display.setRotation(rotation);
    }

    //minimal setup
    void animationSetup(uint8_t rotation = 0) {
        // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
        if (!display.begin(SSD1306_SWITCHCAPVCC)) {
            Serial.println("Display - Allocation failed!");
            for (;;); // Don't proceed, loop forever
        }
        display.clearDisplay();
        setRotation(rotation);
        display.display();

        addSnowFlakes();

        Serial.println("Display - Setup done!");
    }

    //setup with more configuration
    void animationSetup(const unsigned char *bitmap, uint8_t color, uint8_t rotation, size_t spriteWidth, size_t spriteHeight, size_t bytesPerBitmap_) {
        // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
        if (!display.begin(SSD1306_SWITCHCAPVCC)) {
            Serial.println("Display - Allocation failed!");
            for (;;); // Don't proceed, loop forever
        }
        
        setRotation(rotation);
        setBitmap(bitmap, spriteWidth, spriteHeight, bytesPerBitmap_);
        setColor(color);

        display.clearDisplay();
        display.display();

        addSnowFlakes();

        Serial.println("Display - Setup done!");
    }

    //draws a singular image on screen
    void drawImage(size_t index) {
        startFrame = max((size_t) 0, index);
        endFrame = max((size_t) 0, index);
        frameCounter = startFrame;
        duration = (uint64_t) 1000 / 5;
        play = true;
    }

    //loops once throug a specified area of a bitmap, printing the images stored on screen
    void drawAnimation(size_t startFrame_, size_t endFrame_, uint64_t fps) {
        duration = (uint64_t) 1000 / max((uint64_t) 0, min(fps, (uint64_t) 1000));
        startFrame = max((size_t) 0, min(startFrame_, endFrame_));
        endFrame = max((size_t) startFrame, endFrame_);
        frameCounter = startFrame;
        play = true;
    }

    void loop() {
        if (play && millis() - timestamp >= duration && bitmap != nullptr) {
            frameCounter++;
            if (frameCounter > endFrame) {
                frameCounter = startFrame;
            }
            timestamp = millis();
            display.clearDisplay(); // Clear the display buffer
            display.setRotation(rotation);
            display.drawBitmap(0, 0, &bitmap[frameCounter * bytesPerBitmap], spriteWidth, spriteHeight, color);

            if (snow) {
                updateSnowflakePosition();
            }

            display.display();
        }
    } 
    
    private:
        Adafruit_SSD1306 display;

        uint64_t timestamp = 0;
        bool play = false;
        bool endLoop = false;
        size_t frameCounter = 0;

        size_t startFrame = 0;
        size_t endFrame = 0;
        uint64_t duration = 0;
        uint8_t rotation = 0;
        const unsigned char *bitmap;
        size_t bytesPerBitmap = 1024;
        size_t spriteWidth = 64;
        size_t spriteHeight = 128;
        uint8_t color = SSD1306_WHITE;
        
        // SNOW STUFF
        bool snow = false;

        struct flakeCoords
        {
            int x = 0;
            int y = 0;
            uint8_t type  = 0;
        };

        static const int NUMBER_OF_FLAKES = 50;

        flakeCoords flakeData[NUMBER_OF_FLAKES];

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
                case 1:
                case 2:
                case 3:   // medium snowflake, filled
                    display.drawPixel(xPos, yPos, SSD1306_INVERSE);             //    # 
                    display.drawPixel(xPos - 1, yPos + 1, SSD1306_INVERSE);     //   ###
                    display.drawPixel(xPos, yPos + 1, SSD1306_INVERSE);         //    #
                    display.drawPixel(xPos + 1, yPos + 1, SSD1306_INVERSE);     
                    display.drawPixel(xPos, yPos + 2, SSD1306_INVERSE);      
                    break;
                case 4:
                case 5:
                case 6:
                case 7:   // medium snowflake, empty
                    display.drawPixel(xPos, yPos, SSD1306_INVERSE);             //    # 
                    display.drawPixel(xPos - 1, yPos + 1, SSD1306_INVERSE);     //   # #
                    display.drawPixel(xPos + 1, yPos + 1, SSD1306_INVERSE);     //    #
                    display.drawPixel(xPos, yPos + 2, SSD1306_INVERSE);
                    break;
                case 8:   // large snowflake
                    for (int i = 0; i < 28; i++) {
                        display.drawPixel(xPos + pixelOffset[i][0], yPos + pixelOffset[i][1], SSD1306_INVERSE);
                    }
                    break;
            }

        }

        void addSnowFlakes() {
            for (size_t  j = 0; j < NUMBER_OF_FLAKES; j++) {
                flakeData[j].x = rand()%129;
                flakeData[j].y = -rand()%64;
                flakeData[j].type = rand()%9;
                drawSnowFlake(flakeData[j].x, flakeData[j].y, flakeData[j].type);
            }
        }

        void updateSnowflakePosition() {
            for (size_t k = 0; k < NUMBER_OF_FLAKES; k++) {
                //update y
                int32_t flakeSpeed = rand()%7+3;
                flakeData[k].y += flakeSpeed;
                //update x
                int32_t horizontalFlakeOffset = rand()%5-2;
                flakeData[k].x += horizontalFlakeOffset;
                if (flakeData[k].x < 0 || flakeData[k].x > 128) {
                    flakeData[k].x -= horizontalFlakeOffset;
                }
                drawSnowFlake(flakeData[k].x, flakeData[k].y, flakeData[k].type);
                    if (flakeData[k].y > 64) {
                    flakeData[k].y = -1;
                }
            }
        }
        
};