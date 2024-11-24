# LED-SOFTWARE-PWM

Aplikacija je napravljena za Raspberry Pi 4B.

Potrebno je na GPIO pin 18 postaviti svjetleću diodu (*eng. LED*) i otpornik (2 kOhm) u seriju ih na drugi kraju povezati s GND:

```
        +-----------------+
        | RASPBERRY PI 4B |
        |                 |
        |         GPIO 18 +---------------------+
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

- koristi hardversku implementiranu pulsnu širinsku modulaciju kako bi lagano pojačala i prigušila svjetlinu svjetleće diode
- ne podržavaju svi pinovi hardversku implementiranu pulsnu širinsku modulaciju, potrebno je pogledati raspored i objašnjenja funkcija svakog pina [ovdje](https://pinout.xyz/).
