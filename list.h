/***********************************************************************
 * Header:
 *    LIST
 * Summary:
 *    Our custom implementation of std::list
 *      __       ____       ____         __
 *     /  |    .'    '.   .'    '.   _  / /
 *     `| |   |  .--.  | |  .--.  | (_)/ /
 *      | |   | |    | | | |    | |   / / _
 *     _| |_  |  `--'  | |  `--'  |  / / (_)
 *    |_____|  '.____.'   '.____.'  /_/
 *
 *
 *    This will contain the class definition of:
 *        List         : A class that represents a List
 *        ListIterator : An iterator through List
 * Author
 *    Sam Heaven, Abram Hansen
 ************************************************************************/

#pragma once
#include <cassert>     // for ASSERT
#include <iostream>    // for nullptr
#include <new>         // std::bad_alloc
#include <memory>      // for std::allocator

class TestList;        // forward declaration for unit tests
class TestHash;        // to be used later

namespace custom
{

   /**************************************************
    * LIST
    * Just like std::list
    **************************************************/
   template <typename T>
   class list
   {
      friend class ::TestList; // give unit tests access to the privates
      friend class ::TestHash;
      friend void swap(list& lhs, list& rhs);
   public:
      // 
      // Construct
      //

      list();
      list(list <T>& rhs);
      list(list <T>&& rhs);
      list(size_t num, const T& t);
      list(size_t num);
      list(const std::initializer_list<T>& il);
      template <class Iterator>
      list(Iterator first, Iterator last);
      ~list()
      {
         clear();
      }

      // 
      // Assign
      //

      list <T>& operator = (list& rhs);
      list <T>& operator = (list&& rhs);
      list <T>& operator = (const std::initializer_list<T>& il);
      void swap(list <T>& rhs);

      //
      // Iterator
      //

      class  iterator;
      iterator begin() { return iterator(pHead); }
      iterator rbegin() { return iterator(pTail); }
      iterator end() { return iterator(nullptr); }

      //
      // Access
      //

      T& front();
      T& back();

      //
      // Insert
      //

      void push_front(const T& data);
      void push_front(T&& data);
      void push_back(const T& data);
      void push_back(T&& data);
      iterator insert(iterator it, const T& data);
      iterator insert(iterator it, T&& data);

      //
      // Remove
      //

      void pop_back();
      void pop_front();
      void clear();
      iterator erase(const iterator& it);

      // 
      // Status
      //

      bool empty()  const { return pHead == NULL; }
      size_t size() const { return numElements; }


   private:
      // nested linked list class
      class Node;

      // member variables
      size_t numElements; // though we could count, it is faster to keep a variable
      Node* pHead;    // pointer to the beginning of the list
      Node* pTail;    // pointer to the ending of the list
   };

   /*************************************************
    * NODE
    * the node class.  Since we do not validate any
    * of the setters, there is no point in making them
    * private.  This is the case because only the
    * List class can make validation decisions
    *************************************************/
   template <typename T>
   class list <T> ::Node
   {
   public:
      //
      // Construct
      //
      Node() : pPrev(nullptr), pNext(nullptr)
      {

      }
      Node(const T& data) : data(data), pPrev(nullptr), pNext(nullptr)
      {

      }
      Node(T&& data)
      {
         this->data = std::move(data);
         pPrev = pNext = nullptr;
      }


      //
      // Data
      //

      T data;                 // user data
      Node* pNext;       // pointer to next node
      Node* pPrev;       // pointer to previous node
   };

   /*************************************************
    * LIST ITERATOR
    * Iterate through a List, non-constant version
    ************************************************/
   template <typename T>
   class list <T> ::iterator
   {
      friend class ::TestList; // give unit tests access to the privates
      friend class ::TestHash;
      template <typename TT>
      friend class custom::list;
   public:
      // constructors, destructors, and assignment operator
      iterator() : p(nullptr)
      {

      }
      iterator(Node* pNode) : p(pNode)
      {

      }
      iterator(const iterator& rhs) : p(nullptr)
      {
         *this = rhs;
      }
      iterator& operator = (const iterator& rhs)
      {
         p = rhs.p; 
         return *this; 
      }

