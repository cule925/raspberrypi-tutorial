# LED-SOFTWARE-PWM

Aplikacija je napravljena za Raspberry Pi 4B.

Potrebno je na GPIO pin 17 postaviti svjetleću diodu (*eng. LED*) i otpornik (2 kOhm) u seriju ih na drugi kraju povezati s GND. Također, potrebno je povezati tipkalo između GPIO pina 27 i GND:

```
        +-----------------+
        | RASPBERRY PI 4B |
        |                 |
        |         GPIO 27 +--------------------------------------+
        |                 |                                      |
        |         GPIO 17 +---------------------+                |
        |                 |                     |                |
        |             GND +----+               _|_              -+-
        |                 |    |               \ / LED         | O | BUTTON
        +-----------------+    |                |               -+-
                               |                |                |
                               |               +++               |
                               |               | | R - 2kOhm     |
                               |               | |               |
                               |               +++               |
                               |                |                |
                               +----------------+----------------+
```

Aplikacija radi sljedeće:

- čita stanje tipkala svako malo i ako očita nisku razinu (tipka pritisnuta) pali svjetleću diodu, ovo se u ovom slučaju ponavlja 5 puta
