#include <iostream>
#include <algorithm>

#include "common.hpp"

std::vector<DataPoint> kmeans(std::vector<DataPoint> data, int k);
bool assignCentroids(std::vector<DataPoint> &data, const std::vector<DataPoint> &centroids);
std::vector<DataPoint> newCentroids(const std::vector<DataPoint> &data, const std::vector<DataPoint> &oldCentroids);
void checkErrors(cudaError_t err);

__global__ void foo() {}

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

    DataPoint *deviceData;
    auto dataSize = sizeof(DataPoint) * data.size();
    checkErrors(cudaMalloc(&deviceData, dataSize));
    checkErrors(cudaMemcpy(deviceData, data.data(), dataSize, cudaMemcpyHostToDevice));

    auto blocks = data.size() / 1024 + 1;

    do {
        centroids = newCentroids(data, centroids);
        changed = assignCentroids(data, centroids);
    } while (changed && iterations++ < max_iterations);

    return data;
}

bool assignCentroids(std::vector<DataPoint> &data, const std::vector<DataPoint> &centroids) {
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

void checkErrors(cudaError_t err) {
    if (err != cudaSuccess) {
        std::cerr << cudaGetErrorName(err) << ": " << cudaGetErrorString(err) << std::endl;
        std::exit(EXIT_FAILURE);
    }
}
