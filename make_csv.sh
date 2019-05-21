#!/usr/bin/bash
> test.csv
echo "matrixsize,totalenergy,avgenergy,avgpower,totallatency,avglatency" >> test.csv
for value in {1..8}
do
	x=$((2**$value))
	echo -n "$x," >> test.csv
	./mmul $x $x $x $x
	./crosspoint -f "mmultrace.bin" >> test.csv
done