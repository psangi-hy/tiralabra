#include <assert.h>
#include <complex.h>
#include <math.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define TEST_BUILD
#define die(...) do { _die_flag = 1; return 0; } while (0)

static int _die_flag = 0;

#include "fft.c"
#include "application.c"

#define LENGTH(A) (sizeof(A) / sizeof((A)[0]))

/* Hylkää testin, jos ehdon A arvo on epätosi. */
#define TEST_ASSERT(A) _test_assert(A, #A)

/* Suorittaa funktiossa T määritellyn testin. */
#define RUN_TEST(T) _run_test(T, #T)

static size_t num_tests_run = 0;
static size_t num_tests_passed = 0;

/* Testien suorittamisen apufunktioiden omistamat globaalit muuttujat. */
static int _current_test_passed;
static char *_current_test_name = 0;

/*
 * Testien suorittamisen apufunktiot.
 */

/* Kutsutaan RUN_TEST-makron kautta. */
static void
_run_test(void (*test_function)(void), char *test_name)
{
	num_tests_run++;
	_current_test_passed = 1;
	_current_test_name = test_name;

	test_function();

	_current_test_name = 0;
	num_tests_passed += _current_test_passed;
}

/* Kutsutaan TEST_ASSERT-makron kautta. */
static void
_test_assert(int value, char *string)
{
	assert(_current_test_name);

	if (value)
		return;

	_current_test_passed = 0;
	printf("\"%s\" epäonnistui testissä \"%s\".\n", string, _current_test_name);
}

/* Kertoo, onko ohjelma kutsunut die-funktiota sen
 * jälkeen, kun se viimeksi kutsui tätä funktiota. */
static int
reset_die_flag()
{
	int res = _die_flag;
	_die_flag = 0;
	return res;
}

/*
 * Testit.
 */

/* Funktion (1, 1, 0, 0, 0, 0, 0, 0) dft. */
static float complex fft_test_amplitudes_1[8] = {
	6.0f     + I * 0.0f,
	-0.7071f - I * 1.7071f,
	1.0f     - I * 1.0f,
	0.7071f  + I * 0.2929f,
	0.0f     + I * 0.0f,
	0.7071f  - I * 0.2929f,
	1.0f     + I * 1.0f,
	-0.7071f + I * 1.7071f,
};

/* Yhden pisteen funktion dft on funktio itse. */
static float complex fft_test_amplitudes_2[1] = { 8.0f };

/* Nollafunktion dft on nollafunktio. */
static float complex fft_test_amplitudes_3[4096];

/* Vakiofunktion dft. */
static float complex fft_test_amplitudes_4[4096] = {
	[0] = 8000.0f,
};

/* Reaalisen funktion dft. */
static float complex fft_test_amplitudes_5[4096] = {
	[200] = 2000.0f,
	[300] = 1400.0f + I * 1400.0f,
	[400] = I * 2000.0f,
	[3696] = -I * 2000.0f,
	[3796] = 1400.0f - I * 1400.0f,
	[3896] = 2000.0f,
};

/* Vertaa n-pituisten taulukoiden a ja b arvoja.
 * Pidemmällä taulukolla sallitaan suurempi virhe. */
static int
complex64_array_approx_equal(float complex *a, float complex *b, size_t n)
{
	float epsilon = 0.000244140625 * n;

	for (size_t i = 0; i < n; i++)
		if (cabsf(a[i] - b[i]) >= epsilon)
			return 0;

	return 1;
}

/* Vertaa n-pituisten taulukoiden a ja b arvoja.
 * Pidemmällä taulukolla sallitaan suurempi virhe. */
static int
real32_array_approx_equal(float *a, float *b, size_t n)
{
	float epsilon = 0.000244140625 * n;

	for (size_t i = 0; i < n; i++)
		if (fabsf(a[i] - b[i]) >= epsilon)
			return 0;

	return 1;
}

/* Muodostaa signaalin dft-komponenteista suoraan
 * sini- ja kosinifunktioiden avulla. Tuloksen
 * pitäisi olla sama kuin funktiolla inverse_fft. */
static float *
construct_signal(float complex *amplitudes, size_t n)
{
	float *signal = calloc(n, sizeof(float));

	for (size_t i = 0; i < n; i++)
		signal[i] = cabsf(amplitudes[0]) / n;

	for (size_t i = 1; i < n; i++) {
		float complex amp = amplitudes[i];
		float dx = -(2 * M_PI * i) / n; /* Miksi tämän on oltava negatiivinen? */

		for (size_t j = 0; j < n; j++) {
			float x = j * dx;
			signal[j] += (crealf(amp) * cosf(x) + cimagf(amp) * sinf(x)) / n;
		}
	}

	return signal;
}

/* Funktion test_fft apufunktio. */
#define TEST_FFT(A) _test_fft((A), LENGTH(A))
static int
_test_fft(float complex *amplitudes, size_t n)
{
	float *signal = construct_signal(amplitudes, n);
	float complex *transform = calloc(n, sizeof(float complex));

	fft(transform, signal, n);

	int res = complex64_array_approx_equal(amplitudes, transform, n);

	free(signal);
	free(transform);

	return res;
}

