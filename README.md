hpack
=====

> WIP!!! THIS CODE IS NOT FINISHED YET!!!

1. Why another HPACK implementation?

  I either didn't like the license or the style of the existing C code.
And I wanted to have a small HPACK implementation that fits into a
single `.c` and `.h` file.

2. What is the purpose of this HPACK code?

  I want to include it, one day, into relayd or httpd.

3. Why is this an OpenBSD library?

  This is just for testing and development.  The final code will
eventually be included into the software under OpenBSD.

TODO
----

- encoding
- dynamic table
