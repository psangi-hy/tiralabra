static int
is_power_of_2(size_t n)
{
	/* Totta joss luvussa n on täsmälleen yksi ykkösbitti. */
	return n && !(n & (n - 1));
}

/* Lukee taulukon src joka step:nnestä arvosta ensimmäiset n kappaletta,
 * suorittaa näihin diskreetin Fourier-muunnoksen ja tallettaa tuloksena
 * saadut arvot peräkkäin taulukkoon dest. Luvun n on oltava 2:n potenssi. */
static void
fft_recursive(float complex *dest, float *src, size_t n, size_t step)
{
	/* Algoritmi perustuu pseudokoodiin, joka löytyy sivulta
	 * https://en.wikipedia.org/wiki/Cooley%E2%80%93Tukey_FFT_algorithm */

	assert(is_power_of_2(n));

	if (n == 1) {
		dest[0] = (float complex)src[0];
		return;
	}

	size_t half = n / 2;

	fft_recursive(dest,        src,        half, step * 2);
	fft_recursive(dest + half, src + step, half, step * 2);

	for (size_t i = 0; i < half; i++) {
		float complex p = dest[i];
		float complex q = dest[i + half]
			* cexpf(I * -2 * M_PI / n * i);

		dest[i] = p + q;
		dest[i + half] = p - q;
	}
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

/* Suorittaa taulukon src ensimmäiseen n arvoon diskreetin Fourier-muunnoksen,
 * ja tallettaa tuloksen taulukkoon dest. Luvun n on oltava 2:n potenssi. */
static void
fft_iterative(float complex *dest, float *src, size_t n)
{
	/* Perustuu pseudokoodiin, joka löytyy sivulta
	 * https://en.wikipedia.org/wiki/Cooley%E2%80%93Tukey_FFT_algorithm */

	assert(is_power_of_2(n));

	int log_n = trailing_zeroes(n);

	for (size_t i = 0; i < n; i++) {
		dest[reverse_bits(i, log_n)] = (float complex)src[i];
	}

	for (size_t i = 0; i <= log_n; i++) {
		size_t m = (size_t)1 << i;
		float complex om = cexpf(I * -2 * M_PI / m);

		for (size_t j = 0; j < n; j += m) {
			float complex o = (float complex)1.0f;

			for (size_t k = 0; k < m / 2; k++) {
				float complex t = o * dest[j + k + m / 2];
				float complex u = dest[j + k];

				dest[j + k] = u + t;
				dest[j + k + m / 2] = u - t;

				o *= om;
			}
		}
	}
}
