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
    $ sh make.sh sys
    $ sh make.sh damn
    $ (etc.)

This will produce a bunch of `.so` files in the `plugins/` directory.

When you run balloons for the first time, it will ask you for the directory in which plugins are stored. Make sure whatever directory you choose exists, then copy all the `.so` files to that directory. You do **not** need to copy the `.c` files.

If you intend to edit plugins later, you need to keep the `.c` files that are still in the `plugins/` directory.

If you do **not** intend to edit plugins later, you can delete anything in `plugins/` ending in `.so` or `.c`. You will probably want to keep `make.sh` for compiling plugins in the future.