#pragma once

#include <vector>
#include <ostream>

/*
 * Code that will be used in all implementations
 */

constexpr int dimensions = 13;
constexpr int max_iterations = 100;

struct DataPoint {
    bool isZero() const;

    double  acousticness = 0.0,
            danceability = 0.0,
            energy = 0.0,
            instrumentalness = 0.0,
            valence = 0.0,
            tempo = 0.0,
            liveness = 0.0,
            loudness = 0.0,
            speechiness = 0.0;
    int     duration = 0,
            popularity = 0,
            year = 0,
            key = 0;
    size_t centroid = 0;
};

std::vector<DataPoint> readCSV(int argc, char **argv);

DataPoint randomDatum(const std::vector<DataPoint> &data);

std::vector<DataPoint> randomCentroids(const std::vector<DataPoint> &data, int k);

std::ostream &operator<<(std::ostream &os, const DataPoint &datum);

std::ostream &operator<<(std::ostream &os, const std::vector<DataPoint> &data);

double operator-(const DataPoint &lhs, const DataPoint &rhs);

void operator+=(DataPoint &lhs, const DataPoint &rhs);

void operator/=(DataPoint &lhs, double rhs);