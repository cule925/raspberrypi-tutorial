# GPIO PINOVI

Raspberry Pi 4B PCB pločica sadrži [40 pinski GPIO konektor](https://www.raspberrypi.com/documentation/computers/raspberry-pi.html#gpio) čiji je raspored GPIO pinova jednak svim starijim pločicama čime se osigurava kompatibilnost s proširivim modulima.

GPIO pinovi se mogu konfigurirati kao ulazi, izlazi ili kao neka specijalizirana alternativna funkcija (I2C pin, SPI pin, PWM pin, UART pin, ...).

Ako je pin softverski konfiguriran kao izlazni, logička razina visoko daje na pinu napon od 3.3 V, a logička razina nisko daje na pinu napon od 0 V.

Ako je pin softverski konfiguriran kao ulazni, maksimalni dozvoljeni ulazni napon je 3.3 V, a minimalni 0 V. Na svakom pinu moguće je i softverski konfigurirati interne pull-up i pull-down otpornike izuzev pinova GPIO 2 i GPIO 3 gdje su fiksirani pull-up otpornici.

Kako bi korisnik softverski mogao konfigurirati GPIO pinove, on mora biti dodan u grupu ```gpio```. Inicijalno stvoren korisnik, onaj definiran u ```userconf.txt``` datoteci ili stvoren pri prvoj inicijalizaciji, je već automatski dodan. Naredba za dodavanje korisnika u grupu ```gpio``` je:

```
sudo usermod -a -G gpio <korisničko ime>
```

## Konfiguracija maksimalne izlazne snage GPIO pinova

Postoje tri grupe GPIO pinova. Adrese u kojoj se upisuju 32-bitne postavke snage za te grupe GPIO pinova su:

- 0x7e10002c - GPIO pinovi 0-27 - **ovi se nalaze na 40 pinskom konektoru**
- 0x7e100030 - GPIO pinovi 28-45
- 0x7e100034 - GPIO pinovi 46-53

Polja bitova na adresama imaju sljedeće značenje:

- bitovi 31-24
	- ime polja: PASSWRD, moguće samo pisanje
	- služi za zaštitu od slučajnog pisanja, mora imati vrijednost 0x5A kada se nešto upisuje na adresu
	- vrijednost pri resetiranju: 0x0
- bitovi 23-5
	- rezervirano
- bit 4
	- ime polja: SLEW, moguće pisanje i čitanje
	- služi za postavljanje brzine promjene stanja:
		- 0x0 - najbrže što može
		- 0x1 - ograničena brzina za smanjene smetnji
	- vrijednost pri resetiranju: 0x1
- bit 3
	- ime polja: HYST, moguće pisanje i čitanje
	- služi za postavljanje histereze za ulazne pinove:
		- 0x0 - onemogući histerezu
		- 0x1 - omogući histerezu, pomaže pri sprječavanju lažnih očitanja pri smetnjama
	- vrijednost pri resetiranju: 0x1
- bitovi 2-0
	- ime polja: DRIVE, moguće pisanje i čitanje
	- služi za postavljanje maksimalne struje koju GPIO pin može isporučiti sa stabilnim izlaznim naponom:
		- 0x0 - 1 mA
		- 0x1 - 2 mA
		- 0x2 - 3 mA
		- 0x3 - 4 mA
		- 0x4 - 5 mA
		- 0x5 - 6 mA
		- 0x6 - 7 mA
		- 0x7 - 8 mA
	- vrijednost pri resetiranju: 0x3

Specifikacije o naponima GPIO pinova nalaze se [ovdje](https://www.raspberrypi.com/documentation/computers/raspberry-pi.html#voltage-specifications).

## Konfiguracija samih GPIO pinova

Konfiguracija GPIO pinova može se napraviti preko konfiguracijske datoteke ```/boot/firmware/config.txt```, naredbi kao što su primjerice ```pinctrl```, ```raspi-config``` i slično ili pak preko vlastitih programa koji koriste određene biblioteke kao što su:

- C biblioteka ```pigpio``
- Python biblioteka ```RPi.GPIO```
- Python biblioteka ```gpiozero```

Primjeri korištenja biblioteka mogu se naći [ovdje](direct-gpio-control).

### Korištenje pinctrl naredbe

Naredba ```pinctrl``` je prvenstveno namijenjena je za debugiranje. Ispis stanja GPIO pinova od 0 do 28 može se napraviti naredbom:

```
pinctrl get 0-28
```

Postavljanje određenog GPIO pina kao izlaznog bez internih pull-up i pull-down otpornika može se napraviti naredbom:

```
pinctrl set <broj GPIO pina> op pn
```

Postavljanje određenog GPIO pina u stanje visoko odnosno nisko može se napraviti naredbom:

```
pinctrl set <broj GPIO pina> <dh|dl>
```

Postavljanje određenog GPIO pina kao ulaznog s internim pull-up ili pull-down otpornikom može se napraviti naredbom:

```
pinctrl set <broj GPIO pina> ip <pu|pd>
```

Izlistavanje alternativnih funkcija za GPIO pinove od 0 do 28 može se napraviti naredbom:

```
pinctrl -c bcm2711 0-28
```

Postavljanje alternativne funkcije GPIO pinu može se napraviti naredbom:

```
pinctrl set <broj GPIO pina> <a0|a1|a2|a3|a4|a5>
```

Postavljanje pina kao izlaza ili ulaza poništava postavljenu alternativnu funkciju. Dodatnu pomoć i informacije u vezi pinctrl naredbe može se dobiti naredbom ```pinctrl help```.

### Konfiguracija GPIO pinova uz pomoć config.txt datoteke

Postavljanje [konfiguracije pinova](https://www.raspberrypi.com/documentation/computers/config_txt.html#gpio-control) može se napraviti kroz datoteku ```/boot/firmware/config.txt``` direktivom ```gpio```. Konfiguracija će se postaviti pri ponovnom pokretanju mikroračunala.

Opcije su slične kao i kod ```pinctrl``` naredbe. Primjerice, konfiguracija GPIO pina kao izlaza i postavljanje mu stanja može se napraviti direktivom:

```
gpio=<broj GPIO pina>=op,<dh|dl>
```

Konfiguracija GPIO pina kao ulaza i postavljanje mu interni pull-up ili pull-down otpornik može se napraviti naredbom:

```
gpio=<broj GPIO pina>=ip,<pu|pd>
```

Konfiguracija alternativne funkcija GPIO pinu može se napraviti direktivom:

```
gpio=<broj GPIO pina>=<a0|a1|a2|a3|a4|a5>
```

#### Konfiguracija preko overlay datoteka

GPIO pinovi se mogu posebno konfigurirati preko overlay datoteka koje uređuju stablo uređaja tijekom pokretanja mikroračunala. Ove overlay datoteke se nalaze u ```/boot/firmware/overlays/``` direktoriju. Primjena overlay datoteke nad stablom uređaja radi se direktivom ```dtoverlay``` u ```/boot/firmware/config.txt``` datoteci, a upute za specifične overlay datoteke nalaze se [ovdje](https://github.com/raspberrypi/firmware/blob/master/boot/overlays/README).

Za neke overlay datoteke, Linux jezgra će učitati i odgovarajuće upravljačke programe ako postoje.

Primjerice, ako se želi primijeniti overlay datoteka koja će postaviti GPIO pinove 19, 20 i 21 kao SPI MOSI, SPI MISO i SPI SCLK (njihove alternativne funkcije) te postaviti tri pina po izboru kao SPI CS pinove, to je moguće dodavanjem direktive:

```
dtoverlay=spi1-3cs=cs0_pin=<GPIO pin za CS0>,cs1_pin=<GPIO pin za CS1>,cs2_pin=<GPIO pin za CS2>
```

GPU firmware ```start.elf``` će učitati ```/boot/firmware/bcm2711-rpi-4-b.dtb``` stablo uređaja i primijeniti overlay ```/boot/firmware/overlays/spi1-3cs.dtbo``` nad njime. Kasnije će Linux jezgra učitati SPI upravljački program i ponuditi 3 sučelja u obliku datoteka uređaja za komunikaciju: ```/dev/spidev1.0```, ```/dev/spidev1.1``` i ```/dev/spidev1.2```.

Datoteka ```config.txt``` se parsira slijedno, odozgo prema dolje, kasniji parametri prepisuju ranije u slučaju konflikata u postavkama pinova.

