balloons (a C bot)
==================
Installation
------------

    $ make
    $ sudo make install

Uninstallation
--------------

    $ sudo make uninstall

Plugins
-------

    $ cd plugins
    $ ./make

This will produce a bunch of `.so` files in the `plugins/` directory.

When you run balloons for the first time, it will ask you for the directory in which plugins are stored. Make sure whatever directory you choose exists, then copy all the `.so` files to that directory. You do **not** need to copy the `.c` or `.o` files.

If you intend to edit plugins later, you need to keep the `.c` files that are still in the `plugins/` directory.

If you do **not** intend to edit plugins later, you can delete anything in `plugins/` ending in `.so`, `.o`, or `.c`. You will probably want to keep `make.sh` for compiling plugins in the future.

Unicode
-------
balloons internally uses the `wchar_t` type in place of `char` for Unicode support. You'll need to do this in your plugins too, but don't worry, it's easy. First, include the `wchar.h` header:

    #include <wchar.h>

For all the `str*` string manipulation functions, like `strlen` and `strchr`, you probably just need to replace "str" with "wcs". A list of functions exported by `wchar.h` is [here](http://pubs.opengroup.org/onlinepubs/7908799/xsh/wchar.h.html).

Also, you must put `L` in front of any string or character literals:

    wprintf(L"Hello%lc world!\n", L',');

