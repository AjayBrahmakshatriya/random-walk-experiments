source ../graphs.sh

for g in $graphs; do
	echo $g
	numactl -i all ./randomwalk_openmp $GAPBS_DIR/$g
	numactl -i all ./randomwalk_cilk $GAPBS_DIR/$g
done
	
