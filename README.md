# RASPBERRY PI

[Raspberry Pi](https://www.raspberrypi.com/) je serija mikroračunala koju je razvila zaklada Raspberry Pi Foundation. Neki od poznatijih modela su: Raspberry Pi 5, Raspberry Pi 4 Model B, Raspberry Pi 3 Model B, Raspberry Pi Zero i Raspberry Pi Pico. Međusobno se razlikuju po sustavu na čipu kojeg koriste (*eng. System on Chip*), veličini pločice, sučeljima za komunikaciju, potrošnji energije, količini GPIO (*eng. General Purpose Input Output*) pinova, cijeni i slično.

## Raspberry Pi 4B

[Raspberry Pi 4B](https://datasheets.raspberrypi.com/rpi4/raspberry-pi-4-product-brief.pdf) je četvrta generacija Raspberry Pi modela. Značajno je poboljšana naspram svojih prethodnika. Veća procesorska snaga uz sličnu potrošnju kao i prethodnik Raspberry Pi 3+, učinio ga je i povoljnim za korištenje kao desktop računalo. [Specifikacije](https://datasheets.raspberrypi.com/rpi4/raspberry-pi-4-datasheet.pdf) su mu sljedeće:

- sustav na čipu [Broadcom BCM2711](https://www.raspberrypi.com/documentation/computers/processors.html#bcm2711):
	- četverojezgreni ARM Cortex-A72 procesor:
		- 64 bitna ARM v8 arhitektura
		- takt 1.5 GHz
	- priručna memorija:
		- L1: 32 KiB podatkovna L1 i 48 KiB instrukcijska po jezgri
		- L2: zajednička 1 MiB
	- grafički procesor VideoCore 6 3D Graphics:
		- 32 bitan
		- takt 500 MHz
		- hardverska podrška:
			- H.265 kodek za dekompresiju 4K60 video sadržaja
			- H.264 kodek za kompresiju 1080p30 i dekompresiju 1080p60 video sadržaja
			- OpenGL ES 3.0 API
	- ulazno-izlazna sučelja:
		- Ethernet
		- 2 DSI (1 dostupan na pločici)
		- 2 CSI (1 dostupan na pločici)
		- 6 I2C (multipleksiran s UART-om)
		- 6 UART (multipleksiran s I2C-om)
		- 6 SPI (5 dostupni na pločici)
		- 2 HDMI
		- Composite video
		- USB
	- podržava maksimalnu vanjsku radnu memoriju veličine 8 GiB
- radna memorija 8 GiB, 4 GiB, 2 GiB ili 1 GiB ovisno o modelu
	- LPDDR4 RAM 
	- on-die ECC
- bežična komunikacija:
	- Wi-Fi 2.4 GHz i 5 GHz, IEEE 802.11 b/g/n/ac
	- Bluetooth 5.0, BLE
- žičana komunikacija i priključci:
	- Ethernet 1 GiB
	- DSI
	- CSI
	- 2 micro HDMI
	- A/V (Audio Video)
	- 2 USB A 3.0
	- 2 USB A 2.0
	- 40 pinski GPIO
	- micro SD
- napajanje minimalno 5 V i 3 A
	- USB C priključak
	- preko GPIO pinova
	- preko Etherneta (*eng Power over Ethernet*), potreban dodatan hardverski modul

### Raspberry Pi OS

[Raspberry Pi OS](https://www.raspberrypi.com/documentation/computers/os.html) je operacijski sustav koji se temelji na Debian Linux distribuciji te je optimiziran za Raspberry Pi mikroračunala. Ovo znači da Raspberry Pi OS prati cikluse ažuriranja kao i Debian odnosno nova inačica Raspberry Pi OS-a izlazi otprilike svake dvije godine uz malo kašnjenje jer se mora testirati kompatibilnost paketa s ARM procesorima. Postoje 64 bitne i 32 bitne inačica Raspberry Pi OS-a te mogu doći s grafičkim sučeljem ili bez (inačica *Lite*).

### Raspberry Pi OS Lite distribucija

Raspberry Pi OS Lite je manje zahtjevna inačica Raspberry Pi OS-a koja ne dolazi s grafičkim sučeljem i dodatnim paketima. Postoji 64 bitna i 32 bitna inačica ovog operacijskog sustava. Za Raspberry Pi 4B preporučuje se preuzeti 64 bitnu sliku. Kao primjer, uzet će se slika **raspios_lite_arm64-2024-10-28**. Ako se žele odabrati neke druge *Lite* inačice, to se može napraviti [ovdje](https://downloads.raspberrypi.com/raspios_lite_arm64/images/). Preporučuje se napraviti direktorij *rpi-images* u korisničkom *home* direktoriju, premjestiti se u njega i pokrenuti preuzimanje sljedećim naredbama u terminalu:

```
mkdir ~/rpi-images && cd ~/rpi-images && \
curl https://downloads.raspberrypi.com/raspios_lite_arm64/images/raspios_lite_arm64-2024-10-28/2024-10-22-raspios-bookworm-arm64-lite.img.xz -o 2024-10-22-raspios-bookworm-arm64-lite.img.xz
```

Slika je trenutačno u XZ kompresiranom formatu. Potrebno ju je dekompresirati naredbom:

```
xz -d 2024-10-22-raspios-bookworm-arm64-lite.img.xz
```

Zatim je potrebno priključiti USB SD čitač kartice s micro SD utorom u računalo i prepisati ju s prethodnom otpakiranom slikom. Ali za početak je potrebno saznati datoteku uređaja koja predstavlja priključenu micro SD karticu:

```
lsblk
```

Najčešće oblika */dev/sda*, */dev/sdb*, */dev/sdc* i slično. Kao pomoć pogledati veličinu koju ima predstavljena datoteka uređaja. Također, prije pisanja je potrebno demontirati particije SD kartice ako su montirane naredbom:

```
sudo umount [datoteka uređaja koja predstavlja SD karticu]*
```

Konačno, sada je moguće pisati sliku na SD karticu naredbom:

```
sudo dd if=2024-10-22-raspios-bookworm-arm64-lite.img of=[datoteka uređaja koja predstavlja SD karticu] bs=4M conv=fsync status=progress
```

### Konfiguracija bez monitora i tipkovnice

Ako se želi Raspberry Pi OS konfigurirati bez monitora i tipkovnice spojenih na njega [(*eng. headless installation*)](https://www.raspberrypi.com/documentation/computers/configuration.html#setting-up-a-headless-raspberry-pi) s udaljenim pristupom protokolom SSH, potrebno je za početak montirati *root* i *firmware* particiju slike s SD kartice na računalo. Onda je potrebno stvoriti točku montiranja *rpi-root* u */mnt* direktoriju i montirati drugu pa prvu particiju na */mnt/rpi-root* i */mnt/rpi-root/boot/firmware*:

```
sudo mkdir /mnt/rpi-root
sudo mount [datoteka uređaja koja predstavlja SD karticu]2 /mnt/rpi-root
sudo mount [datoteka uređaja koja predstavlja SD karticu]1 /mnt/rpi-root/boot/firmware
```

Zatim je potrebno dodati [korisničko ime i SHA-512 sažetu zaporku](https://www.raspberrypi.com/news/raspberry-pi-bullseye-update-april-2022/) u obliku [korisničko ime]:[SHA512 sažeta zaporka] u datoteku *userconf.txt* u korijenu *firmware* particije odnosno na lokaciji */mnt/rpi-root/boot/firmware*:

```
echo "[korisničko ime]:$(openssl passwd -6)" | sudo tee /mnt/rpi-root/boot/firmware/userconf.txt > /dev/null
```

Ova datoteka definira novog običnog korisnika koji će se stvoriti i njegovu zaporku, korisnik ima pravo izvršavati privilegirane naredbe s prefiksom *sudo*. Korisnik *root* nema definiranu zaporku. To se može napraviti iako nije u ovom slučaju potrebno.

Nakon postavljanja korisnika potrebno je stvoriti datoteku *ssh* u korijenu *firmware* particije odnosno na lokaciji */mnt/rpi-root/boot/firmware* koja će omogućiti pokretanje SSH servisa *ssh*.

```
sudo touch /mnt/rpi-root/boot/firmware/ssh
```

Za upravljanje mrežnim postavkama, Raspberry Pi OS koristi paket alata [*NetworkManager*](https://networkmanager.dev/). Alati upravljaju Ethernet sučeljima, Wi-Fi sučeljima i sučeljima mobilne veze. Kako bi alati funkcionirali kako treba u pozadini servis *NetworkManager* mora biti pokrenut.

U slučaju želje automatskog spajanja Raspberry Pi mikroračunala na mrežu, potrebno je definirati [profil konekcije](https://docs.redhat.com/en/documentation/red_hat_enterprise_linux/8/html/configuring_and_managing_networking/assembly_networkmanager-connection-profiles-in-keyfile-format_configuring-and-managing-networking#assembly_networkmanager-connection-profiles-in-keyfile-format_configuring-and-managing-networking) u direktoriju ```/etc/NetworkManager/system-connections/```. Najlakše je koristiti već gotovu datoteku profila konekcije koja se koristi na računalu, uz male preinake. Naredbom ```nmcli connection show``` mogu se izlistati svi profili. Ime profila koji se koristi za spajanje na mrežu ima svoju datoteku koja definira postavke mreže na koju se spaja. Ta se datoteka profila nalazi u */etc/NetworkManager/system-connections/* oblika ```[ime profila konekcije].nmconnection```. Moguće je izvesti kopiranje datoteke */etc/NetworkManager/system-connections/[ime profila konekcije].nmconnection* s računala na */mnt/rpi-root/etc/NetworkManager/system-connections/[ime profila konekcije].nmconnection* na Raspberry Pi mikroračunalo pa ju onda izmijeniti uređivačem teksta, primjerice *nano* uređivačem teksta. Također, u novoj datoteci se predlaže postavljanje novog UUID-a uz naredbu ```uuidgen```. Primjerice, datoteka */mnt/rpi-root/etc/NetworkManager/system-connections/[ime profila konekcije].nmconnection* može izgledati ovako:

```
[connection]
id=<SSID mreže>
uuid=<novi UUID>
type=wifi
interface-name=wlan0

[wifi]
mode=infrastructure
ssid=<SSID mreže>

[wifi-security]
auth-alg=open
key-mgmt=wpa-psk
psk=<zaporka mreže>

[ipv4]
method=auto

[ipv6]
addr-gen-mode=default
method=auto

[proxy]
```

Stavke ```auth-alg=open``` i ```key-mgmt=wpa-psk``` se koriste za većinu kućnih mreža. Prva stavka označava da mreža ne koristi nikakvu autentifikaciju tijekom prve prijave dok druga stavka govori da se koristi javni ključ (PSK) odnosno zaporka za provjeravanje autentičnosti se iz njega stvaraju kriptografski ključevi kojima se kriptiraju podatci tijekom prijenosa. Više informacija o drugim opcijama se može naći [ovdje](https://networkmanager.dev/docs/api/latest/settings-802-11-wireless-security.html).

**Obično se datoteke u /etc/NetworkManager/system-connections ne stvaraju ručno već uz alat nmcli jer svaka mala greška može imati nepredvidivo ponašanje.**

Nakon kopiranja ili stvaranja datoteke na Raspberry Pi mikroračunalu, potrebno joj je promijeniti ovlasti:

```
sudo chmod 600 /mnt/rpi-root/etc/NetworkManager/system-connections/[ime profila konekcije].nmconnection
```

Konačno, odmontiranje *firmware* i *root* particije, brisanje *rpi-root* direktorija te izbacivanje USB SD čitača radi se naredbom:

```
sudo umount /mnt/rpi-root/boot/firmware
sudo umount /mnt/rpi-root
sudo rmdir /mnt/rpi-root
sudo eject [datoteka uređaja koja predstavlja SD karticu]
```

Za kraj, potrebno je SD karticu umetnuti u Raspberry Pi, po mogućnosti spojiti ga na mrežu Ethernetom iako mu je već zadan SSID i zaporka mreže te priključiti mu napajanje. Raspberry Pi OS će sam proširiti drugu particiju, particiju *root*, sve do kraja slobodnog prostora SD kartice.

Datoteke *userconf.txt* i *ssh* će se automatski izbrisati nakon uspješnog prvog pokretanja.

U repozitoriju je dostupna skripta ```headless_config.sh``` koja izvršava sve naredbe navedene.

#### Uređivanje slike prije pisanja na SD karticu

Ako se želi urediti slika prije pisanja na SD karticu, to je moguće napraviti tako da se datoteka poveže s *loop* pseudouređajem. Dakle, potrebno je izvršiti naredbu:

```
sudo losetup -f --show -P 2024-10-22-raspios-bookworm-arm64-lite.img
```

Datoteci je sad moguće pristupiti kao blok uređaju. Ispis prethodne naredbe daje datoteku pseudouređaja koji predstavlja sliku kao blok uređaj. Ako je ispis bio primjerice */dev/loop0* onda su mu mu particije onda mu je *firmware* particija */dev/loop0p1*, a *root* particija */dev/loop0p2*. Kao i u primjeru s SD karticom, particije je potrebno montirati na */mnt/rpi-root* i */mnt/rpi-root/boot/firmware*:

```
sudo mkdir /mnt/rpi-root
sudo mount /dev/loop0p2 /mnt/rpi-root
sudo mount /dev/loop0p1 /mnt/rpi-root/boot/firmware
```

Nakon ovoga postupak za postavljanje korisničkog imena, SSH pristupa i pristupa mreži je isti kao i sa slučajem SD kartice. Nakon dovršenog postupka, particije se odmontiraju naredbama:

```
sudo umount /mnt/rpi-root/boot/firmware
sudo umount /mnt/rpi-root
sudo rmdir /mnt/rpi-root
```

Za kraj je potrebno odvojiti datoteku od pseudouređaja, u ovom slučaju za */dev/loop0* naredba je:

```
sudo losetup -d /dev/loop0
```

Slika se sad najnormalnije može pisati na SD karticu naredbom:

```
sudo dd if=2024-10-22-raspios-bookworm-arm64-lite.img of=[datoteka uređaja koja predstavlja SD karticu] bs=4M conv=fsync status=progress
```

### Generiranje SSH ključeva za udaljeni pristup

Kako bi mogli udaljeno pristupiti Raspberry Pi mikroračunalu koje pokreće SSH servis, potrebno je ako već nije generirati jedan par SSH ključeva. Generiranje jednog para asimetričnih ključeva po krivulji ED25519 može se napraviti naredbom:

```
ssh-keygen -t ed25519
```

Kopiranje javnog ključa na Raspberry Pi radi se naredbom:

```
ssh-copy-id [korisničko ime na Raspberry Pi mikroračunalu]@[IP adresa Raspberry Pi mikroračunala]
```

Autorizacija i otvaranje sesije SSH klijenta (računalo) na SSH poslužitelj (Raspberry Pi mikroračunalo) radi se naredbom:

```
ssh [korisničko ime na Raspberry Pi mikroračunalu]@[IP adresa Raspberry Pi mikroračunala]
```

Izlaz is sesije moguće je naredbama ```exit``` ili ```logout```.

### Instalacija, deinstalacija i ažuriranje softvera

Raspberry Pi OS koristi [APT](https://en.wikipedia.org/wiki/APT_(software)) upravitelj softverskih paketa. Datoteka ```/etc/apt/sources.list``` sadrži popis repozitorija koji APT pregledava u slučaju željom za instalacijom ili ažuriranjem softvera. Kako bi se se ažurirao lokalni popis paketa uz oznaka njihovih verzija potrebno je izvršiti naredbu:

```
sudo apt update
```

Ako se već instalirani paketi žele ažurirati na najnoviju verziju, potrebno je izvršiti naredbu:

```
sudo apt full-upgrade
```

Prije izvršenja ove naredbe, dobro je pogledati količinu slobodnog prostora koji postoji na spremnicima naredbom ```df -h```.

Pretraga lokalnog popisa paketa radi se naredbom:

```
sudo apt-cache [ključna riječ]
```

Detaljnije informacije o paketu se mogu dobiti naredbom:

```
sudo apt show [ime paketa]
```

Instalacija paketa se može napraviti naredbom:

```
sudo apt install [ime paketa]
```

Uklanjanje paketa se može napraviti naredbom:

```
sudo apt remove [ime paketa]
```

Uklanjanje paketa uključujući i njegove konfiguracijske datoteke se može napraviti naredbom:

```
sudo apt purge [ime paketa]
```

Uklanjanje nekorištenih zavisnosti radi se naredbom:

```
sudo apt clean
```

### Nadogradnja firmwarea

Nadogradnja firmwarea se radi samo za **eksperimentalne svrhe** jer se povlači najnovija inačica firmwarea koja ima moguće nestabilnosti. Preuzimaju se prototipne inačice Linux jezgre, stabla uređaja, modula i VideoCore firmwarea. **Dakle, ovaj proces se ne koristi za normalni proces ažuriranja sustava.** Naredba za izvršenje nadogradnje firmwarea radi se naredbom:

```
sudo rpi-update
```

U ovom slučaju potrebno je ponovno pokrenuti Raspberry Pi naredbom: ```sudo reboot```.

### Korisni alati

Neki korisni alati za nadzor sustava bi bili:

- *kmsprint* -
	- detalji o spojenim monitorima na Raspberry Pi
- *vclog* -
	- dohvat zapisa VideoCore grafičkog procesora
- *vcgencmd*
	- informacije o VideoCore grafičkom procesoru
	- za sve opcije potrebno upisati ```vcgencmd commands```
		- primjerice, opcija ```measure_temp``` vraća temperaturu sustava na čipu
			- primjer naredbe: ```vcgencmd measure_temp```
		- opcija ```measure_clock``` vraća takt pojedine komponente u sustavu na čipu, neke od njih mogu biti:
			- ```arm``` - ARM jezgra
			- ```core``` - VideoCore jezgra
			- ```h264``` - H.264 akcelerator
			- ```uart``` - UART
			- ...
			- primjer naredbe: ```vcgencmd measure_clock arm```

### Opća konfiguracija sustava

Za opću konfiguraciju Raspberry Pi OS-a, potrebno je izvršiti naredbu:

```
sudo raspi-config
```

Moguće je pokretati neinteraktivni način rada ove naredbe: ```raspi-config nonint [naredba] [argumenti]```. Opća konfiguracija nudi mnogo opcija koje se mogu naći [ovdje](https://www.raspberrypi.com/documentation/computers/configuration.html#raspi-config). Primjerice, u neinteraktivnom načinu, postavljanje imena Raspberry Pi mikroračunala po želji i vremensku zonu na "Europe/Zagreb":

```
sudo raspi-config nonint do_hostname [ime po želji]
sudo raspi-config nonint do_change_locale en_US.UTF-8 UTF-8
sudo raspi-config nonint do_change_timezone Europe/Zagreb
```

### Daljnje upute

Preporučuje se slijediti sljedeće linkove:

* informacije o *boot* procesu i jezgri: [ovdje](boot-process-and-kernel)
* konfiguriranje mrežnih postavki: [ovdje](networking)
* upravljanje GPIO pinovima: [ovdje](gpio)
