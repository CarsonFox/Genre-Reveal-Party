#include <iostream>
#include <algorithm>

#include <mpi.h>

#include "common.hpp"

std::vector<DataPoint> kmeans(std::vector<DataPoint> data, int k);
bool assignCentroids(std::vector<DataPoint> &data, const std::vector<DataPoint> &centroids);
std::vector<DataPoint> newCentroids(const std::vector<DataPoint> &data, const std::vector<DataPoint> &oldCentroids);

int main(int argc, char **argv) {
    auto data = readCSV(argc, argv);
    data = kmeans(data, 4);

    std::cout << data;

    return 0;
}

std::vector<DataPoint> kmeans(std::vector<DataPoint> data, int k) {
    auto centroids = randomCentroids(data, k);

    bool changed;
    int iterations = 0;

    do {
        centroids = newCentroids(data, centroids);
        changed = assignCentroids(data, centroids);
    } while (changed && iterations++ < max_iterations);

    return data;
}

bool assignCentroids(std::vector<DataPoint> &data, const std::vector<DataPoint> &centroids) {
    //MPI variables
    int comm_size, rank;
    const auto comm = MPI_COMM_WORLD;

    MPI_Init(nullptr, nullptr); {
        MPI_Comm_size(comm, &comm_size);
        MPI_Comm_rank(comm, &rank);

        int dataSize, k;

        if (rank == 0) {
            dataSize = data.size();
            k = centroids.size();
        }

        MPI_Bcast(&dataSize, 1, MPI_INT, 0, comm);
        MPI_Bcast(&k, 1, MPI_INT, 0, comm);

        int chunkSize = dataSize / comm_size;
        std::vector<DataPoint> localData(chunkSize);

        MPI_Finalize();
    }

    std::vector<double> distances(centroids.size(), 0.0);
    bool changed = false;

    for (auto &&datum: data) {
        std::transform(centroids.begin(), centroids.end(), distances.begin(),
                       [&](const DataPoint &centroid){ return datum - centroid; });
        size_t min = std::distance(distances.begin(), std::min_element(distances.begin(), distances.end()));

        changed = changed || (min != datum.centroid);
        datum.centroid = min;
    }

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
