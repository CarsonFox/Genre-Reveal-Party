#include <iostream>
#include <algorithm>

#include "common.hpp"

std::vector<DataPoint> kmeans(std::vector<DataPoint> data, int k);
void assignCentroids(std::vector<DataPoint> &data, const std::vector<DataPoint> &centroids);

int main(int argc, char **argv) {
    auto data = readCSV(argc, argv);
    kmeans({}, 10);
    return 0;
}

std::vector<DataPoint> kmeans(std::vector<DataPoint> data, int k) {
    std::vector<DataPoint> centroids;
    std::generate_n(std::back_inserter(centroids), k, randomDatum);

    for (auto &&datum: centroids) {
        std::cout << datum.instrumentalness << std::endl;
    }

    return {};
}

void assignCentroids(std::vector<DataPoint> &data, const std::vector<DataPoint> &centroids) {
}
