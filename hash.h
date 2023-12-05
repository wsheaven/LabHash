/***********************************************************************
 * Header:
 *    HASH
 * Summary:
 *    Our custom implementation of std::unordered_Set
 *      __       ____       ____         __
 *     /  |    .'    '.   .'    '.   _  / /
 *     `| |   |  .--.  | |  .--.  | (_)/ /
 *      | |   | |    | | | |    | |   / / _
 *     _| |_  |  `--'  | |  `--'  |  / / (_)
 *    |_____|  '.____.'   '.____.'  /_/
 *
 *    This will contain the class definition of:
 *        unordered_set           : A class that represents a hash
 *        unordered_set::iterator : An interator through hash
 * Author
 *    Sam Heaven, Abram Hansen
 ************************************************************************/

#pragma once

#include "list.h"     // because this->buckets[0] is a list
#include <memory>     // for std::allocator
#include <functional> // for std::hash
#include <cmath>      // for std::ceil
   

class TestHash;             // forward declaration for Hash unit tests

namespace custom
{
/************************************************
 * UNORDERED SET
 * A set implemented as a hash
 ************************************************/
template <typename T>
class unordered_set
{
   friend class ::TestHash;   // give unit tests access to the privates
public:
   //
   // Construct
   //
   unordered_set() : numElements(0)
   {
   }
   unordered_set(unordered_set&  rhs) : numElements(rhs.numElements)
   {
      for (size_t i = 0; i < 10; i++)
      {
         this->buckets[i] = rhs.buckets[i]; 
      }
   }
   unordered_set(unordered_set&& rhs) : numElements(rhs.numElements)
   {
      for (size_t i = 0; i < 10; i++)
      {
         this->buckets[i] = rhs.buckets[i];
      }
   }
   template <class Iterator>
   unordered_set(Iterator first, Iterator last)
   {
      for (auto it = first; it != last; it++)
         insert(*it); 
   }

   //
   // Assign
   //
   unordered_set& operator=(unordered_set& rhs)
   {
      return *this;
   }
   unordered_set& operator=(unordered_set&& rhs)
   {
      return *this;
   }
   unordered_set& operator=(const std::initializer_list<T>& il)
   {
      return *this;
   }
   void swap(unordered_set& rhs)
   {
      std::swap(numElements, rhs.numElements);
      for (size_t i = 0; i < 10; i++)
      {
         custom::list<T> temp = buckets[i]; 
         this->buckets[i] = rhs.buckets[i];
         rhs.buckets[i] = temp; 
      }
   }

   // 
   // Iterator
   //
   class iterator;
   class local_iterator;
   iterator begin()
   {
      return iterator();
   }
   iterator end()
   {
      return iterator();
   }
   local_iterator begin(size_t iBucket)
   {
      return local_iterator();
   }
   local_iterator end(size_t iBucket)
   {
      return local_iterator();
   }

   //
   // Access
   //
   size_t bucket(const T& t)
   {
      std::hash <T> hasher;
      return hasher(t) % bucket_count();
   }
   iterator find(const T& t);

   //   
   // Insert
   //
   custom::pair<iterator, bool> insert(const T& t);
   void insert(const std::initializer_list<T> & il);


   // 
   // Remove
   //
   void clear() noexcept
   {
   }
   iterator erase(const T& t);

   //
   // Status
   //
   size_t size() const 
   { 
      return numElements;
   }
   bool empty() const 
   { 
      return numElements == 0;
   }
   size_t bucket_count() const 
   { 
      return 10;
   }
   size_t bucket_size(size_t i) const
   {
      return buckets[i].size();
   }

private:

   custom::list<T> buckets [10];   // exactly 10 buckets
   int numElements;                // number of elements in the Hash
};


/************************************************
 * UNORDERED SET ITERATOR
 * Iterator for an unordered set
 ************************************************/
template <typename T>
class unordered_set <T> ::iterator
{
   friend class ::TestHash;   // give unit tests access to the privates
   template <class TT>
   friend class custom::unordered_set;
public:
   // 
   // Construct
   //
   iterator()  
   {  
   }
   iterator(typename custom::list<T>* pBucket,
            typename custom::list<T>* pBucketEnd,
            typename custom::list<T>::iterator itList)
      : pBucket(pBucket), pBucketEnd(pBucketEnd), itList(itList)
   {
   }
   iterator(const iterator& rhs) 
   { 
   }

