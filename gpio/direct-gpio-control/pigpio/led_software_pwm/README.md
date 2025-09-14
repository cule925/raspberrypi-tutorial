# LED-SOFTWARE-PWM

Aplikacija je napravljena za Raspberry Pi 4B.

Potrebno je na GPIO pin 17 postaviti svjetleću diodu (*eng. LED*) i otpornik (2 kOhm) u seriju ih na drugi kraju povezati s GND:

```
        +-----------------+
        | RASPBERRY PI 4B |
        |                 |
        |         GPIO 17 +---------------------+
        |                 |                     |
        |             GND +----+               _|_
        |                 |    |               \ / LED
        +-----------------+    |                |
                               |                |
                               |               +++
                               |               | | R - 2kOhm
                               |               | |
                               |               +++
                               |                |
                               +----------------+
```

Aplikacija radi sljedeće:

- koristi softversku implementiranu pulsnu širinsku modulaciju kako bi lagano pojačala i prigušila svjetlinu svjetleće diode
