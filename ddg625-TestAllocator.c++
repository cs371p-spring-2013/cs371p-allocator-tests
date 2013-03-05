// ------------------------------------
// projects/allocator/TestAllocator.c++
// Copyright (C) 2013
// Glenn P. Downing
// ------------------------------------

/*
To test the program:
    % ls /usr/include/cppunit/
    ...
    TestFixture.h
    ...
    % locate libcppunit.a
    /usr/lib/libcppunit.a
    % g++ -pedantic -std=c++0x -Wall Allocator.c++ TestAllocator.c++ -o TestAllocator -lcppunit -ldl
    % valgrind TestAllocator >& TestAllocator.out
*/

// --------
// includes
// --------

#include <algorithm> // count
#include <iostream>  // ios_base
#include <memory>    // allocator

#include "cppunit/extensions/HelperMacros.h" // CPPUNIT_TEST, CPPUNIT_TEST_SUITE, CPPUNIT_TEST_SUITE_END
#include "cppunit/TestFixture.h"             // TestFixture
#include "cppunit/TextTestRunner.h"          // TestRunner

#include "Allocator.h"

// ----------------
// TestOurAllocator
// ----------------

template <typename A, int type_size>
struct TestOurAllocator : CppUnit::TestFixture
{
  // --------
  // typedefs
  // --------
  
  typedef typename A::value_type      value_type;
  typedef typename A::difference_type difference_type;
  typedef typename A::pointer         pointer;
  
  // --------------
  // test_Allocator
  // --------------
  
  void test_Allocator ()
  {
    A x;
    CPPUNIT_ASSERT(x.valid());
    int size = sizeof(x.a);
    char* index = x.a;
    int sentinel;
    
    // Varify correct sentinels
    sentinel = *reinterpret_cast<int*> (index);
    CPPUNIT_ASSERT(sentinel = size - 8);
    index += size - 4;
    sentinel = *reinterpret_cast<int*> (index);
    CPPUNIT_ASSERT(sentinel = size - 8);
  }
  
  // ----------
  // test_valid
  // ----------
  
  void test_valid_1 ()
  {
    A x;
    CPPUNIT_ASSERT(x.valid());
    int size = sizeof(x.a);
    char* index = x.a;
    int* sentinel;
    
    // Set up - Block of 40
    sentinel = reinterpret_cast<int*> (index);
    *sentinel = 40;
    index += 44;
    sentinel = reinterpret_cast<int*> (index);
    *sentinel = 40;
    
    // Set up - Block of remaining space
    index += 4;
    sentinel = reinterpret_cast<int*> (index);
    *sentinel = size - 56;
    index += size - 52;
    sentinel = reinterpret_cast<int*> (index);
    *sentinel = size - 56;
    
    // Valid space
    CPPUNIT_ASSERT(x.valid());
  }
  
  void test_valid_2 ()
  {
    A x;
    CPPUNIT_ASSERT(x.valid());
    int size = sizeof(x.a);
    char* index = x.a;
    int* sentinel;
    
    // Set up - Block of -20
    sentinel = (int*) index;
    *sentinel = -20;
    index += 24;
    sentinel = (int*) index;
    *sentinel = -20;
    
    // Set up - Block of -10
    index += 4;
    sentinel = (int*) index;
    *sentinel = -10;
    index += 14;
    sentinel = (int*) index;
    *sentinel = -10;
    
    // Set up - Block of remaining space
    index += 4;
    sentinel = (int*) index;
    *sentinel = size - 54;
    index += size - 50;
    sentinel = (int*) index;
    *sentinel = size - 54;
    
    // Valid space
    CPPUNIT_ASSERT(x.valid());
  }
  
  void test_valid_3 ()
  {
    A x;
    CPPUNIT_ASSERT(x.valid());
    int size = sizeof(x.a);
    char* index = x.a;
    int* sentinel;
    
    sentinel = (int*) index;
    *sentinel = size;
    CPPUNIT_ASSERT(!x.valid());
  }
  
  // -------------
  // test_allocate
  // -------------
  void test_allocate_1 ()
  {
    A x;
    CPPUNIT_ASSERT(x.valid());
    int num = sizeof(x.a);
    
    // Allocate - No block big enough
    try
    {
      x.allocate(num);
      assert(false);
    }
    catch (std::bad_alloc& e)
    {
      assert(true);
      CPPUNIT_ASSERT(x.valid());
    }
    
    
  }
  
