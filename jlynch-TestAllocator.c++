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

#define private public 
#define protected public

#include "Allocator.h"

// -------------
// TestAllocator
// -------------

template <typename A>
struct TestMyAllocator : CppUnit::TestFixture{

    typedef typename A::value_type      value_type;
    typedef typename A::difference_type difference_type;
    typedef typename A::pointer         pointer;

    // ----------------
    // validity tests
    // ----------------

    //test: newly constructed object should be valid
    void test_valid_new(){
      A x;
      CPPUNIT_ASSERT(x.valid());
    }

    //test: just write over first sentinel
    void test_vaild_missing_start(){
      A x;
      int* start = reinterpret_cast<int*> (x.a);
      *start = 0;
      ++start;
      *start = 1;
      bool valid = x.valid();
      CPPUNIT_ASSERT(!valid);
    }

    //test: just write over last sentinel
    void test_valid_missing_end(){
      A x;
      int n = sizeof(x.a);
      x.a[n-1] = 0;
      x.a[n-2] = 0;
      x.a[n-3] = 0;
      x.a[n-4] = 0;
      CPPUNIT_ASSERT(!x.valid());

    }
    
    void test_valid_missing_middle(){
      Allocator<int, 100> x; 
      int* sent_ptr = reinterpret_cast<int*> (x.a);
      *sent_ptr=0;
      ++sent_ptr;
      *sent_ptr=0;
      
      ++sent_ptr;
      *sent_ptr=0;
      ++sent_ptr;
      *sent_ptr=1;
    
      CPPUNIT_ASSERT(!x.valid());
    }

    // Tests with custom allocator object:
    void test_valid_basic(){

      //Assume the constructor hasn't done its job, and set sentinels manually, 
      //so we're independent of it
      A x;
      //Should have size-8 visible bytes
      int size = sizeof(x.a);
      int sentinel_value = size-8;
      int* sentinel = reinterpret_cast<int*> (x.a);
      *sentinel = sentinel_value;
      sentinel = reinterpret_cast<int*> (&x.a[size]);
      --sentinel;
      *sentinel = sentinel_value;

      CPPUNIT_ASSERT(x.valid());
    }

    void test_valid_negative_pass(){
      A x;
      //Should have size-8 visible bytes
      int size = sizeof(x.a);
      int sentinel_value = size-8;
      int* sentinel = reinterpret_cast<int*> (x.a);
      *sentinel = -sentinel_value;
      sentinel = reinterpret_cast<int*> (&x.a[size]);
      --sentinel;
      *sentinel = -sentinel_value;

      CPPUNIT_ASSERT(x.valid());
    }
    void test_valid_negative_fail(){
      A x;
      //Should have size-8 visible bytes
      int size = sizeof(x.a);
      int sentinel_value = size-8;
      int* sentinel = reinterpret_cast<int*> (x.a);
      *sentinel = -sentinel_value;
      sentinel = reinterpret_cast<int*> (&x.a[size]);
      --sentinel;
      *sentinel = sentinel_value;

      CPPUNIT_ASSERT(!x.valid());
    }

    // ----------------
    // constructor tests
    // ----------------
    void test_constructor_basic_int(){
      const int size = 8;
      int expected = size - 8;
      Allocator<int, size> x;
      int* sentinel_ptr = reinterpret_cast<int*> (x.a);
      CPPUNIT_ASSERT_EQUAL(expected, *sentinel_ptr);
      ++sentinel_ptr;
      CPPUNIT_ASSERT_EQUAL(expected, *sentinel_ptr);
    }

    void test_constructor_basic_double(){
      const int size = 12;
      int expected = size - 8;
      Allocator<double, size> x;
      int* sentinel_ptr = reinterpret_cast<int*> (x.a);
      CPPUNIT_ASSERT_EQUAL(expected, *sentinel_ptr);
      ++sentinel_ptr;
      ++sentinel_ptr;
      CPPUNIT_ASSERT_EQUAL(expected, *sentinel_ptr);
    }


    //test constructor with size less than 8
    void test_constructor_too_small(){
      try{
        Allocator<int, 1> a;
        CPPUNIT_ASSERT(false);
      } catch(std::bad_alloc e){
        CPPUNIT_ASSERT(true);
      }
    }

    // ----------------
    // view tests
    // ----------------
    void test_view_zero(){
      A x;
      char arr[]  = {0,0,0,0};
      CPPUNIT_ASSERT_EQUAL(0, x.view(arr));
    }

    void test_view_five(){
      A x;
      char arr[]  = {0,0,0,0};
      int* ptr = reinterpret_cast<int*>(arr);
      *ptr = 5;
      CPPUNIT_ASSERT_EQUAL(5, x.view(arr));
    }

    void test_view_ten(){
      A x;
      char arr[]  = {0,0,0,0};
      int* ptr = reinterpret_cast<int*>(arr);
      *ptr = 10;
      CPPUNIT_ASSERT_EQUAL(10, x.view(arr));
    }

