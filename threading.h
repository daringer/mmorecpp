#ifndef THREADING_H
#define THREADING_H

#include <pthread.h>

namespace TOOLS {
/**
 * @brief An abstract class to derive from to implement a thread.
 *        Simply declare a class like:
 *        class foo : public BaseThread {
 *          public:
 *            void worker();
 *        }
 *        And implement your threadtask/job/... inside the overloaded worker()
 */
class BaseThread {
  public:
    void* retval;

    void run();
    void join();
    bool try_join();

    BaseThread();
    ~BaseThread();

  protected:
    pthread_t thread;

    static void* wrap_thread(void* instance);
    virtual void worker() = 0;

};
/**
 * @brief Simple wrapper for the unix pthread mutex mechanism
 */
class Mutex {
  public:
    Mutex();
    ~Mutex();

    void lock();
    void unlock();

  private:
    pthread_mutex_t mutex;
};

}

#endif
