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
#include <cstring> // strcmp

#include "cppunit/extensions/HelperMacros.h" // CPPUNIT_TEST, CPPUNIT_TEST_SUITE, CPPUNIT_TEST_SUITE_END
#include "cppunit/TestFixture.h"             // TestFixture
#include "cppunit/TextTestRunner.h"          // TestRunner

#define private public
// #define protected public
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

    typedef typename A::value_type      value_type;
    typedef typename A::difference_type difference_type;
    typedef typename A::pointer         pointer;


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

    // --------------
    // test_bad_alloc
    // --------------

    void test_bad_alloc_1 () {
        A x;
        const difference_type s = -1;
        try {
            x.allocate(s);
            CPPUNIT_ASSERT(false);
        } catch (exception& e) {
            CPPUNIT_ASSERT(strcmp(e.what(), "std::bad_alloc") == 0);
        }
    }

    void test_bad_alloc_2 () {
        A x;
        const difference_type s = -100;
        try {
            x.allocate(s);
            CPPUNIT_ASSERT(false);
        } catch (exception& e) {
            CPPUNIT_ASSERT(strcmp(e.what(), "std::bad_alloc") == 0);
        }
    }

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
    CPPUNIT_TEST(test_bad_alloc_1);
    CPPUNIT_TEST(test_bad_alloc_2);
    CPPUNIT_TEST(test_ten);
    CPPUNIT_TEST_SUITE_END();};

// -------------
// OurAllocator
// -------------

template <typename A>
struct OurAllocator : CppUnit::TestFixture {
    // --------
    // typedefs
    // --------

    typedef typename A::value_type      value_type;
    typedef typename A::difference_type difference_type;
    typedef typename A::pointer         pointer;

     // --------------
     // test_Allocator
     // --------------
     void test_Allocator_1(){
         try {
             Allocator<int, 100> x;
             CPPUNIT_ASSERT(x.valid() == true);
         } catch (exception& e) {
             CPPUNIT_ASSERT(false);
         }
     }

     void test_Allocator_2(){
         try {
             Allocator<int, 7> x;
             CPPUNIT_ASSERT(false);
         } catch (exception& e) {
             CPPUNIT_ASSERT(strcmp(e.what(), "std::bad_alloc") == 0);
         }
     }

     void test_Allocator_3(){
         try {
             Allocator<int, 8> x;
             CPPUNIT_ASSERT(false);
         } catch (exception& e) {
             CPPUNIT_ASSERT(strcmp(e.what(), "std::bad_alloc") == 0);
         }
     }

     void test_Allocator_4(){
         try {
             Allocator<int, 9> x;
         } catch (exception& e) {
             CPPUNIT_ASSERT(strcmp(e.what(), "std::bad_alloc") == 0);
         }
     }

     void test_Allocator_5(){
         try {
             Allocator<int, 12> x;
         } catch (exception& e) {
             CPPUNIT_ASSERT(false);
         }
     }

     // ----------
     // test_valid
     //-----------
     void test_valid_1(){
         Allocator<int, 100> x;
         CPPUNIT_ASSERT(x.valid() == true);
     }

     void test_valid_2() {
         Allocator<int, 100> x;
         *reinterpret_cast<int*>(&(x.a[0])) = -92;
         CPPUNIT_ASSERT(x.valid() == false);
     }
 
     void test_valid_3() {
         Allocator<int, 100> x;
         int size = *reinterpret_cast<int*>(&(x.a[0]));
         *reinterpret_cast<int*>(&(x.a[size + sizeof(int)])) = -92;
         CPPUNIT_ASSERT(x.valid() == false);
     }

     // ----------
     // test_view
     //-----------
     void test_view_1() {
         A x;
         int size = sizeof(x.a) - (2*sizeof(int));
         int actual_size = x.view(x.a[0]);
         CPPUNIT_ASSERT(actual_size == size);
         CPPUNIT_ASSERT(actual_size == x.view(x.a[size + 4]));
         x.view(x.a[0]) = sizeof(value_type);
         CPPUNIT_ASSERT(x.view(x.a[0]) == sizeof(value_type));
     }

