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


// set alarms here
// an alarm is: day-of-week, hour, minute
// only one alarm per day!
uint8_t alarms[7][3] {
    {Mon, 7, 30},
    {Tue, 7, 30},
    {Wed, 7, 30},
    {Thu, 7, 30},
    {Fri, 7, 30},
    {Sat, 8, 0},
    {Sun, 8, 0}
};

const int NUM_PIXELS {256};
const uint8_t LED_PIN {2};

Adafruit_NeoPixel pixels {NUM_PIXELS, LED_PIN, NEO_GRB + NEO_KHZ800};

// store the time to wake up here in some dimensionless way?
// could be 0 (light gets on) -> 100 (determined wake up time + sound?)
int wakePercent {0};

long deltaSeconds {0};

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
    return;
}

uint32_t getColor(unsigned long time)
{
    /* Interpolate colors as a function of time
     * Get time: 0 -> ALARM_SECONDS
     * color array indices: 0 -> COLOR_ARRAY_MAX
     */
    
    float fraction = (time * COLOR_ARRAY_MAX / ALARM_SECONDS);
    fraction = fraction - floor(fraction); // 0 -> 1
    uint8_t lower = floor(fraction);

    uint8_t r = (uint8_t) (rgb[lower][0] * (1.0 - fraction)) + (rgb[lower][0] * fraction);
    uint8_t g = (uint8_t) (rgb[lower][1] * (1.0 - fraction)) + (rgb[lower][1] * fraction);
    uint8_t b = (uint8_t) (rgb[lower][2] * (1.0 - fraction)) + (rgb[lower][2] * fraction);

    return Adafruit_NeoPixel::Color(r, g, b);
}

uint8_t getBrightness(unsigned long time)
{
    // linear approximation, could be improved later
    // time: 0 -> ALARM_SECONDS
    // brightness: 0 -> 255
    float brightness = time * 255.0F / ALARM_SECONDS;
    if (brightness > 255.0f) { brightness = 255.0f; }
    else if (brightness < 0.0f) { brightness = 0.0f; }
    return (uint8_t) brightness;
}

void loop() {
    // get time
    time_t utc = (time_t) rtc.getUnixTime(rtc.getTime());
    time_t local = hunTZ_.toLocal(utc);
    
    for (uint8_t alarmIndex = 0; alarmIndex < DAYS_PER_WEEK; alarmIndex++)
    {
        if (alarms[alarmIndex][0] == weekday(local))
        {
            deltaSeconds = alarms[alarmIndex][1] * SECS_PER_HOUR
                + alarms[alarmIndex][2] * SECS_PER_MIN
                - elapsedSecsToday(local);
            if ((deltaSeconds < 0) || (deltaSeconds > ALARM_SECONDS))
            {
                deltaSeconds = 0;
            }
        }
    }

    // get color for the current time
    uint32_t color = getColor(deltaSeconds);

    if (wakePercent > 0)
    {
        pixels.fill(color);
        pixels.setBrightness(getBrightness(deltaSeconds));
    }

    if (deltaSeconds == ALARM_SECONDS)
    {
        alarm();
        delay(5000); // to let things settle down
        pixels.clear();
        deltaSeconds = 0;
    }
}