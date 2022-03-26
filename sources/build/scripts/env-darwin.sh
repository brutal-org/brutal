#!/bin/bash

Xvfb :1 -screen 0 1024x768x24

export PATH=$(brew --prefix xquartz)/bin:$PATH
export PATH=$(brew --prefix llvm)/bin:$PATH
export DISPLAY=":1"

$@
