#!/bin/csh

#number of threads:
foreach t (1 2 4 6 8)
    # number of trials:
    foreach s (1 10 100 1000 10000 100000 500000 1000000)
        g++ -DNUMTRIALS=$s -DNUMT=$t project1.cpp -o project1 -lm -fopenmp
        ./project1
    end
end
