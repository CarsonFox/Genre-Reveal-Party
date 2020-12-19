using CSV
using DataFrames
using Random
using UMAP
using Plots

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

rng = MersenneTwister(69)

function main()
    data =
        CSV.File("data/data.csv", type = Float64, select = datacols) |>
        DataFrame

    embedding = umap(transpose(Matrix(data)); n_neighbors=5, min_dist=0.001)

    x_axis = embedding[1,:]
    y_axis = embedding[2,:]
    scatter(x_axis, y_axis)
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

function assign_centroids!(data, centroids) end

function new_centroids(data, centroids)
    centroids
end

function random_centroid(data)
    [x for x in data[rand(rng, 1:(nrow(data))), datacols]]
end

main()
