#!/usr/bin/bash -x

# > par_se.csv
# echo "array size,matrix size,total energy,avg energy,avg power,total latency,avg latency" >> par_se.csv
# for arr in {1..11}
# do
# 	a=$((2**$arr))
# 	for((mat=1;mat<=arr;mat++))
# 	do
# 		m=$((2**$mat))
# 		echo -n "$a," >> par_se.csv
# 		echo -n "$m," >> par_se.csv
# 		./mmul $m $m $m $m $a $a 1 1 &
# 		wait
# 		./crosspoint -f "mmultrace.bin" -r $a -c $a >> par_se.csv &
# 		wait
# 	done
# done

# > par_rc.csv
# echo "array size,matrix size,total energy,avg energy,avg power,total latency,avg latency" >> par_rc.csv
# for arr in {1..11}
# do
# 	a=$((2**$arr))
# 	for((mat=1;mat<=arr;mat++))
# 	do
# 		m=$((2**$mat))
# 		echo -n "$a," >> par_rc.csv
# 		echo -n "$m," >> par_rc.csv
# 		./mmul $m $m $m $m $a $a 1 0 &
# 		wait
# 		./crosspoint -f "mmultrace.bin" -r $a -c $a >> par_rc.csv &
# 		wait
# 	done
# done

# > nonpar_se.csv
# echo "array size,matrix size,total energy,avg energy,avg power,total latency,avg latency" >> nonpar_se.csv
# for arr in {7..14}
# do
# 	a=$((2**$arr))
# 	for((mat=1;mat<=arr-6;mat++))
# 	do
# 		m=$((2**$mat))
# 		echo -n "$a," >> nonpar_se.csv
# 		echo -n "$m," >> nonpar_se.csv
# 		./mmul $m $m $m $m $a $a 0 1 &
# 		wait
# 		./crosspoint -f "mmultrace.bin" -r $a -c $a >> nonpar_se.csv &
# 		wait
# 	done
# done

> nonpar_rc.csv
echo "array size,matrix size,total energy,avg energy,avg power,total latency,avg latency" >> nonpar_rc.csv
for arr in {7..14}
do
	a=$((2**$arr))
	for((mat=1;mat<=arr-6;mat++))
	do
		m=$((2**$mat))
		echo -n "$a," >> nonpar_rc.csv
		echo -n "$m," >> nonpar_rc.csv
		./mmul $m $m $m $m $a $a 0 0 &
		wait
		./crosspoint -f "mmultrace.bin" -r $a -c $a >> nonpar_rc.csv &
		wait
	done
done
