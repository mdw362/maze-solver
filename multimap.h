#include <iostream>
#include <vector>
//#include <lpthread.h>
template <typename K, typename V>
class Multimap {
  private:
    pthread_rwlock_t rwlock;
    std::vector <std::vector <V> > data;
    unsigned long size;
    unsigned long elements;
    std::vector <K> keys;

  public:

    Multimap ();
    Multimap (const unsigned long);
    bool insert(const K& key, const V& value);
    bool find(const K& key);
    bool find(const K& key, std::vector<V>& values);
    unsigned long remove(const K& key);
    std::pair<K,V> operator[] (unsigned int);
    void truncate (unsigned int);
    unsigned long get_elements();
    void extend (const unsigned long);
    void print();   
};

template <typename R>
unsigned long hashcode (const R& key);
