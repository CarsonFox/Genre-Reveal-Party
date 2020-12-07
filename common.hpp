#pragma once

#include <vector>
#include <ostream>

/*
 * Code that will be used in all implementations
 */

struct DataPoint {
    double  acousticness,
            danceability,
            energy,
            instrumentalness,
            valence,
            tempo,
            liveness,
            loudness,
            speechiness;
    int     duration,
            popularity,
            year,
            key;
    size_t centroid = 0;
};

constexpr int dimensions = 13;
constexpr int max_iterations = 20;

std::vector<DataPoint> readCSV(int argc, char **argv);
DataPoint randomDatum();
std::vector<DataPoint> randomCentroids(int k);
std::ostream &operator<<(std::ostream &os, const DataPoint &datum);