/* Testaa, että testisyötteistä rakennetuista signaaleista
 * saadut fft:t ovat suunnilleen yhtä kuin testisyötteet. */
static void
test_fft()
{
	TEST_ASSERT(TEST_FFT(fft_test_amplitudes_1));
	TEST_ASSERT(TEST_FFT(fft_test_amplitudes_2));
	TEST_ASSERT(TEST_FFT(fft_test_amplitudes_3));
	TEST_ASSERT(TEST_FFT(fft_test_amplitudes_4));
	TEST_ASSERT(TEST_FFT(fft_test_amplitudes_5));
}

/* Funktion test_inverse_fft apufunktio. */
#define TEST_INVERSE_FFT(A) _test_inverse_fft((A), LENGTH(A))
static int
_test_inverse_fft(float complex *amplitudes, size_t n)
{
	float *reference_transform = construct_signal(amplitudes, n);
	float *transform = calloc(n, sizeof(float));

	inverse_fft(transform, amplitudes, n);

	int res = real32_array_approx_equal(reference_transform, transform, n);

	free(reference_transform);
	free(transform);
	
	return res;
}

/* Testaa, että testisyötteistä saadut käänteiset fft:t ovat
 * suunnilleen yhtä kuin niistä triviaalisti rakennetut signaalit. */
static void
test_inverse_fft()
{
	TEST_ASSERT(TEST_INVERSE_FFT(fft_test_amplitudes_1));
	TEST_ASSERT(TEST_INVERSE_FFT(fft_test_amplitudes_2));
	TEST_ASSERT(TEST_INVERSE_FFT(fft_test_amplitudes_3));
	TEST_ASSERT(TEST_INVERSE_FFT(fft_test_amplitudes_4));
	TEST_ASSERT(TEST_INVERSE_FFT(fft_test_amplitudes_5));
}

static void
test_is_power_of_2()
{
	TEST_ASSERT(!is_power_of_2(0));
	TEST_ASSERT(is_power_of_2(1));
	TEST_ASSERT(is_power_of_2(32));
	TEST_ASSERT(!is_power_of_2(68));
	TEST_ASSERT(!is_power_of_2(477883288410294UL));
	TEST_ASSERT(is_power_of_2(1UL << 63));
}

static void
test_reverse_bits()
{
	TEST_ASSERT(reverse_bits(1, 1) == 1);
	TEST_ASSERT(reverse_bits(2, 1) == 0);
	TEST_ASSERT(reverse_bits(6, 8) == 96);
	TEST_ASSERT(reverse_bits(SIZE_MAX, 64) == SIZE_MAX);
	TEST_ASSERT(reverse_bits(1, 64) == 1UL << 63);
}

static void
test_parse_size()
{
	TEST_ASSERT(parse_size("0") == 0 && !reset_die_flag());
	TEST_ASSERT(parse_size("999") == 999);
	TEST_ASSERT(parse_size("232339923") == 232339923);
	TEST_ASSERT(parse_size("hello") == 0 && reset_die_flag());
	TEST_ASSERT(parse_size("20000k") == 0 && reset_die_flag());
}

static void
test_interpolate_signal()
{
	static float zero_array[128];
	static float input[128];
	static float output[128];

	for (size_t i = 0; i < LENGTH(input); i++)
		input[i] = (float)i;

	/* Interpolaatio pituudella 0 ei tee mitään. */
	interpolate_signal(output, 0, input, input);
	TEST_ASSERT(real32_array_approx_equal(output, zero_array, LENGTH(output)));

	/* Interpolaatio signaalista itseensä tuottaa saman funktion. */
	interpolate_signal(output, LENGTH(output), input, input);
	TEST_ASSERT(real32_array_approx_equal(output, input, LENGTH(output)));

	/* Tulosfunktio on jokaisessa pisteessä syötefunktioiden välissä. */
	interpolate_signal(output, LENGTH(output), zero_array, input);

	for (size_t i = 0; i < LENGTH(output); i++)
		TEST_ASSERT(output[i] >= zero_array[i] && output[i] <= input[i]);

	/* Interpolaatio vakiofunktioiden välillä on lineaarinen interpolaatio vakiosta toiseen. */
	for (size_t i = 0; i < LENGTH(input); i++)
		input[i] = 1.0f;

	interpolate_signal(output, LENGTH(output), zero_array, input);

	float step = 1.0f / LENGTH(output);
	for (size_t i = 0; i < LENGTH(output); i++)
		TEST_ASSERT(fabsf(output[i] - (step / 2 + step * i)) < 0.0001f);
}

int
main()
{
	RUN_TEST(test_fft);
	RUN_TEST(test_inverse_fft);
	RUN_TEST(test_is_power_of_2);
	RUN_TEST(test_reverse_bits);
	RUN_TEST(test_parse_size);
	RUN_TEST(test_interpolate_signal);

	printf("%lu/%lu testiä onnistui.\n", num_tests_passed, num_tests_run);

	if (num_tests_passed == num_tests_run)
		return 0;
	else
		return 1;
}
