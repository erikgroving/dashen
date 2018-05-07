#!/usr/bin/env python

import os
import sys

def main():
    # reading data
    with open('tmp.log', 'r') as f:
        data_read = f.read()
    # splitting data by linebreak
    data = data_read.split(sep='\n')[:-1]


    cur_string = ""
    file_counter = 0
    pass_counter = 0

    # print PASSED
    for line in data:
        if line.startswith("LVL"):
            file_counter += 1
        if line.startswith("Length"):
            cur_string += '{0:8}'.format(line[9:])
            try:
                steps = int(line[9:])
                if steps < 3000:
                    pass_counter += 1
            except:
                pass
    print("PASSED {0:4} of {1:4}, {2:.3}".format(pass_counter, file_counter, pass_counter/file_counter))

    # print header
    cur_string = '{0:25}  {1:8}{2:12}'.format("filename", "steps", "time")
    print()
    print(cur_string)

    # print rest
    cur_string = ""
    row_counter = 0
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
    