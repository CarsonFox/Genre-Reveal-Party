#include <iostream>
#include <algorithm>
#include <mutex>

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

/*
 * The main loop is unchanged.
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
 * Assign centroids in parallel. This loop is embarrassingly parallel,
 * and the only necessary synchronization is when deciding to keep going.
 */
bool assignCentroids(std::vector<DataPoint> &data, const std::vector<DataPoint> &centroids) {
    std::mutex mutex;
    bool changed = false;

    #pragma omp parallel default(none) shared(data, centroids, mutex, changed)
    {
        //Since there are few centroids, copy them to speed up memory access
        const auto localCentroids = centroids;
        bool localChanged = false;

        #pragma omp for
        for (auto &&datum: data) {
            std::vector<double> distances(centroids.size(), 0.0);

            std::transform(localCentroids.begin(), localCentroids.end(), distances.begin(),
                           [&](const DataPoint &centroid){ return datum - centroid; });
            size_t min = std::distance(distances.begin(), std::min_element(distances.begin(), distances.end()));

            localChanged |= min != datum.centroid;
            datum.centroid = min;
        }

        //Synchronize, then decide whether to keep iterating
        std::lock_guard<std::mutex> guard(mutex);
        changed |= localChanged;
    }

    return changed;
}

/*
 * This function is unchanged. It would be very hard to parallelize this and achieve
 * any meaningful speedup, since there's so much shared memory access.
 */
std::vector<DataPoint> newCentroids(const std::vector<DataPoint> &data, const std::vector<DataPoint> &oldCentroids) {
    std::vector<DataPoint> newCentroids(oldCentroids.size());
    std::vector<double> counts(oldCentroids.size(), 0.0);

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
