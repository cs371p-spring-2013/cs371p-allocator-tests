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
// -------
// defines
// -------

//#define private public
//#define protected public
//#define class struct

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
#define class struct

#include "Allocator.h"

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
        A x; //Default constructor
        const difference_type s = 1;
        const value_type      v = 2;
        const pointer         p = x.allocate(s);    //Equiv to new
        x.construct(p, v);  //Equiv to *p = v
        CPPUNIT_ASSERT(*p == v);
        x.destroy(p);
        x.deallocate(p, s);
    }

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

template <typename A>
struct ourTests : CppUnit::TestFixture {
    // --------
    // typedefs
    // --------

    typedef typename A::value_type      value_type;
    typedef typename A::difference_type difference_type;
    typedef typename A::pointer         pointer;

    // --------
    // test constructors
    // --------
    void test_constructor1() {
        try 
        {
            A x;
            CPPUNIT_ASSERT(true); // We should never see this line in some tests.
        } 
        catch (std::bad_alloc& b) {
            CPPUNIT_ASSERT(true);
        }
    }
    void test_constructor2() {
        try 
        {
            A x;
            int first_sentinel_value = x.a[0];
            int last_sentinel_value = x.a[sizeof(x.a)-sizeof(int)];
            CPPUNIT_ASSERT(first_sentinel_value == last_sentinel_value);
        } 
        catch (std::bad_alloc& b) {
            CPPUNIT_ASSERT(true);
        }
    }
    // --------
    // test valid
    // --------
    void test_valid1() {
        try {
            A x;
            x.view(x.a[0]) = -92;
            x.view(x.a[sizeof(x.a)-sizeof(int)]) = -92;
            CPPUNIT_ASSERT(x.valid());
        } 
        catch (std::bad_alloc& b) 
        {
            CPPUNIT_ASSERT(true);
        }
    }
    void test_valid2() {
        try 
        {
            A x;
            x.view(x.a[0]) = -92;
            x.view(x.a[sizeof(x.a)-sizeof(int)]) = 92;
            CPPUNIT_ASSERT(!x.valid());
        } 
        catch (std::bad_alloc& b) 
        {
            CPPUNIT_ASSERT(true);
        }
    }
    void test_valid3() {
        try
        {
            A x;
            const difference_type s = 2;
            const value_type      v = 42;
            // make 2 or 3 blocks for valid to check
            const pointer p1 = x.allocate(s);
            const pointer p2 = x.allocate(1);

            x.construct(p1, v);
            x.construct(p2, v);

            x.destroy(p1);
            x.destroy(p2);

            x.deallocate(p1, 2);
            x.deallocate(p2, 1);

            CPPUNIT_ASSERT(x.valid());
        }
        catch (std::bad_alloc& b) {
            CPPUNIT_ASSERT(true);
        }
        
    }
    // --------
    // test allocate
    // --------
    void test_allocate1() {
        try {
            A x;
            const difference_type s = 1;
            const pointer p = x.allocate(s);

            int first = x.a[0];
            int last = x.a[sizeof(x.a)-sizeof(int)];
            
            CPPUNIT_ASSERT(first != last);
            
            x.deallocate(p);

            first = x.a[0];
            last = x.a[sizeof(x.a)-sizeof(int)];
            
            CPPUNIT_ASSERT(first == last);

            x.destroy(p);
        } 
        catch (std::bad_alloc& b) {
            CPPUNIT_ASSERT(true);
        }

    }
    void test_allocate2() {
        try {
            A x;
            // allocate a few sections, then allocate too much
            const pointer p1 = x.allocate(1);
            const pointer p2 = x.allocate(2);

            const pointer p3 = x.allocate(sizeof(x.a)*2);
            CPPUNIT_ASSERT(false);
            x.construct(p1, 42);
            x.construct(p2, 42);
            x.construct(p3, 42);

            x.destroy(p1);
            x.destroy(p2);
            x.destroy(p3);

            x.deallocate(p1,1);
            x.deallocate(p2,2);
            x.deallocate(p3,sizeof(x.a)*2);

        } 
        catch (std::bad_alloc& b) {
            CPPUNIT_ASSERT(true);
        }
        
    }
    void test_allocate3() {
        try 
        {
            A x;
            // try allocating too much

            const pointer p1 = x.allocate(sizeof(x.a)*10);
            x.deallocate(p1, sizeof(x.a)*10);
            CPPUNIT_ASSERT(false);
        } 
        catch (std::bad_alloc& b) {
            CPPUNIT_ASSERT(true);
        }
    }
    // --------
    // test deallocate
    // --------
    void test_deallocate1() {
        try 
        {
            A x;
            // practice moving stuff
            pointer p1 = x.allocate(1);
            pointer p2 = x.allocate(2);
            pointer p3 = x.allocate(3);

            CPPUNIT_ASSERT(x.valid());



            x.deallocate(p1);
            x.deallocate(p2);
            x.deallocate(p3);
            CPPUNIT_ASSERT(x.valid());

        } 
        catch (std::bad_alloc& b) {
            CPPUNIT_ASSERT(true);
        }
    }
    void test_deallocate2() {
        try 
        {
            A x;

            pointer p1 = x.allocate(1);
            pointer p2 = x.allocate(1);

            x.deallocate(p2);
            x.deallocate(p1);

            CPPUNIT_ASSERT(x.a[0] == x.a[sizeof(x.a)-sizeof(int)]);

            CPPUNIT_ASSERT(x.valid());

        } 
        catch (std::bad_alloc& b) {
            CPPUNIT_ASSERT(true);
        }
    }
    void test_deallocate3() {
        try 
        {
            A x;
            // ensure deallocate merges multiple blocks
            pointer p1 = x.allocate(2);
            pointer p2 = x.allocate(1);
            pointer p3 = x.allocate(2);

            x.deallocate(p3);
            x.deallocate(p2);
            x.deallocate(p1);
            
            CPPUNIT_ASSERT(x.valid());
        } 
        catch (std::bad_alloc& b) {
            CPPUNIT_ASSERT(true);
        }
    }

