Testausdokumentti
=================

Yksikkötestauksen kattavuusraportti
-----------------------------------

	4/4 tests passed.

	File 'fft.c'
	Lines executed:100.00% of 51
	Creating 'fft.c.gcov'

	Lines executed:100.00% of 51

Raportoitu 3.2.2024.

Mitä ja miten
-------------

Ainoa automaattinen testaus on tällä hetkellä funktioiden yksikkötestaus. Tämä
tapahtuu omalla testikehyksellä, kun ajetaan `run_tests.sh` -skripti. Tällä
hetkellä testataan vain `fft.c` -tiedoston funktioita, koska tiedostossa
`main.c` ei oikein ole mitään järkevästi yksikkötestattavaa. Itse ohjelmaa
on testattu vain karkeasti kuuntelemalla sen läpi musiikkia.
