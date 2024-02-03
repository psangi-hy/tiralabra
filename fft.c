static int
is_power_of_2(size_t n)
{
	/* Totta joss luvussa n on täsmälleen yksi ykkösbitti. */
	return n && !(n & (n - 1));
}

static int
trailing_zeroes(size_t n)
{
	return __builtin_ctz(n);
}

/* Palauttaa arvon, jonka alimmat num_bits bittiä ovat arvon bits alimmat
 * num_bits bittiä käänteisessä järjestyksessä. */
static size_t
reverse_bits(size_t bits, int num_bits)
{
	size_t res = 0;

	int dist = num_bits - 1;
	int b1 = 1;
	int b2 = 1 << dist;

	while (dist > 0) {
		res |= (bits & b1) << dist;
		res |= (bits & b2) >> dist;
		dist -= 2;
		b1 <<= 1;
		b2 >>= 1;
	}

	if (b1 == b2) {
		res |= bits & b1;
	}

	return res;
}

static void
fft_in_place(float complex *data, size_t n) {
	for (size_t m = 1; m <= n; m <<= 1) {
		float complex om = cexpf(I * -2 * M_PI / m);

		for (size_t j = 0; j < n; j += m) {
			float complex o = (float complex)1.0f;

			for (size_t k = 0; k < m / 2; k++) {
				float complex t = o * data[j + k + m / 2];
				float complex u = data[j + k];

				data[j + k] = u + t;
				data[j + k + m / 2] = u - t;

				o *= om;
			}
		}
	}
}

/* Suorittaa diskreetin Fourier-muunnoksen n-pituiseen
 * taulukkoon src, ja tallettaa tuloksen taulukkoon dest.
 * Luvun n on oltava kahden potenssi.*/
static void
fft(float complex *dest, float *src, size_t n)
{
	assert(is_power_of_2(n));

	int log_n = trailing_zeroes(n);

	for (size_t i = 0; i < n; i++) {
		dest[reverse_bits(i, log_n)] = (float complex)src[i];
	}

	fft_in_place(dest, n);
}

/* Muuntaa Fourier-muunnoksen n-pituisesta taulukosta src
 * takaisin signaaliksi, ja tallettaa sen taulukkoon dest.
 * Luvun n on oltava kahden potenssi.*/
static void
inverse_fft(float *dest, float complex *src, size_t n)
{
	assert(is_power_of_2(n));

	int log_n = trailing_zeroes(n);

	for (size_t i = 0; i < n; i++) {
		size_t rev = reverse_bits(i, log_n);

		if (rev > i) {
			float complex tmp = src[i];
			src[i] = src[rev];
			src[rev] = tmp;
		}

		src[i] = conjf(src[i]);
	}

	fft_in_place(src, n);

	for (size_t i = 0; i < n; i++) {
		dest[i] = crealf(src[i]) / n;
	}
}
