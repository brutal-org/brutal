#!/usr/bin/env python3

# Create a new header file including all the .h file in a directory

import os
from sys import stdout
import argparse

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Create a new header file including all the .h file in a directory")
    parser.add_argument("-d", "--directory", help="Directory to scan", required=True)
    parser.add_argument("-p", "--prefix", help="Prefix to add to the header file", required=True)
    args = parser.parse_args()

    # Get all the .h files
    h_files = []
    for root, dirs, files in os.walk(args.directory):
        for file in files:
            if file.endswith(".h"):
                final_path = os.path.relpath(os.path.join(root, file), args.directory)
                h_files.append(os.path.join(args.prefix, final_path))

    # Create the header file
    stdout.write("#pragma once\n")
    stdout.write("\n")
    for h_file in h_files:
        stdout.write("#include <{}>\n".format(h_file))
    stdout.write("\n")