     void test_view_2() {
         A x;
         int size = sizeof(x.a) - (2*sizeof(int));
         int actual_size = x.view(x.a[0]);
         CPPUNIT_ASSERT(actual_size == size);
         CPPUNIT_ASSERT(actual_size == x.view(x.a[size + 4]));
         x.view(x.a[0]) = -static_cast<signed int>(sizeof(value_type));
         CPPUNIT_ASSERT(x.view(x.a[0]) == -static_cast<signed int>(sizeof(value_type)));
     }

     void test_view_3() {
         A x;
         int size = sizeof(x.a) - (2*sizeof(int));
         int actual_size = x.view(x.a[0]);
         CPPUNIT_ASSERT(actual_size == size);
         CPPUNIT_ASSERT(actual_size == x.view(x.a[size + 4]));
         x.view(x.a[0]) = sizeof(value_type) * 20;
         CPPUNIT_ASSERT(x.view(x.a[0]) == sizeof(value_type) * 20);
     }


     // ----------
     // test_peek
     //-----------
     void test_peek_1() {
        const difference_type s = 13;
         Allocator<int, s> x;
         int size = sizeof(x.a) - (2*sizeof(int));
         int actual_size = x.peek(x.a[0]);
         CPPUNIT_ASSERT(actual_size == size);
         CPPUNIT_ASSERT(actual_size == x.peek(x.a[size + 4]));
     }
     
     void test_peek_2() {
        const difference_type s = 100;
         Allocator<double, s> x;
         int size = sizeof(x.a) - (2*sizeof(int));
         int actual_size = x.peek(x.a[0]);
         CPPUNIT_ASSERT(actual_size == size);
         CPPUNIT_ASSERT(actual_size == x.peek(x.a[size + 4]));
     }

     void test_peek_3() {
        const difference_type s = 50;
         Allocator<long, s> x;
         int size = sizeof(x.a) - (2*sizeof(int));
         int actual_size = x.peek(x.a[0]);
         CPPUNIT_ASSERT(actual_size == size);
         CPPUNIT_ASSERT(actual_size == x.peek(x.a[size + 4]));
     }

     // -------------
     // test_allocate
     // -------------
     void test_allocate_1() {
        A x;
        const difference_type s = 1;
        try {
            x.allocate(s);
            CPPUNIT_ASSERT(x.valid());
        } catch (exception& e) {
            CPPUNIT_ASSERT(strcmp(e.what(), "std::bad_alloc") == 0);
            int k = 0;
            int block_size;
            while(k < static_cast<signed int>(sizeof(x.a))) {
                block_size = x.peek(x.a[k]);
                if((block_size > 0) && static_cast<signed int>(s * sizeof(value_type)) <= block_size) {
                    CPPUNIT_ASSERT(false);
                } else {
		    k += (abs(block_size) + 8);
                }
            }
        }
     }

     void test_allocate_2() {
        A x;
        const difference_type s = 1;
        try {
            x.allocate(s);
            CPPUNIT_ASSERT(x.valid());
            x.allocate(s);
            CPPUNIT_ASSERT(x.valid());
        } catch (exception& e) {
            CPPUNIT_ASSERT(strcmp(e.what(), "std::bad_alloc") == 0);
            int k = 0;
            int block_size;
            while(k < static_cast<signed int>(sizeof(x.a))) {
                block_size = x.peek(x.a[k]);
                if((block_size > 0) && static_cast<signed int>(s * sizeof(value_type)) <= block_size) {
                    CPPUNIT_ASSERT(false);
                } else {
		    k += (abs(block_size) + 8);
                }
            }
        }

     }