    // ----------------
    // allocate tests
    // ----------------
    //Note: I can't seem to find a way to get the array size that works
    // for both my allocator and the std one, so I'm just assuming they have
    // less than 1000 units and at least 100

    //Failure tests

    //test: allocate once and then more than remaining
    void test_allocate_extra(){
      A x;
      const int n = sizeof(x.a);
      const difference_type s = 1;
      pointer p;
      CPPUNIT_ASSERT_NO_THROW(p = x.allocate(s));
      CPPUNIT_ASSERT(p != 0);
      //Should fail
      CPPUNIT_ASSERT_THROW(x.allocate(n), std::bad_alloc);
    }
    //test: allocate once with more than full size
    void test_allocate_over_max(){
      A x;
      const difference_type s = sizeof(x.a);
      //This should fail
      CPPUNIT_ASSERT_THROW(x.allocate(s), std::bad_alloc);

    }
    //test: allocate once so that only 8 bytes left, then attempt to allocate again
    void test_allocate_with_little_remainder(){
      const int size = 20;
      Allocator<int, size> x;

      //Min bytes for one int allocation: 12
      //Currently, 8 are occupied by the sentinels, so the block we can get is
      //of size 12
      //Allocating one int will take 12 bytes, and leave 8, which is less than
      //the 12 we would need for the new free block to be useful so the whole 
      //thing should get taken up
      int* p;
      CPPUNIT_ASSERT_NO_THROW((p = x.allocate(1)));
      CPPUNIT_ASSERT(p != 0);
      
      //Allocating at all should fail
      CPPUNIT_ASSERT_THROW(x.allocate(1), std::bad_alloc);
    }

    //Success tests -i.e. nothing should go wrong here
    //test: allocate several times with total less than full size, shouldn't get error
    void test_allocate_repeatedly(){
      A x;
      pointer p;
      pointer prev = 0;
      for(int i = 0 ; i < 5; i++){
        CPPUNIT_ASSERT_NO_THROW(p = x.allocate(1));
        CPPUNIT_ASSERT(p != 0);
        if(p!=0){
          CPPUNIT_ASSERT(p != prev);
        }
        prev = p;
      }
    }

    //test: allocate entire array
    void test_allocate_entire_array(){
      Allocator<int, 100> x;
      //There will be 92 available slots, lets take them all
      int* p;
      CPPUNIT_ASSERT_NO_THROW(p = x.allocate(92/sizeof(int)));
      CPPUNIT_ASSERT(p != 0);
    }

    //test: allocate and leave less than 8+sizeof(T)
    void test_allocate_with_remainder_too_small(){
      const int size = 20;
      Allocator<int, size> x;
      int* p;
      CPPUNIT_ASSERT_NO_THROW(p = x.allocate(1));
      CPPUNIT_ASSERT(p!=0);
    }

    //Allocate everything and deallocate everything
    void test_deallocate_left_to_right(){
      A x;
      
      int allocate_size = 1;
      const pointer p1 = x.allocate(allocate_size);
      CPPUNIT_ASSERT(p1!=0);
      const pointer p2 = x.allocate(allocate_size);
      CPPUNIT_ASSERT(p2!=0);
      const pointer p3 = x.allocate(allocate_size);
      CPPUNIT_ASSERT(p3!=0);

      //dealloc front
      x.deallocate(p1, allocate_size);
      //dealloc middle
      x.deallocate(p2, allocate_size);
      //dealloc end
      x.deallocate(p3, allocate_size);

      CPPUNIT_ASSERT_NO_THROW(x.allocate(allocate_size));
    }
    void test_deallocate_mixed_thorough(){
      Allocator<int, 36> x;
      //36 = 3* 12 which means we can alloc 3 individual ints
      int* p1 = x.allocate(1);
      CPPUNIT_ASSERT(p1 != 0);
      int* p2 = x.allocate(1);
      CPPUNIT_ASSERT(p2 != 0);
      int* p3 = x.allocate(1);
      CPPUNIT_ASSERT(p3 != 0);
      
      x.deallocate(p1);
      x.deallocate(p3);
      x.deallocate(p2);

      //Now let's allocate the whole thing, which should all be free if
      //everything worked as intended.
      p1 = x.allocate(7);
      
    }

    CPPUNIT_TEST_SUITE(TestMyAllocator);
      CPPUNIT_TEST(test_view_zero);
      CPPUNIT_TEST(test_view_five);
      CPPUNIT_TEST(test_view_ten);

      CPPUNIT_TEST(test_valid_new);
      CPPUNIT_TEST(test_vaild_missing_start);
      CPPUNIT_TEST(test_valid_missing_end);
      CPPUNIT_TEST(test_valid_missing_middle);
      CPPUNIT_TEST(test_valid_basic);

      CPPUNIT_TEST(test_valid_negative_pass);
      CPPUNIT_TEST(test_valid_negative_fail);

