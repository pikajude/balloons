#!/bin/bash
debug=
while getopts ":d" opt; do
    case $opt in
        d)
            debug=g
            ;;
        \?)
            echo "Invalid option: -$OPTARG" >&2
            ;;
    esac
done

if [[ -z $1 ]]; then
    echo "Usage: sh make.sh name_of_library"
    exit
fi
gcc -Wall -fPIC "-c$debug" "$1.c" -I../balloons 
gcc -shared "-Wl,-install_name,$1.so" -o "$1.so" "$1.o" ../build/*.o -lcurl
