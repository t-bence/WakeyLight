#!/usr/bin/env python3

import pandas as pd

data = pd.read_csv("selected_rgb.csv", sep="\t", header=None)

for index, row in data.iterrows():
    r, g, b = row
    print("    {{{}, {}, {}}},".format(r, g, b))
