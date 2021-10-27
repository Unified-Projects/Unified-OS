typedef volatile int lock_t;

#define CHECK_DEADLOCK
#ifdef CHECK_DEADLOCK

#define acquireLock(lock) ({ \
    unsigned i = 0; \
    while(__sync_lock_test_and_set(lock, 1) && ++i < 0xFFFFFFF) asm("pause"); \
})
#else
#define acquireLock(lock) ({while(__sync_lock_test_and_set(lock, 1)) asm("pause");})
#endif

#define releaseLock(lock) ({ __sync_lock_release(lock); });

#define acquireTestLock(lock) ({int status; status = __sync_lock_test_and_set(lock, 1); status;})

class ScopedSpinLock final {
public:
    ScopedSpinLock(lock_t& lock) : m_lock(lock) { acquireLock(&m_lock); }
    ~ScopedSpinLock() { releaseLock(&m_lock); }
private:
    lock_t& m_lock;
};