#include <assert.h>
#include <complex.h>
#include <math.h>
#include <stdio.h>

#include "fft.c"

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
	printf("Assertion \"%s\" failed in test \"%s\".\n", string, _current_test_name);
}

/*
 * Testit.
 */

static float fft_test_input[8] = { 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f };
static float complex fft_test_desired_output[8] = {
	6.0f     + I * 0.0f,
	-0.7071f - I * 1.7071f,
	1.0f     - I * 1.0f,
	0.7071f  + I * 0.2929f,
	0.0f     + I * 0.0f,
	0.7071f  - I * 0.2929f,
	1.0f     + I * 1.0f,
	-0.7071f + I * 1.7071f,
};

/* Vertaa n-pituisten taulukoiden a ja b arvoja. */
static int
complex64_array_approx_equal(float complex *a, float complex *b, size_t n)
{
	static float epsilon = 0.0001f;

	for (size_t i = 0; i < n; i++)
		if (cabsf(a[i] - b[i]) >= epsilon)
			return 0;

	return 1;
}

static void
test_fft_recursive()
{
	float complex output[8];

	fft_recursive(output, fft_test_input, 8, 1);

	TEST_ASSERT(complex64_array_approx_equal(output, fft_test_desired_output, 8));
}

static void
test_fft_iterative()
{
	float complex output[8];

	fft_iterative(output, fft_test_input, 8);

	TEST_ASSERT(complex64_array_approx_equal(output, fft_test_desired_output, 8));
}

int
main()
{
	RUN_TEST(test_fft_recursive);
	RUN_TEST(test_fft_iterative);

	printf("%lu/%lu tests passed.\n", num_tests_passed, num_tests_run);

	if (num_tests_passed == num_tests_run)
		return 0;
	else
		return 1;
}
