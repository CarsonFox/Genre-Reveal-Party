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

    bool changed;
    int iterations = 0;

    MPI_Init(nullptr, nullptr);
    MPI_Comm_size(comm, &comm_size);
    MPI_Comm_rank(comm, &rank);

    do {
        if (rank == 0) {
            centroids = newCentroids(data, centroids);
        }
        MPI_Barrier(comm);
        changed = assignCentroids(data, centroids);
    } while (changed && iterations++ < max_iterations);

//    if (rank == 0)
//        std::cout << data;

    MPI_Finalize();
}

bool assignCentroids(std::vector<DataPoint> &data, const std::vector<DataPoint> &centroids) {
    //MPI variables
    int comm_size, rank;
    const auto comm = MPI_COMM_WORLD;

    MPI_Comm_size(comm, &comm_size);
    MPI_Comm_rank(comm, &rank);

    bool changed = false;
    int dataSize, k;

    if (rank == 0) {
        dataSize = data.size();
        k = centroids.size();
    }

    MPI_Bcast(&dataSize, 1, MPI_INT, 0, comm);
    MPI_Bcast(&k, 1, MPI_INT, 0, comm);

    const auto chunkSize = dataSize / comm_size;
    const auto chunkBytes = chunkSize * sizeof(DataPoint);

    std::vector<DataPoint> localData(chunkSize);
    std::vector<DataPoint> localCentroids(k);

    MPI_Scatter(data.data(), chunkBytes, MPI_BYTE, localData.data(), chunkBytes, MPI_BYTE, 0, comm);
    MPI_Bcast(localCentroids.data(), sizeof(DataPoint) * k, MPI_BYTE, 0, comm);

    size_t remainder;
    if (rank == 0) {
        remainder = data.size() - (chunkSize * comm_size);
        std::copy(data.begin() + remainder, data.end(), std::back_inserter(localData));
    }

    std::vector<double> distances(k, 0.0);
    bool localChanged = false;

    for (auto &&datum: localData) {
        std::transform(centroids.begin(), centroids.end(), distances.begin(),
                       [&](const DataPoint &centroid){ return datum - centroid; });
        size_t min = std::distance(distances.begin(), std::min_element(distances.begin(), distances.end()));

        localChanged |= (min != datum.centroid);
        datum.centroid = min;
    }

    MPI_Gather(localData.data(), chunkBytes, MPI_BYTE, data.data(), chunkBytes, MPI_BYTE, 0, comm);

    if (rank == 0) {
        std::copy(localData.end() - remainder, localData.end(), data.end() - remainder);
    }

    MPI_Allreduce(&localChanged, &changed, 1, MPI_CXX_BOOL, MPI_LOR, comm);

    MPI_Barrier(comm);

    return changed;
}

std::vector<DataPoint> newCentroids(const std::vector<DataPoint> &data, const std::vector<DataPoint> &oldCentroids) {
    std::vector<DataPoint> newCentroids(oldCentroids.size());
    std::vector<double> counts(oldCentroids.size(), 0.0);

    /*
     * Each new centroid is the geometric mean of its data
     */
    for (auto &&datum: data) {
        newCentroids[datum.centroid] += datum;
        counts[datum.centroid] += 1.0;
    }
    for (size_t i = 0; i < newCentroids.size(); i++) {
        if (counts[i] == 0.0) {
            newCentroids[i] = randomDatum(data);
        } else {
            newCentroids[i] /= counts[i];
        }
    }

    return newCentroids;
}
