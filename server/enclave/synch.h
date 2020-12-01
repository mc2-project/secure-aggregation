#include <atomic>

namespace Synch {

    class Lock {
        private:
            std::atomic_flag locked;

        public:
            Lock(void);
            void lock(void);
            void unlock(void);
    };
}
