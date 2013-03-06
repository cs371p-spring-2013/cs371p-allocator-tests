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
#include <cstring>   // strcmp

#include "cppunit/extensions/HelperMacros.h" // CPPUNIT_TEST, CPPUNIT_TEST_SUITE, CPPUNIT_TEST_SUITE_END
#include "cppunit/TestFixture.h"             // TestFixture
#include "cppunit/TextTestRunner.h"          // TestRunner

// ------
// macros
// ------

#define private public
#define protected public
//#define struct class



#include "Allocator.h"

// -------------
// TestAllocator
// -------------

template <typename A>
struct TestHelperFunctions : CppUnit::TestFixture {

    // ---------
    // ypte_defs
    // ---------

    typedef typename A::value_type      value_type;
    typedef typename A::difference_type difference_type;
    typedef typename A::pointer         pointer;



    // ----------
    // test_valid
    // ----------

    void test_valid_1() {
        A x;
        CPPUNIT_ASSERT(x.valid());
    }

    void test_valid_2() {
        A x;
        *reinterpret_cast<int*>(&(x.a[0])) = -92;
        CPPUNIT_ASSERT(x.valid() == false);
    }

    void test_valid_3() {
        A x;
        int size = *reinterpret_cast<int*>(&(x.a[0]));
        *reinterpret_cast<int*>(&(x.a[size + sizeof(int)])) = -92;
        CPPUNIT_ASSERT(x.valid() == false);
    }

    void test_valid_4() {
        A x;
        int size = *reinterpret_cast<int*>(&(x.a[0]));
        *reinterpret_cast<int*>(&(x.a[2 * sizeof(int)])) = size - 2*sizeof(int);
        *reinterpret_cast<int*>(&(x.a[size + sizeof(int)])) = size - 2*sizeof(int);
        *reinterpret_cast<int*>(&(x.a[0])) = 0;
        *reinterpret_cast<int*>(&(x.a[sizeof(int)])) = 0;
        CPPUNIT_ASSERT(x.valid() == true);
    }

    void test_valid_5() {
        A x;
        int size = *reinterpret_cast<int*>(&(x.a[0]));
        *reinterpret_cast<int*>(&(x.a[0])) = 3 * size;
        CPPUNIT_ASSERT(x.valid() == false);
    }


    // -----------
    // constructor
    // -----------
    void test_constructor_1 () {
        Allocator<int , 66> x;
        int sent1 = *reinterpret_cast<int*>(&(x.a[0]));
        int sent2 = *reinterpret_cast<int*>(&(x.a[62]));
        CPPUNIT_ASSERT(sent1 == sent2);
        CPPUNIT_ASSERT(sent1 == 58);
    }

    void test_constructor_2 () {
        Allocator<double , 66> x;
        int sent1 = *reinterpret_cast<int*>(&(x.a[0]));
        int sent2 = *reinterpret_cast<int*>(&(x.a[62]));
        CPPUNIT_ASSERT(sent1 == sent2);
        CPPUNIT_ASSERT(sent1 == 58);
    }

    void test_constructor_3 () {
        Allocator<int , 10000> x;
        int sent1 = *reinterpret_cast<int*>(&(x.a[0]));
        int sent2 = *reinterpret_cast<int*>(&(x.a[9996]));
        CPPUNIT_ASSERT(sent1 == sent2);
        CPPUNIT_ASSERT(sent1 == 9992);
    }


    // --------
    // allocate
    // --------
    
    
    void test_allocate_1 () {
        A x;
        const difference_type s = 4;
        const pointer         b = x.allocate(s);
        x.deallocate(b, s);
        CPPUNIT_ASSERT(*reinterpret_cast<int*>(&(x.a[0])) > 0);
    }

    void test_allocate_2 () {
        A x;
        const difference_type   s1 = 5;
        const difference_type   s2 = 4;
        const pointer           b1 = x.allocate(s1);
        const pointer           b2 = x.allocate(s2);
        CPPUNIT_ASSERT(b1 != b2);
        x.deallocate(b1, s1);
        x.deallocate(b2, s2);
    }


    void test_allocate_3 () {
        A x;
        const difference_type   s1 = 5;
        const difference_type   s2 = 5;
        const pointer           b1 = x.allocate(s1);
        x.deallocate(b1, s1);
        const pointer           b2 = x.allocate(s2);
        CPPUNIT_ASSERT(b1 == b2);
        x.deallocate(b2, s2);
    }

