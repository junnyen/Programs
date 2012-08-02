#!/bin/bash

test -z "$1" && {
	echo "Usage: $0 sleeplog.txt"
	exit 23
}

#Count events:
seconds='60 300'
for i in $seconds; do
	./sleepcount.sh "$1" "$i" &
done

#Approximate size of graph
size="$(tail -n 1 "$1" | cut -d . -f 1)"
test $size -gt 3600 && size="$(( $size/10 ))" #For prolonged periods
size="$(( 600 + $size ))"

#Plot it
gnuplot << EOF
set output "$1.png"
set terminal png size $size,300

set title "Sleep motions"
set xlabel "time"
set ylabel "motion"

set yrange [-0.5:3]

set xdata time
set timefmt "%s"
#set format x "%H:%M:%S"

set grid

#set pointsize 0.5
#"$1.counts.$seconds" using 1:(\$2/5) title "Activations in last $seconds seconds" smooth csplines
plot "$1" using 2:5 title "Sensor state" with steps,\
"$1.counts.60" using 1:(\$2/5) title "Activations in last 60 seconds" smooth csplines,\
"$1.counts.300" using 1:(\$2/10) title "Activations in last 300 seconds" smooth csplines\

EOF




