# KONFIGURACIJA MREŽNIH POSTAVKI

Konfiguracija mrežnih postavki Raspberry Pi 4B mikroračunala se najčešće radi uz pomoć [*NetworkManagera*](https://networkmanager.dev/). Skup alata omogućuje olakšano upravljanje Ethernet, Wi-Fi i mobilnim mrežnim sučeljima. Nudi potporu za većinu sigurnosnih protokola, primjerice:

- WPA/WPA2/WPA3
- žičani 802.1x
- MACsec
- VPN

Postavljanje mrežnih postavki radi servis ```NetworkManager```, a sa servisom je moguće komunicirati alatom ```nmcli```. Kod Raspberry Pi OS-a, servis je zadano omogućen, dakle on bi trebao biti već pokrenut prilikom pokretanja OS-a. Ako nije pokrenut, to se može napraviti naredbom:

```
sudo systemctl start NetworkManager
```

Zaustavljanje servisa može se napraviti naredbom ```sudo systemctl stop NetworkManager```. Kako bi omogućili automatsko pokretanje servisa pri svakom ponovnom pokretanju OS-a, potrebno je izvršiti naredbu:

```
sudo systemctl enable NetworkManager
```

A ako se servis ne želi pokretati pri svakom ponovnom pokretanju OS-a, potrebno je izvršiti naredbu ```sudo systemctl disable NetworkManager```.

## Konfiguracijske datoteke

Zadana konfiguracijska datoteka za NetworkManager servis je ```/etc/NetworkManager/NetworkManager.conf```. Dodatne konfiguracije s ```.conf``` nastavkom mogu se dodati u zasebnim direktorijima. Datoteke tih direktorija, uključujući i samu zadanu konfiguracijsku datoteku, parsiraju se sljedećim redoslijedom:

- ```/usr/lib/NetworkManager/conf.d``` - paketi pri instalaciji najčešće dodaju svoje konfiguracije ovdje
- ```/run/NetworkManager/conf.d``` - dodatne konfiguracije koje paketi mogu dodati
- ```/etc/NetworkManager/NetworkManager.conf```
- ```/etc/NetworkManager/conf.d``` - vlastite konfiguracije

Svako sljedeće parsiranje prepisuje prethodne konfliktne konfiguracije. Nakon uređivanja konfiguracija potrebno ih je učitati naredbom:

```
sudo systemctl reload NetworkManager
```

Informacije o ```NetworkManager.conf``` datoteci mogu se naći [ovdje](https://networkmanager.pages.freedesktop.org/NetworkManager/NetworkManager/NetworkManager.conf.html).

## Upravljanje sučeljima

Servis NetworkManager zadano upravlja svim dostupnim mrežnim sučeljima u sustavu. Sva sučelja koja se nalaze u sustavu mogu se izlistati naredbom:

```
sudo nmcli device
```

Ako se neko sučelje želi ukloniti od nadzora NetworkManagera, to se može napraviti naredbom:

```
sudo nmcli device set <ime mrežnog sučelja> managed no
```

Ako ga se opet želi staviti pod nadzor, to se može napraviti naredbom:

```
sudo nmcli device set <ime mrežnog sučelja> managed yes
```

Prethodne naredbe neće napraviti postavke trajne. Da bi se trajno uklonilo mrežno sučelje od nadzora NetworkManagera, potrebno je dodati vlastitu konfiguracijsku datoteku u ```/etc/NetworkManager/conf.d``` direktorij. Datoteka mora imati sljedeći sadržaj:

```
[device-<ime mrežnog sučelja>-unmanage]
match-device=interface-name:<ime mrežnog sučelja>
managed=0
```

Potrebno je ponovno učitati konfiguracije alatom ```nmcli```.

### Upravljanje Wi-Fi sučeljem

Za razliku od standardnog Ethernet sučelja gdje se uređaj povezuje na mrežu automatski spajanjem UTP kabela, korisnik za Wi-Fi sučelje mora odabrati mrežu i potencijalno upisati lozinku za uspješno povezivanje na mrežu.

Paljenje ili gašenje radija svih Wi-Fi sučelja u sustavu može se napraviti naredbom:

```
sudo nmcli radio wifi <on|off>
```

#### Skeniranje i izlistavanje mreža

Servis NetworkManager periodično izvršava skeniranje za obližnje Wi-Fi mreža. Međutim, ako se želi odmah skenirati za obližnje Wi-Fi mreže to se može napraviti naredbom:

```
sudo nmcli device wifi rescan
```

Naredba će pokrenuti skeniranje na svim Wi-Fi sučeljima. Izlistavanje liste pronađenih mreža od zadnjeg skeniranja može se napraviti naredbom:

```
sudo nmcli device wifi list
```

#### Spajanje na mrežu i odspajanje s mreže

Spajanje na mrežu sa specifičnog Wi-Fi sučelja može se napraviti naredbom:

```
sudo nmcli device wifi connect <SSID ili BSSID mreže> password <zaporka> ifname <Wi-Fi sučelje>
```

Ako se izostavi ime sučelja, servis NetworkManager će koristiti prvo slobodno sučelje, a ako prvo slobodno sučelje ne postoji, koristit će zadano primarno sučelje (najprioritetnije Wi-Fi sučelje zadano metrikom). Prvim spajanjem na mrežu stvorit će se profil konekcije koji će sadržavati sve informacije o mrežnim konfiguracijama za tu mrežu (SSID, zaporka, sučelje koje se koristi za povezivanje i slično).

Odspajanje s mreže sa specifičnog Wi-Fi sučelja može se napraviti naredbom:

```
sudo nmcli device disconnect ifname <Wi-Fi sučelje>
```

## Profili konekcije

Profili konekcije su mrežne konfiguracije kojima upravlja NetworkManager te se oni spremaju na lokaciju ```/etc/NetworkManager/system-connections```. Jedini profili konekcija koji se ne spremaju na toj lokaciji su automatski stvoreni profili Ethernet sučelja. Svi profili u sustavu mogu se vidjeti naredbom:

```
sudo nmcli connection show
```

### Onemogućivanje automatske aktivacije profila Ethernet sučelja

NetworkManager za sva Ethernet sučelja u sustavu ima pripremljene profile za aktivaciju u slučaju da se fizički spoji kabel na sučelje. Ovo možda nije uvijek poželjno jer ti profili mogu imati primjerice postavljenu DHCPv4 autokonfiguraciju ili DHCPv6 autokonfiguraciju gdje to možda i nije poželjno. Kako bi onemogućila automatska aktivacija profila, potrebno je u datoteci ```/etc/NetworkManager/NetworkManager.conf``` dodati:

```
[main]
no-auto-default=<ime sučelja>
```

Potrebno je ponovno učitati konfiguracije alatom ```nmcli```.

### Aktivacija mrežnog sučelja odmah pri pokretanju

NetworkManager aktivira profil Ethernet sučelja kad se računalo spoji na mrežni uređaj kabelom odnosno kad se pojave električni signali na sučelju. Ovo možda nije pogodno za aplikacije koje zahtijevaju da sučelje bude aktivirano odmah pri pokretanju. Ako se želi odmah omogućiti Ethernet sučelje, bilo da je spojen kabel ili ne, u ```/etc/NetworkManager/NetworkManager.conf``` datoteku je potrebno dodati:

```
[main]
ignore-carrier=<ime sučelja>
```

Potrebno je ponovno učitati konfiguracije alatom ```nmcli```.

### Korištenje vlastitih profila konekcije

Iako spajanje s Ethernet ili Wi-Fi sučelja automatski aktivira ili stvara i aktivira profile konekcije, moguće je unaprijed stvoriti vlastiti profil konekcije (vlastite IP adrese DNS poslužitelja, statičke adrese i slično).

Naredba za stvaranje profila konekcije je sljedeća:

```
sudo nmcli connection add <vrsta konekcije> <argumenti>
```

Vrsta konekcija ima mnogo, a pogotovo argumenata za svaku od tih vrsta. Više u dokumentaciji [ovdje](https://networkmanager.dev/docs/api/latest/nmcli.html).

Uređivanje profila konekcije može se napraviti naredbom:

```
sudo nmcli connection edit <ime profila konekcije ili UUID profila konekcije>
```

Brisanje profila konekcije može se napraviti naredbom:

```
sudo nmcli connection delete <ime profila konekcije ili UUID profila konekcije>
```

### Aktivacija i deaktivacija profila konekcije

Profil konekcije može se aktivirati odnosno deaktivirati naredbom:

```
sudo nmcli connection <up|down> <ime profila konekcije ili UUID profila konekcije>
```

## Konfiguracija Raspberry Pi mikroračunala kao pristupna točka

Raspberry Pi mikroračunalo moguće je konfigurirati kao [pristupnu točku (*eng. Access Point*)](https://www.raspberrypi.com/documentation/computers/configuration.html#enable-hotspot). Za ovakvu konfiguraciju potrebno je spojiti Raspberry Pi na mrežni uređaj (primjerice usmjernik) uz pomoć Ethernet sučelja dok će Wi-Fi sučelje odašiljati signal preko kojeg će se drugi uređaji spajati na mrežu. Naredba za stvaranje profila konekcije koje će koristiti Wi-Fi sučelje *wlan0* za odašiljanje signala bit će:

```
sudo nmcli device wifi hotspot con-name <ime profila konekcije> ifname wlan0 ssid <ime mreže koje će biti vidljivo skeniranjem> password <zaporka pristupa mreži>
```

U ovom slučaju će se sav promet sa sučelja *wlan0* prosljeđivati na *default gateway* odnosno sučelje *eth0* i usput raditi mrežnu translaciju adresa (*NAT - Network Address Translation*). Sučelje koje se koristi kao *default gateway* može se vidjeti naredbom ```ip route```. Deaktivacija, aktivacija ili brisanje profila konekcije moguće je već navedenim ```nmcli connection ...``` naredbama.

Raspberry Pi mikroračunalo će u ovom slučaju imati ulogu usmjernika.

## Konfiguracija Raspberry Pi mikroračunala kao mrežni most

Raspberry Pi mikroračunalo moguće je konfigurirati kao i [mrežni most](https://www.raspberrypi.com/documentation/computers/configuration.html#use-your-raspberry-pi-as-a-network-bridge). Za ovakvu konfiguraciju potrebno je spojiti Raspberry Pi na mrežni uređaj (primjerice usmjernik) uz pomoć Ethernet sučelja dok će Wi-Fi sučelje odašiljati signal preko kojeg će se drugi uređaji spajati na mrežu.

Prvo je potrebno stvoriti sučelje mrežnog mosta, primjerice imena ```bridge0```.

```
sudo nmcli connection add type bridge con-name "Bridge" ifname bridge0
```

Onda je potrebno dodati Ethernet sučelje *eth0* kao *slave* sučelju *bridge0*:

```
sudo nmcli connection add type ethernet slave-type bridge con-name "Ethernet Slave" ifname eth0 master bridge0
```

Nakon toga je potrebno stvoriti profil konekcije koji će postaviti bežično sučelje *wlan0* u način rada pristupne točke (nije potrebna zaporka). Na kraju je potrebno modificirati profil konekcije *Wireless Slave* tako što je potrebno dodati *wlan0* sučelje kao *slave* sučelju *bridge0*:

```
sudo nmcli device wifi hotspot con-name "Wireless Slave" ifname wlan0 ssid <ime mreže koje će biti vidljivo skeniranjem>
sudo nmcli connection modify "Wireless Slave" master bridge0
```

Konačno, aktivacija definiranih profila konekcija radi se naredbama:

```
sudo nmcli connection up Bridge
sudo nmcli connection up "Ethernet Slave"
sudo nmcli connection up "Wireless Slave" --ask
```

Posljednja naredba će pitati za zaporku koja će se koristiti za prijavu na pristupnu točku.

Konfiguracija se može ukloniti sljedećim naredbama:

```
sudo nmcli connection down "Wireless Slave"
sudo nmcli connection down "Ethernet Slave"
sudo nmcli connection down Bridge
sudo nmcli connection delete "Wireless Slave"
sudo nmcli connection delete "Ethernet Slave"
sudo nmcli connection delete Bridge
```

