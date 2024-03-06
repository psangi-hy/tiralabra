Käyttöohje
==========

Ohjelma on kirjoitettu Linux-järjestelmille. Tarvittavien riippuvuuksien pitäisi
olla esiasennettuina fuksiläppäreille, mutta muuten tarvitset seuraavat paketit:

* `gcc`-kääntäjä
* `sox`-äänenkäsittelytyökalu

Yksikkötestit suoritetaan ajamalla skripti `run_tests.sh`. Itse ohjelma
käännetään skriptillä `build.sh`.

Ohjelma ajetaan kääntämisen jälkeen skriptillä `filter.sh`. Komento on muotoa

	./filter.sh <syöte> <tuloste> <otoksen pituus> [<poistettavat taajuudet>...]

missä

* syöte on sen äänitiedoston polku, jota haluat ohjelman lukevan
* tuloste on tiedostopolku, johon haluat tallentaa tuloksen. Tämä voi myös olla
  `-d`, jolloin tulos soitetaan suoraan kaiuttimista.
* otoksen pituus on signaalin kerralla käsiteltävien pisteiden määrä. Samalla
  se on käsiteltävissä olevien taajuuksien lukumäärä, joten poistettavien
  taajuuksien on oltava tätä lukua pienempiä tai yhtä suuria.
* poistettavat taajuudet ovat lukupareja, jossa parin ensimmäinen luku antaa
  välin alkupisteen ja toinen sen loppupisteen. Jos lukuja on pariton määrä,
  viimeisen välin loppupiste on otoksen pituus.

Esimerkiksi

	./filter.sh tiedosto.wav -d 4096 100 1000

soittaa kaiuttimista tiedoston `tiedosto.wav` sisällön, josta on poistettu
taajuudet väliltä 100--1000, kun signaali on jaettu 4096
taajuuteen.
