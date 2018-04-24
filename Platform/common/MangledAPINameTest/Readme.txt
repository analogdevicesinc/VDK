Mangled Names Test
==================


To check the mangled names of the VDK functions for VDK_CPP_Names.h:

1) Remove the MangledNames.vdk file from the directory (don't overwrite).
2) Create a new project with VDK support.
3) Close the project.
4) Replace the generated .vdk file with the file from (1).
5) Re-open the project
6) Add the four source files in this directory to the project.
7) Select "Generate Map File" linker option.
8) Build all.

The mangled names have, in the past, just been cut & pasted from the map file.

