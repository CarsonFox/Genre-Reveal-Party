#include <iostream>
#include <algorithm>

#include "common.hpp"

std::vector<DataPoint> kmeans(std::vector<DataPoint> data, int k);
bool assignCentroids(std::vector<DataPoint> &data, const std::vector<DataPoint> &centroids);
std::vector<DataPoint> newCentroids(const std::vector<DataPoint> &data, int k);

int main(int argc, char **argv) {
    auto data = readCSV(argc, argv);
    data = kmeans(data, 10);

    for (auto &&datum : data) {
        std::cout << datum << std::endl;
    }

    return 0;
}

std::vector<DataPoint> kmeans(std::vector<DataPoint> data, int k) {
    auto centroids = randomCentroids(k);

    bool changed;
    int iterations = 0;

    do {
        changed = assignCentroids(data, centroids);
        centroids = newCentroids(data, k);
    } while (changed && iterations++ < max_iterations);

    return data;
}

bool assignCentroids(std::vector<DataPoint> &data, const std::vector<DataPoint> &centroids) {
    std::vector<double> distances(centroids.size(), 0.0);
    bool changed = false;

    for (auto &&datum: data) {
        std::transform(centroids.begin(), centroids.end(), distances.begin(),
                       [&](const DataPoint &centroid){ return std::abs(datum - centroid); });
        size_t min = std::distance(distances.begin(), std::min_element(distances.begin(), distances.end()));

        changed = min != datum.centroid;
        datum.centroid = min;
    }

    return changed;
}

std::vector<DataPoint> newCentroids(const std::vector<DataPoint> &data, int k) {
    return randomCentroids(k);
}