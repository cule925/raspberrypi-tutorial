# BOOT PROCES I JEZGRA

Raspberry Pi 4B [boot proces](https://www.raspberrypi.com/documentation/computers/raspberry-pi.html#eeprom-boot-flow) podijeljen je u više koraka. Koristi se kod u ROM koji se nalazi u BCM2711 čipu (*OTP Memory - One Time Programmable Memory*), EEPROM koji se nalazi na pločici i bootloader datoteka na nekom od umetnutog vanjskog medija (SD kartica, USB flash, ...).

Dakle, boot proces kada se Raspberry Pi 4B pokrene je sljedeći:

**BOOTLOADER IZ ROM OTP MEMORIJE (1. FAZA BOOTLOADERA):**

- pokreće se VideoCore grafički procesor i on izvršava kod u ROM OTP-u koji se učita u cache memoriju
	- provjerava se jeli kod definira nRPIBOOT pin (koristi se kod Raspberry Pi Compute Module 4 mikroračunala)
	- ako je nRPIBOOT u logičkom stanju visoko ili uopće nije definiran:
		- provjerava se postoji li datoteka ```recovery.bin``` na **boot** particiji na SD kartici/eMMC memoriji
			- ako postoji učitava ju se u memoriju i izvršava
				- ažurira se EEPROM ```pieeprom.bin``` ili ```pieeprom.upd``` datotekom na **boot** particiji na SD kartici/eMMC memoriji
			- učitava se bootloader iz EEPROM-a (2. faza bootloadera)
				- ako se uspije učitat pokreće ga se, inače se nastavlja dalje
	- ponavlja se:
		- provjerava se postoji li datoteka ```recovery.bin``` na **boot** particiji na USB flashu na USB portu (ako je uopće umetnut)
			- ako postoji učitava ju se u memoriju i izvršava
				- ažurira se EEPROM ```pieeprom.bin``` ili ```pieeprom.upd``` datotekom na **boot** particiji na SD kartici/eMMC memoriji
			- učitava se bootloader iz EEPROM-a (2. faza bootloadera)
				- ako se uspije učitat pokreće ga se, inače se nastavlja dalje

**BOOTLOADER IZ EEPROMA (2. FAZA BOOTLOADERA):**

- inicijalizira se takt i SDRAM
- čita se EEPROM konfiguracijska datoteka
- čita se ```PM_RSTS``` registar kako bi se ustanovilo traži li se HALT:
	- provjeravaju se ```POWER_OFF_ON_HALT``` i ```WAKE_ON_GPIO``` postavke u EEPROM konfiguraciji
	- ako je ```POWER_OFF_ON_HALT == 1``` i ```WAKE_ON_GPIO == 0``` koristi se PMIC (*eng. Power Management Integrated Circuit*) kako bi se isključio sustav
	- inače ako je ```WAKE_ON_GPIO == 1``` omogućuje se prekid na padajući brid na GPIO 3 za izlazak iz stanja mirovanja (*eng. sleep mode*)
	- ulazi se u stanje mirovanja
- ponavlja se:
	- čita se ```BOOT_ORDER``` parametar iz EEPROM konfiguracijske datoteke
	- ako je *boot* parametar ```RESTART``` (0xf) vraća se natrag na prvi *boot* način definiran u ```BOOT_ORDER``` parametru
	- ako je *boot* parametar ```STOP``` (0xe) zaustavlja se *boot* proces, ispisuje se poruka greške i zaustavlja se sustav (*HALT* stanje)
	- ako je *boot* parametar ```SD_CARD``` (0x1) isprobava se učitavanje firmware u SDRAM s **boot** particije SD kartice
		- u slučaju uspjeha pokreće se ```start_*.elf``` firmware, inače se nastavlja dalje
	- ako je *boot* parametar ```NETWORK``` (0x2):
		- pravi se DHCP zahtjev za IP adresu (preko Ethernet sučelja)
		- učitava se firmware s DHCP poslužitelja ili statički definiranog TFTP poslužitelja
		- u slučaju uspjeha pokreće se ```start_*.elf``` firmware koja se učitava u SDRAM, inače se nastavlja dalje
	- ako je *boot* parametar ```USB-MSD``` (0x4):
		- redom se ide po dostupnim USB flash uređajima priključenim na USB portovima:
			- proba se učitati u SDRAM ```start_*.elf``` firmware s **boot** particije USB flasha
			- u slučaju uspjeha pokreće se firmware, inače se nastavlja dalje
	- ostali *boot* parametri mogu se vidjeti [ovdje](https://www.raspberrypi.com/documentation/computers/raspberry-pi.html#BOOT_ORDER)

Parametar ```BOOT_ORDER``` je 32 bitni cijeli broj gdje svaka četvrtina bitova (heksadekadski broj) predstavlja *boot* parametar krenuvši od najmanje značajne četvrtine. Dakle, ako je primjerice ```BOOT_ORDER=0xf14``` onda se izvršava redom ```USB-MSD (0x4)```, ```SD_CARD (0x1)``` pa ```RESTART (0xf)```.

Trenutačna konfiguracija EEPROM bootloadera može se vidjeti na Raspberry Pi mikroračunalu naredbom:

```
sudo rpi-eeprom-config
```

Uređivanje parametara EEPROM bootloadera kao što su ```BOOT_ORDER```, ```POWER_OFF_ON_HALT```, ```WAKE_ON_GPIO``` i [drugi](https://www.raspberrypi.com/documentation/computers/raspberry-pi.html#configuration-properties) može se napraviti naredbom:

```
sudo -E rpi-eeprom-config --edit
```

U ```/boot/firmware``` stvorit će se datoteke ```recovery.bin``` (*recovery* izvršna datoteka) i ```pieeprom.upd``` (novi EEPROM bootloader). Također, stvorit će se ```pieeprom.sig``` SHA256 sažetak ```pieeprom.upd``` bootloadera. Kada se proces ažuriranja EEPROM-a završi, ```recovery.bin``` će se preimenovat u ```recovery.000``` kako se ažuriranje ne bi ponovno izvršavalo pri sljedećem ponovnom pokretanju. Ako se novi EEPROM bootloader nazove ```pieeprom.bin```, preimenovanje ```recovery.bin``` u ```recovery.000``` se neće dogoditi (korisnik mora sam obrisati ili preimenovati datoteku).

Dakle, ažuriranje će se dogoditi pri ponovnom pokretanju. Ako se želi zaustaviti proces ažuriranja, potrebno je izvršiti naredbu:

```
sudo rpi-eeprom-update -r
```

Naredba će obrisati datoteke ```recovery.bin``` i ```pieeprom.*```.

**IZVRŠAVANJE GPU FIRMWAREA (3. FAZA BOOTLOADERA)**

Učitani bootloader s EEPROM-a pokušava učitati jednu od ```start_*.elf``` [datoteka](https://www.raspberrypi.com/documentation/computers/configuration.html#device-trees-overlays-and-parameters) s **boot** particije. Specifično, za Raspberry Pi 4B, firmware datoteka je ```start4.elf``` i ona radi sljedeće:

- čita konfiguracijsku datoteku ```config.txt```, učitava s **boot** particije datoteku ```bcm2711-rpi-4-b.dtb``` stabla uređaja koje definira hardver i primjenjuje odgovarajuće ```*.dtbo``` **overlay** datoteke koje se također nalaze u **boot** particiji na to stablo uređaja
- učitava Linux jezgru u SDRAM što je u slučaju Raspberry Pi 4B mikroračunala datoteka ```kernel8.img``` na **boot** particiji, prosljeđuje joj jezgrene parametre iz ```cmdline.txt``` datoteke i prosljeđuje pokazivač na stablo uređaja
- predaje kontrolu ARM procesoru

**LINUX BOOT FAZA**

ARM procesor pokreće Linux jezgru. Jezgra montira privremeni datotečni sustav ```initramfs8``` s **boot** particije koji sadrži najosnovnije alate i upravljačke programe. Inicijalizira osnovni hardver, a zatim montira **root** particiju i pokreće ```init``` proces.

Informacije Linux jezgre specifične za Raspberry Pi nalaze se [ovdje](https://www.raspberrypi.com/documentation/computers/linux_kernel.html).

## Sadržaj boot particije

Boot particija Raspberry Pi OS-a sadrži sljedeće datoteke:

- ```start_*.elf``` - firmware koji GPU izvršava
- ```fixup_*.dat``` - linker datoteka za ```start_*.elf```
- ```*.dtb``` - stabla uređaja za specifične Raspberry Pi modele, za Raspberry Pi 4B koristi se ```bcm2711-rpi-4-b.dtb```
- ```config.txt``` - parametri za ```start_*.elf```, definira brzinu takta, ukupnu memoriju, rezoluciju, overlay datoteke za stablo uređaja (slično BIOS postavkama)
- ```overlay/*.dtbo``` - overlay datoteke za stablo uređaja
- ```issue.txt``` - informacije o datumu i *commit ID-u* Raspberry Pi OS distribucije
- ```kernel*.img``` - Linux jezgra
- ```cmdline.txt``` - parametri Linux jezgre
- ```initramfs*``` - privremeni datotečni sustav za Linux jezgru

Raspberry Pi OS boot particija se inače montira na ```/boot/firmware``` direktorij.

## Izgradnja vlastite Linux jezgre

[Linux jezgra Raspberry Pi OS-a](https://www.raspberrypi.com/documentation/computers/linux_kernel.html) je modificirana inačica Linux jezgre s potpunom podrškom za Raspberry Pi hardver. Sadrži specifične upravljačke programe i proširenja za stablo uređaja za Raspberry Pi hardver. Raspberry Pi inačica jezgra prati uglavnom stabilne LTS (Long Term Support) Linux inačice, ali ne u stvarnom vremenu zbog dodatnog testiranja. Trenutačna inačica jezgre može se vidjeti naredbom ```sudo uname -r```. Izvorni kod Raspberry Pi OS Linux jezgre nalazi se [ovdje](https://github.com/raspberrypi/firmware/). Ažuriranje jezgre preko APT-a radi se naredbama ```sudo apt update``` i ```sudo apt full-upgrade```.

### Izgradnja jezgre na Raspberry Pi 4B mikroračunalu

Za izgradnju Linux jezgre za Raspberry Pi 4B mikroračunalo ARM64 arhitekture, prvo je potrebno preuzeti alate za izgradnju jezgre:

```
sudo apt install git bc bison flex libssl-dev make libncurses5-dev
```

Zatim je potrebno preuzeti izvorni kod najnovije inačice Raspberry Pi OS Linux jezgre u korisnikovom *home* direktoriju:

```
cd ~/
git clone --depth=1 https://github.com/raspberrypi/linux
```

Izgradnja uobičajene konfiguracije za 64 bitnu Linux jezgru Raspberry Pi OS-a koji se pokreće na Raspberry Pi 4B mikroračunalu radi se sljedećim naredbama:

```
cd linux
KERNEL=kernel8
make bcm2711_defconfig
```

Naredba će spremiti konfiguraciju u ```.config``` datoteku. Ako se želi dalje uređivati značajke jezgre koje će se spremati u ```.config``` datoteku, to se može naredbom:

```
make menuconfig
```

Izgradnja 64 bitne jezgre, jezgrenih modula i stabla uređaja GCC i DTC prevodiocima radi se naredbom:

```
make -j6 Image.gz modules dtbs
```

Opcija ```-j6``` označava broj paralelnih procesa koji će sudjelovati u izgradnji što je u ovom slučaju 6. Za izgradnju na Raspberry Pi mikroračunalima, preporuka je postaviti broj koji je 1.5 puta veći nego broj procesorskih jezgri koji se može vidjeti naredbom ```nproc```. Nakon uspješne izgradnje, potrebno je:

- kopirati nove jezgrene module u ```/lib/modules/<nova verzija jezgre>``` direktorij
- stvoriti sigurnosnu kopiju stare jezgre u ```/boot/firmware``` direktoriju
- kopirati novu jezgru i stablo uređaja u ```/boot/firmware``` direktorij
- kopirati overlay datoteke i README tekstualnu datoteku u ```/boot/firmware/overlay``` direktorij

To se može napraviti sljedećim naredbama:

```
sudo make -j6 modules_install
sudo cp /boot/firmware/$KERNEL.img /boot/firmware/$KERNEL-backup.img
sudo cp arch/arm64/boot/Image.gz /boot/firmware/$KERNEL.img
sudo cp arch/arm64/boot/dts/broadcom/*.dtb /boot/firmware/
sudo cp arch/arm64/boot/dts/overlays/*.dtb* /boot/firmware/overlays/
sudo cp arch/arm64/boot/dts/overlays/README /boot/firmware/overlays/
```

Za kraj, potrebno je ponovno pokrenuti mikroračunalo naredbom ```sudo reboot```.

### Izgradnja jezgre za Raspberry Pi 4B mikroračunalo unakrsnim prevođenjem

Za izgradnju Linux jezgre za Raspberry Pi 4B mikroračunalo ARM64 arhitekture **na računalu x86 arhitekture koje pokreće Arch Linux**, prvo je potrebno preuzeti alate za izgradnju jezgre:

```
sudo pacman -Syu
sudo pacman -S git bc bison flex openssl make glibc ncurses aarch64-linux-gnu-gcc
```

Zatim je potrebno preuzeti izvorni kod najnovije inačice Raspberry Pi OS Linux jezgre u korisnikovom *home* direktoriju:

```
cd ~/
git clone --depth=1 https://github.com/raspberrypi/linux
```

Izgradnja uobičajene konfiguracije za 64 bitnu Linux jezgru Raspberry Pi OS-a koji se pokreće na Raspberry Pi 4B mikroračunalu radi se sljedećim naredbama:

```
cd linux
KERNEL=kernel8
make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- bcm2711_defconfig
```

Naredba će spremiti konfiguraciju u ```.config``` datoteku. Ako se želi dalje uređivati značajke jezgre koje će se spremati u ```.config``` datoteku, to se može naredbom:

```
make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- menuconfig
```

Izgradnja 64 bitne jezgre, jezgrenih modula i stabla uređaja GCC i DTC prevodiocima radi se naredbom:

```
make -j<broj procesa> ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- Image modules dtbs
```

Opcija ```-j<broj procesa>``` označava broj paralelnih procesa koji će sudjelovati u izgradnji. Za izgradnju na osobnom računalu, preporuka je postaviti broj koji je od 1 do 1.5 puta veći nego broj procesorskih jezgri koji se može vidjeti naredbom ```nproc```. Nakon uspješne izgradnje, potrebno je:

- kopirati nove jezgrene module u ```/lib/modules/<nova verzija jezgre>``` direktorij
- stvoriti sigurnosnu kopiju stare jezgre u ```/boot/firmware``` direktoriju
- kopirati novu jezgru i stablo uređaja u ```/boot/firmware``` direktorij
- kopirati overlay datoteke i README tekstualnu datoteku u ```/boot/firmware/overlay``` direktorij

Prije izvršenja potrebnih naredbi, potrebno je umetnuti SD karticu u računalo (preko čitača SD kartice) gdje se nalazi instaliran Raspberry Pi OS koji se pokreće na Raspberry Pi 4B mikroračunalu. Zatim je potrebno odmontirati automatski montirane particije te SD kartice te montirati **root** particiju na ```/mnt/rpi-root``` i **boot** na ```/mnt/rpi-root/boot/firmware```.

```
sudo umount /dev/<datoteka uređaja koja predstavlja SD karticu>*
sudo mkdir /mnt/rpi-root
sudo mount /dev/<datoteka uređaja koja predstavlja SD karticu>2 /mnt/rpi-root
sudo mount /dev/<datoteka uređaja koja predstavlja SD karticu>1 /mnt/rpi-root/boot/firmware
```

Tek sada se može izvršiti:

```
sudo env PATH=$PATH make -j<broj procesa> ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- INSTALL_MOD_PATH=/mnt/rpi-root modules_install
sudo cp /mnt/rpi-root/boot/firmware/$KERNEL.img /mnt/rpi-root/boot/firmware/$KERNEL-backup.img
sudo cp arch/arm64/boot/Image /mnt/rpi-root/boot/firmware/$KERNEL.img
sudo cp arch/arm64/boot/dts/broadcom/*.dtb /mnt/rpi-root/boot/firmware/
sudo cp arch/arm64/boot/dts/overlays/*.dtb* /mnt/rpi-root/boot/firmware/overlays/
sudo cp arch/arm64/boot/dts/overlays/README /mnt/rpi-root/boot/firmware/overlays/
```

Za kraj, potrebno je odmontirati particije SD kartice i umetnuti SD karticu u Raspberry Pi 4B mikroupravljač. Naredbe za demontiranje particija i izbacivanje SD kartice su:

```
sudo umount /mnt/rpi-root/boot/firmware/
sudo umount /mnt/rpi-root/
sudo rmdir /mnt/rpi-root/
sudo eject /dev/<datoteka uređaja koja predstavlja SD karticu>
```