      // equals, not equals operator
      bool operator == (const iterator& rhs) const { return rhs.p == this->p; }
      bool operator != (const iterator& rhs) const { return rhs.p != this->p; }

      // dereference operator, fetch a node
      T& operator * ()
      {
         if (p)
            return p->data; 
         else
            throw "ERROR: Trying to dereference a nullptr pointer";
      }

      // postfix increment
      iterator operator ++ (int postfix)
      {
         iterator old(*this);
         if (p)
            p = p->pNext;
         return old;
      }

      // prefix increment
      iterator& operator ++ ()
      {
         if (p)
            p = p->pNext;
         return *this;
      }

      // postfix decrement
      iterator operator -- (int postfix)
      {
         iterator old(*this);
         if (p)
            p = p->pPrev;
         return old;
      }

      // prefix decrement
      iterator& operator -- ()
      {
         if (p)
            p = p->pPrev;
         return *this;
      }

      // two friends who need to access p directly
      friend iterator list <T> ::insert(iterator it, const T& data);
      friend iterator list <T> ::insert(iterator it, T&& data);
      friend iterator list <T> ::erase(const iterator& it);

   private:

      typename list <T> ::Node* p;
   };

   /*****************************************
    * LIST :: NON-DEFAULT constructors
    * Create a list initialized to a value
    ****************************************/
   template <typename T>
   list <T> ::list(size_t num, const T& t)
      : numElements(0), pHead(nullptr), pTail(nullptr)
   {
      if (num)
      {
         list <T> ::Node* pNew;
         list <T> ::Node* pPrevious;

         pHead = pPrevious = pNew = new list <T> ::Node(t); 
         pHead->pPrev = nullptr;

         for (size_t i = 1; i < num; i++)
         {
            assert(pPrevious != nullptr);
            pNew = new list <T> ::Node(t);
            pNew->pPrev = pPrevious;
            pNew->pPrev->pNext = pNew;
            pPrevious = pNew;
         }
         pNew->pNext = nullptr;
         pTail = pNew; 
         numElements = num; 
      }
   }

   /*****************************************
    * LIST :: ITERATOR constructors
    * Create a list initialized to a set of values
    ****************************************/
   template <typename T>
   template <class Iterator>
   list <T> ::list(Iterator first, Iterator last)
   {
      for (auto it = first; it != last; ++it)
         push_back(*it); 
   }

   /*****************************************
    * LIST :: INITIALIZER constructors
    * Create a list initialized to a set of values
    ****************************************/
   template <typename T>
   list <T> ::list(const std::initializer_list<T>& il)
      : numElements(0), pHead(nullptr), pTail(nullptr)
   {
      *this = il; 
   }

   /*****************************************
    * LIST :: NON-DEFAULT constructors
    * Create a list initialized to a value
    ****************************************/
   template <typename T>
   list <T> ::list(size_t num) 
      : numElements(0), pHead(nullptr), pTail(nullptr)
   {
      if (num)
      {
         list <T> ::Node* pNew;
         list <T> ::Node* pPrevious;

         pHead = pPrevious = pNew = new list <T> ::Node();
         pHead->pPrev = nullptr;

         for (size_t i = 1; i < num; i++)
         {
            assert(pPrevious != nullptr);
            pNew = new list <T> ::Node();
            pNew->pPrev = pPrevious;
            pNew->pPrev->pNext = pNew;
            pPrevious = pNew;
         }
         pNew->pNext = nullptr;
         pTail = pNew;
         numElements = num;
      }

   }

   /*****************************************
    * LIST :: DEFAULT constructors
    ****************************************/
   template <typename T>
   list <T> ::list()
      : numElements(0), pHead(nullptr), pTail(nullptr)
   {
   }

   /*****************************************
    * LIST :: COPY constructors
    ****************************************/
   template <typename T>
   list <T> ::list(list& rhs)
      : numElements(0), pHead(nullptr), pTail(nullptr)
   {
      *this = rhs;
   }

