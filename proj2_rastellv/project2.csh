#!/bin/csh

# number of threads:
foreach t (1 2 4 6 8)
    # number of nodes:
    foreach s (5 10 100 500 1500 3000 4500 7000)
        g++ -DNUMNODES=$s -DNUMT=$t project2.cpp -o project2 -lm -fopenmp
        ./project2
    end
end