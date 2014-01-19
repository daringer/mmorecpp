#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

#include "threading.h"

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

BaseProcess::BaseProcess() : BaseParallel() { }

BaseProcess::~BaseProcess() { }

void BaseProcess::run() {
  pid = fork();
  int out = -1;
  if(pid < 0) {
    perror("Could not fork");
  } else if(pid == 0) {
    worker();
    exit(0);
  } else
    cout << "[i] child started - pid: " << pid << endl;
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

