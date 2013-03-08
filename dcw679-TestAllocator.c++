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
#include <vector>
#include "cppunit/extensions/HelperMacros.h" // CPPUNIT_TEST, CPPUNIT_TEST_SUITE, CPPUNIT_TEST_SUITE_END
#include "cppunit/TestFixture.h"             // TestFixture
#include "cppunit/TextTestRunner.h"          // TestRunner

#define private public

#include "Allocator.h"

//#define protected public
//#define class struct

using namespace std;

// -------------
// TestOurAllocator
// -------------
template <typename A>
struct TestOurAllocator : CppUnit::TestFixture {

    typedef typename A::value_type      value_type;
    typedef typename A::difference_type difference_type;
    typedef typename A::pointer         pointer;
   

    // ------------------------
    // test_allocate_deallocate
    // ------------------------

    // will fit in all of the allocators given to the fixture
    void test_allocate_deallocate_1 () {
        cout << endl << "Running TestOurAllocator..." << endl;
        A x;
        try {
            pointer a = x.allocate(3);
            pointer b = x.allocate(2);
            pointer c = x.allocate(1);
            CPPUNIT_ASSERT(x.valid());
            x.deallocate(a, 3);
            x.deallocate(b, 2);
            x.deallocate(c, 1);

        } catch (...) {
            CPPUNIT_ASSERT(false);
        }

        CPPUNIT_ASSERT(x.valid());
    }
    
    // will also fit in all allocators given
    void test_allocate_deallocate_2 () {
        A x;
        try {
            pointer a = x.allocate(3);
            pointer b = x.allocate(1);
            CPPUNIT_ASSERT(x.valid());
            x.deallocate(a, 3);
            x.deallocate(b, 1);
            CPPUNIT_ASSERT(x.valid());

        } catch (...) {
            CPPUNIT_ASSERT(false);
        }
 
        CPPUNIT_ASSERT(x.valid());
    }

  

    CPPUNIT_TEST_SUITE(TestOurAllocator);
    CPPUNIT_TEST(test_allocate_deallocate_1);
    CPPUNIT_TEST(test_allocate_deallocate_2);
    CPPUNIT_TEST_SUITE_END();

};


// -------
// TestStatic
// -------
// doesn't use template arguments to set size of allocator

struct TestStatic : CppUnit::TestFixture {


    void test_size_good () {
        cout << endl << "Running TestStatic...." << endl;
        try {
            Allocator<int, 100> x;
            CPPUNIT_ASSERT(x.valid());
        } catch (...) {
            CPPUNIT_ASSERT(false);
        }
  
    }

    void test_size_bad () {
        try {
            Allocator<int, 4> x;
            CPPUNIT_ASSERT(false);
        } catch (std::bad_alloc& e) {
            CPPUNIT_ASSERT(true);
        } 
    }

    // -----
    // valid
    // -----
    void test_valid_1 () {
        Allocator<int, 100> x;
        x.view(x.a[0]) = -10;
        x.view(x.a[24]) = -10;
        x.view(x.a[28]) = -30;
        x.view(x.a[52]) = -30;
        x.view(x.a[56]) = 36;
        x.view(x.a[96]) = 36;
        CPPUNIT_ASSERT(!x.valid());
    }
    
    void test_valid_2 () {
        Allocator<double, 100> x;
        x.view(x.a[0]) =  -50;
        x.view(x.a[24]) = -50;
        CPPUNIT_ASSERT(!x.valid());
    }
    
    void test_valid_3 () {
        Allocator<int, 100> x;
        x.view(x.a[0]) = -20;
        x.view(x.a[24]) = -20;
        x.view(x.a[28]) = 0;
        x.view(x.a[32]) = 0;
        x.view(x.a[36]) = 56;
        x.view(x.a[96]) = 56;
        CPPUNIT_ASSERT(!x.valid());
    }

    void test_valid_4 () {
        Allocator<int, 100> x;
        x.view(x.a[0]) = -4;
        x.view(x.a[4]) = 13; 
        x.view(x.a[8]) = -4;
        x.view(x.a[12]) = 80;
        x.view(x.a[96]) = 80;
        CPPUNIT_ASSERT(x.valid());
    }

    // trying to force a bad_alloc throw
    void test_allocate_deallocate_3 () {
        Allocator<int, 12> x;
        int* p = x.allocate(1);
        CPPUNIT_ASSERT(x.valid());

        try {
            const int* p2  = x.allocate(1);
            CPPUNIT_ASSERT(false);
        } catch (std::bad_alloc& e) {
            CPPUNIT_ASSERT(true);
        }
   
        x.deallocate(p, 1);
        CPPUNIT_ASSERT(x.valid());
    }

