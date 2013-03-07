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
#include <iostream> // ios_base
#include <memory> // allocator

#include "cppunit/extensions/HelperMacros.h" // CPPUNIT_TEST, CPPUNIT_TEST_SUITE, CPPUNIT_TEST_SUITE_END
#include "cppunit/TestFixture.h" // TestFixture
#include "cppunit/TextTestRunner.h" // TestRunner

#define private public
#define protected public
// #define class struct

#include "Allocator.h"

// -------------
// TestAllocator
// -------------

template <typename A>
struct TestAllocator : CppUnit::TestFixture {
    // --------
    // typedefs
    // --------

    typedef typename A::value_type value_type;
    typedef typename A::difference_type difference_type;
    typedef typename A::pointer pointer;

    // --------
    // test_one
    // --------

    void test_one () {
        A x;
        const difference_type s = 1;
        const value_type v = 2;
        const pointer p = x.allocate(s);
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
        const value_type v = 2;
        const pointer b = x.allocate(s);
              pointer e = b + s;
              pointer p = b;
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


    // -----
    // suite
    // -----

    CPPUNIT_TEST_SUITE(TestAllocator);
    CPPUNIT_TEST(test_one);
    CPPUNIT_TEST(test_ten);
    CPPUNIT_TEST_SUITE_END();};

struct MyTests : CppUnit::TestFixture {

    // -----
    // valid
    // -----

    void test_valid_1() {
        Allocator<double, 100>  x;
        CPPUNIT_ASSERT(x.valid());
    }

    void test_valid_2() {
        Allocator<double, 100>  x;
        *reinterpret_cast<int*>(&(x.a[0])) = -92;
        CPPUNIT_ASSERT(!x.valid());
    }

    void test_valid_3() {
        Allocator<double, 100>  x;
        int free = *reinterpret_cast<int*>(&(x.a[0]));
        *reinterpret_cast<int*>(&(x.a[free + sizeof(int)])) = -92;
        CPPUNIT_ASSERT(!x.valid());
    }

    void test_valid_4() {
        Allocator<double, 100>  x;
        *reinterpret_cast<int*>(&(x.a[0])) = -40;
        *reinterpret_cast<int*>(&(x.a[44])) = -40;
        *reinterpret_cast<int*>(&(x.a[48])) = 44;
        *reinterpret_cast<int*>(&(x.a[96])) = 44;
        CPPUNIT_ASSERT(x.valid());
    }

    void test_valid_5() {
        Allocator<double, 100>  x;
        *reinterpret_cast<int*>(&(x.a[0])) = -20;
        *reinterpret_cast<int*>(&(x.a[24])) = -20;
        *reinterpret_cast<int*>(&(x.a[28])) = -30;
        *reinterpret_cast<int*>(&(x.a[62])) = -30;
        *reinterpret_cast<int*>(&(x.a[66])) = 26;
        *reinterpret_cast<int*>(&(x.a[96])) = 26;
        CPPUNIT_ASSERT(x.valid());
    }
 
    //------
    // view
    //------

    void test_view_1() {
      char a[100] = {0, 0, 0, 0, 0, 0, 0};
      Allocator<char, 100> x;
      CPPUNIT_ASSERT(x.view(a[2]) == 0);
    }

    void test_view_2() {
      char a[100] = {104, 0, 0, 0};
      Allocator<char, 100> x;
      CPPUNIT_ASSERT(x.view(a[0]) == 104);
    }
 
    void test_view_3() {
      char a[100] = {70, 0, 0, 0};
      Allocator<char, 100> x;
      CPPUNIT_ASSERT(x.view(a[0]) == 70);
    }

    // --------
    // constructor
    // --------

    void test_constructor_1(){
        try{
            Allocator<int,4> x;
            CPPUNIT_ASSERT(false);
        }
        catch(std::bad_alloc& e){
            CPPUNIT_ASSERT(true);
        }
    }

    void test_constructor_2(){
        Allocator<int, 100> x;
        CPPUNIT_ASSERT(x.valid());
        Allocator<double, 100> y;
        CPPUNIT_ASSERT(y.valid());
    }
    
   //---------
   // allocate
   //---------

    void test_allocate_1() {
      Allocator<double, 100> x;
      x.allocate(5);
      CPPUNIT_ASSERT(x.view(x.a[0]) ==  -40);
      CPPUNIT_ASSERT(x.view(x.a[44]) == -40);
      CPPUNIT_ASSERT(x.view(x.a[48]) == 44);
      CPPUNIT_ASSERT(x.view(x.a[96]) == 44);
      CPPUNIT_ASSERT(x.valid());
    }    

   void test_allocate_2 () {
     Allocator<int, 100> x;
     x.allocate(3);

     CPPUNIT_ASSERT(x.view(x.a[0]) == -12);
     CPPUNIT_ASSERT(x.view(x.a[16]) == -12);
     CPPUNIT_ASSERT(x.view(x.a[20]) == 72);
     CPPUNIT_ASSERT(x.view(x.a[96]) == 72);
     CPPUNIT_ASSERT(x.valid());
    }
    
