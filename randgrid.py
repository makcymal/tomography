import os
from random import randint
from dotenv import load_dotenv

load_dotenv("config.env")
pts = int(os.getenv("POINTS"))
dirs = int(os.getenv("DIRECTIONS"))

with open("grid.dat", "w") as file:
    for _ in range(dirs):
        for _ in range(2 * pts + 1):
            file.write(str(randint(10, 99)) + ' ')
        file.write('\n')
