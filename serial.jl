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

rng = Random.MersenneTwister(69)

function main()
    data =
        CSV.File("data/data-small.csv", type = Float64, select = datacols) |>
        DataFrames.DataFrame

    kmeans(data, 5)
end

function kmeans(data, k)
    centroids = [random_centroid(data) for _ = 1:k]
    max_iter = 100

    for i = 1:max_iter
        old_centroids = centroids

        centroids = new_centroids(data, centroids)
        assign_centroids!(data, centroids)

        if old_centroids == centroids
            break
        end
    end
end

function new_centroids(data, centroids)
    centroids
end

function assign_centroids!(data, centroids) end

function random_centroid(data)
    [x for x in data[Random.rand(rng, 1:(DataFrames.nrow(data))), datacols]]
end

main()
