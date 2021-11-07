#!/bin/bash

export PATH=$(brew --prefix llvm)/bin:$PATH
$@
