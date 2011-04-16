#include <iostream>
#include <unistd.h>

#include "threading.h"

using namespace TOOLS;
using namespace std;

BaseThread::BaseThread() { }

BaseThread::~BaseThread() { }

void BaseThread::run() {
  pthread_create(&thread, NULL, &(BaseThread::wrap_thread), this);
}

void* BaseThread::wrap_thread(void* instance) {
  reinterpret_cast<BaseThread*>(instance)->worker();
  return NULL;
}

void BaseThread::join() {
  pthread_join(thread, &retval);
}

bool BaseThread::try_join() {
  return (pthread_tryjoin_np(thread, &retval) == 0) ? true : false;
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

