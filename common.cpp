#include "common.hpp"

#include <iostream>
#include <random>
#include <limits>

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
    static std::default_random_engine rand((std::random_device())());
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

void usage() {
    std::cerr << "Usage: ./bin data.csv" << std::endl;
    std::exit(EXIT_FAILURE);
}
