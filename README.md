# Watershed image segmentation algorithm

This project contains school project, so please do not expect perfect
code.

Main aim of this project is to play around with Octave's internals and
try to implement reasonably efficient watershed algorithm. If something
approximately usable comes out of this project, well, let just say that
I would be pleasantly surprised;)


# Compiling watershed function

In order to be able to use `watershed` function from octave, it needs to
be compiled into `oct` file. Compiling on GNU/Linux poses no problem,
since all we need to do is to execute following command:

    mkoctfile watershed.cc

This will compile and link watershed module.


# Using watershed function

In order to use new `watershed` function, simply start octave from the
same folder that `watershed.oct`.

For example calls of this function, consult `test.m` file that
demonstrates usage of automatic and guided watershed. More usage
examples can also be found in `coin.m` and `make_video.m` files.
