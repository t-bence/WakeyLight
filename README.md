# WakeyLight

## Description

This is going to be an alarm clock that wakes you up by gradually changing the color temperature, 
simulating the rising sun. Brightness is also increased, and there may even be a sound at the end.
The whole lighting up process lasts for 30 mins, which is the approx. duration of sunrise.

## Tools

This uses an Arduino Uno-compatible mirocontroller and a WS2812B-style individually addressable LED
matrix. At the moment, a 16x16 matrix is used, but this can easily be adjusted. I haven't decided
yet whether it will have sound or not.

## Notes

**A beefy power supply is probably needed: could draw up to 16A @ 5V!!!**
 
Sunrise is about 2000 K, bright day is about 6500 K.
These are rough numbers from
https://en.wikipedia.org/wiki/Golden_hour_(photography)
 
The RGB colors for color temperatures are from:
https://github.com/jonls/redshift/blob/master/README-colorramp
