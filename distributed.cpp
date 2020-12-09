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

void kmeans(std::vector<DataPoint> data, int k) {
    //MPI variables
    int comm_size, rank;
    const auto comm = MPI_COMM_WORLD;

    auto centroids = randomCentroids(data, k);

    MPI_Init(nullptr, nullptr);
    MPI_Comm_size(comm, &comm_size);
    MPI_Comm_rank(comm, &rank);

    auto chunkSize = data.size() / comm_size;
    auto chunkBytes = chunkSize * sizeof(DataPoint);
    auto remainder = data.size() % chunkSize;

    std::vector<DataPoint> localData(chunkSize);

    MPI_Scatter(data.data(), chunkBytes, MPI_BYTE, localData.data(), chunkBytes, MPI_BYTE, 0, comm);

    if (rank == 0) {
        std::copy(data.end() - remainder, data.end(), std::back_inserter(localData));
    }

    bool changed;
    int iterations = 0;

    do {
        centroids = newCentroids(localData, centroids);
        changed = assignCentroids(localData, centroids);
    } while (changed && iterations++ < max_iterations);

    MPI_Gather(localData.data(), chunkBytes, MPI_BYTE, data.data(), chunkBytes, MPI_BYTE, 0, comm);

    if (rank == 0) {
        std::copy(localData.end() - remainder, localData.end(), data.end() - remainder);
        std::cout << data;
    }

    MPI_Finalize();
}

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

    MPI_Allreduce(&localChanged, &changed, 1, MPI_CXX_BOOL, MPI_LOR, comm);

    return changed;
}

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
     * Each new centroid is the geometric mean of its data
     */
    for (auto &&datum: data) {
        newCentroids[datum.centroid] += datum;
        counts[datum.centroid] += 1;
    }

    if (rank == 0) {
        std::vector<DataPoint> recvCentroids(k);
        std::vector<double> recvCounts(k);
        for (int src = 1; src < comm_size; src++) {
            MPI_Recv(recvCentroids.data(), byteCount, MPI_BYTE, src, src, comm, MPI_STATUS_IGNORE);
            MPI_Recv(recvCounts.data(), k, MPI_DOUBLE, src, src + comm_size, comm, MPI_STATUS_IGNORE);

            for (size_t i = 0; i < k; i++) {
                newCentroids[i] += recvCentroids[i];
                counts[i] += recvCounts[i];
            }
        }

        for (size_t i = 0; i < k; i++) {
            if (counts[i] == 0) {
                newCentroids[i] = randomDatum(data);
            } else {
                newCentroids[i] /= counts[i];
            }
        }
    } else {
        MPI_Send(newCentroids.data(), byteCount, MPI_BYTE, 0, rank, comm);
        MPI_Send(counts.data(), k, MPI_DOUBLE, 0, rank + comm_size, comm);
    }

    MPI_Bcast(newCentroids.data(), byteCount, MPI_BYTE, 0, comm);

    return newCentroids;
}
