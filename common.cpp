#include "common.hpp"

#include <iostream>
#include <random>
#include <algorithm>

#include "csv.hpp"

std::default_random_engine randomEngine((std::random_device()) ());

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
       datum.centroid << ',';
    return os;
}

std::ostream &operator<<(std::ostream &os, const std::vector<DataPoint> &data) {
    os << "acousticness" << ',' <<
       "danceability" << ',' <<
       "energy" << ',' <<
       "instrumentalness" << ',' <<
       "valence" << ',' <<
       "tempo" << ',' <<
       "liveness" << ',' <<
       "loudness" << ',' <<
       "speechiness" << ',' <<
       "duration" << ',' <<
       "popularity" << ',' <<
       "year" << ',' <<
       "key" << ',' <<
       "centroid," << std::endl;
    for (auto &&datum: data) {
        os << datum << std::endl;
    }
    return os;
}

DataPoint randomDatum(const std::vector<DataPoint> &data) {
    std::uniform_int_distribution<> indexDist(0, data.size());
    return data[indexDist(randomEngine)];
}

std::vector<DataPoint> randomCentroids(const std::vector<DataPoint> &data, int k) {
    std::vector<DataPoint> centroids;
    std::generate_n(std::back_inserter(centroids), k, [&](){ return randomDatum(data); });
    return centroids;
}

inline std::array<double, dimensions> getFeatures(const DataPoint &datum) {
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
    auto lhs_features = getFeatures(lhs), rhs_features = getFeatures(rhs);

    auto difference_squared = [](double a, double b) { return std::pow(a - b, 2); };

    /*
     * Returns the square root of the sum of the squared differences. This is naive and slow.
     */
    return std::sqrt(
            std::transform_reduce(lhs_features.begin(), lhs_features.end(), rhs_features.begin(),
                                  0.0, std::plus{}, difference_squared));
}

void operator+=(DataPoint &lhs, const DataPoint &rhs) {
    lhs.acousticness += rhs.acousticness;
    lhs.danceability += rhs.danceability;
    lhs.energy += rhs.energy;
    lhs.instrumentalness += rhs.instrumentalness;
    lhs.valence += rhs.valence;
    lhs.tempo += rhs.tempo;
    lhs.liveness += rhs.liveness;
    lhs.loudness += rhs.loudness;
    lhs.speechiness += rhs.speechiness;
    lhs.duration += rhs.duration;
    lhs.popularity += rhs.popularity;
    lhs.year += rhs.year;
    lhs.key += rhs.key;
}

void operator/=(DataPoint &lhs, double rhs) {
    lhs.acousticness /= rhs;
    lhs.danceability /= rhs;
    lhs.energy /= rhs;
    lhs.instrumentalness /= rhs;
    lhs.valence /= rhs;
    lhs.tempo /= rhs;
    lhs.liveness /= rhs;
    lhs.loudness /= rhs;
    lhs.speechiness /= rhs;
    lhs.duration = static_cast<int>(static_cast<double>(lhs.duration) / rhs);
    lhs.popularity = static_cast<int>(static_cast<double>(lhs.popularity) / rhs);
    lhs.year = static_cast<int>(static_cast<double>(lhs.year) / rhs);
    lhs.key = static_cast<int>(static_cast<double>(lhs.key) / rhs);
}

void usage() {
    std::cerr << "Usage: ./bin data.csv" << std::endl;
    std::exit(EXIT_FAILURE);
}

bool DataPoint::isZero() const {
    static_assert(static_cast<double>(0) == 0.0);
    auto features = getFeatures(*this);
    return std::all_of(features.begin(), features.end(), [](double x) {return x == 0.0; });
}