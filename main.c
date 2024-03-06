#include <assert.h>
#include <complex.h>
#include <math.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "fft.c"

struct interval {
	size_t start, end;
};

static char *argv0;

/* Tulosta viesti ja pysähdy. */
static void
die(char *format, ...)
{
	if (format && *format != '\n') {
		fprintf(stderr, "%s VIRHE: ", argv0);

		va_list ap;

		va_start(ap, format);
		fprintf(stderr, format, ap);
		va_end(ap);
	}

	static char *usage_format =
		"Käyttö: %s otospituus [alku loppu]...\n"
		"missä\n"
		"\totospituus on kahden potenssi\n"
		"\talku ja loppu ovat kokonaislukuja\n"
		"\tviimeinen loppu voidaan jättää pois\n";

	fprintf(stderr, usage_format, argv0);

	exit(1);
}

/* Lue size_t merkkijonosta s.
 * Pysähdy, jos s sisältää muuta roskaa. */
static size_t
parse_size(char *s)
{
	char *end;
	size_t res;

	res = strtoul(s, &end, 10);

	if (*s == '\0' || *end != '\0')
		die("\"%s\" ei ole epänegatiivinen kokonaisluku.\n", s);

	return res;
}

/* Aseta nollaksi taulukon data alkiot, jotka annetaan num_interval-pituisessa
 * taulukossa intervals. */
static void
zero_intervals(float complex *data, size_t n, struct interval *intervals, size_t num_intervals)
{
	for (size_t i = 0; i < num_intervals; i++) {
		for (size_t j = intervals[i].start; j <= intervals[i].end; j++) {
			data[j] = 0.0f;
			data[(1 - j) & (n - 1)] = 0.0f;
		}
	}
}

/* Luo taulukkoon dest signaali, jossa taulukon from signaali hiipuu tasaisesti
 * pois samalla kun taulukon to signaali hiipuu sisään. Luku n on taulukoiden
 * dest, from ja to pituus. */
static void
interpolate_signal(float *dest, size_t n, float *from, float *to)
{
	if (!n)
		return;

	float step = 1.0f / n;
	float frac = step / 2;

	for (size_t i = 0; i < n; i++) {
		dest[i] = (1.0f - frac) * from[i] + frac * to[i];
		frac += step;
	}
}

int
main(int argc, char **argv)
{
	/* Komentoriviargumenttien käsittely. */

	argv0 = argv[0];

	if (argc < 2)
		die(0);

	char *end;

	size_t sample_length = parse_size(argv[1]) * 2;

	if (!is_power_of_2(sample_length))
		die("%s ei ole kahden potenssi.\n", argv[1]);

	size_t num_intervals = ((argc - 2) + 1) / 2;
	struct interval *intervals = calloc(num_intervals, sizeof(struct interval));

	for (size_t i = 0; i < num_intervals; i++) {
		intervals[i].start = parse_size(argv[i * 2 + 2]);

		if (argc > i * 2 + 3) {
			intervals[i].end = parse_size(argv[i * 2 + 3]);

			if (intervals[i].end <= sample_length / 2)
				continue;
		}

		intervals[i].end = sample_length / 2;
	}

	/* Varaa puskurit signaalinkäsittelysilmukkaa varten. */

	float *input = calloc(sample_length, sizeof(float));
	float complex *transform = calloc(sample_length, sizeof(float complex));
	float *intermediate[2] = {
		calloc(sample_length, sizeof(float)),
		calloc(sample_length, sizeof(float))
	};
	float *output = calloc(sample_length / 2, sizeof(float));

	/* Jos luemme naiivisti sample_length pistettä tulosignaalia kerrallaan
	 * ja poistamme siitä taajuuksia, syntyy tulostetussa äänessä ikävää
	 * poksahtelua, joka johtuu signaalin jatkuvuuden rikkoutumisesta eli
	 * yhtäkkisistä muutoksista signaalin arvossa. Ajatukseni tämän ongelman
	 * ratkaisemiseksi on suorittaa tämä operaatio sample_length pisteelle
	 * lomittain aina sample_length / 2 pisteen välein. Jokainen piste
	 * käsitellään siis kaksi kertaa, ja lopullinen tulos interpoloidaan
	 * näiden vuorottelevien välitulosten välillä, jotta jatkuvuus säilyy.
	 * Toivottavasti ymmärsit. */

	/* Ensimmäinen kierros tehdään silmukan ulkopuolella, koska muuten
	 * tulosteen alkuun syntyy ylimääräistä hiljaisuutta, kun interpoloidaan
	 * nollasignaalista ensimmäiseen välitulokseen. */

	size_t nread = fread(input, sizeof(float), sample_length, stdin);

	fft(transform, input, sample_length);
	zero_intervals(transform, sample_length, intervals, num_intervals);
	inverse_fft(intermediate[1], transform, sample_length);
	for (size_t i = 0; i < sample_length / 2; i++)
		output[i] = intermediate[1][i];
	fwrite(output, sizeof(float), sample_length / 2, stdout);

	/* Loput tehdään silmukassa. Viimeinen kierros olisi ehkä myös syytä
	 * tehdä sen ulkopuolella, mutta ööööööh TODO */

	for (size_t i = 0; nread; i++) {
		for (size_t j = 0; j < sample_length / 2; j++)
			input[j] = input[j + sample_length / 2];

		nread = fread(input + sample_length / 2, sizeof(float), sample_length / 2, stdin);

		for (size_t j = sample_length / 2 + nread; j < sample_length; j++)
			input[j] = 0.0f;

		fft(transform, input, sample_length);
		zero_intervals(transform, sample_length, intervals, num_intervals);
		inverse_fft(intermediate[i & 1], transform, sample_length);
		interpolate_signal(output, sample_length / 2,
				intermediate[!(i & 1)] + sample_length / 2,
				intermediate[i & 1]);
		fwrite(output, sizeof(float), sample_length / 2, stdout);
	}
}
