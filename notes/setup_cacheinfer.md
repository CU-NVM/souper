cache_infer counts the optimizations performed and not-performed

-test-pruning was probably used to see if pruning decreased the amount of optimizations

# CACHE_INFER SETUP

install redis: https://redis.io/docs/getting-started/installation/

install redis perl module:

  Perl Module: https://metacpan.org/dist/Redis/view/README.pod

  Tutorial: https://www.ibm.com/docs/en/product-master/12.0.0?topic=perl-installing-modules (just did step 3 for the easiest way)

## changes made to cache_infer
  1. If you run cache_infer and get "Can't locate Redis.pm in @INC (you may need to install the Redis module) ":

    .@INC is a list of directories that Perl looks for Modules for.

    You can either move your Redis install to one of the directories listed after the error message, 
    or add this line above the script "use lib ' (install directory absolute path)'" to add the directory to @INC at runtime.

  2. If you get issues with "BSD::Resource":

    If your on macOS its incompatible, just comment out the 3 lines that use it the ones that use (RLIMIT_CPU,RLIMIT_RSS,RLIMIT_VHEM)

    If you have a different OS, just install it similar to how Redis got installed

  3. If you get errors with @CMAKE_BINARY_DIR@:

    Its a global variable that never gets defined if you run cache_infer by itself. You can convert into a normal variable and set it to your build dir


# CACHE_INFER USAGE:

1. start redis server with `run redis-sever`

2. compile your tests (example: `clang -emit-llvm -c -o testing/tests.bc testing/bulktests.cpp`)

3. get specifications/candidates (example: `build/souper -souper-no-infer -souper-external-cache testing/tests.bc`)

  (after this you can check to see if you the cache has been populated using redis-cli --stat)

4. test pruning `utils/cache_infer.in -test-pruning`

5. flush database using `redis-cli FLUSHALL`

  (database needs to be flushed because cache_infer marks the keys as already done)