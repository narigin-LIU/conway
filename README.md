# conway

A simple implementation of [conway's game](https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life) of life in c with the [SDL3](https://wiki.libsdl.org/SDL3/FrontPage) library.

## Quick start

Firstly, build the binary:

```console
$ chmod +x ./build.sh && ./build.sh
```

Then write an initial map of the game in a file. Like this:

```plaintext
5 5
0 0 0 0 0
0 0 0 0 0
0 1 1 1 0
0 0 0 0 0
0 0 0 0 0
```
Run the game:

```console
$ ./conway [FILE]
```

## Control

Press `q` to quit.

