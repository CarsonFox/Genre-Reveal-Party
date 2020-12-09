# Genre-Reveal-Party

To build, run these commands in the project directory:
```
git clone https://github.com/vincentlaucsb/csv-parser
mkdir build && cd build
cmake ..
make
```

The cmake project expects the csv-parser library to be a subdirectory of the project root.

The programs will take as input the name of a csv file to process, and output to stdout.
Unfortunately the number of clusters is hardcoded, but it's easy to find.
There's a python script for visualizing the output called plot.py.

* The shared memory version is the fastest.
It only parallelizes the centroid assignment step; I couldn't find a way to parallelize the centroid update step without slowing down the computation.
A possible solution to this would be to sort the data by cluster, which could be parallelized.
Then the centroids could be updated in isolation, which would result in much better memory access patterns.
It's doubtful if the sort step would be worth it, though.

* The distributed memory version is slower than shared, but still faster than serial.
It always reaches the iteration limit though, so I'm definitely doing something wrong with reducing the booleans there.
I believe the slowdown is coming from the excessive sends/receives in the centroid update step.
This could be helped by refactoring my data structures into a kind that plays better with MPI's reduce functions.

* I ran out of time to work on the GPU implementaion.
It would have worked in a similar manner to the distributed version though:
The data and centroids would be stored in shared memory on the device, with synchronization of the centroids happening using global memory.
It would have been important to make sure the main loop could run entirely on the gpu, to avoid the overhead of copying data back and forth.

I couldn't get the programs to even compile on the CHPC clusters. Therefore I did a scaling experiment on my 6-core Intel i5-9600k.
The results of the study can be found in Timings.pdf.
The timings were averaged over 10 runs, using 2-6 cpus and the serial version as a baseline.

The speedups were underwhelming, especially for the MPI version.
The results of the shared memory version are probably due to only parallelizing half of the main loop.
The distributed memory version would probably be similar to the openMP version if I updated the centroids only on process 0.

There is example output run on a subset of the data, located in sample-output.png.
It's a scatter plot of duration vs danceability, two dimensions chosen randomly. The colors indicate which cluster a data point ended up in.
