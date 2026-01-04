# RASPBERRY PI 4B

[Raspberry Pi](https://www.raspberrypi.com/) je serija mikroračunala koju je razvila zaklada Raspberry Pi Foundation u kolaboraciji s Broadcom korporacijom. Radi svoje fleksibilnosti, male potrošnje i male potrošnje ima široku upotrebu u edukaciji, istraživanju i u industriji.

[Raspberry Pi 4B](https://datasheets.raspberrypi.com/rpi4/raspberry-pi-4-product-brief.pdf) je četvrta generacija Raspberry Pi modela. Slične potrošnje kao i njegov prethodnik, Raspberry Pi 3+, ali veće procesorske snage, učinio ga je i povoljnim za korištenje kao desktop računalo.

[Specifikacije](https://datasheets.raspberrypi.com/rpi4/raspberry-pi-4-datasheet.pdf) su mu sljedeće:

- sustav na čipu (SoC): [Broadcom BCM2711](https://www.raspberrypi.com/documentation/computers/processors.html#bcm2711):
	- procesor (CPU): četverojezgreni ARM Cortex-A72 procesor (ARMv8, 64 bitni, 1.8 GHz)
	- priručna memorija:
		- L1: 32 KiB podatkovna + 48 KiB instrukcijska po jezgri
		- L2: 1 MiB
	- grafički procesor: VideoCore VI (32 bitni, 500 MHz)
		- hardverska podrška za:
			- H.265 kodek za dekompresiju 4Kp60 video sadržaja
			- H.264 kodek za kompresiju 1080p30 i dekompresiju 1080p60 video sadržaja
			- OpenGL ES 3.0 podrška
	- ulazno-izlazna sučelja:
		- 6 UART portova
		- 6 I2C portova
		- 6 SPI portova
		- PCIe 2.0 x1 sabirnica na kojoj je spojen PCIe most
		- 58 GPIO portova
		- Gigabit Ethernet kontroler port (MAC sloj)
		- 2 DSI portova
		- 2 CSI portova
		- 2 HDMI 2.0 portova
		- USB 2.0 OTG kontroler port
		- SD/eMMC port
- radna memorija (RAM): Micron LPDDR4-2400 SDRAM
	- 1 GiB, 2 GiB, 4 GiB ili 8 GiB, ovisno o varijanti pločice
- bežična komunikacija: Broadcom BCM43455
	- Wi-Fi radio (802.11b/g/n/ac) 2.4 GHz i 5 GHz
	- Bluetooth 5.0, BLE
- portovi dostupni na PCB pločici:
	- 40 pinski priključak od kojih su 28 GPIO priključci koji mogu imati sljedeće funkcije:
		- UART (2 porta)
		- I2C (2 porta)
		- SPI (2 porta)
		- PWM (4 kanala, 2 po kontroleru)
		- Display Parallel Interface (DPI)
		- JTAG
		- GPCLK
	- USB 3.0 portovi (2 USB A porta)
		- spojeni na VL805 USB 3.0 kontroler koji je spojen na PCIe most u BCM2711-u
	- USB 2.0 portovi (2 USB A porta)
		- spojeni na USB hub koji je spojen na USB 2.0 OTG kontroler u BCM2711-u
		- USB 2.0 OTG je zadano u host načinu rada, moguće ga je postaviti u načinu rada periferije (u tom slučaju se koristi USB C port koji je inače za napajanje)
	- Ethernet port
		- spojen na Ethernet kontroler (PHY sloj) koji je spojen na Ethernet kontroler (MAC sloj) u BCM2711-u
	- 1 DSI port
	- 1 CSI port
	- 2 HDMI porta (mikro HDMI priključci)
	- 1 SDIO port (utor za micro SD karticu)
- napajanje:
	- USB C utor za napajanje
		- 5 V, 3 A
	- Power over Ethernet (PoE)
- dodatne informacije:
	- inicijalni bootloader se nalazi u ROM-u
	- postavke za bootloader i neke periferije se nalaze u EEPROM-u

## Raspberry Pi OS

[Raspberry Pi OS](https://www.raspberrypi.com/documentation/computers/os.html) je Debian Linux distribucija koja je optimizirana za Raspberry Pi mikroračunala. Prati cikluse ažuriranja kao i Debian što znači da nova inačica Raspberry Pi OS-a izlazi otprilike svake dvije godine uz malo kašnjenje jer se mora testirati kompatibilnost paketa s ARM procesorima. Postoje 64 bitne i 32 bitne inačice koje mogu doći s grafičkim sučeljem ili bez (inačica *Lite*).

### Preuzimanje slike Lite inačice

Za instalaciju preporučeno je koristiti SD karticu od barem 16 GiB.

Raspberry Pi OS Lite 64 bitna inačica se može preuzeti [ovdje](https://downloads.raspberrypi.com/raspios_lite_arm64/images/). Slika, odnosno ```.img``` datoteka sadrži cijelu sliku diska (shemu particioniranja - Master Boot Record (MBR), particije, datotečne sustave, ...) i nju je potrebno klonirati na SD karticu. Nakon što se slika klonira na SD karticu i kartica se umetne te se pokrene Raspberry Pi, prva inicijalizacija distribucije će povećati particije na stvarnu veličinu SD kartice.

Kao primjer, naredba za preuzimanje slike ```2024-10-22-raspios-bookworm-arm64-lite.img.xz``` u direktorij ```rpi-images``` u korisnikovom *home* direktoriju je:

```
mkdir ~/rpi-images && cd ~/rpi-images && \
curl https://downloads.raspberrypi.com/raspios_lite_arm64/images/raspios_lite_arm64-2024-10-28/2024-10-22-raspios-bookworm-arm64-lite.img.xz -o 2024-10-22-raspios-bookworm-arm64-lite.img.xz
```

Preuzeta slika bit će kompresirana pa ju je prije kloniranja na SD karticu potrebno dekompresirati:

```
xz -d 2024-10-22-raspios-bookworm-arm64-lite.img.xz
```

### Uređivanje preuzete slike mapiranjem na pseudouređaj

Ako se želi urediti slika prije pisanja na SD karticu, primjerice, želi joj se postaviti inicijalnog korisnika unaprijed, to se može napraviti tako što se slika mapira na *loop* pseudouređaj. U tom slučaju se datoteci ```.img``` više neće pristupati kao običnoj datoteci već kao disku odnosno blok uređaju. Povezivanje datoteke s *loop* pseudouređajem može se napraviti naredbom:

```
sudo losetup -f --show -P 2024-10-22-raspios-bookworm-arm64-lite.img
```

Opcija ```-f``` će pronaći prvi slobodni pseudouređaj za mapiranje datoteke, opcija ```--show``` će prikazati koji pseudouređaj je pronađen, a opcija ```-P``` će pretražiti postoje li particije na toj mapiranoj datoteci. Ako se izvrši ```lsblk``` uočit će se novi blok uređaj i particije na tom uređaju. Ako je primjerice, dodijeljeni uređaj bio ```/dev/loop0```, particije će mu biti vjerojatno biti ```/dev/loop0p1``` i ```/dev/loop0p2``` za mapiranu Raspberry Pi OS Lite sliku.

### Montiranje datotečnih sustava

Datotečni sustavi particija ```/dev/loop0p1``` i ```/dev/loop0p2``` (u slučaju da je dodijeljeni pseudouređaj bio ```/dev/loop0```) mogu se montirati naredbama:

```
sudo mkdir /mnt/rpi-root
sudo mount /dev/loop0p2 /mnt/rpi-root
sudo mount /dev/loop0p1 /mnt/rpi-root/boot/firmware
```

Prva particija ```/dev/loop0p1``` je **boot** particija i ona sadrži *bootloader*, Linux jezgru, parametre za Linux jezgru, *initramfs*, datoteke stabla uređaja i slično. Druga particija ```/dev/loop0p2``` je **root** particija i ona sadrži sve ostale datoteke sustava, aplikacije i korisničke podatke.

### Inicijalizacija bez monitora i tipkovnice

Prilikom inicijalizacije, Raspberry Pi OS će uz povećanje particija, zahtijevati od korisnika da upiše inicijalno korisničko ime i zaporku. Ovo može predstavljati problem ako monitor ili tipkovnica nisu dostupni. Međutim i za to postoji rješenje.

#### Postavljanje inicijalnog korisničkog imena i zaporke

Kako bi se unaprijed postavilo korisničko ime i zaporku, potrebno je u datoteku ```userconf.txt``` na **boot** particiji dodati korisničko ime i SHA-512 sažetu zaporku. To se može napravit naredbom:

```
HASHED_PASSWORD=$(openssl passwd -6)
echo "<korisničko ime>:$HASHED_PASSWORD" | sudo tee /mnt/rpi-root/boot/firmware/userconf.txt > /dev/null
unset HASHED_PASSWORD
```

#### Postavljanje imena mikroračunala

Za olakšano korištenje sljedećih naredbi ime računala može se postaviti u varijablu okruženja naredbom:

```
RPI_HOSTNAME="<ime mikroračunala>"
```

Inicijalno ime mikroračunala (*eng. hostname*, datoteka `/etc/hostname`) može se postaviti naredbom:

```
echo "$RPI_HOSTNAME" | sudo tee /mnt/rpi-root/etc/hostname
```

Također, potrebno je postaviti i ispravno razlučivanje ime mikroračunala (datoteka `/etc/hosts`) naredbom:

```
sudo tee /mnt/rpi-root/etc/hosts > /dev/null << EOF
127.0.0.1	localhost
::1		localhost ip6-localhost ip6-loopback
ff02::1		ip6-allnodes
ff02::2		ip6-allrouters

127.0.1.1	$RPI_HOSTNAME
EOF
```

Varijabla okruženja može se obrisati naredbom:

```
unset RPI_HOSTNAME
```

#### Omogućivanje SSH servisa

Ako se želi inicijalno omogućiti SSH servis to se može napraviti stvaranjem prazne datoteke *ssh* na **boot** particiji naredbom:

```
sudo touch /mnt/rpi-root/boot/firmware/ssh
```

#### Povezivanje na Wi-Fi mrežu

U slučaju da ne postoji mogućnost povezivanja na mrežu preko Ethernet sučelja kako bi se primjerice pristupilo mikroračunalu preko SSH servisa, moguće je stvoriti inicijalni profil Wi-Fi konekcije. Raspberry Pi OS koristi servis [*NetworkManager*](https://networkmanager.dev/). Servis upravlja Ethernet sučeljima, Wi-Fi sučeljima i sučeljima mobilne veze. Kod Raspberry Pi OS-a, ovaj servis bi trebao biti zadano omogućen što se može provjeriti naredbom:

```
sudo ls -la /mnt/rpi-root/etc/systemd/system/multi-user.target.wants/NetworkManager.service
```

**Ako simbolička poveznica ne postoji**, potrebno je izvršiti naredbu ```sudo ln -s /mnt/rpi-root/usr/lib/systemd/system/NetworkManager.service /mnt/rpi-root/etc/systemd/system/multi-user.target.wants/NetworkManager.service``` što je jednako izvršenju naredbe ```systemctl enable NetworkManager.service```.

Profil konekcije može se dodati stvarajući datoteku ```preconfigured.nmconnection``` na **root** particiji u direktoriju ```/etc/NetworkManager/system-connections/```. Stvaranje datoteke može se napraviti naredbom:

```
sudo nano /mnt/rpi-root/etc/NetworkManager/system-connections/preconfigured.nmconnection
```

Sadržaj datoteke mora izgledati ovako:

```
[connection]
id=<SSID>
uuid=<UUID>
type=wifi
interface-name=wlan0

[wifi]
mode=infrastructure
ssid=<SSID>

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

UUID se može stvoriti naredbom:

```
uuidgen
```

Informacije o opcijama se mogu naći [ovdje](https://networkmanager.dev/docs/api/latest/settings-802-11-wireless-security.html). Za kraj, potrebno je promijeniti dozvole same datoteke naredbom:

```
sudo chmod 600 /mnt/rpi-root/etc/NetworkManager/system-connections/preconfigured.nmconnection
```

### Demontiranje datotečnih sustava

Demontiranje montiranih datotečnih sustava može se napraviti naredbama:

```
sudo umount /mnt/rpi-root/boot/firmware
sudo umount /mnt/rpi-root
sudo rmdir /mnt/rpi-root
```

### Demapiranje slike sa pseuduređaja

Odmapiranje slike sa pseudouređaja (ako je dodijeljeni pseuduređaj bio ```/dev/loop0```) može se napraviti naredbom:

```
sudo losetup -d /dev/loop0
```

### Kloniranje slike na SD karticu

Potrebno je umetnuti SD karticu u čitač SD kartice te taj čitač spojiti na računalo. Nakon toga, SD kartici će se dodijeliti datoteka uređaja (vidljivo ```lsblk``` naredbom), primjerice ```/dev/sda``` ili ```/dev/sdb```. **Bitno je identificirati koja datoteka uređaja pripada SD kartici.** Ovo se može napraviti naredbom ```sudo dmesg```. Prije pisanja na SD karticu, potrebno je demontirati sve postojeće particije SD kartice koje su se automatski montirale naredbom:

```
sudo umount <datoteka uređaja koja predstavlja SD karticu>*
```

Kloniranje slike se može napraviti sljedećom naredbom:

```
sudo dd if=2024-10-22-raspios-bookworm-arm64-lite.img of=<datoteka uređaja koja predstavlja SD karticu> bs=4M conv=fsync status=progress
```

Sigurno izbacivanje SD kartice može se napraviti sljedećom naredbom:

```
sudo eject <datoteka uređaja koja predstavlja SD karticu>
```

### Inicijalizacija i udaljeni pristup

**Nakon prvog pokretanja root particija će se proširiti do kraja veličine SD kartice.** Nakon toga će se pojaviti upit za postavljanje inicijalnog korisnika i njegove zaporke **samo ako slika nije uređena postavljanjem inicijalnog korisnika u userconf.txt datoteci prije kloniranja**.

Ako se želi mikroračunalu udaljeno pristupati preko SSH protokola, potrebno ga je povezati na lokalnu mrežu bilo preko Ethernet ili Wi-Fi sučelja. SSH servis mora biti unaprijed omogućen uređivanjem slike prije kloniranja. Udaljeni pristup preko SSH protokola je koristan ako pri ruci nije dostupan monitor ili tipkovnica.

U ovom slučaju, uloge su sljedeće:

- **lokalno računalo je SSH klijent**
- **udaljeno računalo odnosno mikroračunalo je SSH poslužitelj**

Kako bi pristupili mikroračunalu potrebno je prvo na računalu stvoriti jedan par SSH ključeva. Stvaranje jednog para asimetričnih ključeva po krivulji ED25519 može se napraviti naredbom:

```
ssh-keygen -t ed25519
```

Ključevi će se nalaziti u direktoriju ```.ssh``` u korisnikovom *home* direktoriju. Obično datoteka javnog ključa ima nastavak ```.pub``` dok datoteka privatnog ključa nema nastavak. Kako bi se javni ključ kopirao s računala na mikroračunalo, potrebno je izvršiti sljedeću naredbu:

```
ssh-copy-id <korisničko ime na Raspberry Pi mikroračunalu>@<IP adresa Raspberry Pi mikroračunala>
```

Tražit će se zaporka korisnika na mikroračunalu.

Nakon izvršenja ove naredbe dogodit će se razmjena javnih ključeva. Javni ključ računala nadodat će se u datoteci ```authorized_keys``` u direktoriju ```.ssh``` u *home* direktoriju udaljenog korisnika na mikroračunalu. Javni ključ mikroračunala nadodat će se u datoteci ```known_hosts``` u direktoriju ```.ssh``` u *home* direktoriju korisnika na računalu.

Ali odakle mikroračunalu par asimetričnih ključeva? Par ključeva se generirao za vrijeme prvog pokretanja Raspberry Pi OS-a. Oni se nalaze u direktoriju ```/etc/ssh```.

Dakle, direktorij ```.ssh``` na lokalnom računalu u korisnikovom *home* direktoriju sadrži:

- datoteku javnog ključa, ako ime nije dano pri stvaranju onda je ```id_ed25519.pub```
- datoteku privatnog ključa, ako ime nije dano pri stvaranju onda je ```id_ed25519```
- datoteku liste javnih ključeva udaljenih računala odnosno mikroračunala ```known_hosts```, tu se nalazi javni ključ mikroračunala (SSH poslužitelj)

Dok direktorij ```.ssh``` na udaljenom mikroračunalu u korisnikovom *home* direktoriju sadrži:

- datoteku liste javnih ključeva računala kojima je dozvoljeno spajanje bez lozinke ```authorized_keys```, tu se nalazi javni ključ lokalnog računala (SSH klijent)

Konačno, pristup s lokalnog računala na udaljeno mikroračunalo moguće je naredbom:

```
ssh <korisničko ime na Raspberry Pi mikroračunalu>@<IP adresa Raspberry Pi mikroračunala>
```

Ako se na lokalnom računalu želi uklonit zapis u ```known_hosts``` datoteci, to se može napraviti naredbom:

```
ssh-keygen -R <ime udaljenog računala|IP adresa udaljenog računala>
```

Prethodni sadržaj ```known_hosts``` datoteke spremit će se u ```known_hosts.old``` u ```.ssh``` direktoriju u *home* direktoriju korisnika.

Također, ako se na udaljenom mikroračunalu želi ukloniti javni ključ lokalnog računala što će onemogućiti pristup s ```ssh``` naredbom, to je uklanjanjem odgovarajuće linije u ```authorized_keys``` datoteci:

```
nano ~/.ssh/authorized_keys
```

S lokalnog računala se više neće moći pristupati udaljenom mikroračunalu ```ssh``` naredbom bez upisivanja zaporke. U tom slučaju potrebno je ponovno kopirati javni ključ lokalnog računala naredbom ```ssh-copy-id```.

Izlaz iz SSH sesije s klijentske strane se može napraviti naredbom ```exit``` ili ```logout```.

## Upravljanje softverom

[APT](https://en.wikipedia.org/wiki/APT_(software)) je alat za upravljanje softverskih alata kojeg, kao i Debian, koristi i Raspberry Pi OS.

Datoteka ```/etc/apt/sources.list``` sadrži popis repozitorija koji APT pregledava kada ažurira popis dostupnih paketa za instalaciju. Dodatne datoteke s URL-ovima repozitorija mogu se dodati u direktoriju ```/etc/apt/sources.list.d/```.

Datoteke s listama paketa i njihovih verzija povučenih iz navedenih repozitorija nalaze se u direktoriju ```/var/lib/apt/lists/```.

U slučaju da se želi ažurirati popis paketa, primjerice ako su na repozitorijima dodani novi softverski paketi ili ažurirani stari, to se može napraviti naredbom:

```
sudo apt update
```

Ako se žele saznati informacije o paketu s lokalnog popisa, potrebno je izvršiti naredbu:

```
sudo apt-cache show <ime paketa>
```

Ako se žele nadograditi trenutačni paketi na najnoviju verzije zapisane u lokalnom popisu, to se može napraviti izvršavajući:

```
sudo apt upgrade
```

Prethodna naredba će nadograditi postojeće pakete, ali ih neće brisati. U slučaju da je potrebno nadograditi cijeli sustav (cijelu distribuciju) gdje je potrebno i ukloniti neke pakete, može se izvršiti naredba:

```
sudo apt full-upgrade
```

Instalaciju paketa s lokalnog popisa moguće je izvršiti naredbom:

```
sudo apt install <ime paketa>
```

Uklanjanje instaliranog paketa moguće je izvršiti naredbom:

```
sudo apt remove <ime paketa>
```

Ako se uz paket želi ukloniti i njegove konfiguracijske datoteke, to se može napraviti naredbom:

```
sudo apt purge <ime paketa>
```

Uklanjanje nekorištenih zavisnosti (primjerice, u slučaju brisanja paketa) može se napraviti naredbom:

```
sudo apt autoremove
```

## Opća konfiguracija sustava

Opća konfiguracija Raspberry Pi OS-a može se napraviti naredbom:

```
sudo raspi-config
```

Naredba će pokrenuti jednostavno interaktivno sučelje u terminalu. Više informacija se može naći [ovdje](https://www.raspberrypi.com/documentation/computers/configuration.html#raspi-config).

Moguće je konfiguraciju preko ```raspi-config``` izvršavati na neinteraktivni način opcijom ```nonint```, primjerice:

```
sudo raspi-config nonint do_hostname raspberrypi-1 # Postavlja ime mikroračunala
sudo raspi-config nonint do_change_locale en_US.UTF-8 UTF-8 # Postavlja regionalne postavke sustava
sudo raspi-config nonint do_change_timezone Europe/Zagreb # Postavlja vremensku zonu
```

## Korisni softverski alati

Za ispis informacija o monitorima može se koristiti alat ```kmsprint```. Primjerice, ispis svih načina rada povezanih monitora može se dobiti izvršavajući:

```
kmsprint -m
```

Ispis logova VideoCore grafičkog procesora može se dobiti alatom ```vclog```. Primjerice, ispis informativnih poruka može se dobiti naredbom:

```
sudo vclog --msg
```

Ispis asertivnih poruka može se dobiti naredbom:

```
sudo vclog --assert
```

[Općenite informacije o VideoCore grafičkom procesoru](https://www.raspberrypi.com/documentation/computers/os.html#vcgencmd) mogu se dobiti naredbom ```vcgencmd```. Kao primjer:

- ```vcgencmd vcos version``` - ispiši verziju firmvera
- ```vcgencmd vcos log status``` - ispiši stanje logova raznih dijelova firmwarea grafičkog procesora
- ```vcgencmd get_throttled``` - ispiši razlog usporavanja sustava (primjerice zbog pregrijavanja), vraća specifični heksadekadski broj (0x0 - nema usporavanja, 0x1 - pad napona detektiran, 0x2 - maksimalna frekvencija ARM procesora dosegnuta, ...)
- ```vcgencmd measure_temp``` - temperatura sustava na čipu
- ```vcgencmd measure_clock <arm|core|h264|uart|hdmi|dpi|...>``` - frekvencija specificiranog takta
- ```vcgencmd measure_volts <core|sdram_c|sdram_i|sdram_p>``` - napon specificiranog bloka (VideoCore jezgre ili SDRAM dijelova)
- ```vcgencmd otp-dump``` - ispis sadržaja *OTP - One Time Programmable* memorije
- ```vcgencmd codec_enabled <AGIF|FLAC|H263|H264|MJPA|MJPB|...>``` - ispis je li omogućen specifični kodek
- ```vcgencmd mem_oom```- ispis statistike vezane uz *OOM - Out Of Memory* događaje na grafičkom procesoru
- ```vcgencmd read_ring_osc``` - ispis brzine, napona i temperature prstenastog oscilatora

## Daljnje upute

Korisne poveznice:

* informacije o *boot* procesu i jezgri: [ovdje](boot-process-and-kernel)
* konfiguriranje mrežnih postavki: [ovdje](networking)
* upravljanje GPIO pinovima: [ovdje](gpio)