      CPPUNIT_TEST(test_constructor_too_small);
      CPPUNIT_TEST(test_constructor_basic_int);
      CPPUNIT_TEST(test_constructor_basic_double);
  
      CPPUNIT_TEST(test_allocate_extra);
      CPPUNIT_TEST(test_allocate_over_max);
      CPPUNIT_TEST(test_allocate_with_little_remainder);
      CPPUNIT_TEST(test_allocate_repeatedly);
      CPPUNIT_TEST(test_allocate_entire_array);
      CPPUNIT_TEST(test_allocate_with_remainder_too_small);

      CPPUNIT_TEST(test_deallocate_left_to_right);
      CPPUNIT_TEST(test_deallocate_mixed_thorough);

    CPPUNIT_TEST_SUITE_END();

};
template <typename A>
struct TestAllocator : CppUnit::TestFixture {
    // --------
    // typedefs
    // --------

    typedef typename A::value_type      value_type;
    typedef typename A::difference_type difference_type;
    typedef typename A::pointer         pointer;

    // --------
    // test_one
    // --------

    void test_one () {
        A x;
        const difference_type s = 1;
        const value_type      v = 2;
        const pointer         p = x.allocate(s);
        x.construct(p, v);
        CPPUNIT_ASSERT(*p == v);
        x.destroy(p);
        x.deallocate(p, s);}

    void test_repeat_3 () {
        A x;
        const difference_type s = 1;
        const value_type      v = 2;
        const pointer         p = x.allocate(s);
        x.construct(p, v);
        CPPUNIT_ASSERT(*p == v);
        x.destroy(p);

        x.construct(p, v);
        CPPUNIT_ASSERT(*p == v);
        x.destroy(p);

        x.construct(p, v);
        CPPUNIT_ASSERT(*p == v);
        x.destroy(p);

        x.deallocate(p, s);}
    // --------
    // test_ten
    // --------

    void test_ten () {
        A x;
        const difference_type s = 10;
        const value_type      v = 2;
        const pointer         b = x.allocate(s);
              pointer         e = b + s;
              pointer         p = b;
        try {
            while (p != e) {
                x.construct(p, v);
                ++p;}}
        catch (...) {
            while (b != p) {
                --p;
                x.destroy(p);}
            x.deallocate(b, s);
            throw;}
        CPPUNIT_ASSERT(std::count(b, e, v) == s);
        while (b != e) {
            --e;
            x.destroy(e);}
        x.deallocate(b, s);}

    void test_deallocate_mixed(){
      A x;
      int s = 1;
      pointer p1 = x.allocate(s);
      pointer p2 = x.allocate(s);
      pointer p3 = x.allocate(s);

      CPPUNIT_ASSERT(p1 != p2);
      CPPUNIT_ASSERT(p1 != p3);
      CPPUNIT_ASSERT(p2 != p3);

      x.deallocate(p1, s);
      x.deallocate(p3, s);
      x.deallocate(p2, s);
    }

    // ----------------
    // deallocate tests
    // ----------------

    void test_deallocate_second(){
    //2 acceptable allocates (so 2 taken, 1 free)
      //dealloc second block first
      A x;
      const difference_type s = 1;
      pointer         p1 = x.allocate(s);
      pointer p2  = x.allocate(s);
      
      x.deallocate(p2, s);
      x.deallocate(p1, s);
      CPPUNIT_ASSERT(true);
    }
    void test_deallocate_first(){
    //2 acceptable allocates (so 2 taken, 1 free)
      //dealloc first block first
      A x;
      const difference_type s = 1;
      pointer         p1 = x.allocate(s);
      pointer p2  = x.allocate(s);
      
      x.deallocate(p1, s);
      x.deallocate(p2, s);
      CPPUNIT_ASSERT(true);
    }

    // -----
    // suite
    // -----

    CPPUNIT_TEST_SUITE(TestAllocator);
    CPPUNIT_TEST(test_one);
    CPPUNIT_TEST(test_ten);
    CPPUNIT_TEST(test_repeat_3);

    CPPUNIT_TEST(test_deallocate_second);
    CPPUNIT_TEST(test_deallocate_first);
    CPPUNIT_TEST(test_deallocate_mixed);

    CPPUNIT_TEST_SUITE_END();};

// ----
// main
// ----

int main () {
    using namespace std;
    ios_base::sync_with_stdio(false);    // turn off synchronization with C I/O
    cout << "TestAllocator.c++" << endl;

    CppUnit::TextTestRunner tr;

    tr.addTest(TestAllocator< std::allocator<int> >::suite());
    tr.addTest(TestMyAllocator< Allocator<int, 100> >::suite()); 
    tr.addTest(TestAllocator< Allocator<int, 100> >::suite()); 

    tr.addTest(TestAllocator< std::allocator<double> >::suite());
    tr.addTest(TestMyAllocator< Allocator<double, 100> >::suite()); 
    tr.addTest(TestAllocator< Allocator<double, 100> >::suite());

    tr.run();

    cout << "Done." << endl;
    return 0;}