   /*****************************************
    * LIST :: MOVE constructors
    * Steal the values from the RHS
    ****************************************/
   template <typename T>
   list <T> ::list(list <T>&& rhs)
      : numElements(0), pHead(nullptr), pTail(nullptr)
   {
      *this = std::move(rhs);
   }

   /**********************************************
    * LIST :: assignment operator - MOVE
    * Copy one list onto another
    *     INPUT  : a list to be moved
    *     OUTPUT :
    *     COST   : O(n) with respect to the size of the LHS
    *********************************************/
   template <typename T>
   list <T>& list <T> :: operator = (list <T>&& rhs)
   {
      if (this != &rhs) {
         pHead = rhs.pHead;
         pTail = rhs.pTail;
         numElements = rhs.numElements;

         // Reset rhs
         rhs.pHead = nullptr;
         rhs.pTail = nullptr;
         rhs.numElements = 0;
      }
      return *this;

   }

   /**********************************************
    * LIST :: assignment operator
    * Copy one list onto another
    *     INPUT  : a list to be copied
    *     OUTPUT :
    *     COST   : O(n) with respect to the number of nodes
    *********************************************/
   template <typename T>
   list <T>& list <T> :: operator = (list <T>& rhs)
   {
      
      auto itRHS = rhs.begin();
      auto itLHS = begin();

      while (itRHS != rhs.end() && itLHS != end())
      {
         *itLHS = *itRHS;
         ++itLHS;
         ++itRHS;
      }

      if (itRHS != rhs.end())
      {
         for (; itRHS != rhs.end(); ++itRHS)
            push_back(*itRHS);
      }

      else if (rhs.empty())
      {
         clear(); 
      }
      
      else if (itLHS != end())
      {
         list <T> ::Node* p = itLHS.p; 
         assert(p); 
         pTail = p->pPrev; 
         list <T> ::Node* pNext = p->pNext; 
         for (p = itLHS.p; p; p = pNext)
         {
            pNext = p->pNext; 
            delete p; 
            numElements--; 
         }
         pTail->pNext = nullptr; 
      }

      return *this;
   }

   /**********************************************
    * LIST :: assignment operator
    * Copy one list onto another
    *     INPUT  : a list to be copied
    *     OUTPUT :
    *     COST   : O(n) with respect to the number of nodes
    *********************************************/
   template <typename T>
   list <T>& list <T> :: operator = (const std::initializer_list<T>& rhs)
   {
      auto itRHS = rhs.begin();
      auto itLHS = begin();

      while (itRHS != rhs.end() && itLHS != end())
      {
         *itLHS = *itRHS;
         ++itLHS;
         ++itRHS;
      }

      if (itRHS != rhs.end())
      {
         for (; itRHS != rhs.end(); ++itRHS)
            push_back(*itRHS);
      }

      else if (rhs.size() == 0)
      {
         clear();
      }

      else if (itLHS != end())
      {
         list <T> ::Node* p = itLHS.p;
         assert(p);
         pTail = p->pPrev;
         list <T> ::Node* pNext = p->pNext;
         for (p = itLHS.p; p; p = pNext)
         {
            pNext = p->pNext;
            delete p;
            numElements--;
         }
         pTail->pNext = nullptr;
      }

      return *this;
   }

   /**********************************************
    * LIST :: CLEAR
    * Remove all the items currently in the linked list
    *     INPUT  :
    *     OUTPUT :
    *     COST   : O(n) with respect to the number of nodes
    *********************************************/
   template <typename T>
   void list <T> ::clear()
   {
      
      list <T> ::Node * pNext;
      for (list <T> ::Node* p = pHead; p; p = pNext)
      {
         pNext = p->pNext;
         delete p; 

      }

      pHead = pTail = nullptr;
      numElements = 0;
   }

