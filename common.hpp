#pragma once

#include <vector>

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

std::vector<DataPoint> readCSV(int argc, char **argv);
DataPoint randomDatum();