    // ----------
    // deallocate
    // ----------
  
    void test_deallocate_1 () {
        A x;
        const difference_type   s1 = 3;
        const difference_type   s2 = 3;
        const difference_type   s3 = 3;
        const pointer           b1 = x.allocate(s1);
        const pointer           b2 = x.allocate(s2);
        const pointer           b3 = x.allocate(s3);
        x.deallocate(b3, s3);
        x.deallocate(b2, s2);
        x.deallocate(b1, s1);
        CPPUNIT_ASSERT(*reinterpret_cast<int*>(&(x.a[0])) > 0); 
    }

    void test_deallocate_2 () {
         A x;
        const difference_type   s1 = 3;
        const difference_type   s2 = 3;
        const difference_type   s3 = 3;
        const difference_type   s4 = 5;
        const pointer           b1 = x.allocate(s1);
        const pointer           b2 = x.allocate(s2);
        const pointer           b3 = x.allocate(s3);
        x.deallocate(b3, s3);
        x.deallocate(b2, s2);
        const pointer           b4 = x.allocate(s4);
        CPPUNIT_ASSERT(b2 == b4);
        x.deallocate(b1, s1);
        x.deallocate(b4, s4);
    }

    void test_deallocate_3 () {
        A x;
        const difference_type   s1 = 5;
        const difference_type   s2 = 5;
        const pointer           b1 = x.allocate(s1);
        x.deallocate(b1, s1);
        const pointer           b2 = x.allocate(s2);
        CPPUNIT_ASSERT(b1 == b2);
        x.deallocate(b2, s2);
    }

    // -------------
    // set_sentinels
    // -------------


    void test_set_sentinels_1 () {
        A x;
        x.set_sentinels(1, 20, 0);
        CPPUNIT_ASSERT(*reinterpret_cast<int*>(&(x.a[0])) == 20);
        CPPUNIT_ASSERT(*reinterpret_cast<int*>(&(x.a[24])) == 20);
    }

   void test_set_sentinels_2 () {
        A x;
        x.set_sentinels(-1, -20, 0);
        CPPUNIT_ASSERT(*reinterpret_cast<int*>(&(x.a[0])) == -20);
        CPPUNIT_ASSERT(*reinterpret_cast<int*>(&(x.a[24])) == -20);
    }

   void test_set_sentinels_3 () {
        A x;
        x.set_sentinels(1, 20, 20);
        CPPUNIT_ASSERT(*reinterpret_cast<int*>(&(x.a[20])) == 20);
        CPPUNIT_ASSERT(*reinterpret_cast<int*>(&(x.a[44])) == 20);
    }


    CPPUNIT_TEST_SUITE(TestHelperFunctions);
    CPPUNIT_TEST(test_valid_1);
    CPPUNIT_TEST(test_valid_2);
    CPPUNIT_TEST(test_valid_3);
    CPPUNIT_TEST(test_valid_4);
    CPPUNIT_TEST(test_valid_5);
    CPPUNIT_TEST(test_constructor_1);
    CPPUNIT_TEST(test_constructor_2);
    CPPUNIT_TEST(test_constructor_3);
    CPPUNIT_TEST(test_allocate_1);
    CPPUNIT_TEST(test_allocate_2);
    CPPUNIT_TEST(test_allocate_3);
    CPPUNIT_TEST(test_deallocate_1);
    CPPUNIT_TEST(test_deallocate_2);
    CPPUNIT_TEST(test_deallocate_3);
    CPPUNIT_TEST(test_set_sentinels_1);
    CPPUNIT_TEST(test_set_sentinels_2);
    CPPUNIT_TEST(test_set_sentinels_3);
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



    // --------
    // test_two
    // --------

    void test_two () {
        A x;
        const difference_type   s = 1000;
        try {
            const pointer           b = x.allocate(s);
            x.deallocate(b, s);
        } catch( std::exception& e) {
            CPPUNIT_ASSERT(strcmp(e.what(), "std::bad_alloc") == 0);
        }
    }

    // ----------
    // test_three
    // ----------

