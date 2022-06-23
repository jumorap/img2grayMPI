#!/bin/bash
> results.txt
touch results.txt
for img in 720p 1080p 4k
do
echo "----------------------------------------" >> results.txt
echo "Running $img test with 1 2 4 8 nodes"
for nodes in 1 2 4 8
do
echo "Running $img test with $nodes nodes"
echo -e "\n>> $img test with $nodes nodes" >> results.txt
mpirun -np ${nodes} --hostfile mpi_hosts ./gray ./Images/${img}.png ./Output/${img}_"${nodes}"nds.png >> results.txt
done
done
