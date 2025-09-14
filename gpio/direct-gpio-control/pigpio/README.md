# PIGPIO

Biblioteka ```pigpio``` može se instalirati sljedećom naredbom:

```
sudo apt install pigpio
```

Upute za korištenje su [ovdje](https://abyz.me.uk/rpi/pigpio/index.html), a informacije o sučelju su [ovdje](https://abyz.me.uk/rpi/pigpio/cif.html). Dodatno, raspored i objašnjenja funkcija svakog GPIO pina za Raspberry Pi 4B može se naći [ovdje](https://pinout.xyz/).

Prevođenje izvornih datoteka u navedenim primjerima radi se naredbom:

```
gcc -Wall -pthread -o main main.c -lpigpio -lrt
```

Pokretanje izvršne datoteke radi se s naredbom:

```
sudo ./main
```

