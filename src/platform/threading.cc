#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>

#include "threading.h"
#include "../core/xstring.h"
#include "../core/converter.h"

using namespace TOOLS;
using namespace std;

BaseParallel::BaseParallel() : retval(new int), retdata(nullptr) {}

BaseParallel::~BaseParallel() {
  delete retval;
  if (retdata != nullptr)
    delete (int*)retdata;
}

void BaseParallel::run() {}
void BaseParallel::run(const string stdout_fn, const string stderr_fn) {}

BaseThread::BaseThread() : BaseParallel() {}

BaseThread::~BaseThread() {}

void BaseThread::run() {
  pthread_create(&thread, NULL, &(BaseThread::wrap_thread), this);
}

void* BaseThread::wrap_thread(void* instance) {
  reinterpret_cast<BaseThread*>(instance)->worker();
  return NULL;
}

void BaseThread::join(bool blocking) {
  if (!blocking)
    try_join();
  else
    pthread_join(thread, &retdata);
}

bool BaseThread::try_join() {
  return (pthread_tryjoin_np(thread, &retdata) == 0) ? true : false;
}

void BaseThread::kill() { throw std::exception(); }

BaseProcess::BaseProcess() : BaseParallel() {}

BaseProcess::~BaseProcess() {}

void BaseProcess::run(const string stdout_fn, const string stderr_fn) {
  pid = fork();
  if (pid < 0) {
    throw ForkError(pid);
  } else if (pid == 0) {
    // if stdout_fn or stderr_fn are != "",
    //  redirect stdout/err to this/these file(s)
    if (stdout_fn != "")
      freopen(stdout_fn.c_str(), "w", stdout);

    if (stderr_fn != "")
      freopen(stderr_fn.c_str(), "w", stderr);

    // child
    worker();
    exit(0);
  } else {
    // parent
  }
}

void BaseProcess::join(bool blocking) {
  int flags = 0;
  if (!blocking)
    flags |= WNOHANG;
  pid_t retpid = waitpid(pid, retval, flags);

  if (retpid == -1)
    perror("waitpid() failed: ");

  if (WIFEXITED(retval) != 0) {
    // child (pid) returned (result)
  } else if (!blocking) {
    // child (pid) still running....
  } else {
    // error during wait
  }

  if (!blocking && WIFEXITED(retval) == 0) {
    // retval contains returncode
  } else {
    *retval = WEXITSTATUS(retval);
    // return WEXITSTATUS(result);
  }
}

bool BaseProcess::try_join() {
  join(false);
  return (*retval == 0);
}

void BaseProcess::kill() {
  // worst realization ever!!!
  system((string("killall ") + str(pid)).c_str());
  system((string("killall -9 ") + str(pid)).c_str());
}

Mutex::Mutex() { pthread_mutex_init(&mutex, NULL); }

Mutex::~Mutex() {
  lock();
  unlock();
  pthread_mutex_destroy(&mutex);
}

void Mutex::lock() { pthread_mutex_lock(&mutex); }

void Mutex::unlock() { pthread_mutex_unlock(&mutex); }

MessageQueue::MessageQueue(const std::string& id, uint maxsize)
    : name(id), buffer(new char[maxsize + 1]), maxsize(maxsize) {}

MessageQueueServer::MessageQueueServer(const string& id, uint maxsize,
                                       bool blocking)
    : MessageQueue(id, maxsize), blocking(blocking) {

  attr.mq_flags = (blocking) ? O_NONBLOCK : 0;
  attr.mq_maxmsg = 10;
  attr.mq_msgsize = maxsize;
  attr.mq_curmsgs = 0;

  mq = mq_open(id.c_str(), O_CREAT | O_RDONLY, 0644, &attr);
  if ((mqd_t) - 1 == mq) {
    cerr << "Could not init MessageQueue: " << id << endl;
    perror("err: ");
    exit(1);
  }
}

bool MessageQueueServer::check_for_msg() {
  string out;
  bzero(buffer, maxsize);
  size_t bytes_read = mq_receive(mq, buffer, maxsize, NULL);

  // ret -1 on empty queue && non-blocking
  if (bytes_read == -1) {
    if (!blocking)
      return (local_queue.size() > 0);
    // err?!
    perror("err: ");
  }

  buffer[bytes_read] = '\0';

  out += buffer;
  local_queue.push_back(out);
  return true;
}

string MessageQueueServer::get_msg() {
  string out = local_queue.front();
  local_queue.pop_front();
  return out;
}

MessageQueueClient::MessageQueueClient(const string& id, uint maxsize)
    : MessageQueue(id, maxsize) {

  mq = mq_open(id.c_str(), O_WRONLY);
  if ((mqd_t) - 1 != mq) {
    cerr << "Could not open MessageQueue: " << id << endl;
    perror("err: ");
    exit(1);
  }
}

void MessageQueueClient::send_msg(const std::string& s) {
  mq = mq_send(mq, s.c_str(), maxsize, 0);
  if (mq < 0) {
    cerr << "Could not send to MessageQueue: " << name << endl;
    perror("err: ");
    exit(1);
  }
}
