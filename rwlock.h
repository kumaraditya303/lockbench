#include "Python.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct NPyRWMutex {
    PyMutex reader_lock;
    PyMutex writer_lock;
    uint32_t reader_count;
    uint32_t level;
    unsigned long writer_id;
} NPyRWMutex;

// Write lock the RWMutex
void NPyRWMutex_Lock(NPyRWMutex *rwmutex);
// Write unlock the RWMutex
void NPyRWMutex_Unlock(NPyRWMutex *rwmutex);
// Read lock the RWMutex
void NPyRWMutex_RLock(NPyRWMutex *rwmutex);
// Read unlock the RWMutex
void NPyRWMutex_RUnlock(NPyRWMutex *rwmutex);

#ifdef __cplusplus
}
#endif