using CSV
using DataFrames
using Random

datacols = [
    :valence,
    :year,
    :acousticness,
    :danceability,
    :duration_ms,
    :energy,
    :explicit,
    :instrumentalness,
    :key,
    :liveness,
    :loudness,
    :popularity,
    :speechiness,
    :tempo,
]

data = CSV.File(
    "data/data-small.csv",
    type = Float64,
    select = datacols,
) |> DataFrames.DataFrame

datasize = DataFrames.nrow(data)

rng = Random.MersenneTwister(69)
random_centroid() = [x for x in data[Random.rand(rng, 1:datasize), datacols]]

print(random_centroid())
