#pragma once

#include <vector>
#include <ostream>

/*
 * This contains code that is common to all implementations.
 * This includes code to parse the csv file, operations that can be done on data points,
 * and random data point selection for initializing centroids.
 */

constexpr int dimensions = 13;
constexpr int max_iterations = 80;

//This should've been implemented as an array of doubles.
struct DataPoint {
    double  acousticness = 0.0,
            danceability = 0.0,
            energy = 0.0,
            instrumentalness = 0.0,
            valence = 0.0,
            tempo = 0.0,
            liveness = 0.0,
            loudness = 0.0,
            speechiness = 0.0,
            duration = 0.0,
            popularity = 0.0,
            year = 0.0,
            key = 0.0;
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

bool operator==(const DataPoint &lhs, const DataPoint &rhs);

void usage();
