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
};

std::vector<DataPoint> readCSV(int argc, char **argv);