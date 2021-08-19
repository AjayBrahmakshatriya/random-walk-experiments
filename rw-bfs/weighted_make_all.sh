source ../graphs.sh
for w in $weightings; do
    make WEIGHT=$w
done