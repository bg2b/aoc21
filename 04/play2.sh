#!/bin/bash
input=$1

rm -f x* nums
head -1 < $input | tr , ' ' > nums
tail -n +3 < $input | split -a 3 -d -l 6

winning_step=0
winning_score=0

for board in x*; do
    output=$( cat $board nums | ./doit1 )
    if [[ ! -z $output ]]; then
	step=$( echo $output | awk '{ print $1; }' )
	score=$( echo $output | awk '{ print $2; }' )
	if (( $step > $winning_step )); then
	    winning_step=$step
	    winning_score=$score
	fi
    fi
done

rm -f x* nums

echo $winning_score
