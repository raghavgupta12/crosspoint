#!/bin/bash -x

> par_se.csv
echo "array size,matrix size,total energy,avg energy,avg power,total latency,avg latency,num ops,num half sel,num full sel,half sel energy,full sel energy" >> par_se.csv
for arr in {1..11}
do
	a=$((2**$arr))
	for((mat=1;mat<=arr;mat++))
	do
		m=$((2**$mat))
		echo -n "$a," >> par_se.csv
		echo -n "$m," >> par_se.csv
		./mmul $m $m $m $m $a $a 1 1 &
		wait
		./crosspoint -p 64 -f "mmultrace.bin" -r $a -c $a >> par_se.csv &
		wait
	done
done

# > par_rc.csv
# echo "array size,matrix size,total energy,avg energy,avg power,total latency,avg latency,num ops,num half sel,num full sel,half sel energy,full sel energy" >> par_rc.csv
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
# 		./crosspoint -p 64 -f "mmultrace.bin" -r $a -c $a >> par_rc.csv &
# 		wait
# 	done
# done

# > nonpar_se.csv
# echo "array size,matrix size,total energy,avg energy,avg power,total latency,avg latency,num ops,num half sel,num full sel,half sel energy,full sel energy" >> nonpar_se.csv
# for arr in {7..17}
# do
# 	a=$((2**$arr))
# 	for((mat=1;mat<=arr-3;mat++))
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

# > nonpar_rc.csv
# echo "array size,matrix size,total energy,avg energy,avg power,total latency,avg latency,num ops,num half sel,num full sel,half sel energy,full sel energy" >> nonpar_rc.csv
# for arr in {7..17}
# do
# 	a=$((2**$arr))
# 	for((mat=1;mat<=arr-6;mat++))
# 	do
# 		m=$((2**$mat))
# 		echo -n "$a," >> nonpar_rc.csv
# 		echo -n "$m," >> nonpar_rc.csv
# 		./mmul $m $m $m $m $a $a 0 0 &
# 		wait
# 		./crosspoint -f "mmultrace.bin" -r $a -c $a >> nonpar_rc.csv &
# 		wait
# 	done
# done

# > par_sweep_se.csv
# echo "num par arr,array size,matrix size,total energy,avg energy,avg power,total latency,avg latency,num ops,num half sel,num full sel,half sel energy,full sel energy" >> par_sweep_se.csv
# for((mat=1;mat<11;mat++))
# do
# 	m=$((2**$mat))
# 	for((par=1;par<=6;par++))
# 	do
# 		p=$((2**$par))
# 		z=$((64 / p))
# 		a=$((z * m))
# 		echo -n "$p," >> par_sweep_se.csv
# 		echo -n "$m x $a," >> par_sweep_se.csv
# 		echo -n "$m," >> par_sweep_se.csv
# 		./mmul $m $m $m $m $m $a $p 1 &
# 		wait
# 		./crosspoint -p $p -f "mmultrace.bin" -r $m -c $a >> par_sweep_se.csv
# 		wait
# 	done
# 	# for((par=6;par<=9;par++))
# 	# do
# 	# 	p=$((2**$par))
# 	# 	y=$((p / 64))
# 	# 	if[ $m -lt $y ]
# 	# 	then

# 	# 	fi
# 	# done
# done

# > par_sweep_rc.csv
# echo "num par arr,array size,matrix size,total energy,avg energy,avg power,total latency,avg latency,num ops,num half sel,num full sel,half sel energy,full sel energy" >> par_sweep_rc.csv
# for((mat=1;mat<11;mat++))
# do
# 	m=$((2**$mat))
# 	for((par=1;par<=6;par++))
# 	do
# 		p=$((2**$par))
# 		z=$((64 / p))
# 		a=$((z * m))
# 		echo -n "$p," >> par_sweep_rc.csv
# 		echo -n "$m x $a," >> par_sweep_rc.csv
# 		echo -n "$m," >> par_sweep_rc.csv
# 		./mmul $m $m $m $m $m $a $p 0 &
# 		wait
# 		./crosspoint -p $p -f "mmultrace.bin" -r $m -c $a >> par_sweep_rc.csv
# 		wait
# 	done
# done

# > par_sweep_rc2.csv
# echo "num par arr,array size,matrix size,total energy,avg energy,avg power,total latency,avg latency,num ops,num half sel,num full sel,half sel energy,full sel energy" >> par_sweep_rc2.csv
# for((mat=1;mat<11;mat++))
# do
# 	m=$((2**$mat))
# 	for((par=1;par<=9;par++))
# 	do
# 		p=$((2**$par))
# 		z=$((64 / p))
# 		a=$((8 * m))
# 		echo -n "$p," >> par_sweep_rc2.csv
# 		echo -n "$a x $a," >> par_sweep_rc2.csv
# 		echo -n "$m," >> par_sweep_rc2.csv
# 		./mmul $m $m $m $m $a $a $p 0 &
# 		wait
# 		./crosspoint -p $p -f "mmultrace.bin" -r $a -c $a >> par_sweep_rc2.csv
# 		wait
# 	done
# done