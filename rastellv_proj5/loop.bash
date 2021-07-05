#!/bin/bash
#SBATCH -J CudaCarlo
#SBATCH -A cs475-575
#SBATCH -p class
#SBATCH --gres=gpu:1
#SBATCH -o cudaCarlo.out
#SBATCH -e cudaCarlo.err
#SBATCH --mail-type=BEGIN,END,FAIL
#SBATCH --mail-user=rastellv@oregonstate.edu
for t in 8 16 32 64 128 256 512 1024 
do
    for s in 2048 4096 8192 16384 32768 65536 131072 262144 524288 1048576 5242880 10485760 15728640 20971520
    do
        /usr/local/apps/cuda/cuda-10.1/bin/nvcc -DBLOCKSIZE=$t -DNUMTRIALS=$s -o cudaCarlo cudaCarlo.cu
        ./cudaCarlo
    done
done