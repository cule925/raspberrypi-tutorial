# UPRAVLJANJE GPIO PINOVIMA

Raspberry Pi 4B pločica ima [40 pinski GPIO konektor](https://www.raspberrypi.com/documentation/computers/raspberry-pi.html#gpio-and-the-40-pin-header) čiji je raspored GPIO pinova, napajanja i uzemljenja jednak svim ostalim Raspberry Pi pločicama. Ovo osigurava kompatibilnost svih Raspberry Pi pločica proširivim modulima.

Svaki GPIO pin se može konfigurirati kao ulaz, izlaz ili kao neka alternativna funkcija (I2C, SPI, PWM, UART, ...) ovisno o kojem GPIO pinu se radi.

GPIO pin označen kao izlaz u stanju visoko daje 3.3 V dok u stanju nisko daje 0 V.

GPIO pin označen kao ulaz za stanje visoko prima 3.3 V dok za stanje nisko prima 0 V.
GPIO 2 i GPIO 3 imaju fiksirane interne *pull-up* otpornike.

Kako bi korisnik mogao pristupiti GPIO pinovima, mora biti dodan u grupu *gpio*. Zadani korisnik koji se stvorio *userconf.txt* datotekom ili ručno prvim pokretanjem je već automatski u toj grupi. Međutim, ako to nije tako jednostavno ga se može dodati naredbom:

```
sudo usermod -a -G gpio [korisničko ime]
```

## Adrese GPIO pinova

Adrese GPIO skupa pinova:

- 0x7e10002c = GPIO pinovi od 0 do 27 (**ovi se nalaze izloženi na 40 pinskom konektoru**)
- 0x7e100030 = GPIO pinovi od 28 do 45
- 0x7e100034 = GPIO pinovi od 46 do 53

Bitovi na svakoj adresi imaju značenja:

- bitovi od 0 do 2
	- izlazna snaga GPIO sklopa napajanja (*eng. Drive Strength*)
	- ime polja je *DRIVE*
- bit 3
	- omogućuje histerezu na ulazu (*eng. Input Hysteresis*)
	- služi za izbjegavanje šuma na ulazu, promjena stanja se očitava samo kad se dosegne određeni napon
	- 1 za omogućeno, 0 za onemogućeno
	- ime polja je *HYST*
- bit 4
	- omogućuje ograničenu brzine promjene napona tijekom promjene stanja (*eng. Slew Rate*)
	- 1 za omogućeno, 0 za onemogućeno
	- ime polja je *SLEW*
- bitovi od 5 do 23
	- rezervirani
- bitovi od 24 do 31
	- tzv. zaporka, služi kao zaštita za pisanje na adresu
	- ako vrijednost bitova vrijednosti koja se piše na adresi nije jednaka 0x5A onda se sama vrijednost na adresu neće upisati
	- ime polja je *PASSWRD*

### Snaga GPIO sklopa napajanja

Potrebno je spomenuti pojam izlazne snage GPIO sklopa napajanja (*eng. Drive Strength*). Za navedene grupe pinova pinova moguće je postaviti maksimalnu izlaznu struju za koju se garantira da će izlazni napon biti stabilan odnosno da će izlazni napon biti jednak specifikaciji. Vrijednosti koje se mogu upisati na GPIO adrese na bitovima 2:0 kod Raspberry Pi 4B mikroračunala mogu biti sljedeće:

- 0 - garantira stabilnost napona ako se na izlazu povuče maksimalno 1 mA
- 1 - garantira stabilnost napona ako se na izlazu povuče maksimalno 2 mA
- 2 - garantira stabilnost napona ako se na izlazu povuče maksimalno 3 mA
- 3 - garantira stabilnost napona ako se na izlazu povuče maksimalno 4 mA
- 4 - garantira stabilnost napona ako se na izlazu povuče maksimalno 5 mA
- 5 - garantira stabilnost napona ako se na izlazu povuče maksimalno 6 mA
- 6 - garantira stabilnost napona ako se na izlazu povuče maksimalno 7 mA
- 7 - garantira stabilnost napona ako se na izlazu povuče maksimalno 8 mA

**Maksimalna struja koja se može povući kroz pinove bez garantiranog oštećenja je 16 mA. Za spajanje svjetleće diode ili tipkala preporuka je koristiti otpornike. Ne preporučuje se izravno pokretati motore preko GPIO pinova.**

### Specifikacije napona i struje GPIO pinova

U slučaju da je pin postavljen kao ulaz:

- dok je na ulazu detektiran maksimalan napon od 0.8 V, registrirat će se niska razina
- dok je na ulazu detektiran minimalan napon od 2 V i ako je histereza omogućena, registrirat će se visoka razina

U slučaju da je pin postavljen kao izlaz:

- kada je na izlazu postavljena niska razina uz izlaznu snagu GPIO sklopa postavljenu na 3 (4 mA), maksimalni mogući izlazni napon je 0.4 V uz izlaznu struju od -4 mA
- kada je na izlazu postavljena visoka razina uz izlaznu snagu GPIO sklopa postavljenu na 3 (4 mA), minimalni mogući izlazni napon je 2.6 V uz izlaznu struju od 4 mA

- kada je na izlazu postavljena niska razina uz izlaznu snagu GPIO sklopa postavljenu na 7 (8 mA), minimalna izlazna struja je 7 mA uz izlazni napon od 0.4 V
- kada je na izlazu postavljena visoka razina uz izlaznu snagu GPIO sklopa postavljenu na 7 (8 mA), minimalna izlazna struja je 7 mA uz izlazni napon od 2.6 V

- minimalna vrijednost *pull-up* otpornika može biti 33 kOhma, a maksimalna 73 kOhma
- minimalna vrijednost *pull-down* otpornika može biti 33 kOhma, a maksimalna 73 kOhma

## Biblioteka za upravljanje pinovima

C biblioteka koja će se koristiti za upravljanje pinovima je [*pigpio*](https://abyz.me.uk/rpi/pigpio/index.html). Ova datoteka se već nalazi instalirana na Raspberry Pi OS-u. U slučaju da nije, potrebno ju je preuzeti naredbama:

```
sudo apt update
sudo apt full-upgrade
sudo apt install pigpio
```

Ako se želi koristiti neki viši programski jezik za upravljanje GPIO pinovima, može se koristiti Python 3 i biblioteka [*gpiozero*](https://gpiozero.readthedocs.io/en/latest/index.html).

### Načini upravljanja GPIO pinova

Dva su načina upravljanja GPIO pinovima uz pomoć *pigpio* biblioteke: [izravno](https://abyz.me.uk/rpi/pigpio/index.html) i [preko *pigpiod* servisa](https://abyz.me.uk/rpi/pigpio/pigpiod.html).

**Izravni način je jednostavniji, ali samo jedan proces može manipulirati GPIO pinovima, inače bi dolazilo do konflikata.**

Primjeri za izravno upravljanje GPIO pinova nalaze se [ovdje](direct-gpio-control).

**Servis način je mrvicu kompleksniji, ali nema problem s konfliktima jer mu procesi šalju zahtjeve za GPIO, a on njime upravlja.**

Servis *pigpiod* nudi upravljanje GPIO pinovima preko mreže, slušajući port 8888 i odgovarajući na zahtjeve. Postoji i [Python modul](https://abyz.me.uk/rpi/pigpio/python.html) za slanje zahtjeva *pigpiod* servisu.

