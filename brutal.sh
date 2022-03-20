#!/bin/bash

set -e

JOBS=""
SILENT="-s"
DEBUG=""
TARGET=""
export CONFIG=${CONFIG:="devel"}
export COVERAGE=""

if [[ -z "${TOOLCHAIN}" ]]; then
  TOOLCHAIN="llvm"
fi

usage()
{
  TOOLS="$(make list-host) "

  echo "Usage: $0 [options...] [tool] [args...]"
  echo ""
  echo "Options:"
  echo "    -a, --all      Make all before"
  echo "    -c, --clean    Do a clean build"
  echo "    -d, --debug    Run the tool inside a debuger"
  echo "        --doc      open the doc in your prefered browser"
  echo "    -n, --nuke     Nuke the build dir"
  echo "    -f, --fast     Do it fast"
  echo "        --format   Format all C and header files in sources/"
  echo "    -h, --help     Show usage"
  echo "    -o, --coverage Generate code coverage"
  echo "    -r, --run      Start the virtual machine"
  echo "        --resolve  print the path of the tool"
  echo "    -v, --verbose  Enable detailed logging"
  echo ""
  echo "Tools:"
  echo "    $TOOLS"
  echo ""
  exit 1
}

eval_arg()
{
  case $1 in
    "-a" | "--all")
      TARGET=all
      ;;

    "-c" | "--clean")
      TARGET=clean
      ;;

    "-d" | "--debug")
      DEBUG="lldb --"
      ;;

    "--doc")
      doxygen meta/doxyfile
      xdg-open bin/doxygen/index.html
      ;;

    "-n" | "--nuke")
      TARGET=nuke
      ;;

    "-o" | "--coverage")
      COVERAGE="yes"
      ;;

    "-f" | "--fast")
      JOBS="-j"
      ;;

    "-h" | "--help")
      usage
      ;;

    "-r" | "--run")
      TARGET=run
      ;;

    "-v" | "--verbose")
      SILENT=""
      ;;

    "--format")
      find sources/ -name '*.c' -o -name '*.h' -exec clang-format -i '{}' \;
      ;;

    *)
      echo "Unknown option '$1'!"
      echo ""
      usage
      ;;
  esac
}

if [ -z "$1" ]; then
  usage
  exit 1
fi

if [ $1 == "--resolve" ]; then
    PKGS=./bin/$CONFIG/$(uname -m)-host-$TOOLCHAIN
    shift
    echo $PKGS/$1
    exit 0
fi

while [[ $1 == -* ]]; do
  eval_arg $1
  shift
done

if [ $# -gt 0 ]; then
  TOOLS="$(make list-host) "
  if [[ $TOOLS != *"$1 "* ]]; then
    echo "Unknown tool '$1'!"
    echo ""
    usage
  fi

  PKGS=./bin/$CONFIG/$(uname -m)-host-$TOOLCHAIN

  make $JOBS $SILENT $PKGS/$1
  $DEBUG $PKGS/$@
else
  make $JOBS $SILENT $TARGET
fi
