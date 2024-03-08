Toteutusdokumentti
==================

Ohjelman ydin on suhteellisen yksinkertainen C:llä kirjoitettu ohjelma, joka
vain tulkitsee komentoriviargumentteja, lukee syötteenä raakaa äänidataa,
hiljentää siitä Fourier-muunnoksen avulla halutut taajuudet ja sylkee ulos tästä
synteneen tuloksen. C-ohjelma käynnistetään skriptin avulla, joka käyttää
ulkoista työkalua (Sox) äänidatan purkamiseen tiedostosta ja sen kirjoittamiseen
uuteen tiedostoon tai soittamiseen kaiuttimesta.

C-ohjelman aloituspiste on tiedoston `main.c` `main`-funktio, joka käsittelee
komentoriviargumentit ja kutsuu ohjelman pääsilmukan sisältävää funktiota.
Pääsilmukka ja sovellukseen liittyvät apufunktiot löytyvät tiedostosta
`application.c`. Fourier-algoritmit taas löytyvät tiedostosta `fft.c`. Tiedosto
`test.c` sisältää toisen `main`-funktion ja käännetään vain yksikkötestauksen
yhteydessä.

Käytetty diskreetti Fourier-muunnosalgoritmi on totetutettu nopeana
Fourier-muunnoksena, eli sen aikavaativuus suhteessa syötteen pituuteen $n$ on
$O(n \log n)$. Käyttäjä voi valita, kuinka pitkiä palasia alkuperäisestä äänestä
ohjelma syöttää kerralla fft-funktiolle, ja koko ohjelman aikavaativuus riippuu
tästä arvosta, jota merkitsemme $m$. Koko syötteen pituutta merkitsemme $n$.
Suoritukseen liittyy datan kopioimista paikasta toiseen, jonka perusteella
saamme aikavaativuudelle alarajan $O(n)$. $2n / m$ kappaleeseen syötteen
$m$-pituisia osia suoritetaan fft, joten saamme aikavaativuudeksi yhteensä
$O(n + (2n/m) m \log m) = O(n + 2n \log m)$.  Koska $m$ on ohjelmassa rajoitettu
olemaan vähintään 2, $n + 2n \log m \le 3n \log m$, ja saamme lopulliseksi
aikavaativuudeksi $O(n \log m)$.

Ohjelman käyttöliittymä on tällä hetkellä varsin epäintuitiivinen, ja siinä on
parantamisen varaa. Poistettavat taajuudet voisi esimerkiksi esittää
hertsiväleinä nykyisen, puhtaasti ohjelman tekniseen toteutukseen liittyvän
esitysmuodon sijaan.
