#include "common.hpp"

#include <iostream>
#include <random>
#include <limits>
#include <algorithm>

#include "csv.hpp"

void usage();

std::vector<DataPoint> readCSV(int argc, char **argv) {
    if (argc < 2) {
        usage();
    }

    csv::CSVReader input(argv[1]);
    std::vector<DataPoint> data;

    for (auto &&row: input) {
        data.push_back({
            row["acousticness"].get<double>(),
            row["danceability"].get<double>(),
            row["energy"].get<double>(),
            row["instrumentalness"].get<double>(),
            row["valence"].get<double>(),
            row["tempo"].get<double>(),
            row["liveness"].get<double>(),
            row["loudness"].get<double>(),
            row["speechiness"].get<double>(),
            row["duration_ms"].get<int>(),
            row["popularity"].get<int>(),
            row["year"].get<int>(),
            row["key"].get<int>()
        });
    }
    return data;
}

DataPoint randomDatum() {
    static std::uniform_real_distribution<> zeroToOne(0.0, 1.0);
    static std::uniform_int_distribution<> intDistribution(0, std::numeric_limits<int>::max());
    static std::default_random_engine rand((std::random_device()) ());
    return {
        zeroToOne(rand),
        zeroToOne(rand),
        zeroToOne(rand),
        zeroToOne(rand),
        zeroToOne(rand),
        zeroToOne(rand) * 100.0 + 50.0,
        zeroToOne(rand),
        zeroToOne(rand) * -60.0,
        zeroToOne(rand),
        intDistribution(rand) % 100000 + 200000,
        intDistribution(rand) % 100,
        intDistribution(rand) % 100 + 1921,
        intDistribution(rand) % 12
    };
}

std::ostream &operator<<(std::ostream &os, const DataPoint &datum) {
    os << datum.acousticness << ',' <<
       datum.danceability << ',' <<
       datum.energy << ',' <<
       datum.instrumentalness << ',' <<
       datum.valence << ',' <<
       datum.tempo << ',' <<
       datum.liveness << ',' <<
       datum.loudness << ',' <<
       datum.speechiness << ',' <<
       datum.duration << ',' <<
       datum.popularity << ',' <<
       datum.year << ',' <<
       datum.key << ',' <<
       datum.centroid;
    return os;
}

std::vector<DataPoint> randomCentroids(int k) {
    std::vector<DataPoint> centroids;
    std::generate_n(std::back_inserter(centroids), k, randomDatum);
    return centroids;
}

inline std::array<double, dimensions> features(const DataPoint &datum) {
    return {
            datum.acousticness,
            datum.danceability,
            datum.energy,
            datum.instrumentalness,
            datum.valence,
            datum.tempo,
            datum.liveness,
            datum.loudness,
            datum.speechiness,
            static_cast<double>(datum.duration),
            static_cast<double>(datum.popularity),
            static_cast<double>(datum.year),
            static_cast<double>(datum.key)
    };
}

/*
 * Implementation of Euclidean distance formula
 */
double operator-(const DataPoint &lhs, const DataPoint &rhs) {
    auto lhs_features = features(lhs), rhs_features = features(rhs);

    auto difference_squared = [](double a, double b) { return std::pow(a - b, 2); };

    /*
     * Returns the square root of the sum of the squared differences. This is naive and slow.
     */
    return std::sqrt(
            std::transform_reduce(lhs_features.begin(), lhs_features.end(), rhs_features.begin(),
                                  0.0, std::plus{}, difference_squared));
}

void usage() {
    std::cerr << "Usage: ./bin data.csv" << std::endl;
    std::exit(EXIT_FAILURE);
}