     void test_allocate_3() {
        A x;
        const difference_type s = 2;
        try {
            x.allocate(s);
            CPPUNIT_ASSERT(x.valid());
        } catch (exception& e) {
            CPPUNIT_ASSERT(strcmp(e.what(), "std::bad_alloc") == 0);
            int k = 0;
            int block_size;
            while(k < static_cast<signed int>(sizeof(x.a))) {
                block_size = x.peek(x.a[k]);
                if((block_size > 0) && static_cast<signed int>(s * sizeof(value_type)) <= block_size) {
                    CPPUNIT_ASSERT(false);
                } else {
		    k += (abs(block_size) + 8);
                }
            }
        }

     }

     void test_allocate_4() {
        A x;
        const difference_type s = 10;
        try {
            x.allocate(s);
            CPPUNIT_ASSERT(x.valid());
        } catch (exception& e) {
            CPPUNIT_ASSERT(strcmp(e.what(), "std::bad_alloc") == 0);
            int k = 0;
            int block_size;
            while(k < static_cast<signed int>(sizeof(x.a))) {
                block_size = x.peek(x.a[k]);
                if((block_size > 0) && static_cast<signed int>(s * sizeof(value_type)) <= block_size) {
                    CPPUNIT_ASSERT(false);
                } else {
		    k += (abs(block_size) + 8);
                }
            }
        }

     }
 
     void test_allocate_5() {
        A x;
        const difference_type s = (sizeof(x.a) - (2 * sizeof(int))) / sizeof(value_type);
        try {
            x.allocate(s);
            CPPUNIT_ASSERT(x.valid());
            x.allocate(1);
            CPPUNIT_ASSERT(false);
        } catch (exception& e) {
            CPPUNIT_ASSERT(strcmp(e.what(), "std::bad_alloc") == 0);
            int k = 0;
            int block_size;
            while(k < static_cast<signed int>(sizeof(x.a))) {
                block_size = x.peek(x.a[k]);
                if((block_size > 0) && static_cast<signed int>(s * sizeof(value_type)) <= block_size) {
                    CPPUNIT_ASSERT(false);
                } else {
		    k += (abs(block_size) + 8);
                }
            }
        }
     }

     void test_allocate_6() {
        A x;
        if (sizeof(value_type) == 8 && x.a[0] == 92) {
          try {
            x.allocate(5);
            CPPUNIT_ASSERT(x.valid());
            x.allocate(3);
            CPPUNIT_ASSERT(x.valid());
            CPPUNIT_ASSERT(x.peek(x.a[96]) == 12);
          } catch (exception& e) {
            CPPUNIT_ASSERT(false);
          }
        }
     }

     void test_allocate_7() {
        A x;
        const difference_type s = -1;
        try {
            x.allocate(s);
            CPPUNIT_ASSERT(false);
        } catch (exception& e) {
            CPPUNIT_ASSERT(strcmp(e.what(), "std::bad_alloc") == 0);
        }
     }

     void test_allocate_8() {
        A x;
        const difference_type s = (sizeof(x.a) - (2 * sizeof(int))) / sizeof(value_type) + 1;
        try {
            x.allocate(s);
            CPPUNIT_ASSERT(false);
        } catch (exception& e) {
            CPPUNIT_ASSERT(strcmp(e.what(), "std::bad_alloc") == 0);
        }
     }


     // ---------------
     // test_deallocate
     // ---------------

     void test_deallocate_1() {
        A x;
        const difference_type s = 1;
        const value_type v = 2;
        const pointer p = x.allocate(s);
        x.construct(p, v);
        CPPUNIT_ASSERT(*p == v);
        x.destroy(p);
        x.deallocate(p, s);
     }

     void test_deallocate_2() {
        A x;
        const difference_type s = (sizeof(x.a) - (2 * sizeof(int))) / sizeof(value_type);
        try {
            const pointer p = x.allocate(s);
            CPPUNIT_ASSERT(x.valid());
            x.deallocate(p);
            CPPUNIT_ASSERT(x.peek(x.a[0]) > 0);
            CPPUNIT_ASSERT(x.peek(x.a[sizeof(x.a) - sizeof(int)]) > 0);
            CPPUNIT_ASSERT(x.peek(x.a[0]) == x.peek(x.a[sizeof(x.a) - sizeof(int)]));
            CPPUNIT_ASSERT(x.valid());
        } catch (exception& e) {
            CPPUNIT_ASSERT(strcmp(e.what(), "std::bad_alloc") == 0);
        }
     }

