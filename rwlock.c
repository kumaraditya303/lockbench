
#include <assert.h>
#include "rwlock.h"
#include <stdatomic.h>

void
NPyRWMutex_Lock(NPyRWMutex *rwmutex)
{
    unsigned long writer_id = atomic_load_explicit((_Atomic unsigned long *)&rwmutex->writer_id, memory_order_acquire);
    if (writer_id != 0 && writer_id == PyThread_get_thread_ident()) {
        // If the current thread already holds the write lock, allow recursive write lock
        rwmutex->level++;
        assert(PyMutex_IsLocked(&rwmutex->writer_lock));
        return;
    }
    PyMutex_Lock(&rwmutex->writer_lock);
    assert(rwmutex->writer_id == 0);
    atomic_store_explicit((_Atomic unsigned long *)&rwmutex->writer_id, PyThread_get_thread_ident(), memory_order_release);
}

void
NPyRWMutex_Unlock(NPyRWMutex *rwmutex)
{
    assert(PyMutex_IsLocked(&rwmutex->writer_lock));
    assert(rwmutex->writer_id == PyThread_get_thread_ident());
    if (rwmutex->level > 0) {
        rwmutex->level--;
        return;
    }
    atomic_store_explicit((_Atomic unsigned long *)&rwmutex->writer_id, 0, memory_order_release);
    PyMutex_Unlock(&rwmutex->writer_lock);
}

void
NPyRWMutex_RLock(NPyRWMutex *rwmutex)
{
    unsigned long writer_id = atomic_load_explicit((_Atomic unsigned long *)&rwmutex->writer_id, memory_order_acquire);
    // If current thread holds the write lock, allow recursive read lock
    if (writer_id != 0 && writer_id == PyThread_get_thread_ident()) {
        rwmutex->level++;
        assert(PyMutex_IsLocked(&rwmutex->writer_lock));
        return;
    }

    PyMutex_Lock(&rwmutex->reader_lock);
    rwmutex->reader_count++;
    if (rwmutex->reader_count == 1) {
        // First reader acquires the write lock to block writers
        PyMutex_Lock(&rwmutex->writer_lock);
    }
    assert(PyMutex_IsLocked(&rwmutex->writer_lock));
    assert(rwmutex->writer_id == 0);
    PyMutex_Unlock(&rwmutex->reader_lock);

}

void
NPyRWMutex_RUnlock(NPyRWMutex *rwmutex)
{
    assert(PyMutex_IsLocked(&rwmutex->writer_lock));
    if (rwmutex->level > 0) {
        assert(rwmutex->writer_id == PyThread_get_thread_ident());
        rwmutex->level--;
        return;
    }
    assert(rwmutex->writer_id == 0);
    PyMutex_Lock(&rwmutex->reader_lock);
    rwmutex->reader_count--;
    if (rwmutex->reader_count == 0) {
        // Last reader releases the writer lock
        PyMutex_Unlock(&rwmutex->writer_lock);
    }
    PyMutex_Unlock(&rwmutex->reader_lock);
}
