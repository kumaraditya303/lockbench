# Compile

```console
CXX='clang -std=c++17' pip install -e. -v
```

# Benchmark

```console
❯ python
Python 3.14.0+ free-threading build (heads/3.14:daf98fe, Nov 20 2025, 17:08:36) [Clang 17.0.0 (clang-1700.4.4.1)] on darwin
Type "help", "copyright", "credits" or "license" for more information.
Cmd click to launch VS Code Native REPL
>>> import lockbench
>>> lockbench.shared_lock()
Elapsed time (shared_lock): 4.04059s
>>> lockbench.rwlock_lock()
Elapsed time (RWLock): 0.695488s
>>>
```