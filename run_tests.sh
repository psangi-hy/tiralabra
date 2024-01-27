#!/bin/sh
[ -e *.gcda ] && rm *.gcda
gcc --coverage -fprofile-exclude-files=test\.c -o test test.c -lm \
	|| exit 1

./test
status=$?

echo ""
gcov test

exit $status
