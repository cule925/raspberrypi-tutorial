# I2C-MASTER

Aplikacija je napravljena za Raspberry Pi 4B.

Potrebno je napraviti sljedeće:

- GPIO 2 potrebno je spojiti SDA nekog I2C *slavea*
- GPIO 3 potrebno je spojiti SCL nekog I2C *slavea*
- GND potrebno je spojiti GND nekog I2C *slavea*

```
        +-----------------+
        | RASPBERRY PI 4B |
        |                 |
        |          GPIO 2 +---------------------- SDA
        |                 |
        |          GPIO 3 +---------------------- SCL     TO SLAVE
        |                 |
        |             GND +---------------------- GND
        |                 |
        +-----------------+
```

Aplikacija radi sljedeće:

- aplikacija inicijalizira I2C 1 sklop i spaja se na njega kao *master*
- šalje *slaveu*, u ovom slučaju s adresom 0x22, dva bajta nizom: "{0x0, 0x1}, {0x0, 0x0}, {0x1, 0x1}, {0x1, 0x0}" 10 puta

Primjer *slavea* može biti [ovaj primjer s ESP32 mikroupravljačem](https://github.com/cule925/esp32-tutorial/tree/master/i2c/i2c_slave), bitno je da *slave* omogućuje primanje dva podatkovna bajta.

**GPIO 2 i GPIO 3 imaju fiksirane pull-up otpornike od 1.8 kOhm na 3.3 V i s njih je jedino moguće koristiti I2C 1 sklop. S GPIO 0 i GPIO 1 je moguće koristiti I2C 0 sklop, ali ih sustav obično koristi za čitanje EEPROM-a s proširivih modula. Zadana [brzina I2C 1 sabirnice](https://github.com/raspberrypi/linux/blob/rpi-4.1.y/arch/arm/boot/dts/overlays/README) je 100000 Hz.**

**Prije korištenja I2C 1 sklopa, potrebno ga je [omogućiti](https://elinux.org/RPiconfig#Device_Tree) u /boot/firmware/config.txt datoteci.** To se može napraviti [naredbom](https://www.raspberrypi.com/documentation/computers/configuration.html#i2c-nonint):

```
sudo raspi-config nonint do_i2c 0
```

Naredba će urediti datoteku */boot/firmware/config.txt* tako što će joj postaviti parametar ```dtparam=i2c_arm=on``` (što je moguće i ručno napraviti). Raspberry Pi mikroračunalo je potrebno ponovno pokrenuti kako bi *firmware* mogao očitati parametar i primijeniti ga na bazno stablo uređaja naredbom:

```
sudo reboot
```

Više informacija o */boot/firmware/config.txt* parametrima koje mogu utjecati na bazno stablo uređaja može se naći [ovdje](https://github.com/raspberrypi/linux/blob/rpi-4.1.y/arch/arm/boot/dts/overlays/README).

Omogućujući I2C 1 sklop, u direktoriju */dev/* pojavit će se datoteka uređaja *i2c-1* koja predstavlja I2C 1 sklop.
