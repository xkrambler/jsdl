# JSDL (I hope this wont be the final name!)

**JSDL** is a lightweight C++11 OpenSource V8 Engine powered JavaScript runner with integrated bindings (SDL 2, SDL Image 2, SDL TTF 2, Serial, File, System).

This project started to build a fast startup integrated JavaScript interpreter with easy-to-use library for multimedia bindings. The first project that used JSDL is ADisplay, a display improvement for an Honda Accord, I will publish it in the future.

## Getting Started

You will need all development packages to compile it. Check your distribution for these:

```
-lpthread -lv8 -lSDL2 -lSDL2_image -lSDL2_ttf
```

## Building

As easy as run the Makefile:

```
make
```

It compiles nicely on 32-bit systems, amd64, as well on ARM devices.

Once compiled, **jsdl** binary will be available to run applications.

## Documentation

As a starter project, this has no docs, so if you want to know what functions are enabled, you can start reading this source code files:
```
examples/example.js
src/lib.cc
```
In *src/lib.cc* first static function *createShellContext* has the definition of the objects you can use in the library *lib*.
```
-> v8::Persistent<v8::Context> LIB::createShellContext()
```
