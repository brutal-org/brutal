#!/usr/bin/env python3
# Convert a manifest.json to a makefile

import os
import json
import argparse

if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Convert a manifest.json to a makefile")
    parser.add_argument("-m", "--manifest",
                        help="manifest.json", required=True)

    args = parser.parse_args()

    manifest = json.load(open(args.manifest))

    dirname = os.path.dirname(args.manifest)
    keyprefix = "PKG_" + manifest["id"].upper()

    print("{}_NAME={}".format(keyprefix, manifest["id"]))
    print("{}_PATH={}".format(keyprefix, dirname))
    print("{}_TYPE={}".format(keyprefix, manifest["type"]))
    print("{}_MANIFEST={}".format(keyprefix, args.manifest))

    pkg_sources = []
    pkg_headers = []
    for root, dirs, files in os.walk(dirname):
        for f in files:
            if f.endswith(".c"):
                pkg_sources.append(os.path.join(root, f))

            if f.endswith(".h"):
                pkg_headers.append(os.path.join(root, f))

    print("{}_SOURCES={}".format(keyprefix, " ".join(pkg_sources)))
    print("{}_HEADERS={}".format(keyprefix, " ".join(pkg_headers)))

    print("")

    print("ALL_PKGS+={}".format(keyprefix))

    if "target" in manifest["available"]:
        print("TARGET_PKGS+={}".format(keyprefix))

    if "host" in manifest["available"]:
        print("HOST_PKGS+={}".format(keyprefix))

    print("")
