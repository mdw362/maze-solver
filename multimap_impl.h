#ifndef MULTIMAP_H
#define MULTIMAP_H

#include <iostream>
#include "multimap.h"
#include <pthread.h>

// By default initialize size of multimap to 1000
template <typename K, typename V>
Multimap <K, V>::Multimap () : size(10000){
  rwlock = PTHREAD_RWLOCK_INITIALIZER;
  data.reserve(size);
  keys.reserve(100);
  int i;
  for (i=0; i<size; i++) data.push_back(std::vector<V>());
}
// A size of n allows 0 ... (n-1) to be placed into map
template <typename K, typename V>
Multimap <K, V>::Multimap (const unsigned long length) : size (length){
  rwlock = PTHREAD_RWLOCK_INITIALIZER;
  data.reserve(length);
  keys.reserve (100);
  int i;
  for (i=0;i<size;i++) data.push_back(std::vector <V> ());
}

/* Adds an element to the multimap entry of the given key.
 * It appends that element to the existing std::vector.
 *
 * */
template <typename K, typename V>
bool Multimap <K,V>::insert(const K& key, const V& value)
{
  pthread_rwlock_wrlock (&rwlock);
  extend(key);
  data[key].push_back(value);
  keys.push_back (key);
  elements+=1;
  pthread_rwlock_unlock(&rwlock);
  return true;
}
/* Verifies that given key is in multimap.
 *
 * */
template <typename K, typename V>
bool Multimap <K,V>::find(const K& key)
{

  pthread_rwlock_rdlock (&rwlock);
  if (key >= data.size()){
    pthread_rwlock_unlock (&rwlock);
    return false;
  }
  bool cond = data[key].size()>0;
  pthread_rwlock_unlock (&rwlock);
  return cond;
}
/* Returns true if key exists in multimap and false
 * otherwise. If true, takes in a std::vector and 
 * appends to it the elements in the map corresponding
 * to that key
 * */
template <typename K, typename V>
bool Multimap<K,V>::find(const K& key, std::vector<V>& values)
{
  pthread_rwlock_rdlock(&rwlock);
  if (key >= data.size()) {
    pthread_rwlock_unlock (&rwlock);
    return false;
  }
  bool ret = data[key].size() > 0;
  int i;
  if (ret)
    for (i=0; i<data[key].size();i++) values.push_back(data[key][i]);
  pthread_rwlock_unlock(&rwlock);
  return ret; 
}
/* Removes all elements corresponding to the given key 
 * within the multimap.
 * */
template <typename K, typename V>
unsigned long Multimap<K,V>::remove(const K& key)
{
  pthread_rwlock_wrlock (&rwlock);
  extend(key);
  unsigned long removed = data[key].size();
  elements -= removed;
  // TODO: remove from keys all instances of key
  data[key].clear();
  pthread_rwlock_unlock(&rwlock);
  return removed;
}
template <typename K, typename V>
std::pair<K,V> Multimap<K,V>::operator[] (unsigned int n) {

  pthread_rwlock_wrlock (&rwlock);
  std::sort (keys.begin(), keys.end());
  // If n is greater than the size of keys then 
  // return the last element
  //
  // Fetch the bucket of values corresponding to index
  std::vector <V> values = 
    data [keys [n >= elements ? elements - 1 : n]];
  int i;

  K fitness = keys [n >= elements ? elements - 1 : n];
  V val = values[0];
  for (i = 0; i < keys.size(); i++)
    if (keys[i] == keys[n >= elements ? elements - 1 : n]) {
      // Get correct element within bucket
      val = values [ (n >= elements ? elements - 1 : n) - i];
      break;
    }
  
  pthread_rwlock_unlock(&rwlock);
  return std::pair <K,V> ( fitness, val);

}
template <typename K, typename V>
void Multimap<K,V>::truncate (unsigned int n)
{

  pthread_rwlock_wrlock (&rwlock);

  if (keys.size() <= n){
    pthread_rwlock_unlock (&rwlock);
    return;
  }
  std::sort (keys.begin(), keys.end());
  unsigned int index = keys.size()-1;
  while (keys.size() > n){
    std::vector <V> values = data[keys[index]];
    // If there's more than 1 element for a key then just remove
    // 1 of those elements
    //if (values.size() > 1) data[keys[index]].remove (keys[index]);

    if (values.size() > 1) {
      data[keys[index]].erase (data[keys[index]].begin());
    }
    
    // Otherwise jsut remove the entire entry
     else data[keys[index]].clear();
    keys.erase (keys.begin() + index);
    index-=1;
    elements -=1;
  }
  pthread_rwlock_unlock(&rwlock);
}
template <typename K, typename V>
unsigned long Multimap<K,V>::get_elements()
{
  return elements;
}
/* Kakes in the current key hashcode and checks if it 
 * is greater than the size of the multimap. If it is
 * then more spots are allocated.
 * */
template <typename K, typename V>
void Multimap<K,V>::extend (const unsigned long code)
{
  if (code > size){
    int i;
    for (i=size; i<=code+1; i++) {
      data.push_back(std::vector<V>());
    }
    size = code;
  }

}
template <typename K, typename V>
void Multimap<K,V>::print ()
{
  pthread_rwlock_rdlock (&rwlock);
  int i,j;
  for (i=0; i<data.size();i++){
    if (data[i].size()>0){
      std::cout<<"INDEX: " << i << " : ";
      std::cout<<std::endl;
      for (j=0; j<data[i].size();j++){
        std::cout<<data[i][j]<<"  ";
      }
      std::cout<<std::endl;
    }
  }
  pthread_rwlock_unlock(&rwlock);
}
/* Generates the hashcode for a key. This hashcode
 * is used to index into the multimap. Uses a 
 * simpler version of the djb2 hash algorithm. 
 *
 * Warning: using non-integer keys or large integers
 * can result in hashcode taking a very long amount 
 * of time
 * */
template <typename R>
unsigned long hashcode (const R& key)
{
  // If we know that all keys are integers greater than
  // or equal to 0, we can just return key.
  //
  // This implementation accommodates for worst case
  // with respect to type parameter and just
  // calls the hashing algorithm on every input.
//  return hashcode (std::to_string(key));
  std::cout<<"Converting " << key <<std::endl;
  return hashcode (key);
}
template <>
unsigned long hashcode (const unsigned long& key)
{
  return key;
}
template <>
unsigned long hashcode (const std::string& key)
{

  //unsigned long hash = 5381;
  // Reduced initial hash value for efficiency
  unsigned long hash=3;
  long c ;
  const char * str = key.c_str();
  
  c = *str++;
  while ( c ) {
    hash = ((hash << 5) + hash) + c; 
    c = *str++;
  }
  return hash;
}

#endif 