   /*********************************************
    * LIST :: PUSH BACK
    * add an item to the end of the list
    *    INPUT  : data to be added to the list
    *    OUTPUT :
    *    COST   : O(1)
    *********************************************/
   template <typename T>
   void list <T> ::push_back(const T& data)
   {

      // Create a new node with the provided data
      Node* newNode = new Node(data);

      newNode->pPrev = pTail;


      if (pHead == NULL) {
         // If the list is empty, set the new node as both head and tail
         pHead = newNode;
         pTail = newNode;
      }
      else {
         // Otherwise, append the new node to the current tail's next
         newNode->pPrev = pTail;
         pTail->pNext = newNode;
         pTail = newNode;
      }

      numElements++;
   }

   template <typename T>
   void list <T> ::push_back(T&& data)
   {
      // Create a new node with the provided data
      Node* newNode = new Node(data);

      newNode->pPrev = pTail;


      if (pHead == NULL) {
         // If the list is empty, set the new node as both head and tail
         pHead = newNode;
         pTail = newNode;
      }
      else {
         // Otherwise, append the new node to the current tail's next
         newNode->pPrev = pTail;
         pTail->pNext = newNode;
         pTail = newNode;
      }

      numElements++;
   }

   /*********************************************
    * LIST :: PUSH FRONT
    * add an item to the head of the list
    *     INPUT  : data to be added to the list
    *     OUTPUT :
    *     COST   : O(1)
    *********************************************/
   template <typename T>
   void list <T> ::push_front(const T& data)
   {
      // Create a new node with the provided data
      Node* pNew = new Node(data);

      // If the list is empty, set the new node as both head and tail
      if (pHead == NULL) {
         pHead = pTail = pNew;
      }
      else {
         pNew->pNext = pHead;
         pHead->pPrev = pNew;
         pHead = pNew;
      }

      numElements++;
   }

   template <typename T>
   void list <T> ::push_front(T&& data)
   {
      // Create a new node with the provided data
      Node* pNew = new Node(data);

      // If the list is empty, set the new node as both head and tail
      if (pHead == NULL) {
         pHead = pTail = pNew;
      }
      else {
         pNew->pNext = pHead;
         pHead->pPrev = pNew;
         pHead = pNew;
      }

      numElements++;
   }


   /*********************************************
    * LIST :: POP BACK
    * remove an item from the end of the list
    *    INPUT  :
    *    OUTPUT :
    *    COST   : O(1)
    *********************************************/
   template <typename T>
   void list <T> ::pop_back()
   {
      if (pTail == nullptr) {
         return;
      }

      if (pTail->pPrev != nullptr) {
         pTail->pPrev->pNext = nullptr;
      }
      else {
         pHead = nullptr;
      }

      Node* oldTail = pTail;
      pTail = pTail->pPrev;
      delete oldTail;
      numElements--;
   }

   /*********************************************
    * LIST :: POP FRONT
    * remove an item from the front of the list
    *    INPUT  :
    *    OUTPUT :
    *    COST   : O(1)
    *********************************************/
   template <typename T>
   void list <T> ::pop_front()
   {
      if (pHead == nullptr) {
         return;
      }

      if (pHead->pNext != nullptr) {
         pHead->pNext->pPrev = nullptr;
      }
      else {
         pTail = nullptr;
      }

      list<T>::Node* temp = pHead;
      pHead = pHead->pNext;

      delete temp;
      numElements--;
   }

   /*********************************************
    * LIST :: FRONT
    * retrieves the first element in the list
    *     INPUT  :
    *     OUTPUT : data to be displayed
    *     COST   : O(1)
    *********************************************/
   template <typename T>
   T& list <T> ::front()
   {
      if (pHead != NULL) {
         return pHead->data;

      }
      else
      {
         const char* sError = "ERROR: unable to access data from an empty list";
         throw sError;
      }

   }

   /*********************************************
    * LIST :: BACK
    * retrieves the last element in the list
    *     INPUT  :
    *     OUTPUT : data to be displayed
    *     COST   : O(1)
    *********************************************/
   template <typename T>
   T& list <T> ::back()
   {
      if (pTail != NULL) {
         return pTail->data;

      }
      else
      {
         const char* sError = "ERROR: unable to access data from an empty list";
         throw sError;
      }
   }

