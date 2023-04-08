from random import randint

with open("pts_dirs.dat") as file:
    n_pts, n_dirs  = map(int, file.read().split())

with open("grid.dat", "w") as file:
    for dir in range(n_dirs):
        for pt in range(-n_pts, n_pts + 1):
            file.write(str(randint(10, 99)) + ' ')
        file.write('\n')
