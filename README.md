# Light-kfifo user manual 

Light-kfifo is a high-performance queue library in C language. It is lock-free when there is only one consumer and only one producer. It supports the insertion of any type of data structure, and even supports direct insertion of structures, we only need declare the structure at the beginning. Another feature of it is that it supports the record mode, which can automatically record the length of a single insertion. Using this mode, the string can be recorded without allocating new memory.

## Compilation Instructions

It does not has any external dependency, so just make it directly in the project root directory.

```shell
cd light-kfifo
make
```

### Run some simple tests

After compiling you can run some simple tests, it can quickly teach you how to use kfifo.

```shell
./examples/simple
./examples/selftest
```

Here is the output of simple:

```shell
light-kfifo test
...
```

## Based on Development 

Light-rbtree library requires only three files to complete the migration.

```
src/kfifo.c
src/kfifo.h
src/macro.h
```

## License

This is an open source red black tree library, which uses the GPLV2 protocol

## Discussion Group

The Tencent QQ Group number is: [763756024](https://jq.qq.com/?_wv=1027&k=UhogIfXA)

