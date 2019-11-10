make clean
make all
rm ./log.txt
srun -psc -n10 ./mpi ./small_tif_test/ ./csv/
squeue