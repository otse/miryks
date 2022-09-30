# Skyrim Engine

A mod and renderer for skyrim, it uses the three.js scene graph and shaders.

It has an engine / app separation. Use the source freely in your endeavors. See the branches for legendary-edition.

![preview](dark-sse_k3N7K33sa8.jpg)

## Where data comes in

Miryks is all about data and data and data.

An important file is [the miryks .hpp file](miryks/include/miryks/miryks.hpp).

This program uses the opengl api at a low level. [/src](miryks/src/opengl), [/include](miryks/include/opengl)

To read plugins, archives and models (esp, bsa and nif), [there is /src/lib](/miryks/src/lib). It's C.

This project would have never worked without nifskope, its helpful hex offsets, and its source code here on github.