    // --------
    // random tests
    // --------

    void test_one() {
        try 
        {
            A x;
            const difference_type   s = 2;
            const value_type    v = 7;
            const pointer b = x.allocate(s);
            
            pointer e = b+s;
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
            x.deallocate(b, s);
        }
        catch (std::bad_alloc& b) {
            CPPUNIT_ASSERT(true);
        }
    }
    void test_two() {
        try 
        {
            A x; //Default constructor
            const difference_type s = 1;
            const value_type      v = 2;
            const pointer         p = x.allocate(s);    //Equiv to new
            x.construct(p, v);  //Equiv to *p = v
            CPPUNIT_ASSERT(*p == v);
            x.destroy(p);
            x.deallocate(p, s);
        }
        catch (std::bad_alloc& b) {
            CPPUNIT_ASSERT(true);
        }
    }
    // -----
    // suite
    // -----

    CPPUNIT_TEST_SUITE (ourTests);
    
    CPPUNIT_TEST(test_constructor1);
    CPPUNIT_TEST(test_constructor2);

    CPPUNIT_TEST(test_valid1);
    CPPUNIT_TEST(test_valid2);
    CPPUNIT_TEST(test_valid3);

    CPPUNIT_TEST(test_allocate1);
    CPPUNIT_TEST(test_allocate2);
    CPPUNIT_TEST(test_allocate3);

    CPPUNIT_TEST(test_deallocate1);
    CPPUNIT_TEST(test_deallocate2);
    CPPUNIT_TEST(test_deallocate3);

    CPPUNIT_TEST(test_one);
    CPPUNIT_TEST(test_two);

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
    tr.addTest(TestAllocator< Allocator<int, 100> >::suite()); // uncomment!

    tr.addTest(TestAllocator< std::allocator<double> >::suite());
    tr.addTest(TestAllocator< Allocator<double, 100> >::suite()); // uncomment!

    //TODO: uncomment tests
    tr.addTest(ourTests< Allocator<int, 100> >::suite());
    tr.addTest(ourTests< Allocator<int, 5> >::suite());
    tr.addTest(ourTests< Allocator<double, 100> >::suite());
    tr.addTest(ourTests< Allocator<double, 15> >::suite());



    tr.run();

    cout << "Done." << endl;
    return 0;
}
