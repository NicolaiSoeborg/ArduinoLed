# Arduino Led

A small Arduino project for fancy LED light.

The project allows for multiple *light modes* that are controlable by a cheap "china button" (one without a debouncer).
My setup was a chief hat with a LED strip and a small microphone -- see the gif below.


## LED programs
 1. Status LED blink (gather background noise)
 2. Sound activated blinking (see gif below)
 3. Blink one LED at a time  (1 sec delay)
 4. Fade blinking (slow)
 5. Fade blinking (fast)
 6. Random blinking


## Pin setup
Pin | Unit
--- | -------------
 1  | Microphone
 3  | Button
 9  | Red LED
 10 | Blue LED
 11 | Green LED
 13 | Small status LED


## Demo
![Chief hat demo](/demo.gif)

(shouting at the hat!)