    void test_allocate_3 () {
     Allocator<char, 100> x;
     x.allocate(90);
    
     CPPUNIT_ASSERT(x.view(x.a[0]) == -92);
     CPPUNIT_ASSERT(x.view(x.a[96]) == -92);
     CPPUNIT_ASSERT(x.valid());	
    }
    
   void test_allocate_4 () {
     Allocator<double, 100> x;
     try {
       x.allocate(500);
       CPPUNIT_ASSERT(false);
     }
     catch(std::bad_alloc& e) {
       CPPUNIT_ASSERT(true);
     }
     }
    

    void test_allocate_5 () {
     Allocator<double, 100> x;
     x.allocate(6);
     x.allocate(2);
     x.allocate(1);
    
     CPPUNIT_ASSERT(x.view(x.a[0]) == -48);
     CPPUNIT_ASSERT(x.view(x.a[52]) == -48);
     CPPUNIT_ASSERT(x.view(x.a[56]) == -16);
     CPPUNIT_ASSERT(x.view(x.a[76]) == -16);
     CPPUNIT_ASSERT(x.view(x.a[80]) == -12);
     CPPUNIT_ASSERT(x.view(x.a[96]) == -12);
     CPPUNIT_ASSERT(x.valid());
    }
 
   void test_allocate_6()
    {
      Allocator<int, 100> x;
      x.allocate(10);
      x.allocate(8);
      x.allocate(1);
      try
      {
        x.allocate(1);
        CPPUNIT_ASSERT(false);
      }
      catch (std::bad_alloc e)
      {
        CPPUNIT_ASSERT(true);
      }
      CPPUNIT_ASSERT(x.valid());
    }    

    //-----------
    // deallocate
    //-----------
    
    void test_deallocate_1 () {
     Allocator<char, 100> x;
     char* p1 = x.allocate(91);
     x.deallocate(p1);
     CPPUNIT_ASSERT(x.view(x.a[0]) == 92);
     CPPUNIT_ASSERT(x.view(x.a[96]) == 92);
     CPPUNIT_ASSERT(x.valid());	
    }

    void test_deallocate_2 () {
     Allocator<char, 100> x;
     char* p1 = x.allocate(5);
     x.deallocate(p1);
     CPPUNIT_ASSERT(x.view(x.a[0]) == 92);
     CPPUNIT_ASSERT(x.view(x.a[96]) == 92);
     CPPUNIT_ASSERT(x.valid());
    }
   
    void test_deallocate_3()
    {
      Allocator<int, 100> x;
      x.allocate(5);
      int* p = x.allocate(5);
      x.allocate(5);
      x.deallocate(p);
      CPPUNIT_ASSERT(x.valid());
    }
 
    void test_deallocate_4() {
      Allocator<int, 100> x;
      int* p = x.allocate(5);
      int* q = x.allocate(5);
      x.allocate(5);
      x.deallocate(p);
      x.deallocate(q);
      CPPUNIT_ASSERT(x.valid());
    }

    // -----
    // suite
    // -----

    CPPUNIT_TEST_SUITE(MyTests);
    CPPUNIT_TEST(test_valid_1);
    CPPUNIT_TEST(test_valid_2);
    CPPUNIT_TEST(test_valid_3);
    CPPUNIT_TEST(test_valid_4);
    CPPUNIT_TEST(test_valid_5);
    CPPUNIT_TEST(test_view_1);
    CPPUNIT_TEST(test_view_2);
    CPPUNIT_TEST(test_view_3);
    CPPUNIT_TEST(test_constructor_1);
    CPPUNIT_TEST(test_constructor_2);
    CPPUNIT_TEST(test_allocate_1);
    CPPUNIT_TEST(test_allocate_2);
    CPPUNIT_TEST(test_allocate_3);
    CPPUNIT_TEST(test_allocate_4);
    CPPUNIT_TEST(test_allocate_5);
    CPPUNIT_TEST(test_allocate_6);
    CPPUNIT_TEST(test_deallocate_1);
    CPPUNIT_TEST(test_deallocate_2);
    CPPUNIT_TEST(test_deallocate_3);
    CPPUNIT_TEST(test_deallocate_4);
    CPPUNIT_TEST_SUITE_END();};


// ----
// main
// ----

int main () {
    using namespace std;
    ios_base::sync_with_stdio(false); // turn off synchronization with C I/O
    cout << "TestAllocator.c++" << endl;

    CppUnit::TextTestRunner tr;

    tr.addTest(TestAllocator< std::allocator<int> >::suite());
    tr.addTest(TestAllocator< Allocator<int, 100> >::suite()); // uncomment!

    tr.addTest(TestAllocator< std::allocator<double> >::suite());
    tr.addTest(TestAllocator< Allocator<double, 100> >::suite()); // uncomment!
   
    tr.addTest(MyTests::suite());   


    tr.run();

    cout << "Done." << endl;
    return 0;}
