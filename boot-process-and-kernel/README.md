# BOOT PROCES I JEZGRA

Od Raspberry Pi 4B serije mikroračunala pa nadalje, ne koristi se više datoteka *bootcode.bin* kao *bootloader* već se koristi ugrađeni [SPI flash EEPROM čip](https://www.raspberrypi.com/documentation/computers/raspberry-pi.html#eeprom-boot-flow) koji sadržava *boot* kod.

Kada se uključi Raspberry Pi 4B mikroračunalo, grafički procesor VideoCore 6 koji se nalazi u sustavu na čipu (*eng. System on Chip - SoC*) se pokreće i izvršava obje faze *bootloadera*, ARM procesor ostaje i tek se pali nakon što se Linux jezgra učita.

Prva faza *bootloadera*:

- čita se OTP (*eng. One Time Programmable*) memorija kako bi se utvrdilo je li definiran *nRPIBOOT* GPIO
	- odabrani *nRPIBOOT* GPIO pin može biti jedan od sljedećih: 2, 4, 5, 6, 7 i 8, preporučuje se koristiti 8 jer je on inicijalno u stanju visoko
- ako je *nRPIBOOT*:
	- u stanju visoko ili uopće nije definiran, provjerava se OTP kako bi se ustanovilo može li se učitati *recovery.bin* s SD kartice:
		- ako je omogućeno učitavanje *recovery.bin* s SD kartice, provjerava se SD kartica za datoteku *recovery.bin*:
			- ako je *recovery.bin* pronađen, pokreće se i ažurira se EEPROM
		- provjerava se EEPROM za drugu fazu *bootloadera*
	- u stanju nisko, *recovery.bin* se pokušava s USB uređaja:
		- ako je pronađen *recovery.bin* s USB uređaja, pokreće se i ažurira se EEPROM, inače se ponovno pokušava

Druga faza *bootloadera*:

- inicijaliziraju se taktovi i SDRAM
- čita se EEPROM konfiguracijska datoteka
- čita se *PM_RSTS* registar kako bi se ustanovilo traži li se HALT:
	- provjeravaju se *POWER_OFF_ON_HALT* i *WAKE_ON_GPIO* u konfiguracijskoj datoteci:
		- ako je *POWER_OFF_ON_HALT* u 1 i *WAKE_ON_GPIO* u 0 iskorištava se PMIC (*eng. Power Management Integrated Circuit*) kako bi se isključio sustav
		- ako je *WAKE_ON_GPIO* u 1 omogućuje se prekid na padajući brid na GPIO 3 koji će probuditi sustav (GPIO 3 je u stanju visoko i ako se povuče u stanje nisko događa se prekid)
		- sustav ide u spavanje
- čita se *BOOT_ORDER* parametar iz konfiguracijske datoteke, vrijednosti u polju biti:, *STOP*, *SD CARD*, *NETWORK*, *USB-MSD*, *NVME*, ...
	- *RESTART* - skače natrag na prvo polje u *BOOT_ORDER* parametru
	- *STOP* - ispisuje da *start.elf* nije pronađen, čeka zauvijek
	- *SD CARD*
		- učitava *firmware* s SD kartice
		- ako je uspješno učitan pokreće ga, ako nije nastavlja dalje po polju
	- *NETWORK*
		- iskorištava DHCP da bi dobio IP adresu, učitava *firmware* s TFTP poslužitelja
		- ako je uspješno učitan pokreće ga, ako nije nastavlja dalje po polju
	- *USB-MSD*
		- po detektiranim USB uređajima:
			- učitava *firmware* s USB-a
			- ako je uspješno učitan pokreće ga, ako nije nastavlja dalje sljedećem USB uređaju
		- ako nije pronađen *firmware* na ni jednom USB uređaju nastavlja dalje po polju

Nakon izvršavanja *firmwarea* te učitavanje Linux jezgre i njenih parametara, VideoCore 6 budi ARM procesor i prepušta mu kontrolu.

Trenutačna konfiguracija koju koristi *bootloader* se može vidjeti naredbom:

```
sudo rpi-eeprom-config
```

Uređivanje konfiguracije koju će koristiti *bootloader* pri sljedećem ponovnom pokretanju može se napraviti naredbom:

```
sudo -E rpi-eeprom-config --edit
```

Stvorit će se datoteke oblika *pieeprom.\** u */boot/firmware* direktoriju koje će sadržavati novu konfiguraciju koja će se pisati u EEPROM. Nova konfiguracija će se učitati u EEPROM u drugoj boot fazi, nakon čega će se EEPROM ponovno pokrenuti odnosno ponovit će se druga boot faza.

Ako se želi spriječiti ovo, može se izvršiti naredba:

```
sudo rpi-eeprom-update -r
```

## Sadržaj firmware direktorija i boot proces

Tijekom prve i druge faze boot procesa, Raspberry Pi mikroračunalo pristupa FAT32 formatiranoj particiji na SD kartici (i drugim pohranama). Shema particioniranja pohrana mora biti MBR (*eng. Master Boot Record*). Ova particija se u Raspberry Pi OS-u montira na lokaciji */boot/firmware* i sadrži [sljedeće datoteke](https://www.raspberrypi.com/documentation/computers/configuration.html#boot-folder-contents):

- *bootcode.bin*
	- *bootloader* za starija Raspberry Pi mikroračunala, Raspberry Pi 4B ne koristi ovaj *bootloader* već SPI flash EEPROM
- *start\*.elf*
	- osnovni *firmware* kojeg izvršava VideoCore grafički procesor
- *fixup\*.dat*
	- linker datoteke koje odgovaraju svakoj *start\*.elf* datoteci
- *\*.dtb*
	- sadrži hardverske definicije za određene modele Raspberry Pi mikroračunala, predstavlja stablo uređaja u strojnom kodu
	- učitava ju *firmware*
	- za Raspberry Pi 4B se koristi *bcm2711-rpi-4-b.dtb*
- *config.txt*
	- parametri za osnovni *firmware*
	- sadrži informacije kako konfigurirati Raspberry Pi, primjerice brzinu takta, ukupnu memoriju, rezoluciju, hardverske opcije
	- nešto slično konfiguriranju postavki u BIOS-u
	- VideoCore grafički procesor ju čita i tako konfigurira sustav prije inicijalizacije ARM procesora
- *issue.txt*
	- sadrži informacije o datumu i *commit ID-u* Raspberry Pi OS distribucije
- *initramfs\**
	- privremeni datotečni sustav koji se učitava prije Linux jezgre u RAM
	- za Raspberry Pi 4B se koristi *initramfs8.img*
- *cmdline.txt*
	- sadrži parametre koje će se proslijediti Linux jezgri
- *\*.img*
	- Linux jezgra koja se učitava u RAM
	- za Raspberry Pi 4B se koristi *kernel8.img*

Također, sadrži i direktorij:

- *overlays*
	- sadrži *\*.dtbo* datoteke koje dinamično mogu prilagođavati hardverske definicije definirane u *\*.dtb* datotekama
	- opisuju dodatan hardver

### Stablo uređaja

[Stablo uređaja (*eng. Device Tree - DT*)](https://www.raspberrypi.com/documentation/computers/configuration.html#part2) je način na koji Raspberry Pi Linux jezgra i *firmware* opisuju hardver koji postoji na sustavu. *Firmware* *start\*.elf* učitava odgovarajuću *\*.dtb* datoteku gledajući reviziju pločice iz OTP memorije i vrši izmjene nad njom kako bi ju dalje prilagodio odgovarajućoj pločici čitajući korisnički definiranu datoteku *config.txt* i odgovarajuće *\*.dtbo* datoteke iz direktorija *overlays*. Konačno, *firmware* prosljeđuje pokazivač na konačnu stablo uređaja Linux jezgri kao parametar tijekom učitavanja.

Sustavi arhitekture x86 su informacije o hardveru sustava imali opisane u ACPI tablicama koje bi nudio firmware (UEFI, BIOS, ...). Ugradbeni sustavi bazirani na ARM, RISC-V ili sličnim arhitekturama bi koristili podatkovne strukture prevedene u *bootloaderu*. Ovo se pokazalo nepraktično za ugradbene uređaje pa se predstavilo rješenje sa stablom uređaja koji ne bi ovisio o *bootloaderu* ili o operacijskom sustavu.

U slučaju Linuxa, stablo uređaja omogućuje da se Linux jezgra ne treba prevoditi za svaki hardver zasebno kako bi se dobila slika o postojećim uređajima već se informacije o uređajima na hardveru prosljeđuje kao pripremljenu struktura podataka odnosno stablo uređaja. Stablo uređaja sadrži informacije o procesoru, memoriji i periferijama. Podatci su strukturirani kao hijerarhija čvorova. Čvor može opisivati funkcionalnosti hardverske komponente te može sadržavati još čvorova koji opisuju neke podfunkcionalnosti. Gledajući analogiju s datotečnim sustavom, čvorovi bi bili kao nekakvi direktoriji, a svojstva datoteke.

Izvorni kod specifičnog stabla uređaja se piše u tekstualnom formatu DTS (*eng. Device Tree Source*) i datoteka mu ima nastavak *.dts*. Sintaksa izvornog koda je slična jeziku C. Međutim, mnogi mikroupravljači i mikroračunala imaju neke zajedničke dijelove, primjerice isti sustav na čipu. Kako bi se smanjila veličina same *.dts* datoteke, neki hardverski dijelovi kao što je sustav na čipu imaju svoje standardizirane opise u *.dtsi* datotekama koje se mogu uključiti u *.dts* datoteku. Za datoteku *.dtsi* se kaže da sadrži opis bazne konfiguracije dok *.dts* datoteka koja uključuje takve *.dtsi* datoteke modificira te bazne konfiguracije, unoseći im proširenja (ili ograničenja).

Ove izvorne datoteke se prevode u binarnu uz prevoditelj DTC (*eng. Device Tree Compiler*).

Prevedene izvorne datoteke stvaraju binarnu datoteku s nastavkom *.dtb* koja se naziva *Device Tree Blob*. U Raspberry Pi kontekstu ovo se naziva još i bazno stablo uređaja. Međutim u direktoriju */boot/firmware/overlays* nalaze se posebne binarne datoteke s nastavkom *.dtbo* koje se još nazivaju *Device Tree Overlay*. Ove datoteke mogu dinamički izmijeniti binarnu datoteku baznog stabla uređaja (*.dtb*) proširujući ili izmjenjujući joj definicije uređaja definicije uređaja koji trenutačno postoje u stablu. Datoteka *.dtbo* se isto kao i datoteka *.dtb* prevodi iz datoteka *dts* i *dtsi*, ali s posebnim zastavicama.

U slučaju Raspberry Pi mikroračunala, o tome koje *overlay* datoteke (*.dtbo*) će modificirati bazno stablo (*.dtb*) će odlučiti *firmware* *start\*.elf* čitajući reviziju pločice iz OTP memorije. Ako korisnik želi ručno uključiti neki *overlay* ili omogućiti odnosno onemogućiti neku periferiju u baznom stablu, to može napraviti upisujući u datoteku */boot/firmware/config.txt* odgovarajuće parametre.

Kako je već navedeno, *firmware* *start\*.elf* kombinira *\*.dtb*, *\*.dtbo* i odgovarajuće parametre */boot/firmware/config.txt* datoteke kako bi stvorio konačno stablo uređaja u memoriji čiji pokazivač prosljeđuje Linux jezgri kao parametar.

Više informacija o */boot/firmware/config.txt* parametrima koje mogu utjecati na bazno stablo uređaja može se naći [ovdje](https://github.com/raspberrypi/linux/blob/rpi-4.1.y/arch/arm/boot/dts/overlays/README).

## Linux jezgra

[Linux jezgra Raspberry Pi OS-a](https://www.raspberrypi.com/documentation/computers/linux_kernel.html) za razliku od originalne Linux jezgre kasni s izbacivanjem nove inačice zbog dodatnog testiranja i optimizacije za Raspberry Pi hardver. Kada izađe nova inačica originalne jezgre, Raspberry Pi Foundation stvara novu granu *next* na [GitHub-u](https://github.com/raspberrypi/firmware/) gdje se isprobavaju nove značajke radi utvrđivanja njihove ispravnosti. Kada se utvrdi da je nova inačica jezgre spremna za Raspberry Pi, grana *next* se spaja s granom *master*.

Grana *master* sadrži najnoviji potvrđeni kod za Linux jezgre za Raspberry Pi. Kada jezgra prođe još dodatna testiranja, ta se jezgra kopira na granu *stable* što je označava najnovijom stabilnom inačicom.

Ažuriranjem Raspberry Pi OS-a s naredbom ```sudo apt full-upgrade```, Linux jezgra se automatski ažurira na najnoviju stabilnu inačicu. Ako se želi instalirati nestabilna inačica jezgre, potrebno je to napraviti ručno. Kod Raspberry Pi OS-a Linux jezgra se nalazi u */boot/firmware* direktoriju. U slučaju Raspberry Pi 4B mikroupravljača to je datoteka *kernel8.img*.

Moguće je [izgraditi prilagođenu jezgru](https://www.raspberrypi.com/documentation/computers/linux_kernel.html#building), uređujući joj razne značajke.

### Izgradnja prilagođene jezgre na Raspberry Pi OS-u za Raspberry Pi 4B mikroračunalo

Ovaj postupak gradi prilagođenu Linux jezgru na Raspberry Pi OS-u koji se pokreće na Raspberry Pi 4B mikroračunalu. Postupak može znatno potrajati jer se jezgra gradi koristeći resurse Raspberry Pi 4B mikroračunala koji su slabiji nego resursi prosječnog osobnog računala. Dakle, gradi se jezgra za ARM64 arhitekturu na ARM64 arhitekturi.

Prije izgradnje, potrebno je preuzeti potrebne pakete i alate za izgradnju:

```
sudo apt install git bc bison flex libssl-dev make libncurses5-dev
```

Zatim je potrebno premjestiti se u korisnikovom *home* direktoriju i preuzeti izvorni kod zadnje inačice Linux jezgre Raspberry Pi OS-a:

```
cd ~/
git clone --depth=1 https://github.com/raspberrypi/linux
```

Naredba dohvaća GitHub repozitorij bez povijesti *commitova*, ako se želi dohvatiti cijeli repozitorij s povijesti *commitova*, potrebno je ukloniti zastavicu ```--branch```. Veličina cijele povijesti je dosta velika pa je potrebno ustanoviti ima li pohrana na Raspberry Pi mikroračunalu dovoljno mjesta za cijeli repozitorij.

Izgradnja uobičajene konfiguracije za 64 bitnu Linux jezgru Raspberry Pi OS-a koji se pokreće na Raspberry Pi 4B mikroračunalu radi se sljedećim naredbama:

```
cd linux
KERNEL=kernel8
make bcm2711_defconfig
```

Naredba će spremiti konfiguraciju u *.config* datoteku. Daljnja konfiguracija jezgre koja će se također spremati u *.config* datoteku moguća je naredbom:

```
make menuconfig
```

Konačno, izgradnja 64 bitne jezgre i stabla uređaja GCC prevodiocem i DTC prevodiocem radi se naredbom:

```
make -j6 Image.gz modules dtbs
```

Parametar ```-j6``` označava broj procesa koji će paralelno raditi na procesu prevođenja. Optimalan broj je 1.5 puta broj jezgri procesora nad kojim se izvodi prevođenje. U slučaju Raspberry Pi 4B mikroračunala koristi se četverojezgreni ARM Cortex-A72 procesor.

Nakon izgradnje potrebno je instalirati nove jezgrene module u */lib/modules/[nova verzija jezgre]* naredbom:

```
sudo make -j6 modules_install
```

Sljedeće što je potrebno je stvoriti sigurnosnu kopiju stare jezgre u */boot/firmware* direktoriju, kopirati novu jezgru i bazno stablo (datoteka *\*.dtb*) u */boot/firmware* direktorij te kopirati *overlay* datoteke (datoteke *\*.dtbo*) i *README* tekstualnu datoteku u */boot/firmware/overlays* direktorij:

```
sudo cp /boot/firmware/$KERNEL.img /boot/firmware/$KERNEL-backup.img
sudo cp arch/arm64/boot/Image.gz /boot/firmware/$KERNEL.img
sudo cp arch/arm64/boot/dts/broadcom/*.dtb /boot/firmware/
sudo cp arch/arm64/boot/dts/overlays/*.dtb* /boot/firmware/overlays/
sudo cp arch/arm64/boot/dts/overlays/README /boot/firmware/overlays/
```

Za kraj, potrebno je ponovno pokrenuti Raspberry Pi 4B mikroračunalo naredbom:

```
sudo reboot
```

Nakon ponovnog pokretanja Raspberry Pi OS-a, nova jezgra bi trebala biti pokrenuta.

### Izgradnja jezgre na Arch Linuxu za Raspberry Pi OS-u

Ovaj postupak gradi prilagođenu Linux jezgru na Arch Linuxu koji se pokreće na osobnom računalu. Jezgra se gradi koristeći resurse osobnog računala koji su u prosjeku znatno brži nego resursi Raspberry Pi 4B mikroračunala. Dakle, gradi se jezgra za ARM64 arhitekturu na AMD64 arhitekturi.

Prije izgradnje, potrebno je preuzeti potrebne pakete i alate za izgradnju:

```
sudo pacman -Syu
sudo pacman -S git bc bison flex openssl make glibc ncurses aarch64-linux-gnu-gcc
```

Zatim je potrebno premjestiti se u korisnikovom *home* direktoriju i preuzeti izvorni kod zadnje inačice Linux jezgre Raspberry Pi OS-a:

```
cd ~/
git clone --depth=1 https://github.com/raspberrypi/linux
```

Izgradnja uobičajene konfiguracije za 64 bitnu Linux jezgru Raspberry Pi OS-a koji se pokreće na Raspberry Pi 4B mikroračunalu i sprema se u *.config* datoteku radi se sljedećim naredbama:

```
cd linux
KERNEL=kernel8
make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- bcm2711_defconfig
```

Naredba će spremiti konfiguraciju u *.config* datoteku. Daljnja konfiguracija jezgre koja će se također spremati u *.config* datoteku moguća je naredbom:

```
make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- menuconfig
```

Konačno, izgradnja 64 bitne jezgre i stabla uređaja unakrsnim GCC prevodiocem i DTC prevodiocem radi se naredbom:

```
make -j[broj procesa] ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- Image modules dtbs
```

Broj procesa koji će paralelno raditi na procesu prevođenja može biti broj jezgri AMD64 procesora puta 2 plus 1. Broj jezgri može se dobiti iščitavanjem izlaza naredbe ```lscpu```.

Nakon izgradnje potrebno je umetnuti SD karticu (ili neku drugu pohranu) koja na sebi ima instaliran Raspberry Pi OS i instalirati nove jezgrene module u */lib/modules/[nova verzija jezgre]* na *root* particiju SD kartice. Dakle, potrebno je montirati *root* particiju na lokaciju */mnt/rpi-root* i instalirati module na */mnt/rpi-root/lib/modules/[nova verzija jezgre]*:

```
sudo umount [datoteka uređaja koja predstavlja SD karticu]*
sudo mkdir /mnt/rpi-root
sudo mount [datoteka uređaja koja predstavlja SD karticu]2 /mnt/rpi-root
sudo env PATH=$PATH make -j[broj procesa] ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- INSTALL_MOD_PATH=/mnt/rpi-root modules_install
```

Sljedeće što je potrebno je stvoriti sigurnosnu kopiju stare jezgre u */boot/firmware* direktoriju, kopirati novu jezgru i bazno stablo (datoteka *\*.dtb*) u */boot/firmware* direktorij te kopirati *overlay* datoteke (datoteke *\*.dtbo*) i *README* tekstualnu datoteku u */boot/firmware/overlays* direktorij. Dakle, potrebno je montirati *firmware* particiju na lokaciju */mnt/rpi-root/boot/firmware* te napraviti stvoriti sigurnosnu kopiju stare jezgre u */mnt/rpi-root/boot/firmware/* direktoriju, kopirati novu jezgru i bazno stablo u */mnt/rpi-root/boot/firmware/* direktorij te kopirati *overlay* datoteke i *README* tekstualnu datoteku u */mnt/rpi-root/boot/firmware/overlays/* direktorij:

```
sudo mount [datoteka uređaja koja predstavlja SD karticu]1 /mnt/rpi-root/boot/firmware
sudo cp /mnt/rpi-root/boot/firmware/$KERNEL.img /mnt/rpi-root/boot/firmware/$KERNEL-backup.img
sudo cp arch/arm64/boot/Image /mnt/rpi-root/boot/firmware/$KERNEL.img
sudo cp arch/arm64/boot/dts/broadcom/*.dtb /mnt/rpi-root/boot/firmware/
sudo cp arch/arm64/boot/dts/overlays/*.dtb* /mnt/rpi-root/boot/firmware/overlays/
sudo cp arch/arm64/boot/dts/overlays/README /mnt/rpi-root/boot/firmware/overlays/
```

Prije uklanjanja SD kartice (ili neke druge pohrane) iz osobnog računala, potrebno je odmontirati sve dosad montirane particije SD kartice:

```
sudo umount /mnt/rpi-root/boot/firmware/
sudo umount /mnt/rpi-root/
```

Nakon umetanja SD kartice u Raspberry Pi 4B mikroračunalo i pokretanja Raspberry Pi OS-a, nova jezgra bi trebala biti pokrenuta.