    void test_coalesce_1() {
        Allocator<int, 100> x;
        x.view(x.a[0]) = -4;
        x.view(x.a[4]) = 13; 
        x.view(x.a[8]) = -4;
        x.view(x.a[12]) = 80;
        x.view(x.a[96]) = 80;
        
        x.coalesce();
        
        CPPUNIT_ASSERT(x.valid());
        CPPUNIT_ASSERT(x.view(x.a[0]) == -4);
        CPPUNIT_ASSERT(x.view(x.a[4]) == 13); 
        CPPUNIT_ASSERT(x.view(x.a[8]) == -4);
        CPPUNIT_ASSERT(x.view(x.a[12]) == 80);
        CPPUNIT_ASSERT(x.view(x.a[96]) == 80);

    }
    
    void test_coalesce_2 () {
        Allocator<int, 100> x;   
        x.view(x.a[0]) = -20;
        x.view(x.a[24]) = -20;
        x.view(x.a[28]) = 4;
        x.view(x.a[36]) = 4;
        x.view(x.a[40]) = 52;
        x.view(x.a[96]) = 52;
        
        x.coalesce();
        
        CPPUNIT_ASSERT(x.valid());
        CPPUNIT_ASSERT(x.view(x.a[0]) == -20);
        CPPUNIT_ASSERT(x.view(x.a[24]) == -20); 
        CPPUNIT_ASSERT(x.view(x.a[28]) == 64);
        CPPUNIT_ASSERT(x.view(x.a[96]) == 64);
    }
    
    void test_coalesce_3 () {
        Allocator<int, 100> x;   
        x.view(x.a[0]) = 20;
        x.view(x.a[24]) = 20;
        x.view(x.a[28]) = 64;
        x.view(x.a[96]) = 64;
        
        x.coalesce();
        
        CPPUNIT_ASSERT(x.valid());
        CPPUNIT_ASSERT(x.view(x.a[0]) == 92);
        CPPUNIT_ASSERT(x.view(x.a[96]) == 92);
    }
    
    
    CPPUNIT_TEST_SUITE(TestStatic);
    CPPUNIT_TEST(test_size_good);
    CPPUNIT_TEST(test_size_bad);
    CPPUNIT_TEST(test_valid_1);
    CPPUNIT_TEST(test_valid_2);
    CPPUNIT_TEST(test_valid_3);
    CPPUNIT_TEST(test_valid_4);
    CPPUNIT_TEST(test_allocate_deallocate_3);
    CPPUNIT_TEST(test_coalesce_1);
    CPPUNIT_TEST(test_coalesce_2);
    CPPUNIT_TEST(test_coalesce_3);
    CPPUNIT_TEST_SUITE_END();

};


// -------------
// TestAllocator
// -------------

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
        cout << endl << "Running TestAllocator... " << endl;
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

    void test_all () {
      
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
        x.deallocate(b, s);
        
    }


    void test_deallocate () {
      
      A x;
      try {
        pointer p1 = x.allocate(1);
        pointer p2 = x.allocate(1);
        pointer p3 = x.allocate(1);
        pointer p4 = x.allocate(1);
        pointer p5 = x.allocate(1);
        pointer p6 = x.allocate(1);
        //cout << p1 << " " << p5 << " " << p6 << endl;
        //cout << "DEALLOCATING P1..." << " " << *p1 << endl;
        x.deallocate(p1, 1);
        x.deallocate(p2, 1);
        x.deallocate(p3, 1);
        x.deallocate(p4, 1);
        x.deallocate(p5, 1);
        x.deallocate(p6, 1);
      } catch (...) {
        CPPUNIT_ASSERT(false);
      }
      
    }

    void test_vector () {
        
        std::vector<value_type, A> vector;
        value_type v;
        vector.push_back(v);
        vector.push_back(v);
                   
    }


    // -----
    // suite
    // -----

    CPPUNIT_TEST_SUITE(TestAllocator);
    CPPUNIT_TEST(test_one);
    CPPUNIT_TEST(test_deallocate);
    CPPUNIT_TEST(test_all);
    CPPUNIT_TEST(test_vector);
    CPPUNIT_TEST_SUITE_END();
};

// ----
// main
// ----

int main () {
    using namespace std;
    ios_base::sync_with_stdio(false);    // turn off synchronization with C I/O
    cout << "TestAllocator.c++" << endl;

    CppUnit::TextTestRunner tr;

    tr.addTest(TestAllocator< std::allocator<int> >::suite());
    tr.addTest(TestAllocator< Allocator<int, 100> >::suite());

    tr.addTest(TestAllocator< std::allocator<double> >::suite());
    tr.addTest(TestAllocator< Allocator<double, 100> >::suite());

    // we should be careful to make sure we don't make assumptions about the size of the memory pool
    // and test dynamically according to the allocator size for this struct

    tr.addTest(TestOurAllocator< Allocator<double, 100> >::suite());

    tr.addTest(TestOurAllocator< Allocator<double, 1000> >::suite());
    tr.addTest(TestOurAllocator< Allocator<char, 500> >::suite());

    tr.addTest(TestOurAllocator< Allocator<long, 100> > ::suite());
    

    // these tests, on the other hand, have no template argument, allocator size can be adjusted on a
    // per test basis
    tr.addTest(TestStatic::suite());
    tr.run();

    cout << "Done." << endl;
    return 0;
}
