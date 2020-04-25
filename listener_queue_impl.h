
#ifndef LISTENER_QUEUE_H
#define LISTENER_QUEUE_H


template <typename T>
ThreadSafeListenerQueue<T>::ThreadSafeListenerQueue () : empty(true) {
  mutex = PTHREAD_MUTEX_INITIALIZER;
  pop_mutex = PTHREAD_MUTEX_INITIALIZER;
  cond = PTHREAD_COND_INITIALIZER;
}

// Adds element to front of queue
template <typename T>
bool ThreadSafeListenerQueue<T>::push (const T element){

  pthread_mutex_lock (&mutex);
  queue.push_front(element);
  empty = false;
  pthread_cond_signal(&cond);
  pthread_mutex_unlock (&mutex);
  return true;
}

// Removes element from end of queue
template <typename T>
bool ThreadSafeListenerQueue<T>::pop (T& element){

  // Second mutex because if listener calls pop and
  // we only had 1 mutex, it'd have to unlock that
  // mutex and then relock it here. Can cause undefined
  // behavior since another thread can pick up that
  // lock quicker than the first.
  //
  // pop_mutex works if pop is called alone. If listen
  // is called, this serves as a second lock to mitigate 
  // the aforementioned issue.
  pthread_mutex_lock (&mutex);
  bool can_remove = true;
  if (queue.size()==0){
    empty = true;
    can_remove = false;
  } 
  else {
    element = queue.back();
    queue.pop_back();
    if (queue.size() == 0)
      empty = true;
  }
  pthread_mutex_unlock (&mutex);
  return can_remove;
}

template <typename T>
bool ThreadSafeListenerQueue<T>::listen (T& element){

  pthread_mutex_lock (&mutex);
  // Need a loop. If there are two threads 
  // waiting and they both receive signal,
  // one will attain the lock, finish, and
  // the second thread will get the lock. 
  // However, if after the signal was sent and
  // there is only 1 element in the queue, 
  // both pops cannot execute.
  while (empty) {
    // Block this thread and get rid of the
    // mutex. Once another thread sends signal,
    // mutex is acquired again
    pthread_cond_wait(&cond, &mutex);
  }
  element = queue.back();
  queue.pop_back();
  if (queue.size() == 0) empty = true;
  pthread_mutex_unlock (&mutex);
  return true;
}
template <typename T>
bool ThreadSafeListenerQueue<T>::is_empty() 
{
  bool tmp = false;
  pthread_mutex_lock (&mutex);
  tmp = empty;
  pthread_mutex_unlock (&mutex);
  return tmp;
}
#endif