  void test_allocate_2 ()
  {
    A x;
    CPPUNIT_ASSERT(x.valid());
    int block = 2 * type_size - 1;
    int mem_size = sizeof(x.a);
    char* index = x.a;
    int* sentinel;
    int* begin;
    int* end;
    
    // Set up - No room for free block
    begin = (int*) index;
    *begin = block;
    index += block + 4;
    end = (int*) index;
    *end = block;
    index += 4;
    sentinel = (int*) index;
    *sentinel = -(mem_size - block - 16);
    index += mem_size - block - 12;
    sentinel = (int*) index;
    *sentinel = -(mem_size - block - 16);
    CPPUNIT_ASSERT(x.valid());
    
    // Allocate block - No room for block
    pointer return_pointer = x.allocate(1);
    CPPUNIT_ASSERT(return_pointer == (pointer) (begin + 1));
    CPPUNIT_ASSERT(*(int*) begin == -block);
    CPPUNIT_ASSERT(*(int*) end == -block);
  }
  
  void test_allocate_3 ()
  {
    A x;
    CPPUNIT_ASSERT(x.valid());
    int block = 2 * type_size + 8;
    int mem_size = sizeof(x.a);
    char* index = x.a;
    int* sentinel;
    int* begin;
    int* end;
    
    // Set up - Just enough room for free block
    begin = (int*) index;
    *begin = block;
    index += block + 4;
    end = (int*) index;
    *end = block;
    index += 4;
    sentinel = (int*) index;
    *sentinel = -(mem_size - block - 16);
    index += mem_size - block - 12;
    sentinel = (int*) index;
    *sentinel = -(mem_size - block - 16);
    CPPUNIT_ASSERT(x.valid());
    
    // Allocate block - Room for block
    pointer return_pointer = x.allocate(1);
    CPPUNIT_ASSERT(return_pointer == (pointer) (begin + 1));
    CPPUNIT_ASSERT(*begin == -(type_size));
    CPPUNIT_ASSERT(*end == type_size);  
  }
  
  // -------------
  // test_deallocate
  // -------------
  
  void test_deallocate_1 ()
  {
    A x;
    CPPUNIT_ASSERT(x.valid());
    int mem_size = sizeof(x.a) - 8;
    int num = mem_size / type_size;
    int* begin = (int*)(x.a);
    int* end = (int*)(x.a + mem_size + 4);
    
    // Allocate block - No room for block
    pointer return_pointer = x.allocate(num);
    CPPUNIT_ASSERT(x.valid());
    CPPUNIT_ASSERT(*begin == -mem_size);
    CPPUNIT_ASSERT(*end == -mem_size);
    
    // Deallocate block - No free space
    x.deallocate(return_pointer, num);
    CPPUNIT_ASSERT(x.valid());
    CPPUNIT_ASSERT(*begin == mem_size);
    CPPUNIT_ASSERT(*end == mem_size);
  }
  
  void test_deallocate_2 ()
  {
    A x;
    CPPUNIT_ASSERT(x.valid());
    int mem_size = sizeof(x.a) - 8;
    int num = 5;
    int* begin = (int*)(x.a);
    int* end = (int*)(x.a + mem_size + 4);
    
    // Allocate block - Room for block
    pointer return_pointer = x.allocate(num);
    CPPUNIT_ASSERT(x.valid());
    CPPUNIT_ASSERT(*begin == -(num * type_size));
    CPPUNIT_ASSERT(*end == mem_size - (num * type_size) - 8);
    
    // Deallocate block - Free space after
    x.deallocate(return_pointer, num);
    CPPUNIT_ASSERT(x.valid());
    CPPUNIT_ASSERT(*begin == mem_size);
    CPPUNIT_ASSERT(*end == mem_size);
  }
  
