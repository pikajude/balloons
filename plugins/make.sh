if [[ -z $1 ]]; then
    echo "Usage: sh make.sh name_of_library"
    exit
fi
gcc -Wall -fPIC -c -g "$1.c" -I../balloons 
gcc -shared "-Wl,-install_name,$1.so" -o "$1.so" "$1.o" ../build/*.o -lcurl
rm -f "$1.o"
