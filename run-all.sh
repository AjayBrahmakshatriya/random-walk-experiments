#versions="rw-vec-parallel rw-vec-parallel-condense rw-outer-parallel-no-atomics rw-fused-atomics-parallel rw-hashmap-condensed"
versions="rw-vec-parallel-condense rw-hashmap-condensed rw-bfs rw-bfs-per-node"
for dir in $versions; do
    echo $dir
    (cd $dir && make) > /dev/null
done
#for dir in $versions; do
#    echo $dir
#    (cd $dir && bash run.sh)
#done