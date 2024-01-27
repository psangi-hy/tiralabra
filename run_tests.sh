#!/bin/sh
rm *.gcda
gcc --coverage -fprofile-exclude-files=test\.c -o test test.c -lm
status=$?

./test
echo ""
gcov test

exit $status
