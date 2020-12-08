#include <iostream>
#include <algorithm>
#include <mutex>

#include "common.hpp"

std::vector<DataPoint> kmeans(std::vector<DataPoint> data, int k, int threads);
int getNumThreads(int argc, char **argv);
bool assignCentroids(std::vector<DataPoint> &data, const std::vector<DataPoint> &centroids, int threads);
std::vector<DataPoint> newCentroids(const std::vector<DataPoint> &data, const std::vector<DataPoint> &oldCentroids);

int main(int argc, char **argv) {
    auto data = readCSV(argc, argv);
    int threads = getNumThreads(argc, argv);

    data = kmeans(data, 4, threads);

    std::cout << data;

    return 0;
}

std::vector<DataPoint> kmeans(std::vector<DataPoint> data, int k, int threads) {
    auto centroids = randomCentroids(data, k);

    bool changed;
    int iterations = 0;

    do {
        changed = assignCentroids(data, centroids, threads);
        centroids = newCentroids(data, centroids);
    } while (changed && iterations++ < max_iterations);

    return data;
}

bool assignCentroids(std::vector<DataPoint> &data, const std::vector<DataPoint> &centroids, int threads) {
    std::mutex mutex;
    bool changed = false;

    #pragma omp parallel default(none) shared(data, centroids, mutex, changed) num_threads(threads)
    {
        auto localCentroids = centroids;
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

        std::lock_guard<std::mutex> guard(mutex);
        changed |= localChanged;
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

void usage() {
    std::cerr << "Usage: ./parallel data.csv NUM_THREADS" << std::endl;
    std::exit(EXIT_FAILURE);
}

int getNumThreads(int argc, char **argv) {
    if (argc < 3)
        usage();
    return std::atoi(argv[2]);
}
