# LED-BLINK

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

- pali i gasi svjetleću diodu periodično u ovom slučaju 20 puta
