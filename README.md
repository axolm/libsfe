### libsfe
Library for extracting stacktrace from exception.

*Work in progress...*

Versions:
  * v1 -- doesn't work with libc++, because `__cxa_free_exception` calls without `plt` there
  * old v2 -- doesn't work, because there is no destructor call in `__cxa_free_exception`
  * current v2 -- stores backtrace in global map and doen't free it
  * v3 -- dumps stacktrace in raw memory after user obj => we don't need to call destructor => avoid v2 problem

Make targets:
  * `make run-tests` -- Run tests
  * `AVAILABLE_COMPILERS=g++ make run-tests` -- Run tests with chosen compilers
  * `make build-release` -- Build release versions of `.so` files. They are placed in `build/release/src/sfe/libsfe_*`
