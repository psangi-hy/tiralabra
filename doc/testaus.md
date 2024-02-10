Testausdokumentti
=================

Yksikkötestauksen kattavuusraportti
-----------------------------------

	4/4 tests passed.

	File 'fft.c'
	Lines executed:100.00% of 51
	Creating 'fft.c.gcov'

	Lines executed:100.00% of 51

Raportoitu 10.2.2024.

Mitä ja miten
-------------

Ainoa automaattinen testaus on tällä hetkellä funktioiden yksikkötestaus. Tämä
tapahtuu omalla testikehyksellä, kun ajetaan `run_tests.sh` -skripti. Tällä
hetkellä testataan vain `fft.c` -tiedoston funktioita.  Tiedostossa `main.c` on
nyt muutama apufunktio, mutta nämä ovat niin triviaaleja, etten toistaiseksi
ole katsonut aiheelliseksi yksikkötestata niitä. Itse ohjelmaa on testattu vain
karkeasti kuuntelemalla sen läpi musiikkia.
