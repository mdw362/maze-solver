#include <list>
#include <pthread.h>

template <typename T>
class ThreadSafeListenerQueue {

  private:
    bool empty;
    std::list <T> queue;
    pthread_mutex_t mutex;
    pthread_mutex_t pop_mutex;
    pthread_cond_t cond;
  public:
    ThreadSafeListenerQueue();
    bool push (const T);
    bool pop (T&);
    bool listen (T&);
    bool is_empty();

};

#include "listener_queue_impl.h"