  void test_deallocate_3 ()
  {
    A x;
    CPPUNIT_ASSERT(x.valid());
    int mem_size = sizeof(x.a) - 8;
    int num = 2;
    int* begin = (int*)(x.a);
    int* end = (int*)(x.a + mem_size + 4);
    
    // Allocate first block - Room for block
    pointer return_pointer_1 = x.allocate(num);
    CPPUNIT_ASSERT(x.valid());
    CPPUNIT_ASSERT(*begin == -(num * type_size));
    CPPUNIT_ASSERT(*end == mem_size - (num * type_size) - 8);
    
    // Allocate second block - Room for block
    pointer return_pointer_2 = x.allocate(num);
    CPPUNIT_ASSERT(x.valid());
    CPPUNIT_ASSERT(*begin == -(num * type_size));
    CPPUNIT_ASSERT(*end == mem_size - 2 * (num * type_size) - 16);
    
    // Deallocate first block - No free space
    x.deallocate(return_pointer_1, num);
    CPPUNIT_ASSERT(x.valid());
    CPPUNIT_ASSERT(*begin == num * type_size);
    CPPUNIT_ASSERT(*end == mem_size - 2 * (num * type_size) - 16);
    
    // Deallocate second block - Free space on both sides
    x.deallocate(return_pointer_2, num);
    CPPUNIT_ASSERT(x.valid());
    CPPUNIT_ASSERT(*begin == mem_size);
    CPPUNIT_ASSERT(*end == mem_size);
  }
  
  // -----
  // suite
  // -----

  CPPUNIT_TEST_SUITE(TestOurAllocator);
  CPPUNIT_TEST(test_Allocator);
  CPPUNIT_TEST(test_valid_1);
  CPPUNIT_TEST(test_valid_2);
  CPPUNIT_TEST(test_valid_3);
  CPPUNIT_TEST(test_allocate_1);
  CPPUNIT_TEST(test_allocate_2);
  CPPUNIT_TEST(test_allocate_3);
  CPPUNIT_TEST(test_deallocate_1);
  CPPUNIT_TEST(test_deallocate_2);
  CPPUNIT_TEST(test_deallocate_3);
  CPPUNIT_TEST_SUITE_END();
};


// -------------
// TestAllocator
// -------------

template <typename A>
struct TestAllocator : CppUnit::TestFixture
{
  // --------
  // typedefs
  // --------
  
  typedef typename A::value_type      value_type;
  typedef typename A::difference_type difference_type;
  typedef typename A::pointer         pointer;
  
  // --------
  // test_one
  // --------

  void test_one ()
  {
    A x;
    const difference_type s = 1;
    const value_type      v = 2;
    const pointer         p = x.allocate(s);
    x.construct(p, v);
    CPPUNIT_ASSERT(*p == v);
    x.destroy(p);
    x.deallocate(p, s);
  }

  // --------
  // test_ten
  // --------

  void test_ten () 
  {
    A x;
    const difference_type s = 10;
    const value_type      v = 2;
    const pointer         b = x.allocate(s);
          pointer         e = b + s;
          pointer         p = b;
    try
    {
      while (p != e)
      {
        x.construct(p, v);
        ++p;
      }
    }
    catch (...)
    {
      while (b != p) 
      {
        --p;
        x.destroy(p);
      }
      x.deallocate(b, s);
      throw;
    }
    CPPUNIT_ASSERT(std::count(b, e, v) == s);
    while (b != e)
    {
      --e;
      x.destroy(e);
    }
    x.deallocate(b, s);
  }
  
  // -----
  // suite
  // -----

  CPPUNIT_TEST_SUITE(TestAllocator);
  CPPUNIT_TEST(test_one);
  CPPUNIT_TEST(test_ten);
  CPPUNIT_TEST_SUITE_END();
};
  
// ----
// main
// ----

int main ()
{
  using namespace std;
  ios_base::sync_with_stdio(false);    // turn off synchronization with C I/O
  cout << "TestAllocator.c++" << endl;
  
  CppUnit::TextTestRunner tr;
  
  // Testing TestAllocator
  tr.addTest(TestAllocator< std::allocator<int> >::suite());
  tr.addTest(TestAllocator< std::allocator<double> >::suite());
  tr.addTest(TestAllocator< Allocator<int, 100> >::suite());
  tr.addTest(TestAllocator< Allocator<double, 100> >::suite());
  tr.addTest(TestAllocator< Allocator<int, 278> >::suite());
  
  // Test TestOurAllocator
  tr.addTest(TestOurAllocator< Allocator<int, 100>, 4 >::suite());
  tr.addTest(TestOurAllocator< Allocator<double, 100>, 8 >::suite());
  tr.addTest(TestOurAllocator< Allocator<int, 278>, 4 >::suite());
  tr.addTest(TestOurAllocator< Allocator<double, 450>, 8 >::suite());


  tr.run();
  
  cout << "Done." << endl;
  return 0;
}
