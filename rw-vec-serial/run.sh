source ../graphs.sh

for g in $graphs; do
	echo $g
	./randomwalk $GAPBS_DIR/$g
done
	
