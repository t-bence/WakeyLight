/*
 * Alarm clock with color temperature and strength adjustment to simulate
 * the Sun rising. Designed for a 16*16 element WS2812B LED matrix.
 * A beefy power supply is probably needed: could draw up to 16A @ 5V!!!
 * 
 * Sunrise is about 2000 K, bright day is about 6500 K.
 * These are rough numbers from
 * https://en.wikipedia.org/wiki/Golden_hour_(photography)
 * 
 * The RGB colors for color temperatures are from:
 * https://github.com/jonls/redshift/blob/master/README-colorramp
 */

#ifdef UNIT_TEST
    #include "ArduinoFake.h"
#else
    #include "Arduino.h"
#endif
#include <DS3231.h>
#include <TimeLib.h>        // http://www.arduino.cc/playground/Code/Time
#include <Timezone.h>       // https://github.com/JChristensen/Timezone
#include <Adafruit_NeoPixel.h>
#include "main.h"

enum time_units {
    HOUR,
    MINUTE
};

enum RGB {
    R,
    G,
    B
};

// set alarm here
uint8_t alarmTime[2] = {7, 45};

// the length of the 
const int ALARM_SECONDS = 30 * SECS_PER_MIN;

const int NUM_PIXELS {256};
const uint8_t LED_PIN {2};

Adafruit_NeoPixel pixels {NUM_PIXELS, LED_PIN, NEO_GRB + NEO_KHZ800};

int secondsLeft {0};

// the real time clock
DS3231 rtc { SDA, SCL };

TimeChangeRule myDST_{ "CEST", Last, Sun, Mar, 2, 120 };
// Standard time = UTC + 1 hours
TimeChangeRule mySTD_{ "CET", Last, Sun, Oct, 3, 60 };
Timezone hunTZ_{ myDST_, mySTD_ };

void setup()
{
    rtc.begin();
    pixels.begin();
    pixels.clear();
    pixels.setBrightness(0);
    pixels.show();
}

void alarm()
{
    // noises? blinking annoying colors?
    delay(5000); // to let things settle down
    return;
}

uint32_t getColor(float x, uint8_t maxValues)
{
    /* Interpolate colors
     * x: 0 -> 50.0
     * color array indices: 0 -> maxValues
     */

    uint8_t r {0}; uint8_t g {0}; uint8_t b {0};

    if (x >= maxValues) {
        r = rgb[maxValues][R];
        g = rgb[maxValues][G];
        b = rgb[maxValues][B];
    }
    else if (x <= 0) {
        r = rgb[0][R];
        g = rgb[0][G];
        b = rgb[0][B];
    }
    else {    
        uint8_t lower = floor(x);
        float fraction = x - floor(x); // 0 -> 1

        r = (uint8_t) (rgb[lower][0] * (1.0F - fraction)) + (rgb[lower + 1][0] * fraction);
        g = (uint8_t) (rgb[lower][1] * (1.0F - fraction)) + (rgb[lower + 1][1] * fraction);
        b = (uint8_t) (rgb[lower][2] * (1.0F - fraction)) + (rgb[lower + 1][2] * fraction);
    }

    return Adafruit_NeoPixel::Color(r, g, b);
}

uint8_t getBrightness(unsigned long timeLeft, unsigned int alarmLength)
{
    // linear approximation, could be improved later
    // time: 0 -> ALARM_SECONDS
    // brightness: 0 -> 255
    if (timeLeft > alarmLength) { return 0; }
    if (timeLeft < 0) { return 0; }
    return (uint8_t) timeLeft * 255.0F / alarmLength;
}

void loop() {
    // get time
    time_t utc = (time_t) rtc.getUnixTime(rtc.getTime());
    time_t local = hunTZ_.toLocal(utc);
    
    secondsLeft = alarmTime[HOUR] * SECS_PER_HOUR
        + alarmTime[MINUTE] * SECS_PER_MIN
        - elapsedSecsToday(local);

    // get color for the current time
    uint32_t color = getColor(
        ((float) secondsLeft) / ALARM_SECONDS * COLOR_ARRAY_MAX,
        COLOR_ARRAY_MAX);

    pixels.fill(color);
    pixels.setBrightness(
        getBrightness(secondsLeft, ALARM_SECONDS));

    if (secondsLeft <= 0)
    {
        alarm();
        pixels.clear();
    }
}