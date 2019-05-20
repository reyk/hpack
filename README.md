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

TESTS
-----

Make sure to checkout all git submodules.

```
$ git submodule update --init
```

The tests use OpenBSD's regress framework.

```
$ make regress
===> regress
cc -O2 -pipe  -DJSMN_PARENT_LINKS -Wall -I. -I.. -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wshadow -Wpointer-arith -Wcast-qual -Wsign-compare  -MD -MP  -c main.c
cc -O2 -pipe  -DJSMN_PARENT_LINKS -Wall -I. -I.. -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wshadow -Wpointer-arith -Wcast-qual -Wsign-compare  -MD -MP  -c jsmn.c
cc -O2 -pipe  -DJSMN_PARENT_LINKS -Wall -I. -I.. -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wshadow -Wpointer-arith -Wcast-qual -Wsign-compare  -MD -MP  -c json.c
cc -O2 -pipe  -DJSMN_PARENT_LINKS -Wall -I. -I.. -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wshadow -Wpointer-arith -Wcast-qual -Wsign-compare  -MD -MP  -c ../hpack.c
cc   -o hpacktest main.o jsmn.o json.o hpack.o
./hpacktest hpack-test-case
SUCCESS: hpack-test-case/go-hpack/story_00.json: 3 tests
SUCCESS: hpack-test-case/go-hpack/story_01.json: 2 tests
SUCCESS: hpack-test-case/go-hpack/story_02.json: 10 tests
SUCCESS: hpack-test-case/go-hpack/story_03.json: 10 tests
SUCCESS: hpack-test-case/go-hpack/story_04.json: 10 tests
SUCCESS: hpack-test-case/go-hpack/story_05.json: 10 tests
SUCCESS: hpack-test-case/go-hpack/story_06.json: 10 tests
SUCCESS: hpack-test-case/go-hpack/story_07.json: 10 tests
SUCCESS: hpack-test-case/go-hpack/story_08.json: 10 tests
SUCCESS: hpack-test-case/go-hpack/story_09.json: 10 tests
SUCCESS: hpack-test-case/go-hpack/story_10.json: 10 tests
SUCCESS: hpack-test-case/go-hpack/story_11.json: 10 tests
SUCCESS: hpack-test-case/go-hpack/story_12.json: 10 tests
SUCCESS: hpack-test-case/go-hpack/story_13.json: 10 tests
SUCCESS: hpack-test-case/go-hpack/story_14.json: 10 tests
SUCCESS: hpack-test-case/go-hpack/story_15.json: 10 tests
SUCCESS: hpack-test-case/go-hpack/story_16.json: 10 tests
SUCCESS: hpack-test-case/go-hpack/story_17.json: 10 tests
SUCCESS: hpack-test-case/go-hpack/story_18.json: 10 tests
SUCCESS: hpack-test-case/go-hpack/story_19.json: 10 tests
SUCCESS: hpack-test-case/go-hpack/story_20.json: 164 tests
SUCCESS: hpack-test-case/go-hpack/story_21.json: 366 tests
SUCCESS: hpack-test-case/go-hpack/story_22.json: 455 tests
SUCCESS: hpack-test-case/go-hpack/story_23.json: 363 tests
SUCCESS: hpack-test-case/go-hpack/story_24.json: 33 tests
SUCCESS: hpack-test-case/go-hpack/story_25.json: 256 tests
SUCCESS: hpack-test-case/go-hpack/story_26.json: 117 tests
SUCCESS: hpack-test-case/go-hpack/story_27.json: 219 tests
SUCCESS: hpack-test-case/go-hpack/story_28.json: 128 tests
SUCCESS: hpack-test-case/go-hpack/story_29.json: 335 tests
SUCCESS: hpack-test-case/go-hpack/story_30.json: 646 tests
SUCCESS: hpack-test-case/go-hpack/story_31.json: 117 tests
SUCCESS: hpack-test-case/haskell-http2-linear-huffman/story_00.json: 3 tests
SUCCESS: hpack-test-case/haskell-http2-linear-huffman/story_01.json: 2 tests
SUCCESS: hpack-test-case/haskell-http2-linear-huffman/story_02.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-linear-huffman/story_03.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-linear-huffman/story_04.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-linear-huffman/story_05.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-linear-huffman/story_06.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-linear-huffman/story_07.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-linear-huffman/story_08.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-linear-huffman/story_09.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-linear-huffman/story_10.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-linear-huffman/story_11.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-linear-huffman/story_12.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-linear-huffman/story_13.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-linear-huffman/story_14.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-linear-huffman/story_15.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-linear-huffman/story_16.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-linear-huffman/story_17.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-linear-huffman/story_18.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-linear-huffman/story_19.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-linear-huffman/story_20.json: 164 tests
SUCCESS: hpack-test-case/haskell-http2-linear-huffman/story_21.json: 366 tests
SUCCESS: hpack-test-case/haskell-http2-linear-huffman/story_22.json: 455 tests
SUCCESS: hpack-test-case/haskell-http2-linear-huffman/story_23.json: 363 tests
SUCCESS: hpack-test-case/haskell-http2-linear-huffman/story_24.json: 33 tests
SUCCESS: hpack-test-case/haskell-http2-linear-huffman/story_25.json: 256 tests
SUCCESS: hpack-test-case/haskell-http2-linear-huffman/story_26.json: 117 tests
SUCCESS: hpack-test-case/haskell-http2-linear-huffman/story_27.json: 219 tests
SUCCESS: hpack-test-case/haskell-http2-linear-huffman/story_28.json: 128 tests
SUCCESS: hpack-test-case/haskell-http2-linear-huffman/story_29.json: 335 tests
SUCCESS: hpack-test-case/haskell-http2-linear-huffman/story_30.json: 646 tests
SUCCESS: hpack-test-case/haskell-http2-linear-huffman/story_31.json: 117 tests
SUCCESS: hpack-test-case/haskell-http2-linear/story_00.json: 3 tests
SUCCESS: hpack-test-case/haskell-http2-linear/story_01.json: 2 tests
SUCCESS: hpack-test-case/haskell-http2-linear/story_02.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-linear/story_03.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-linear/story_04.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-linear/story_05.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-linear/story_06.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-linear/story_07.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-linear/story_08.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-linear/story_09.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-linear/story_10.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-linear/story_11.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-linear/story_12.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-linear/story_13.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-linear/story_14.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-linear/story_15.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-linear/story_16.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-linear/story_17.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-linear/story_18.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-linear/story_19.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-linear/story_20.json: 164 tests
SUCCESS: hpack-test-case/haskell-http2-linear/story_21.json: 366 tests
SUCCESS: hpack-test-case/haskell-http2-linear/story_22.json: 455 tests
SUCCESS: hpack-test-case/haskell-http2-linear/story_23.json: 363 tests
SUCCESS: hpack-test-case/haskell-http2-linear/story_24.json: 33 tests
SUCCESS: hpack-test-case/haskell-http2-linear/story_25.json: 256 tests
SUCCESS: hpack-test-case/haskell-http2-linear/story_26.json: 117 tests
SUCCESS: hpack-test-case/haskell-http2-linear/story_27.json: 219 tests
SUCCESS: hpack-test-case/haskell-http2-linear/story_28.json: 128 tests
SUCCESS: hpack-test-case/haskell-http2-linear/story_29.json: 335 tests
SUCCESS: hpack-test-case/haskell-http2-linear/story_30.json: 646 tests
SUCCESS: hpack-test-case/haskell-http2-linear/story_31.json: 117 tests
SUCCESS: hpack-test-case/haskell-http2-naive-huffman/story_00.json: 3 tests
SUCCESS: hpack-test-case/haskell-http2-naive-huffman/story_01.json: 2 tests
SUCCESS: hpack-test-case/haskell-http2-naive-huffman/story_02.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-naive-huffman/story_03.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-naive-huffman/story_04.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-naive-huffman/story_05.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-naive-huffman/story_06.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-naive-huffman/story_07.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-naive-huffman/story_08.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-naive-huffman/story_09.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-naive-huffman/story_10.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-naive-huffman/story_11.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-naive-huffman/story_12.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-naive-huffman/story_13.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-naive-huffman/story_14.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-naive-huffman/story_15.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-naive-huffman/story_16.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-naive-huffman/story_17.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-naive-huffman/story_18.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-naive-huffman/story_19.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-naive-huffman/story_20.json: 164 tests
SUCCESS: hpack-test-case/haskell-http2-naive-huffman/story_21.json: 366 tests
SUCCESS: hpack-test-case/haskell-http2-naive-huffman/story_22.json: 455 tests
SUCCESS: hpack-test-case/haskell-http2-naive-huffman/story_23.json: 363 tests
SUCCESS: hpack-test-case/haskell-http2-naive-huffman/story_24.json: 33 tests
SUCCESS: hpack-test-case/haskell-http2-naive-huffman/story_25.json: 256 tests
SUCCESS: hpack-test-case/haskell-http2-naive-huffman/story_26.json: 117 tests
SUCCESS: hpack-test-case/haskell-http2-naive-huffman/story_27.json: 219 tests
SUCCESS: hpack-test-case/haskell-http2-naive-huffman/story_28.json: 128 tests
SUCCESS: hpack-test-case/haskell-http2-naive-huffman/story_29.json: 335 tests
SUCCESS: hpack-test-case/haskell-http2-naive-huffman/story_30.json: 646 tests
SUCCESS: hpack-test-case/haskell-http2-naive-huffman/story_31.json: 117 tests
SUCCESS: hpack-test-case/haskell-http2-naive/story_00.json: 3 tests
SUCCESS: hpack-test-case/haskell-http2-naive/story_01.json: 2 tests
SUCCESS: hpack-test-case/haskell-http2-naive/story_02.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-naive/story_03.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-naive/story_04.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-naive/story_05.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-naive/story_06.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-naive/story_07.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-naive/story_08.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-naive/story_09.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-naive/story_10.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-naive/story_11.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-naive/story_12.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-naive/story_13.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-naive/story_14.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-naive/story_15.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-naive/story_16.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-naive/story_17.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-naive/story_18.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-naive/story_19.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-naive/story_20.json: 164 tests
SUCCESS: hpack-test-case/haskell-http2-naive/story_21.json: 366 tests
SUCCESS: hpack-test-case/haskell-http2-naive/story_22.json: 455 tests
SUCCESS: hpack-test-case/haskell-http2-naive/story_23.json: 363 tests
SUCCESS: hpack-test-case/haskell-http2-naive/story_24.json: 33 tests
SUCCESS: hpack-test-case/haskell-http2-naive/story_25.json: 256 tests
SUCCESS: hpack-test-case/haskell-http2-naive/story_26.json: 117 tests
SUCCESS: hpack-test-case/haskell-http2-naive/story_27.json: 219 tests
SUCCESS: hpack-test-case/haskell-http2-naive/story_28.json: 128 tests
SUCCESS: hpack-test-case/haskell-http2-naive/story_29.json: 335 tests
SUCCESS: hpack-test-case/haskell-http2-naive/story_30.json: 646 tests
SUCCESS: hpack-test-case/haskell-http2-naive/story_31.json: 117 tests
SUCCESS: hpack-test-case/haskell-http2-static-huffman/story_00.json: 3 tests
SUCCESS: hpack-test-case/haskell-http2-static-huffman/story_01.json: 2 tests
SUCCESS: hpack-test-case/haskell-http2-static-huffman/story_02.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-static-huffman/story_03.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-static-huffman/story_04.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-static-huffman/story_05.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-static-huffman/story_06.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-static-huffman/story_07.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-static-huffman/story_08.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-static-huffman/story_09.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-static-huffman/story_10.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-static-huffman/story_11.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-static-huffman/story_12.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-static-huffman/story_13.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-static-huffman/story_14.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-static-huffman/story_15.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-static-huffman/story_16.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-static-huffman/story_17.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-static-huffman/story_18.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-static-huffman/story_19.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-static-huffman/story_20.json: 164 tests
SUCCESS: hpack-test-case/haskell-http2-static-huffman/story_21.json: 366 tests
SUCCESS: hpack-test-case/haskell-http2-static-huffman/story_22.json: 455 tests
SUCCESS: hpack-test-case/haskell-http2-static-huffman/story_23.json: 363 tests
SUCCESS: hpack-test-case/haskell-http2-static-huffman/story_24.json: 33 tests
SUCCESS: hpack-test-case/haskell-http2-static-huffman/story_25.json: 256 tests
SUCCESS: hpack-test-case/haskell-http2-static-huffman/story_26.json: 117 tests
SUCCESS: hpack-test-case/haskell-http2-static-huffman/story_27.json: 219 tests
SUCCESS: hpack-test-case/haskell-http2-static-huffman/story_28.json: 128 tests
SUCCESS: hpack-test-case/haskell-http2-static-huffman/story_29.json: 335 tests
SUCCESS: hpack-test-case/haskell-http2-static-huffman/story_30.json: 646 tests
SUCCESS: hpack-test-case/haskell-http2-static-huffman/story_31.json: 117 tests
SUCCESS: hpack-test-case/haskell-http2-static/story_00.json: 3 tests
SUCCESS: hpack-test-case/haskell-http2-static/story_01.json: 2 tests
SUCCESS: hpack-test-case/haskell-http2-static/story_02.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-static/story_03.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-static/story_04.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-static/story_05.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-static/story_06.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-static/story_07.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-static/story_08.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-static/story_09.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-static/story_10.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-static/story_11.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-static/story_12.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-static/story_13.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-static/story_14.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-static/story_15.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-static/story_16.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-static/story_17.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-static/story_18.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-static/story_19.json: 10 tests
SUCCESS: hpack-test-case/haskell-http2-static/story_20.json: 164 tests
SUCCESS: hpack-test-case/haskell-http2-static/story_21.json: 366 tests
SUCCESS: hpack-test-case/haskell-http2-static/story_22.json: 455 tests
SUCCESS: hpack-test-case/haskell-http2-static/story_23.json: 363 tests
SUCCESS: hpack-test-case/haskell-http2-static/story_24.json: 33 tests
SUCCESS: hpack-test-case/haskell-http2-static/story_25.json: 256 tests
SUCCESS: hpack-test-case/haskell-http2-static/story_26.json: 117 tests
SUCCESS: hpack-test-case/haskell-http2-static/story_27.json: 219 tests
SUCCESS: hpack-test-case/haskell-http2-static/story_28.json: 128 tests
SUCCESS: hpack-test-case/haskell-http2-static/story_29.json: 335 tests
SUCCESS: hpack-test-case/haskell-http2-static/story_30.json: 646 tests
SUCCESS: hpack-test-case/haskell-http2-static/story_31.json: 117 tests
SUCCESS: hpack-test-case/nghttp2-16384-4096/story_00.json: 3 tests
SUCCESS: hpack-test-case/nghttp2-16384-4096/story_01.json: 2 tests
SUCCESS: hpack-test-case/nghttp2-16384-4096/story_02.json: 10 tests
SUCCESS: hpack-test-case/nghttp2-16384-4096/story_03.json: 10 tests
SUCCESS: hpack-test-case/nghttp2-16384-4096/story_04.json: 10 tests
SUCCESS: hpack-test-case/nghttp2-16384-4096/story_05.json: 10 tests
SUCCESS: hpack-test-case/nghttp2-16384-4096/story_06.json: 10 tests
SUCCESS: hpack-test-case/nghttp2-16384-4096/story_07.json: 10 tests
SUCCESS: hpack-test-case/nghttp2-16384-4096/story_08.json: 10 tests
SUCCESS: hpack-test-case/nghttp2-16384-4096/story_09.json: 10 tests
SUCCESS: hpack-test-case/nghttp2-16384-4096/story_10.json: 10 tests
SUCCESS: hpack-test-case/nghttp2-16384-4096/story_11.json: 10 tests
SUCCESS: hpack-test-case/nghttp2-16384-4096/story_12.json: 10 tests
SUCCESS: hpack-test-case/nghttp2-16384-4096/story_13.json: 10 tests
SUCCESS: hpack-test-case/nghttp2-16384-4096/story_14.json: 10 tests
SUCCESS: hpack-test-case/nghttp2-16384-4096/story_15.json: 10 tests
SUCCESS: hpack-test-case/nghttp2-16384-4096/story_16.json: 10 tests
SUCCESS: hpack-test-case/nghttp2-16384-4096/story_17.json: 10 tests
SUCCESS: hpack-test-case/nghttp2-16384-4096/story_18.json: 10 tests
SUCCESS: hpack-test-case/nghttp2-16384-4096/story_19.json: 10 tests
SUCCESS: hpack-test-case/nghttp2-16384-4096/story_20.json: 164 tests
SUCCESS: hpack-test-case/nghttp2-16384-4096/story_21.json: 366 tests
SUCCESS: hpack-test-case/nghttp2-16384-4096/story_22.json: 455 tests
SUCCESS: hpack-test-case/nghttp2-16384-4096/story_23.json: 363 tests
SUCCESS: hpack-test-case/nghttp2-16384-4096/story_24.json: 33 tests
SUCCESS: hpack-test-case/nghttp2-16384-4096/story_25.json: 256 tests
SUCCESS: hpack-test-case/nghttp2-16384-4096/story_26.json: 117 tests
SUCCESS: hpack-test-case/nghttp2-16384-4096/story_27.json: 219 tests
SUCCESS: hpack-test-case/nghttp2-16384-4096/story_28.json: 128 tests
SUCCESS: hpack-test-case/nghttp2-16384-4096/story_29.json: 335 tests
SUCCESS: hpack-test-case/nghttp2-16384-4096/story_30.json: 646 tests
SUCCESS: hpack-test-case/nghttp2-change-table-size/story_00.json: 3 tests
SUCCESS: hpack-test-case/nghttp2-change-table-size/story_01.json: 2 tests
SUCCESS: hpack-test-case/nghttp2-change-table-size/story_02.json: 10 tests
SUCCESS: hpack-test-case/nghttp2-change-table-size/story_03.json: 10 tests
SUCCESS: hpack-test-case/nghttp2-change-table-size/story_04.json: 10 tests
SUCCESS: hpack-test-case/nghttp2-change-table-size/story_05.json: 10 tests
SUCCESS: hpack-test-case/nghttp2-change-table-size/story_06.json: 10 tests
SUCCESS: hpack-test-case/nghttp2-change-table-size/story_07.json: 10 tests
SUCCESS: hpack-test-case/nghttp2-change-table-size/story_08.json: 10 tests
SUCCESS: hpack-test-case/nghttp2-change-table-size/story_09.json: 10 tests
SUCCESS: hpack-test-case/nghttp2-change-table-size/story_10.json: 10 tests
SUCCESS: hpack-test-case/nghttp2-change-table-size/story_11.json: 10 tests
SUCCESS: hpack-test-case/nghttp2-change-table-size/story_12.json: 10 tests
SUCCESS: hpack-test-case/nghttp2-change-table-size/story_13.json: 10 tests
SUCCESS: hpack-test-case/nghttp2-change-table-size/story_14.json: 10 tests
SUCCESS: hpack-test-case/nghttp2-change-table-size/story_15.json: 10 tests
SUCCESS: hpack-test-case/nghttp2-change-table-size/story_16.json: 10 tests
SUCCESS: hpack-test-case/nghttp2-change-table-size/story_17.json: 10 tests
SUCCESS: hpack-test-case/nghttp2-change-table-size/story_18.json: 10 tests
SUCCESS: hpack-test-case/nghttp2-change-table-size/story_19.json: 10 tests
SUCCESS: hpack-test-case/nghttp2-change-table-size/story_20.json: 164 tests
SUCCESS: hpack-test-case/nghttp2-change-table-size/story_21.json: 366 tests
SUCCESS: hpack-test-case/nghttp2-change-table-size/story_22.json: 455 tests
SUCCESS: hpack-test-case/nghttp2-change-table-size/story_23.json: 363 tests
SUCCESS: hpack-test-case/nghttp2-change-table-size/story_24.json: 33 tests
SUCCESS: hpack-test-case/nghttp2-change-table-size/story_25.json: 256 tests
SUCCESS: hpack-test-case/nghttp2-change-table-size/story_26.json: 117 tests
SUCCESS: hpack-test-case/nghttp2-change-table-size/story_27.json: 219 tests
SUCCESS: hpack-test-case/nghttp2-change-table-size/story_28.json: 128 tests
SUCCESS: hpack-test-case/nghttp2-change-table-size/story_29.json: 335 tests
SUCCESS: hpack-test-case/nghttp2-change-table-size/story_30.json: 646 tests
SUCCESS: hpack-test-case/nghttp2/story_00.json: 3 tests
SUCCESS: hpack-test-case/nghttp2/story_01.json: 2 tests
SUCCESS: hpack-test-case/nghttp2/story_02.json: 10 tests
SUCCESS: hpack-test-case/nghttp2/story_03.json: 10 tests
SUCCESS: hpack-test-case/nghttp2/story_04.json: 10 tests
SUCCESS: hpack-test-case/nghttp2/story_05.json: 10 tests
SUCCESS: hpack-test-case/nghttp2/story_06.json: 10 tests
SUCCESS: hpack-test-case/nghttp2/story_07.json: 10 tests
SUCCESS: hpack-test-case/nghttp2/story_08.json: 10 tests
SUCCESS: hpack-test-case/nghttp2/story_09.json: 10 tests
SUCCESS: hpack-test-case/nghttp2/story_10.json: 10 tests
SUCCESS: hpack-test-case/nghttp2/story_11.json: 10 tests
SUCCESS: hpack-test-case/nghttp2/story_12.json: 10 tests
SUCCESS: hpack-test-case/nghttp2/story_13.json: 10 tests
SUCCESS: hpack-test-case/nghttp2/story_14.json: 10 tests
SUCCESS: hpack-test-case/nghttp2/story_15.json: 10 tests
SUCCESS: hpack-test-case/nghttp2/story_16.json: 10 tests
SUCCESS: hpack-test-case/nghttp2/story_17.json: 10 tests
SUCCESS: hpack-test-case/nghttp2/story_18.json: 10 tests
SUCCESS: hpack-test-case/nghttp2/story_19.json: 10 tests
SUCCESS: hpack-test-case/nghttp2/story_20.json: 164 tests
SUCCESS: hpack-test-case/nghttp2/story_21.json: 366 tests
SUCCESS: hpack-test-case/nghttp2/story_22.json: 455 tests
SUCCESS: hpack-test-case/nghttp2/story_23.json: 363 tests
SUCCESS: hpack-test-case/nghttp2/story_24.json: 33 tests
SUCCESS: hpack-test-case/nghttp2/story_25.json: 256 tests
SUCCESS: hpack-test-case/nghttp2/story_26.json: 117 tests
SUCCESS: hpack-test-case/nghttp2/story_27.json: 219 tests
SUCCESS: hpack-test-case/nghttp2/story_28.json: 128 tests
SUCCESS: hpack-test-case/nghttp2/story_29.json: 335 tests
SUCCESS: hpack-test-case/nghttp2/story_30.json: 646 tests
SUCCESS: hpack-test-case/nghttp2/story_31.json: 117 tests
SUCCESS: hpack-test-case/node-http2-hpack/story_00.json: 3 tests
SUCCESS: hpack-test-case/node-http2-hpack/story_01.json: 2 tests
SUCCESS: hpack-test-case/node-http2-hpack/story_02.json: 10 tests
SUCCESS: hpack-test-case/node-http2-hpack/story_03.json: 10 tests
SUCCESS: hpack-test-case/node-http2-hpack/story_04.json: 10 tests
SUCCESS: hpack-test-case/node-http2-hpack/story_05.json: 10 tests
SUCCESS: hpack-test-case/node-http2-hpack/story_06.json: 10 tests
SUCCESS: hpack-test-case/node-http2-hpack/story_07.json: 10 tests
SUCCESS: hpack-test-case/node-http2-hpack/story_08.json: 10 tests
SUCCESS: hpack-test-case/node-http2-hpack/story_09.json: 10 tests
SUCCESS: hpack-test-case/node-http2-hpack/story_10.json: 10 tests
SUCCESS: hpack-test-case/node-http2-hpack/story_11.json: 10 tests
SUCCESS: hpack-test-case/node-http2-hpack/story_12.json: 10 tests
SUCCESS: hpack-test-case/node-http2-hpack/story_13.json: 10 tests
SUCCESS: hpack-test-case/node-http2-hpack/story_14.json: 10 tests
SUCCESS: hpack-test-case/node-http2-hpack/story_15.json: 10 tests
SUCCESS: hpack-test-case/node-http2-hpack/story_16.json: 10 tests
SUCCESS: hpack-test-case/node-http2-hpack/story_17.json: 10 tests
SUCCESS: hpack-test-case/node-http2-hpack/story_18.json: 10 tests
SUCCESS: hpack-test-case/node-http2-hpack/story_19.json: 10 tests
SUCCESS: hpack-test-case/node-http2-hpack/story_20.json: 164 tests
SUCCESS: hpack-test-case/node-http2-hpack/story_21.json: 366 tests
SUCCESS: hpack-test-case/node-http2-hpack/story_22.json: 455 tests
SUCCESS: hpack-test-case/node-http2-hpack/story_23.json: 363 tests
SUCCESS: hpack-test-case/node-http2-hpack/story_24.json: 33 tests
SUCCESS: hpack-test-case/node-http2-hpack/story_25.json: 256 tests
SUCCESS: hpack-test-case/node-http2-hpack/story_26.json: 117 tests
SUCCESS: hpack-test-case/node-http2-hpack/story_27.json: 219 tests
SUCCESS: hpack-test-case/node-http2-hpack/story_28.json: 128 tests
SUCCESS: hpack-test-case/node-http2-hpack/story_29.json: 335 tests
SUCCESS: hpack-test-case/node-http2-hpack/story_30.json: 646 tests
SUCCESS: hpack-test-case/node-http2-hpack/story_31.json: 117 tests
SUCCESS: hpack-test-case/python-hpack/story_00.json: 3 tests
SUCCESS: hpack-test-case/python-hpack/story_01.json: 2 tests
SUCCESS: hpack-test-case/python-hpack/story_02.json: 10 tests
SUCCESS: hpack-test-case/python-hpack/story_03.json: 10 tests
SUCCESS: hpack-test-case/python-hpack/story_04.json: 10 tests
SUCCESS: hpack-test-case/python-hpack/story_05.json: 10 tests
SUCCESS: hpack-test-case/python-hpack/story_06.json: 10 tests
SUCCESS: hpack-test-case/python-hpack/story_07.json: 10 tests
SUCCESS: hpack-test-case/python-hpack/story_08.json: 10 tests
SUCCESS: hpack-test-case/python-hpack/story_09.json: 10 tests
SUCCESS: hpack-test-case/python-hpack/story_10.json: 10 tests
SUCCESS: hpack-test-case/python-hpack/story_11.json: 10 tests
SUCCESS: hpack-test-case/python-hpack/story_12.json: 10 tests
SUCCESS: hpack-test-case/python-hpack/story_13.json: 10 tests
SUCCESS: hpack-test-case/python-hpack/story_14.json: 10 tests
SUCCESS: hpack-test-case/python-hpack/story_15.json: 10 tests
SUCCESS: hpack-test-case/python-hpack/story_16.json: 10 tests
SUCCESS: hpack-test-case/python-hpack/story_17.json: 10 tests
SUCCESS: hpack-test-case/python-hpack/story_18.json: 10 tests
SUCCESS: hpack-test-case/python-hpack/story_19.json: 10 tests
SUCCESS: hpack-test-case/python-hpack/story_20.json: 164 tests
SUCCESS: hpack-test-case/python-hpack/story_21.json: 366 tests
SUCCESS: hpack-test-case/python-hpack/story_22.json: 455 tests
SUCCESS: hpack-test-case/python-hpack/story_23.json: 363 tests
SUCCESS: hpack-test-case/python-hpack/story_24.json: 33 tests
SUCCESS: hpack-test-case/python-hpack/story_25.json: 256 tests
SUCCESS: hpack-test-case/python-hpack/story_26.json: 117 tests
SUCCESS: hpack-test-case/python-hpack/story_27.json: 219 tests
SUCCESS: hpack-test-case/python-hpack/story_28.json: 128 tests
SUCCESS: hpack-test-case/python-hpack/story_29.json: 335 tests
SUCCESS: hpack-test-case/python-hpack/story_30.json: 646 tests
SUCCESS: hpack-test-case/python-hpack/story_31.json: 117 tests
```
