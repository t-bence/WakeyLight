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

#include <Arduino.h>
#include <DS3231.h>
#include <TimeLib.h>        // http://www.arduino.cc/playground/Code/Time
#include <Timezone.h>       // https://github.com/JChristensen/Timezone
#include <Adafruit_NeoPixel.h>


// set alarms here
// an alarm is: day-of-week, hour, minute
// only one alarm per day!
byte alarms[7][3] {
    {Mon, 7, 30},
    {Tue, 7, 30},
    {Wed, 7, 30},
    {Thu, 7, 30},
    {Fri, 7, 30},
    {Sat, 8, 0},
    {Sun, 8, 0}
};

const int ALARM_SECONDS = 30 * SECS_PER_MIN;

// other stuff

const int NUM_PIXELS {256};
const byte LED_PIN {2};

// the array for RGB values
const byte rgb[50][3] = {
    {255, 127, 0},
    {255, 132, 0},
    {255, 139, 22},
    {255, 144, 36},
    {255, 150, 47},
    {255, 155, 56},
    {255, 160, 65},
    {255, 164, 73},
    {255, 168, 81},
    {255, 172, 89},
    {255, 176, 96},
    {255, 180, 103},
    {255, 184, 109},
    {255, 187, 116},
    {255, 190, 122},
    {255, 193, 128},
    {255, 196, 134},
    {255, 199, 139},
    {255, 202, 145},
    {255, 204, 150},
    {255, 207, 155},
    {255, 209, 160},
    {255, 211, 165},
    {255, 213, 170},
    {255, 216, 175},
    {255, 218, 179},
    {255, 220, 184},
    {255, 221, 188},
    {255, 223, 192},
    {255, 225, 196},
    {255, 227, 200},
    {255, 228, 204},
    {255, 230, 208},
    {255, 232, 211},
    {255, 234, 215},
    {255, 236, 218},
    {255, 238, 221},
    {255, 239, 225},
    {255, 241, 228},
    {255, 243, 231},
    {255, 244, 234},
    {255, 246, 237},
    {255, 248, 240},
    {255, 249, 243},
    {255, 251, 246},
    {255, 252, 249},
    {255, 254, 252},
    {255, 255, 255},
    {252, 253, 255},
    {250, 252, 255}
};


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
    float color[3] {};

    for (byte i = 0; i < 3; i++)
    {
        byte lower = (byte) ((time * 50 / ALARM_SECONDS) % 2);
    }

    return Adafruit_NeoPixel::Color((byte) color[0], (byte) color[1], (byte) color[2]);
}

byte getBrightness(unsigned long time)
{
    // linear approximation, could be improved later
    float brightness = time * 2.55F / ALARM_SECONDS;
    if (brightness > 255.0f) { brightness = 255.0f; }
    else if (brightness < 0.0f) { brightness = 0.0f; }
    return (byte) brightness;
}

void loop() {
    // get time
    time_t utc = (time_t) rtc.getUnixTime(rtc.getTime());
    time_t local = hunTZ_.toLocal(utc);
    
    for (byte alarmIndex = 0; alarmIndex < DAYS_PER_WEEK; alarmIndex++)
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