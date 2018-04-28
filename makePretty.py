#!/usr/bin/env python

import os
import sys

def main():
    # reading data
    with open('tmp.log', 'r') as f:
        data_read = f.read()
    # splitting data by linebreak
    data = data_read.split(sep='\n')[:-1]

    row_counter = 0
    cur_string = ""

    for line in data:
        if line.startswith("LVL"):
            cur_string = '{0:25}  '.format(line[4:-4])

        if line.startswith("Length"):
            cur_string += '{0:8}'.format(line[9:])
        if row_counter == 1 and not line.startswith("Length"):
            cur_string += '{0:8}'.format("-")

        if line.startswith("user"):
            pos_m = line.index('m')
            pos_dot = line.index('.')
            if pos_dot - pos_m == 2:
                line = line[:pos_m+1] + "0" + line[pos_m+1:]
            cur_string += '{0:12}'.format(line[5:])
            print(cur_string)
            row_counter = 0
            continue

        row_counter += 1
    
    return


if __name__ == '__main__':
    sys.exit(main())
    