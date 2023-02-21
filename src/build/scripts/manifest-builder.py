#!/usr/bin/env python3

import subprocess
import json
import sys


def _popen(args):
    process = subprocess.Popen(args, stdout=subprocess.PIPE)
    (output, err) = process.communicate()
    code = process.wait()
    return (code, output.decode("utf-8").rstrip())


def _tool(args):
    code, tool_path = _popen(["./brutal.sh", "--resolve", args[0]])
    return _popen([tool_path] + args[1:])


def _load_module(name):
    path = "sources/protos/" + name + ".idl"
    code, output = _tool(["idl", path, "--json"])
    return json.loads(output)


def _load_interface(name):
    module_name, interface_name = name.split(".", 1)
    interface_name = module_name.title() + interface_name
    module = _load_module(module_name)

    return [interface for interface in module["ifaces"] if interface["name"] == interface_name][0]


def _load_manifest(path):
    with open(path) as f:
        return json.load(f)


def _process_manifest(path):
    manifest = _load_manifest(path)

    if "consume" in manifest:
        ids = []
        for c in manifest["consume"]:
            interface = _load_interface(c)
            ids.append(interface["id"])

        manifest["consume"] = ids

    return manifest


print(json.dumps(_process_manifest(sys.argv[1])))
