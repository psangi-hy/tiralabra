#!/bin/sh

input=$1
output=$2

shift 2

sample_rate=$(soxi -r "$input")

sox "$input" -c 1 -t f32 - | ./a.out "$@" | sox -c 1 -t f32 -r $sample_rate - "$output"