   //
   // Assign
   //
   iterator& operator = (const iterator& rhs)
   {
      return *this;
   }

   //
   // Compare
   //
   bool operator != (const iterator& rhs) const 
   { 
      return true;
   }
   bool operator == (const iterator& rhs) const 
   { 
      return true;
   }

   // 
   // Access
   //
   T& operator * ()
   {
      return *(new T());
   }

   //
   // Arithmetic
   //
   iterator& operator ++ ();
   iterator operator ++ (int postfix)
   {
      return *this;
   }

private:
   custom::list<T> *pBucket;
   custom::list<T> *pBucketEnd;
   typename list<T>::iterator itList;
};


/************************************************
 * UNORDERED SET LOCAL ITERATOR
 * Iterator for a single bucket in an unordered set
 ************************************************/
template <typename T>
class unordered_set <T> ::local_iterator
{
   friend class ::TestHash;   // give unit tests access to the privates

   template <class TT>
   friend class custom::unordered_set;
public:
   // 
   // Construct
   //
   local_iterator()  
   {
   }
   local_iterator(const typename custom::list<T>::iterator& itList) 
   {
   }
   local_iterator(const local_iterator& rhs) 
   { 
   }

   //
   // Assign
   //
   local_iterator& operator = (const local_iterator& rhs)
   {
      return *this;
   }

   // 
   // Compare
   //
   bool operator != (const local_iterator& rhs) const
   {
      return true;
   }
   bool operator == (const local_iterator& rhs) const
   {
      return true;
   }

   // 
   // Access
   //
   T& operator * ()
   {
      return *(new T());
   }

   // 
   // Arithmetic
   //
   local_iterator& operator ++ ()
   {
      return *this;
   }
   local_iterator operator ++ (int postfix)
   {
      return *this;
   }

private:
   typename list<T>::iterator itList;
};


/*****************************************
 * UNORDERED SET :: ERASE
 * Remove one element from the unordered set
 ****************************************/
template <typename T>
typename unordered_set <T> ::iterator unordered_set<T>::erase(const T& t)
{
   return iterator();
}

/*****************************************
 * UNORDERED SET :: INSERT
 * Insert one element into the hash
 ****************************************/
template <typename T>
custom::pair<typename custom::unordered_set<T>::iterator, bool> unordered_set<T>::insert(const T& t)
{
   auto iBucket = bucket(t); 
   
   for (auto it = buckets[iBucket].begin(); it != buckets[iBucket].end(); it++)
   {
      if (*it == t)
      {
         return custom::pair<custom::unordered_set<T>::iterator, bool>(iterator(buckets, buckets + 10, buckets[0].begin()), false);
      }
   }

   buckets[iBucket].push_back(t);
   numElements++; 

 
   return custom::pair<custom::unordered_set<T>::iterator, bool>(iterator(buckets + iBucket, buckets + 10, buckets[0].begin()), true);
}
template <typename T>
void unordered_set<T>::insert(const std::initializer_list<T> & il)
{
}

/*****************************************
 * UNORDERED SET :: FIND
 * Find an element in an unordered set
 ****************************************/
template <typename T>
typename unordered_set <T> ::iterator unordered_set<T>::find(const T& t)
{
   return iterator();
}

/*****************************************
 * UNORDERED SET :: ITERATOR :: INCREMENT
 * Advance by one element in an unordered set
 ****************************************/
template <typename T>
typename unordered_set <T> ::iterator & unordered_set<T>::iterator::operator ++ ()
{
   return *this;
}

/*****************************************
 * SWAP
 * Stand-alone unordered set swap
 ****************************************/
template <typename T>
void swap(unordered_set<T>& lhs, unordered_set<T>& rhs)
{
}

}
