Testausdokumentti
=================

Yksikkötestauksen kattavuusraportti
-----------------------------------

	6/6 testiä onnistui.
	
	File 'application.c'
	Lines executed:27.66% of 47
	Creating 'application.c.gcov'
	
	File 'fft.c'
	Lines executed:100.00% of 51
	Creating 'fft.c.gcov'
	
	Lines executed:65.31% of 98

Raportoitu 8.3.2024.

Mitä ja miten
-------------

Ainoa automaattinen testaus on tällä hetkellä funktioiden yksikkötestaus. Tämä
tapahtuu omalla testikehyksellä, kun ajetaan `run_tests.sh` -skripti.
Yksikkötestaus koskee kaikkia tiedoston `fft.c` funktiota sekä joitakin
tiedoston `application.c` funktioista. Viimeksi mainitussa tiedostossa ei
testata seuraavia funktiota:

* `die`, joka pysäyttää ohjelman virhetilanteessa, ja jota ei ole kovin
  mielekästä yksikkötestata,
* `zero_intervals`, jota on triviaaliuteensa nähden kovin hankala testata,
* `process_input`, joka sisältää ohjelman pääsilmukan.
