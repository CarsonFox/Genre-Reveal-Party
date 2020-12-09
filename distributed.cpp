#include <iostream>
#include <algorithm>

#include <mpi.h>

#include "common.hpp"

void kmeans(std::vector<DataPoint> data, int k);
bool assignCentroids(std::vector<DataPoint> &data, const std::vector<DataPoint> &centroids);
std::vector<DataPoint> newCentroids(const std::vector<DataPoint> &data, const std::vector<DataPoint> &oldCentroids);

int main(int argc, char **argv) {
    auto data = readCSV(argc, argv);
    kmeans(data, 4);

    return 0;
}

/*
 * This function changed a lot from the serial version.
 * The data is distributed among ranks, with rank 0 handling the remainder.
 * The centroids are shared and must be kept synchronized,
 * but the data is only gathered at the end.
 */
void kmeans(std::vector<DataPoint> data, int k) {
    //MPI variables
    int comm_size, rank;
    const auto comm = MPI_COMM_WORLD;

    //Initialize random centroids
    auto centroids = randomCentroids(data, k);

    //Start distributed section
    MPI_Init(nullptr, nullptr);
    MPI_Comm_size(comm, &comm_size);
    MPI_Comm_rank(comm, &rank);

    /*
     * Compute how much data each process gets, and how much of the
     * remainder rank 0 must handle.
     */
    auto chunkSize = data.size() / comm_size;
    auto chunkBytes = chunkSize * sizeof(DataPoint);
    auto remainder = data.size() % chunkSize;

    //Allocate memory to hold this processes data, then scatter
    std::vector<DataPoint> localData(chunkSize);
    MPI_Scatter(data.data(), chunkBytes, MPI_BYTE, localData.data(), chunkBytes, MPI_BYTE, 0, comm);

    //Handle remaining data
    if (rank == 0) {
        std::copy(data.end() - remainder, data.end(), std::back_inserter(localData));
    }

    /*
     * Main loop is unchanged from here, because it now
     * works on a subset of the data.
     */
    bool changed;
    int iterations = 0;

    do {
        centroids = newCentroids(localData, centroids);
        changed = assignCentroids(localData, centroids);
    } while (changed && iterations++ < max_iterations);

    //This is probably inefficient. All we need to know is which cluster each data point is assigned.
    MPI_Gather(localData.data(), chunkBytes, MPI_BYTE, data.data(), chunkBytes, MPI_BYTE, 0, comm);

    //Get the remainder back from the local data, and output the csv
    if (rank == 0) {
        std::copy(localData.end() - remainder, localData.end(), data.end() - remainder);
        std::cout << data;
    }

    MPI_Finalize();
}

/*
 * This is mostly unchanged. The only difference is that the processes
 * must communicate to decide whether to stop iterating.
 */
bool assignCentroids(std::vector<DataPoint> &data, const std::vector<DataPoint> &centroids) {
    int comm_size, rank;
    const auto comm = MPI_COMM_WORLD;
    MPI_Comm_size(comm, &comm_size);
    MPI_Comm_rank(comm, &rank);

    std::vector<double> distances(centroids.size(), 0.0);
    bool changed = false, localChanged = false;

    for (auto &&datum: data) {
        std::transform(centroids.begin(), centroids.end(), distances.begin(),
                       [&](const DataPoint &centroid){ return datum - centroid; });
        size_t min = std::distance(distances.begin(), std::min_element(distances.begin(), distances.end()));

        localChanged |= (min != datum.centroid);
        datum.centroid = min;
    }

    //Synchronize information about changed clusters
    MPI_Allreduce(&localChanged, &changed, 1, MPI_CXX_BOOL, MPI_LOR, comm);

    return changed;
}

/*
 * An attempt was made to parallelize this. It's probably the reason
 * this version doesn't scale well - there's too much overhead here.
 */
std::vector<DataPoint> newCentroids(const std::vector<DataPoint> &data, const std::vector<DataPoint> &oldCentroids) {
    int comm_size, rank;
    const auto comm = MPI_COMM_WORLD;
    MPI_Comm_size(comm, &comm_size);
    MPI_Comm_rank(comm, &rank);

    const auto k = oldCentroids.size();
    const auto byteCount = k * sizeof(DataPoint);

    std::vector<DataPoint> newCentroids(k);
    std::vector<int> counts(k, 0);

    /*
     * The sums can be computed in parallel,
     * since they don't depend on any other processes' data.
     */
    for (auto &&datum: data) {
        newCentroids[datum.centroid] += datum;
        counts[datum.centroid] += 1;
    }

    /*
     * Rank 0 receives all of the centroid sums and counts.
     * This should be done using a reduce operation, but I'm not working with the right shape of data.
     * The counts could be summed with a reduce operation, but not the DataPoint structs.
     */
    if (rank == 0) {
        std::vector<DataPoint> recvCentroids(k);
        std::vector<double> recvCounts(k);

        //For each process, receive their sums, and add them to the global sum.
        for (int src = 1; src < comm_size; src++) {
            MPI_Recv(recvCentroids.data(), byteCount, MPI_BYTE, src, src, comm, MPI_STATUS_IGNORE);
            MPI_Recv(recvCounts.data(), k, MPI_DOUBLE, src, src + comm_size, comm, MPI_STATUS_IGNORE);

            for (size_t i = 0; i < k; i++) {
                newCentroids[i] += recvCentroids[i];
                counts[i] += recvCounts[i];
            }
        }

        //Calculate the averages, same as the serial version.
        for (size_t i = 0; i < k; i++) {
            if (counts[i] == 0) {
                newCentroids[i] = randomDatum(data);
            } else {
                newCentroids[i] /= counts[i];
            }
        }
    } else {
        //Send this process' sums to rank 0
        MPI_Send(newCentroids.data(), byteCount, MPI_BYTE, 0, rank, comm);
        MPI_Send(counts.data(), k, MPI_DOUBLE, 0, rank + comm_size, comm);
    }

    //Send the new centroids to the other processes
    MPI_Bcast(newCentroids.data(), byteCount, MPI_BYTE, 0, comm);

    return newCentroids;
}
