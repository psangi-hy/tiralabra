Aineopintojen harjoitustyö: algoritmit ja tekoäly
=================================================

Dokumentit
----------

[Määrittelydokumentti](doc/maarittely.pdf)

[Testausdokumentti](doc/testaus.md)

[Toteutusdokumentti](doc/toteutus.md)

Viikkoraportit
--------------

[1](doc/viikkoraportti_1.pdf)
[2](doc/viikkoraportti_2.pdf)
[3](doc/viikkoraportti_3.pdf)
[4](doc/viikkoraportti_4.pdf)
[5](doc/viikkoraportti_5.pdf)

Käyttöohje
----------

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
  taajuuksien on oltava tätä lukua pienempiä.
* poistettavat taajuudet ovat lukupareja, jossa parin ensimmäinen numero antaa
  välin alkupisteen ja toinen sen loppupisteen. Jos lukuja on pariton määrä,
  viimeisen välin loppupiste on otoksen pituus.

Esimerkiksi

	./filter.sh tiedosto.wav -d 4096 96 1000 3096 4000

soittaa kaiuttimista tiedoston `tiedosto.wav` sisällön, josta on poistettu
taajuudet väleiltä 96--1000 ja 3096--4000, kun signaali on jaettu 4096
taajuuteen. Huomaa, että matalat taajuudet vaikuttavat olevan reunoilla ja
korkeat keskellä, joten poistettavat taajuudet kannattaa peilata keskipisteen
suhteen (tässä tapauksessa 2048).
