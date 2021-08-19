source ../graphs.sh
for w in randomwalk_*; do
echo $w
for g in $graphs; do
	echo $g
	numactl -i all ./$w $GAPBS_DIR/$g 20000 1000 6
done
done
