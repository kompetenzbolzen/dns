# DNS

A simple authorative DNS server implementation in C89.

This is a work-in-progress.

## Building

Build with

´´´
make build
´´´

executable is in `build/dnsd`

## Testing

always run `make clean` before and after compiling tests.

### Run all tests

```
make test
```

### Coverage

```
make coverage
```

### Valgrind

Run tests with Valgrind to check for memory leaks
```
make valgrind_test
```

License: MIT
