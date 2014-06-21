#ifndef THREADING_H
#define THREADING_H

#include <pthread.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <mqueue.h>

#include <deque>

#include "exception.h"

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

DEFINE_EXCEPTION(ForkError, BaseException);

class BaseParallel {
 public:
  int* retval;
  void* retdata;

  virtual void run();
  virtual void run(const std::string stdout_fn = "",
                   const std::string stderr_fn = "");
  virtual void join(bool blocking = true) = 0;
  virtual bool try_join() = 0;
  virtual void kill() = 0;

  BaseParallel();
  virtual ~BaseParallel();

 protected:
  virtual void worker() = 0;
};

class BaseThread : public BaseParallel {
 public:
  void run();

  void join(bool blocking = true);
  bool try_join();
  void kill();

  BaseThread();
  virtual ~BaseThread();

 protected:
  pthread_t thread;

  static void* wrap_thread(void* instance);
  void worker() = 0;
};

class BaseProcess : public BaseParallel {
 public:
  void run(const std::string stdout_fn = "", const std::string stderr_fn = "");

  void join(bool blocking = true);
  bool try_join();
  void kill();

  BaseProcess();
  virtual ~BaseProcess();

 protected:
  int pid;

  virtual void worker() = 0;
};

class Mutex {
 public:
  Mutex();
  ~Mutex();

  void lock();
  void unlock();

 private:
  pthread_mutex_t mutex;
};

class MessageQueue {
 public:
  MessageQueue(const std::string& id, uint maxsize = 1024);

 protected:
  const std::string name;
  mqd_t mq;
  char* buffer;
  const uint maxsize;
};

class MessageQueueServer : public MessageQueue {
 public:
  MessageQueueServer(const std::string& id, uint maxsize = 1024,
                     bool blocking = false);
  ~MessageQueueServer();

  bool check_for_msg();
  std::string get_msg();

 private:
  struct mq_attr attr;
  std::deque<std::string> local_queue;
  bool blocking;
};

class MessageQueueClient : public MessageQueue {
 public:
  MessageQueueClient(const std::string& id, uint maxsize = 1024);

  void send_msg(const std::string& s);
};
}

#endif
