/*
 Copyright (c) 2014-present PlatformIO <contact@platformio.org>
 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at
    http://www.apache.org/licenses/LICENSE-2.0
 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
**/

#include <unity.h>

// void setUp(void) {
// // set stuff up here
// }

// void tearDown(void) {
// // clean stuff up here
// }

// the array for RGB values
const uint8_t rgb[50][3] = {
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
const uint8_t COLOR_ARRAY_MAX {50};

const unsigned long ALARM_SECONDS {60*30};

uint8_t getColor(unsigned long time)
{
    /* Interpolate colors as a function of time
     * Get time: 0 -> ALARM_SECONDS
     * color array indices: 0 -> COLOR_ARRAY_MAX
     */
    
    float fraction = (time * COLOR_ARRAY_MAX / ALARM_SECONDS);
    fraction = fraction - floor(fraction); // 0 -> 1
    uint8_t lower = floor(fraction);

    uint8_t r = (uint8_t) (rgb[lower][0] * (1.0 - fraction)) + (rgb[lower][0] * fraction);
    // uint8_t g = (uint8_t) (rgb[lower][1] * (1.0 - fraction)) + (rgb[lower][1] * fraction);
    // uint8_t b = (uint8_t) (rgb[lower][2] * (1.0 - fraction)) + (rgb[lower][2] * fraction);

    return r;
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

void test_getBrightness(void) {
    
    TEST_ASSERT_EQUAL(getBrightness(ALARM_SECONDS), 255);
    TEST_ASSERT(getBrightness(100) > -2);
}

void test_getColor(void) {
    
    TEST_ASSERT_EQUAL(getColor(ALARM_SECONDS), 250);
    TEST_ASSERT_EQUAL(getColor(0), 255);
}


int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_getBrightness);
    UNITY_END();

    return 0;
}