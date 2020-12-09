import csv
import matplotlib.pyplot as plt

with open('data/out.csv', newline='') as csvfile:
    reader = csv.DictReader(csvfile)
    lines = list(map(lambda row: [row['duration'],
                                  row['danceability'],
                                  row['centroid']], reader))

    x = list(map(lambda l: float(l[0]), lines))
    y = list(map(lambda l: float(l[1]), lines))
    centroids = list(map(lambda l: float(l[2]), lines))

    plt.scatter(x, y, c=centroids)
    plt.show()
