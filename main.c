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

int
main(int argc, char **argv)
{
	/* Komentoriviargumenttien käsittely. */

	argv0 = argv[0];

	if (argc < 2)
		die(0);

	char *end;

	size_t sample_length = parse_size(argv[1]);

	if (!is_power_of_2(sample_length))
		die("%s ei ole kahden potenssi.\n", argv[1]);

	size_t num_intervals = ((argc - 2) + 1) / 2;
	struct interval *intervals = calloc(num_intervals, sizeof(struct interval));

	for (size_t i = 0; i < num_intervals; i++) {
		intervals[i].start = parse_size(argv[i * 2 + 2]);

		if (argc > i * 2 + 3)
			intervals[i].end = parse_size(argv[i * 2 + 3]);
		else
			intervals[i].end = sample_length;
	}

	/* Signaalinkäsittelysilmukka. */

	float *buf = calloc(sample_length, sizeof(float));
	float complex *transform = calloc(sample_length, sizeof(float complex));

	for (size_t nread; nread = fread(buf, sizeof(float), sample_length, stdin);) {
		for (size_t i = nread; i < sample_length; i++)
			buf[i] = 0.0f;

		fft(transform, buf, sample_length);

		for (size_t i = 0; i < num_intervals; i++) {
			for (size_t j = intervals[i].start; j < intervals[i].end; j++) {
				transform[j] = 0.0f;
			}
		}

		inverse_fft(buf, transform, sample_length);

		fwrite(buf, sizeof(float), sample_length, stdout);
	}
}
