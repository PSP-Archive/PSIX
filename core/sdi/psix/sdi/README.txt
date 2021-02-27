SDI - Simple Display Interface

    - Written By Cameron Cooper (Qubitz)

SDI is an interface designed to simplify use of a graphics system, in
particular, for the PSP. It is designed to support swappable backends
which allow for testing of graphics code on different platforms.
Currently, it is designed to support the PSP frammbuffer, PSP GU, and
SDL for PC testing.

The idea is simple. SDI maintains a list of objects which may or may 
not be rendered to the screen following rules for position, alpha 
values, stacking order and other options. Objects can be loaded and 
unloaded at runtime, and rearranged as well.
