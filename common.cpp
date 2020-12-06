#include "common.hpp"

#include <iostream>

void usage();

std::vector<DataPoint> readCSV(int argc, char **argv) {
    if (argc < 2) {
        usage();
    }

    std::string filename(argv[1]);
    std::vector<DataPoint> data;

    io::CSVReader<13, io::trim_chars<' ', '\t'>, io::double_quote_escape<'"', '"'>> in(filename);
    in.read_header(io::ignore_extra_column,
                   "valence",
                   "acousticness",
                   "danceability",
                   "energy",
                   "instrumentalness",
                   "tempo",
                   "liveness",
                   "loudness",
                   "speechiness",
                   "duration",
                   "popularity",
                   "year",
                   "key");

    DataPoint datum = {};
    while (in.read_row(
            datum.acousticness,
            datum.danceability,
            datum.energy,
            datum.instrumentalness,
            datum.valence,
            datum.tempo,
            datum.liveness,
            datum.loudness,
            datum.speechiness,
            datum.duration,
            datum.popularity,
            datum.year,
            datum.key)) {
        data.push_back(datum);
    }

    return data;
}

void usage() {
    std::cerr << "Usage: ./bin data.csv" << std::endl;
    std::exit(EXIT_FAILURE);
}
