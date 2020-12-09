#include <iostream>
#include <algorithm>

#include "common.hpp"

std::vector<DataPoint> kmeans(std::vector<DataPoint> data, int k);
bool assignCentroids(std::vector<DataPoint> &data, const std::vector<DataPoint> &centroids);
std::vector<DataPoint> newCentroids(const std::vector<DataPoint> &data, const std::vector<DataPoint> &oldCentroids);

int main(int argc, char **argv) {
    auto data = readCSV(argc, argv);
    data = kmeans(data, 3);

    std::cout << data;

    return 0;
}

/*
 * Naive k-means implementation. Begin with random centroids.
 * Repeatedly assign data points to the nearest centroid,
 * then update the centroids to be the mean of all corresponding points.
 * Stop at a maximum number of iterations, or when the centroids stop moving.
 */
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

/*
 * For each data point, compute the closest centroid
 */
bool assignCentroids(std::vector<DataPoint> &data, const std::vector<DataPoint> &centroids) {
    std::vector<double> distances(centroids.size(), 0.0);
    bool changed = false;

    for (auto &&datum: data) {
        //Compute a list of distances, then find the index of the minimum.
        std::transform(centroids.begin(), centroids.end(), distances.begin(),
                       [&](const DataPoint &centroid){ return datum - centroid; });
        size_t min = std::distance(distances.begin(), std::min_element(distances.begin(), distances.end()));

        //If any of the points change centroids, keep iterating.
        changed = changed || (min != datum.centroid);
        datum.centroid = min;
    }

    return changed;
}

/*
 * Compute new centroids via arithmetic mean
 */
std::vector<DataPoint> newCentroids(const std::vector<DataPoint> &data, const std::vector<DataPoint> &oldCentroids) {
    std::vector<DataPoint> newCentroids(oldCentroids.size());
    std::vector<double> counts(oldCentroids.size(), 0.0);

    //Sum the data corresponding to each centroid,
    //and count how many belong to which centroid.
    for (auto &&datum: data) {
        newCentroids[datum.centroid] += datum;
        counts[datum.centroid] += 1.0;
    }

    //Compute the averages
    for (size_t i = 0; i < newCentroids.size(); i++) {
        //If there aren't any data points in a cluster,
        //use a random data point as a new centroid.
        if (counts[i] == 0.0) {
            newCentroids[i] = randomDatum(data);
        } else {
            newCentroids[i] /= counts[i];
        }
    }

    return newCentroids;
}
