# KONFIGURIRANJE MREŽNIH POSTAVKI

Konfiguriranje mrežnih postavki Raspberry Pi OS-a od verzije Bookworm radi se skupom alata [*NetworkManager*](https://networkmanager.dev/). Omogućuje olakšano upravljanje Ethernet, Wi-Fi i mobilnim mrežnim sučeljima. Nudi potporu za većinu sigurnosnih protokola, primjerice bežični WPA, WPA2 (Personal i Enterprise), WPA3 (Personal i Enterprise), žičani 802.1x, MACsec i VPN-ove.

Kako bi se moglo upravljati mrežnim postavkama, potrebno je u pozadini imati pokrenut servis *NetworkManager*. Kod Raspberry Pi OS-a servis *NetworkManager* se obično pokreće pri pokretanju. U slučaju da to nije tako, servisu je potrebno omogućiti pokretanje pri svakom pokretanju mikroračunala naredbom:

```
sudo systemctl enable NetworkManager
```

Onemogućivanje servisa radi se naredbom ```sudo systemctl disable NetworkManager```.

Trenutačno pokretanje servisa radi se naredbom:

```
sudo systemctl start NetworkManager
```

Zaustavljanje rada servisa se radi naredbom ```sudo systemctl start NetworkManager```, a ponovno pokretanje rada servisa naredbom ```sudo systemctl start NetworkManager```.

## Konfiguracijske datoteke

Iako *NetworkManager* servis ima zadanu konfiguracijsku datoteku */etc/NetworkManager/NetworkManager.conf*. Vlastite konfiguracije se mogu dodavati u direktoriju */etc/NetworkManager/conf.d*. Nadalje, neki paketi mogu dodavati svoje konfiguracije u direktoriju */usr/lib/NetworkManager/conf.d* ili */run/NetworkManager/conf.d*. Redoslijed parsiranja konfiguracijskih datoteka je sljedeći:

- datoteke u */usr/lib/NetworkManager/conf.d*
- datoteke u */run/NetworkManager/conf.d*
- datoteka */etc/NetworkManager/NetworkManager.conf*
- datoteke u */etc/NetworkManager/conf.d*

Servis *NetworkManager* za svaki žičani Ethernet port namješta nekakve uobičajene postavke, primjerice DHCPv4 autokonfiguraciju ili IPv6 autokonfiguraciju što u poslužiteljskim okolinama možda i nije poželjno. Ove konekcije se mogu izlistati naredbom ```nmcli connection show``` te imaju naziv *Wired connection 1*, *Wired connection 2* i slično, ovisno koliko Ethernet sučelja postoji na računalu. Kako bi se to onemogućilo, u */etc/NetworkManager/NetworkManager.conf* datoteci je potrebno dodati:

```
[main]
no-auto-default=*
```

Ovo će onemogućiti stvaranje zadanih konfiguracija za sva Ethernet sučelja (oznaka *). Oznaka [sekcije](https://networkmanager.pages.freedesktop.org/NetworkManager/NetworkManager/NetworkManager.conf.html) *main* označava da se sljedeće postavke odnose na općenite postavke.

Također, *NetworkManager* aktivira mrežno Ethernet mrežno sučelje samo ako postoji fizička konekcija kabelom na drugo Ethernet sučelje (odnosno *carrier*). Ovo možda nije pogodno za neke aplikacije koje zahtijevaju da sučelje bude odmah aktivno pri pokretanju računala. Dakle, ako se želi onemogućiti čekanje na fizičku konekciju i odmah omogućiti Ethernet sučelje, potrebno je u datoteku */etc/NetworkManager/NetworkManager.conf* dodati:

```
[main]
ignore-carrier=*
```

Ovo će onemogućiti čekanje na fizičku konekciju za sva Ethernet sučelja (oznaka *) i *NetworkManager* će ih odmah omogućiti. Oznaka [sekcije](https://networkmanager.pages.freedesktop.org/NetworkManager/NetworkManager/NetworkManager.conf.html) *main* označava da se sljedeće postavke odnose na općenite postavke.

Općenito, nakon uređivanja bilo kojih konfiguracijskih datoteka *NetworkManager* servisa, potrebno ih je ponovno učitati naredbom:

```
systemctl reload NetworkManager
```

### Isključivanje upravljanja sučeljem

Uobičajeno, *NetworkManager* servis upravlja svim mrežnim sustavima na sustavu. Ako se želi trenutačno isključiti pojedino mrežno sučelje iz upravljanja *NetworkManager* servisom, potrebno je izvršiti naredbu:

```
nmcli device set enp1s0 managed no
```

U slučaju da se ova postavka želi trajno primijenit, potrebno je u */etc/NetworkManager/NetworkManager.conf* datoteci ili u vlastitu *.conf* datoteku u direktoriju */etc/NetworkManager/conf.d* dodati:

```
[device-<ime sučelja>-unmanage]
match-device=interface-name:<ime sučelja>
managed=0
```

Naravno, potrebno je opet učitati konfiguracijske datoteke naredbom ```systemctl reload NetworkManager```.

## Osnovno korištenje

Za osnovno korištenje *NetworkManager* servisa koristi se alat *nmcli*. Neke osnovne naredbe su:

- ```sudo nmcli device``` - izlistaj sva mrežna sučelja
- ```sudo nmcli radio wifi on``` - uključi sve Wi-Fi radije
- ```sudo nmcli radio wifi off``` - isključi sve Wi-Fi radije
- ```sudo nmcli device wifi list``` - izlistaj skenirane Wi-Fi mreže
- ```sudo nmcli device wifi connect [SSID mreže ili BSSID mreže] password [zaporka]``` - spoji se na ciljanu Wi-Fi mrežu primarnim mrežnim sučeljem
- ```sudo nmcli device wifi connect [SSID mreže ili BSSID mreže] password [zaporka] ifname [bežično mrežno sučelje] name [ime profila konekcije koji će se stvoriti]``` - spoji se na ciljanu Wi-Fi mrežu ciljanim sučeljem
- ```sudo nmcli device disconnect ifname [mrežno sučelje]``` - odspoji mrežno sučelje s mreže
- ```sudo nmcli connection show``` - pokaži sve profile konekcije
- ```sudo nmcli connection up [ime profila konekcije ili UUID profila konekcije]``` - poveži se na mrežu koristeći profil konekcije
- ```sudo nmcli connection down [ime profila konekcije ili UUID profila konekcije]``` - odspoji se s mreže koristeći profil konekcije

### Profili konekcije

Profili konekcije su konfiguracije kojima upravlja *NetworkManager* i one se spremaju na lokaciji */etc/NetworkManager/system-connections* osim automatski stvorenih žičanih Ethernet profila konekcija (*Wired connection 1*, *Wired connection 2*, ...). Profili konekcije se mogu uređivati naredbom:

```
sudo nmcli connection edit [ime profila konekcije ili UUID profila konekcije]
```

Moguće je stvoriti i vlastitu konfiguraciju profila konekcije naredbom:

```
sudo nmcli connection add [argumenti]
```

Korištenjem ```nmcli device connection add ...``` ili ```nmcli device wifi connect ...``` servis *NetworkManager* automatski stvara datoteku profila konekcije u direktoriju */etc/NetworkManager/system-connections*.

Isto tako, moguće ih je i ukloniti naredbom:

```
sudo nmcli connection delete [ime profila konekcije ili UUID profila konekcije]
```

Konfiguracije profila konekcija je moguće stvoriti i na drugi način: izravno stvaranje datoteka oblika *[ime profila konekcije].nmconnection* i popunjavanje odgovarajućih sekcija s konfiguracijama. **Ovdje treba bit vrlo oprezan pri konfiguraciji jer se zbog malih grešaka NetworkManager servis može ponašati nepredviđeno.**

## Konfiguracija Raspberry Pi mikroračunala kao pristupna točka

Kako bi se Raspberry Pi mikroračunalo konfiguriralo kao [pristupna točka](https://www.raspberrypi.com/documentation/computers/configuration.html#enable-hotspot) na mrežu, ono mora biti fizički spojeno na mrežu uz pomoć Etherneta. Tek nakon toga, moguće je postaviti uređaj kao pristupnu točku na mrežu. Naredba za stvaranje profila konekcije imena *Raspberry Pi Hotspot* koja koristi sučelje *wlan0* može se napraviti naredbom:

```
sudo nmcli device wifi hotspot con-name "Raspberry Pi Hotspot" ifname wlan0 ssid [ime mreže koje će biti vidljivo skeniranjem] password [zaporka pristupa mreži]
```

U ovom slučaju sav se promet koji dođe na sučelje *wlan0* prosljeđuje na *default gateway* odnosno *eth0* sučelje.

Ako se pristupna točka želi onemogućiti to se može naredbom:

```
sudo nmcli connection down "Raspberry Pi Hotspot"
```

Brisanje samog profila konekcije se može napraviti naredbom:

```
sudo nmcli connection delete "Raspberry Pi Hotspot"
```

Uređaji koji su izravno spojeni na glavnoj mreži **ne mogu** direktno komunicirati s uređajima koji su spojeni preko pristupne točke.

## Konfiguracija Raspberry Pi mikroračunala kao mrežni most

U slučaju da se Raspberry Pi mikroračunalo želi konfigurirati kao mrežni most, prvo je potrebno stvoriti sučelje mrežnog mosta, primjerice imena *bridge0* u definiciji profila konekcije imena *Bridge*:

```
sudo nmcli connection add type bridge con-name "Bridge" ifname bridge0
```

Zatim je potrebno dodati Ethernet sučelje *eth0* kao *slave* sučelju *bridge0* u definiciji profila konekcije imena *Ethernet Slave*:

```
sudo nmcli connection add type ethernet slave-type bridge con-name "Ethernet Slave" ifname eth0 master bridge0
```

Nakon toga je potrebno stvoriti profil konekcije *Wireless Slave* koja će postaviti bežično sučelje *wlan0* u način rada pristupne točke (nije potrebna zaporka). Na kraju je potrebno modificirati profil konekcije *Wireless Slave* tako što je potrebno dodati *wlan0* sučelje kao *slave* sučelju *bridge0*:

```
sudo nmcli device wifi hotspot con-name "Wireless Slave" ifname wlan0 ssid [ime mreže koje će biti vidljivo skeniranjem]
sudo nmcli connection modify "Wireless Slave" master bridge0
```

Konačno, aktivacija definiranih profila konekcija radi se naredbama:

```
sudo nmcli connection up Bridge
sudo nmcli connection up "Ethernet Slave"
sudo nmcli connection up "Wireless Slave" --ask
```

Posljednja naredba će pitati za zaporku koja će se koristiti za prijavu na pristupnu točku.

Ako je potrebno ukloniti konfiguraciju, to se radi sljedećim deaktivacijama i brisanjem profila konekcija, odnosno naredbama:

```
sudo nmcli connection down "Wireless Slave"
sudo nmcli connection down "Ethernet Slave"
sudo nmcli connection down Bridge
sudo nmcli connection delete "Wireless Slave"
sudo nmcli connection delete "Ethernet Slave"
sudo nmcli connection delete Bridge
```

Uređaji koji su izravno spojeni na glavnoj mreži **mogu** direktno komunicirati s uređajima koji su spojeni preko pristupne točke.

