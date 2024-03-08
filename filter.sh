#!/bin/sh

input=$1
output=$2

shift 2

sample_rate=$(soxi -r "$input")

[ $? != 0 ] && exit 1
[ -z $sample_rate ] && echo "VIRHE: Tiedostolla $input on vaihteleva tai tuntematon näytetaajuus, \
eikä ohjelma pysty käsittelemään sitä." && exit 1

sox "$input" -c 1 -t f32 - | ./a.out "$@" | sox -c 1 -t f32 -r $sample_rate - "$output"
