#include "synch.h"
#include <atomic>

#define PAUSE() asm("pause")

namespace Synch {

    Lock::Lock(void) {
        this->locked.clear();
    }

    void Lock::lock(void) {
        while (this->locked.test_and_set()) {
            PAUSE();
        }
    }

    void Lock::unlock(void) {
        this->locked.clear();
    }
}
