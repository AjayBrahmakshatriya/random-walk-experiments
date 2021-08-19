source ../graphs.sh
for g in $graphs; do
	echo $g
	echo numactl -i all ./randomwalk_openmp $GAPBS_DIR/$g
	echo numactl -i all ./randomwalk_cilk $GAPBS_DIR/$g
done
	