     void test_deallocate_3() {
        A x;
        const difference_type s = (sizeof(x.a) - (2 * sizeof(int))) / sizeof(value_type);
        try {
            const pointer p = x.allocate(s);
            CPPUNIT_ASSERT(x.valid());
            x.deallocate(p - 5);
            CPPUNIT_ASSERT(x.peek(x.a[0]) < 0);
            CPPUNIT_ASSERT(x.peek(x.a[sizeof(x.a) - sizeof(int)]) < 0);
            CPPUNIT_ASSERT(x.peek(x.a[0]) == x.peek(x.a[sizeof(x.a) - sizeof(int)]));
            CPPUNIT_ASSERT(x.valid());
        } catch (exception& e) {
            CPPUNIT_ASSERT(strcmp(e.what(), "std::bad_alloc") == 0);
        }
     }

     void test_deallocate_4() {
        A x;
        const difference_type s = 10;
        const value_type v = 2;
        pointer b = 0;
        pointer e = 0;
        pointer p = 0;
        try {
            b = x.allocate(s);
            e = b + s;
            p = b;
        } catch (exception& e) {
            CPPUNIT_ASSERT(strcmp(e.what(), "std::bad_alloc") == 0);
            return;
        }
        
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

     void test_deallocate_5() {
        A x;
        if (sizeof(value_type) == 8 && x.a[0] == 92) {
            const difference_type s = 1;
            x.allocate(s);
            const pointer p = x.allocate(s);
            x.allocate(s);
            x.deallocate(p, s);
            difference_type header = x.peek(x.a[16]);
            difference_type tail = x.peek(x.a[28]);
            CPPUNIT_ASSERT(header == 8);
            CPPUNIT_ASSERT(tail == 8);
            header = x.peek(x.a[32]);
            tail = x.peek(x.a[44]);
            CPPUNIT_ASSERT(header == -8);
            CPPUNIT_ASSERT(tail == -8);
            header = x.peek(x.a[0]);
            tail = x.peek(x.a[12]);
            CPPUNIT_ASSERT(header == -8);
            CPPUNIT_ASSERT(tail == -8);
            CPPUNIT_ASSERT(x.valid());
        }
     }

     void test_deallocate_6() {
        A x;
        if (sizeof(value_type) == 8 && x.a[0] == 92) {
            const difference_type s = 1;
            x.allocate(s);
            x.allocate(s);
            const pointer p = x.allocate(s);
            x.deallocate(p, s);
            difference_type header = x.peek(x.a[32]);
            difference_type tail = x.peek(x.a[96]);
            CPPUNIT_ASSERT(header == 60);
            CPPUNIT_ASSERT(tail == 60);
            header = x.peek(x.a[0]);
            tail = x.peek(x.a[12]);
            CPPUNIT_ASSERT(header == -8);
            CPPUNIT_ASSERT(tail == -8);
            header = x.peek(x.a[16]);
            tail = x.peek(x.a[28]);
            CPPUNIT_ASSERT(header == -8);
            CPPUNIT_ASSERT(tail == -8);
            CPPUNIT_ASSERT(x.valid());
        }
     }

     void test_deallocate_7() {
        A x;
        if (sizeof(value_type) == 8 && x.a[0] == 92) {
            const difference_type s = 1;
            const pointer p = x.allocate(s);
            x.allocate(s);
            x.allocate(s);
            x.deallocate(p, s);
            difference_type header = x.peek(x.a[0]);
            difference_type tail = x.peek(x.a[12]);
            CPPUNIT_ASSERT(header == 8);
            CPPUNIT_ASSERT(tail == 8);
            header = x.peek(x.a[16]);
            tail = x.peek(x.a[28]);
            CPPUNIT_ASSERT(header == -8);
            CPPUNIT_ASSERT(tail == -8);
            header = x.peek(x.a[32]);
            tail = x.peek(x.a[44]);
            CPPUNIT_ASSERT(header == -8);
            CPPUNIT_ASSERT(tail == -8);
            CPPUNIT_ASSERT(x.valid());
        }
     }

     void test_deallocate_8() {
        A x;
        if (sizeof(value_type) == 8 && x.a[0] == 92) {
            const difference_type s = 1;
            const pointer p = x.allocate(s);
            const pointer b = x.allocate(s);
            const pointer e = x.allocate(s);
            x.deallocate(p, s);
            x.deallocate(e, s);
            difference_type header = x.peek(x.a[0]);
            difference_type tail = x.peek(x.a[12]);
            CPPUNIT_ASSERT(header == 8);
            CPPUNIT_ASSERT(tail == 8);
            header = x.peek(x.a[16]);
            tail = x.peek(x.a[28]);
            CPPUNIT_ASSERT(header == -8);
            CPPUNIT_ASSERT(tail == -8);
            header = x.peek(x.a[32]);
            tail = x.peek(x.a[96]);
            CPPUNIT_ASSERT(header == 60);
            CPPUNIT_ASSERT(tail == 60);
            CPPUNIT_ASSERT(x.valid());
            x.deallocate(b, s);
            header = x.peek(x.a[0]);
            tail = x.peek(x.a[96]);
            CPPUNIT_ASSERT(header == 92);
            CPPUNIT_ASSERT(tail == 92);
        }
     }

    // make a test case for the very right-most edge of the array

    // -----
    // suite
    // -----

    CPPUNIT_TEST_SUITE(OurAllocator);
    CPPUNIT_TEST(test_Allocator_1);
    CPPUNIT_TEST(test_Allocator_2);
    CPPUNIT_TEST(test_Allocator_3);
    CPPUNIT_TEST(test_Allocator_4);
    CPPUNIT_TEST(test_Allocator_5);
    CPPUNIT_TEST(test_valid_1);
    CPPUNIT_TEST(test_valid_2);
    CPPUNIT_TEST(test_valid_3);
    CPPUNIT_TEST(test_view_1);
    CPPUNIT_TEST(test_view_2);
    CPPUNIT_TEST(test_view_3);
    CPPUNIT_TEST(test_peek_1);
    CPPUNIT_TEST(test_peek_2);
    CPPUNIT_TEST(test_peek_3);
    CPPUNIT_TEST(test_allocate_1);
    CPPUNIT_TEST(test_allocate_2);
    CPPUNIT_TEST(test_allocate_3);
    CPPUNIT_TEST(test_allocate_4);
    CPPUNIT_TEST(test_allocate_5);
    CPPUNIT_TEST(test_allocate_6);
    CPPUNIT_TEST(test_allocate_7);
    CPPUNIT_TEST(test_allocate_8);
    CPPUNIT_TEST(test_deallocate_1);
    CPPUNIT_TEST(test_deallocate_2);
    CPPUNIT_TEST(test_deallocate_3);
    CPPUNIT_TEST(test_deallocate_4);
    CPPUNIT_TEST(test_deallocate_5);
    CPPUNIT_TEST(test_deallocate_6);
    CPPUNIT_TEST(test_deallocate_7);
    CPPUNIT_TEST(test_deallocate_8);
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
    tr.addTest(TestAllocator< std::allocator<double> >::suite());

    tr.addTest(OurAllocator< Allocator<short, 10> >::suite());
    tr.addTest(OurAllocator< Allocator<short, 100> >::suite());
    tr.addTest(OurAllocator< Allocator<int, 100> >::suite());
    tr.addTest(OurAllocator< Allocator<int, 13> >::suite());
    tr.addTest(OurAllocator< Allocator<double, 16> >::suite());
    tr.addTest(OurAllocator< Allocator<double, 24> >::suite());
    tr.addTest(OurAllocator< Allocator<double, 49> >::suite());
    tr.addTest(OurAllocator< Allocator<double, 100> >::suite());

    tr.run();

    cout << "Done." << endl;
    return 0;}

