#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

#include "threading.h"
#include "xstring.h"

using namespace TOOLS;
using namespace std;

BaseParallel::BaseParallel() : retval(new int), retdata(nullptr) { }

BaseParallel::~BaseParallel() {
  delete retval;
  if(retdata != nullptr)
    delete retdata;
}

BaseThread::BaseThread() : BaseParallel() { }

BaseThread::~BaseThread() { }

void BaseThread::run() {
  pthread_create(&thread, NULL, &(BaseThread::wrap_thread), this);
}

void* BaseThread::wrap_thread(void* instance) {
  reinterpret_cast<BaseThread*>(instance)->worker();
  return NULL;
}

void BaseThread::join(bool blocking) {
  if(!blocking) 
    try_join();
  else
    pthread_join(thread, &retdata);
}

bool BaseThread::try_join() {
  return (pthread_tryjoin_np(thread, &retdata) == 0) ? true : false;
}

void BaseThread::kill() {
  throw std::exception();
}

BaseProcess::BaseProcess() : BaseParallel() { }

BaseProcess::~BaseProcess() { }

void BaseProcess::run(const string stdout_fn, const string stderr_fn) {
  pid = fork();
  int out = -1;
  if(pid < 0) {
    throw ForkError(pid);
  } else if(pid == 0) {
    // if stdout_fn or stderr_fn are != "",
    //  redirect stdout/err to this/these file(s)
    if(stdout_fn != "")
      freopen(stdout_fn.c_str(), "w", stdout); 

    if(stderr_fn != "")
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
  if(!blocking)
    flags |= WNOHANG;
  pid_t retpid = waitpid(pid, retval, flags);

  if(retpid == -1)
    perror("waitpid() failed: ");

  if(WIFEXITED(retval) != 0) {
    // child (pid) returned (result)  
  } else if(!blocking) {
    // child (pid) still running....
  } else {
    // error during wait
  }
  
  if(!blocking && WIFEXITED(retval) == 0) {
    //retval contains returncode
  } else {
    *retval = WEXITSTATUS(retval);
    //return WEXITSTATUS(result);
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

Mutex::Mutex() {
  pthread_mutex_init(&mutex, NULL);
}

Mutex::~Mutex() {
  lock();
  unlock();
  pthread_mutex_destroy(&mutex);
}

void Mutex::lock() {
  pthread_mutex_lock(&mutex);
}

void Mutex::unlock() {
  pthread_mutex_unlock(&mutex);
}

