#!/bin/bash 

cat $1 \
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
		echo "${line_pref}String<$(./to_string.sh $string_literal)>${line_suff}",
	else
		echo $line,
	fi
done | tr '\n' '|' | rev | cut -c3- | rev | tr '|' '\n'
