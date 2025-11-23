#include "Python.h"
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <new>
#include <iostream>
#include <chrono>
#include "rwlock.h"

static constexpr int NUM_THREADS = 10;
static constexpr int NUM_ITERATIONS = 10000000;

static PyObject*
lockbench_shared_lock(PyObject* self, PyObject* args)
{
    std::shared_mutex mutex{};
    auto reader = [&mutex]() {
        for (int i = 0; i < NUM_ITERATIONS; ++i) {
            std::shared_lock<std::shared_mutex> lock(mutex);
        }
    };

    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    std::thread threads[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; ++i) {
        threads[i] = std::thread(reader);
    }
    for (int i = 0; i < NUM_THREADS; ++i) {
        threads[i].join();
    }
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    std::cout << "Elapsed time (shared_lock): " << elapsed_seconds.count() << "s\n";
    Py_RETURN_NONE;
}

static PyObject*
lockbench_rwlock_lock(PyObject* self, PyObject* args)
{
    NPyRWMutex rwmutex;
    memset(&rwmutex, 0, sizeof(NPyRWMutex));
    auto reader = [&rwmutex]() {
        for (int i = 0; i < NUM_ITERATIONS; ++i) {
            NPyRWMutex_RLock(&rwmutex);
            NPyRWMutex_RUnlock(&rwmutex);
        }
    };

    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    std::thread threads[NUM_THREADS];
    for (int i = 0; i < NUM_THREADS; ++i) {
        threads[i] = std::thread(reader);
    }
    for (int i = 0; i < NUM_THREADS; ++i) {
        threads[i].join();
    }
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    std::cout << "Elapsed time (RWLock): " << elapsed_seconds.count() << "s\n";
    Py_RETURN_NONE;
}


static PyMethodDef LockbenchMethods[] = {
    {"shared_lock", lockbench_shared_lock, METH_NOARGS, "Benchmark shared_lock performance."},
    {"rwlock_lock", lockbench_rwlock_lock, METH_NOARGS, "Benchmark RWLock shared lock performance."},
    {NULL, NULL, 0, NULL}
};


PyMODINIT_FUNC
PyInit_lockbench(void)
{
    static struct PyModuleDef moduledef = {
        PyModuleDef_HEAD_INIT,
        "lockbench",
        "A module for benchmarking locks",
        -1,
        LockbenchMethods,
        NULL,
        NULL,
        NULL,
        NULL,
    };
    auto mod = PyModule_Create(&moduledef);
    PyUnstable_Module_SetGIL(mod, Py_MOD_GIL_NOT_USED);
    return mod;
}