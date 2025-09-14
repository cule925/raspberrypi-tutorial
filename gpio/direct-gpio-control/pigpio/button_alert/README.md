# LED-SOFTWARE-PWM

Aplikacija je napravljena za Raspberry Pi 4B.

Potrebno je povezati tipkalo između GPIO pina 27 i GND:

```
        +-----------------+
        | RASPBERRY PI 4B |
        |                 |
        |         GPIO 27 +---------------------+
        |                 |                     |
        |             GND +----+               -+-
        |                 |    |              | O | BUTTON
        +-----------------+    |               -+-
                               |                |
                               +----------------+
```

Aplikacija radi sljedeće:

- u slučaju pritiska tipke na dogodit će se promjena stanja na pinu 27 što će pozvati *callback* funkciju koja će ispisati da je tipka pritisnuta, ovo će se dogoditi u ovom slučaju maksimalno 5 puta