   /******************************************
    * LIST :: REMOVE
    * remove an item from the middle of the list
    *     INPUT  : an iterator to the item being removed
    *     OUTPUT : iterator to the new location
    *     COST   : O(1)
    ******************************************/
   template <typename T>
   typename list <T> ::iterator  list <T> ::erase(const list <T> ::iterator& it)
   {
      assert(numElements >= 0); 
      list <T> ::iterator itNext = end(); 

      if (it == end())
         return it; 
      assert(pHead && pTail); 


      if (it.p->pNext)
      {
         it.p->pNext->pPrev = it.p->pPrev;
         itNext = it.p->pNext;
      }
      else
      {
         pTail = pTail->pPrev; 
      }

      if (it.p->pPrev)
      {
         it.p->pPrev->pNext = it.p->pNext; 
      }
      else
         pHead = pHead->pNext;


      delete it.p; 
      numElements--; 
      return itNext; 
   }

   /******************************************
    * LIST :: INSERT
    * add an item to the middle of the list
    *     INPUT  : data to be added to the list
    *              an iterator to the location where it is to be inserted
    *     OUTPUT : iterator to the new item
    *     COST   : O(1)
    ******************************************/
   template <typename T>
   typename list <T> ::iterator list <T> ::insert(list <T> ::iterator it,
      const T& data)
   {
      if (empty())
      {
         pHead = pTail = new list <T> ::Node(data); 
         numElements = 1; 
         return begin(); 
      }

      try
      {
         list <T> ::Node* pNew = new list <T> ::Node(data); 

         if (it == end())
         {
            pTail->pNext = pNew;
            pNew->pPrev = pTail;
            pTail = pNew; 

            it = pNew; 
         }
         else
         {
            pNew->pPrev = it.p->pPrev; 
            pNew->pNext = it.p; 

            if (pNew->pPrev)
               pNew->pPrev->pNext = pNew;
            else
               pHead = pNew; 
            if (pNew->pNext)
               pNew->pNext->pPrev = pNew;
            else
               pTail = pNew; 

            it = pNew; 
         }
         numElements++; 
      }
      catch (const std::exception&)
      {
         throw " ERROR: unable to allocate a new node for a list" ;
      }
      return it; 
   }

   template <typename T>
   typename list <T> ::iterator list <T> ::insert(list <T> ::iterator it,
      T&& data)
   {
      if (empty())
      {
         pHead = pTail = new list <T> ::Node(std::move(data));
         numElements = 1;
         return begin();
      }

      try
      {
         list <T> ::Node* pNew = new list <T> ::Node(std::move(data));

         if (it == end())
         {
            pTail->pNext = pNew;
            pNew->pPrev = pTail;
            pTail = pNew;

            it = pNew;
         }
         else
         {
            pNew->pPrev = it.p->pPrev;
            pNew->pNext = it.p;

            if (pNew->pPrev)
               pNew->pPrev->pNext = pNew;
            else
               pHead = pNew;
            if (pNew->pNext)
               pNew->pNext->pPrev = pNew;
            else
               pTail = pNew;

            it = pNew;
         }
         numElements++;
      }
      catch (const std::exception&)
      {
         throw " ERROR: unable to allocate a new node for a list";
      }
      return it;
   }

   /**********************************************
    * LIST :: assignment operator - MOVE
    * Copy one list onto another
    *     INPUT  : a list to be moved
    *     OUTPUT :
    *     COST   : O(n) with respect to the size of the LHS
    *********************************************/
   template <typename T>
   void swap(list <T>& lhs, list <T>& rhs)
   {

      std::swap(lhs.pHead, rhs.pHead);
      std::swap(lhs.pTail, rhs.pTail);
      std::swap(lhs.numElements, rhs.numElements);
   }

   template <typename T>
   void list<T>::swap(list <T>& rhs)
   {
      list <T> ::Node* tempHead = rhs.pHead;
      rhs.pHead = pHead;
      pHead = tempHead;

      list <T> ::Node* tempTail = rhs.pTail;
      rhs.pTail = pTail;
      pTail = tempTail;

      int tempElements = rhs.numElements;
      rhs.numElements = numElements;
      numElements = tempElements;
   }

   //#endif
}; // namespace custom