    void test_three () {
        A x;
        const difference_type   s = -1;
        try {
            const pointer           b = x.allocate(s);
            x.deallocate(b, s);
        } catch( std::exception& e) {
            CPPUNIT_ASSERT(strcmp(e.what(), "std::bad_alloc") == 0);
        }
    }

    // ---------
    // test_four
    // ---------

    void test_four () {
        A x;
        const pointer           b1 = x.allocate(0);
        x.deallocate(b1, 0);
    }


    // ---------
    // test_five
    // ---------

    void test_five () {
        A x;
        const difference_type   s1 = 5;
        const difference_type   s2 = 5;
        const pointer           b1 = x.allocate(s1);
        x.deallocate(b1, s1);
        const pointer           b2 = x.allocate(s2);
        x.deallocate(b2, s2);
    }

       
    // --------
    // test_six
    // --------

    void test_six () {
        A x;
        const difference_type s = 4;
        const pointer         b = x.allocate(s);
        x.deallocate(b, s);
    }

    // ----------
    // test_seven
    // ---------

    void test_seven () {
        A x;
        const difference_type   s1 = 3;
        const difference_type   s2 = 3;
        const difference_type   s3 = 3;
        const pointer           b1 = x.allocate(s1);
        const pointer           b2 = x.allocate(s2);
        const pointer           b3 = x.allocate(s3);
        x.deallocate(b3, s3);
        x.deallocate(b2, s2);
        x.deallocate(b1, s1);
    }

    // ----------
    // test_eight
    // ----------

    void test_eight () {
         A x;
        const difference_type   s1 = 3;
        const difference_type   s2 = 3;
        const difference_type   s3 = 3;
        const difference_type   s4 = 5;
        const pointer           b1 = x.allocate(s1);
        const pointer           b2 = x.allocate(s2);
        const pointer           b3 = x.allocate(s3);
        x.deallocate(b3, s3);
        x.deallocate(b2, s2);
        const pointer           b4 = x.allocate(s4);
        x.deallocate(b1, s1);
        x.deallocate(b4, s4);
    }

    // ---------
    // test_nine
    // ---------

    void test_nine () {
        A x;
        const difference_type   s1 = 5;
        const difference_type   s2 = 5;
        const pointer           b1 = x.allocate(s1);
        x.deallocate(b1, s1);
        const pointer           b2 = x.allocate(s2);
        x.deallocate(b2, s2);
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
        x.deallocate(b, s);}

    // -----
    // suite
    // -----

    CPPUNIT_TEST_SUITE(TestAllocator);
    CPPUNIT_TEST(test_one);
    CPPUNIT_TEST(test_two);
    CPPUNIT_TEST(test_three);
    CPPUNIT_TEST(test_four);
    CPPUNIT_TEST(test_five);
    CPPUNIT_TEST(test_six);
    CPPUNIT_TEST(test_seven);
    CPPUNIT_TEST(test_eight);
    CPPUNIT_TEST(test_nine);
    CPPUNIT_TEST(test_ten);
    CPPUNIT_TEST_SUITE_END();};

// ----
// main
// ----

int main () {
    using namespace std;
    ios_base::sync_with_stdio(false);    // turn off synchronization with C I/O
    cout << "TestAllocator.c++" << endl;

    CppUnit::TextTestRunner tr;

    tr.addTest(TestHelperFunctions<Allocator<char,      100> >::suite());
    tr.addTest(TestHelperFunctions<Allocator<int,       100> >::suite());
    tr.addTest(TestHelperFunctions<Allocator<float,     100> >::suite());
    tr.addTest(TestHelperFunctions<Allocator<double,    100> >::suite());
    tr.addTest(TestHelperFunctions<Allocator<long,      100> >::suite());
    tr.addTest(TestHelperFunctions<Allocator<long long, 100> >::suite());
    tr.addTest(TestHelperFunctions<Allocator<bool,      100> >::suite());
    tr.addTest(TestHelperFunctions<Allocator<short,     100> >::suite());

    tr.addTest(TestAllocator< std::allocator<int> >::suite());
    tr.addTest(TestAllocator< Allocator<int, 100> >::suite()); // uncomment!

    tr.addTest(TestAllocator< std::allocator<double> >::suite());
    tr.addTest(TestAllocator< Allocator<double, 100> >::suite()); // uncomment!

    tr.run();

    cout << "Done." << endl;
    return 0;}
