#!/usr/bin/env bash

function to_string() {
	echo $1 | sed "s/\(.\)/'\1',/g" | rev | cut -c2- | rev
}

cat Crypto.asm \
	| sed 's/REVERSE \(.RX\)/REVERSE<\1>/g' \
	| sed 's/XOR \([^ ]*\) \([^ ]*\)/XOR<\1, \2>/' \
	| sed 's/MOV \([^ ]*\) \([^ ]*\)/MOV<\1, \2>/' \
	| sed 's/TRX/TRX_TAG/g' \
	| sed 's/DRX/DRX_TAG/g' \
	| while read line
do
	if echo $line | grep -q '"'
	then
		line_pref="$(echo $line | sed 's/".*//g')"
		string_literal="$(echo $line | sed 's/.*"\(.*\)".*/\1/')"
		line_suff="$(echo $line | sed 's/.*"//g')"
		echo "${line_pref}String<$(to_string $string_literal)>${line_suff}",
	else
		echo $line,
	fi
done | tr '\n' '|' | rev | cut -c3- | rev | tr '|' '\n' > preprocessed.h

g++ -std=c++2a emulator.cpp -O3 -S -o /dev/stdout | grep movb | awk '{print $2}' | tr -d '$' | tr -d ',' | while read c; do printf "%x" $c ; done | xxd -r -p

rm preprocessed.h
