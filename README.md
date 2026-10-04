# C-Template
Personal template for repositories with c code. Generalizes code style, license and types.

# Usage
The name of the executable is defined in the first line and should be changed.

The included Makefile includes several targets:
<pre class="notranslate" lang="{bash}">
<code>release - default, O2 optimization and no debug flags
debug - forces O0 optimization and sets -DDEBUG -DVERBOSE as well as address san

clean, clean-debug, clean-release, clean-tests - their respective cleaning operations

tests - builds automatically detected tests (see <a href="https://github.com/KompetenzAirbag/c-template#Unit-Testing">Unit Testing</a> below)</code>
</pre>
All targets have their respective `run` operations as well to run the executable afterwards.

# Included Libraries
## Logging
Logging library with formatted output. Redirect the log to a file (seen in `tests/test_log.c`). By default redirected to stdout. Compiler flags:

```{bash}
-DDEBUG defines DEBUG and will enable LOG_DEBUG. This will also enable pinpoint location for LOG_WARN
-DVERBOSE defines VERBOSE and will enable LOG_VERBOSE
-DNO_WARN disables warnings (LOG_WARN)
-DNO_COLOR disables color coding of logs
```
## Unit Testing
Semi-automated unit tests. Add any `test_<name>.c` to `tests` dir and `make run-tests` will automatically detect and run it. Each unit tests needs a main function and returns 0 on success. See the `test_log.c` example. Anything else in the `tests` dir will be build like any other source file or included. `make tests` will build all tests and put the executables in `build/tests` for individual testing.
