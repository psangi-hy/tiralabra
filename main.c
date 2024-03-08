#include <assert.h>
#include <complex.h>
#include <math.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "fft.c"
#include "application.c"

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

	/* Loput ohjelmasta. */
	process_input(sample_length, intervals, num_intervals);
}
