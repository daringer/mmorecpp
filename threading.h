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

class BaseParallel {
  public:
    int* retval;
    void* retdata;

    virtual void run() = 0;
    virtual void join(bool blocking=true) = 0;
    virtual bool try_join() = 0;

    BaseParallel();
    virtual ~BaseParallel();

  protected:
    virtual void worker() = 0;
};

class BaseThread : public BaseParallel {
  public:
    void run();
    void join(bool blocking=true);
    bool try_join();

    BaseThread();
    virtual ~BaseThread();

  protected:
    pthread_t thread;

    static void* wrap_thread(void* instance);
    virtual void worker() = 0;
};

class BaseProcess : public BaseParallel {
  public:
    void run();
    void join(bool blocking=true);
    bool try_join();

    BaseProcess();
    virtual ~BaseProcess();
  protected:
    int pid;  
    
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
