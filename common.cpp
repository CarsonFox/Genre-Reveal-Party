#include "common.hpp"

#include <iostream>

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

void usage() {
    std::cerr << "Usage: ./bin data.csv" << std::endl;
    std::exit(EXIT_FAILURE);
}
