# IZRAVNO UPRAVLJANJE GPIO PINOVIMA

Za izravno upravljanje GPIO pinova potrebno je koristiti biblioteku [pigpio](https://abyz.me.uk/rpi/pigpio/index.html). Funkcionira na svim inačicama Raspberry Pi mikroračunala.

Svi GPIO pinovi se identificiraju svojim *Broadcom* brojem (*GPIO N* umjesto rednog broja na pločici). Raspored i objašnjenja funkcija svakog pina može se naći [ovdje](https://pinout.xyz/).

Informacije o sučelju za korištenje biblioteke nalazi se [ovdje](https://abyz.me.uk/rpi/pigpio/cif.html).

Prevođenje izvornih datoteka u navedenim primjerima se radi naredbom:

```
gcc -Wall -pthread -o main main.c -lpigpio -lrt
```

Pokretanje izvršne datoteke radi se s naredbom ```sudo ./main```.
