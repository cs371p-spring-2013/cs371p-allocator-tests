// ------------------------------------
// projects/allocator/TestAllocator.c++
// Copyright (C) 2011
// Glenn P. Downing
// ------------------------------------

/**
 * To test the program:
 *     g++ -ansi -pedantic -lcppunit -ldl -Wall TestAllocator.c++ -o TestAllocator.app
 *     valgrind TestAllocator.app >& TestAllocator.out
 */

// --------
// includes
// --------

#include <algorithm> // count
#include <memory>    // allocator
#include <string>

#include "cppunit/extensions/HelperMacros.h" // CPPUNIT_TEST, CPPUNIT_TEST_SUITE, CPPUNIT_TEST_SUITE_END
#include "cppunit/TestFixture.h"             // TestFixture
#include "cppunit/TextTestRunner.h"          // TestRunner

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
        const value_type      v = 2;
        const pointer         p = x.allocate(s);
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
        x.deallocate(b, s);
    }

    // -----
    // suite
    // -----

    CPPUNIT_TEST_SUITE(TestAllocator);
    CPPUNIT_TEST(test_one);
    CPPUNIT_TEST(test_ten);
    CPPUNIT_TEST_SUITE_END();};

// -----
// Dummy class Foo
// -----

static int numFoo = 0;
static int deleteCount = 0;

class Foo {
private:
    int a;
    double b;

public:
    Foo() {
    }
    Foo(const Foo& that) {
        ++numFoo;
    }
    ~Foo() {
        ++deleteCount;
    }
};

// -------------
// TestMyAllocator
// -------------

template <typename A>
struct TestMyAllocator : CppUnit::TestFixture {
    // --------
    // typedefs
    // --------

    typedef typename A::value_type      value_type;
    typedef typename A::difference_type difference_type;
    typedef typename A::pointer         pointer;
    
    static const int S_SIZE = sizeof(int);
    
    // -----
    // test constructor of allocator
    // -----

    void test_constructor_1 () {
        Allocator<int, 16>  x;
        int* p = x.allocate(1);
        x.deallocate(p, 1);

        int blockSize = 16 - 2*sizeof(int);
        CPPUNIT_ASSERT(*(p - 1) == blockSize);
        CPPUNIT_ASSERT(*((int*)((char*)p + blockSize)) == blockSize);
    }

    void test_constructor_2 () {
        Allocator<Foo, 32>  x;
        Foo* p = x.allocate(1);
        x.deallocate(p, 1);

        int blockSize = 32 - 2*sizeof(int);
        CPPUNIT_ASSERT(*((int*)p - 1) == blockSize);
        CPPUNIT_ASSERT(*((int*)((char*)p + blockSize)) == blockSize);
    }

    void test_constructor_3 () {
        Allocator<Foo, 64>  x;
        Foo* p = x.allocate(1);
        x.deallocate(p, 1);

        int blockSize = 64 - 2*sizeof(int);
        CPPUNIT_ASSERT(*((int*)p - 1) == blockSize);
        CPPUNIT_ASSERT(*((int*)((char*)p + blockSize)) == blockSize);
    }

    // -----
    // test_construct
    // -----

    void test_construct_1() {
        numFoo = 0;
        Allocator<Foo, 50> x;
        Foo* p = x.allocate(1);
        Foo v;

        x.construct(p, v);
        CPPUNIT_ASSERT(numFoo == 1);
    }

    void test_construct_2() {
        numFoo = 0;
        CPPUNIT_ASSERT(numFoo == 0);
        Allocator<Foo, 200> x;
        Foo* p = x.allocate(12);
        Foo v;

        Foo* ptr = p;
        for (int i = 0; i < 12; ++i) {
            x.construct(ptr, v);
            ++ptr;
        }
        CPPUNIT_ASSERT(numFoo == 12);
    }

    void test_construct_3() {
        numFoo = 0;
        Allocator<Foo, 2000> x;
        Foo* p = x.allocate(20);
        Foo v;

        Foo* ptr = p;
        for (int i = 0; i < 12; ++i) {
            x.construct(ptr, v);
            ++ptr;
        }

        p = x.allocate(30);

        ptr = p;
        for (int i = 0; i < 24; ++i) {
            x.construct(ptr, v);
            ++ptr;
        }

        CPPUNIT_ASSERT(numFoo == 36);
    }

    // -----
    // test_destroy
    // -----

    void test_destroy_1() {
        deleteCount = 0;
        Allocator<Foo, 200> x;
        Foo* p = x.allocate(1);
        Foo v;

        x.construct(p, v);
        x.destroy(p);
        CPPUNIT_ASSERT(deleteCount == 1);
    }

    void test_destroy_2() {
        deleteCount = 0;
        Allocator<Foo, 2000> x;
        Foo* p = x.allocate(10);
        Foo v;

        Foo* ptr = p;
        for (int i = 0; i < 10; ++i) {
            x.construct(ptr, v);
            ++ptr;
        }
        
        ptr = p;
        for (int i = 0; i < 10; ++i) {
            x.destroy(ptr);
            ++ptr;
        }
        CPPUNIT_ASSERT(deleteCount == 10);
    }

    void test_destroy_3() {
        deleteCount = 0;
        Allocator<Foo, 2000> x;
        Foo* p = x.allocate(10);
        Foo v;

        Foo* ptr = p;
        for (int i = 0; i < 8; ++i) {
            x.construct(ptr, v);
            ++ptr;
        }
        
        ptr = p;
        for (int i = 0; i < 6; ++i) {
            x.destroy(ptr);
            ++ptr;
        }

        p = x.allocate(5);
        ptr = p;
        for (int i = 0; i < 4; ++i) {
            x.construct(ptr, v);
            ++ptr;
        }
        
        ptr = p;
        for (int i = 0; i < 4; ++i) {
            x.destroy(ptr);
            ++ptr;
        }
        CPPUNIT_ASSERT(deleteCount == 10);
    }
   
   
    
    // -----
    // test_allocate
    // -----
    
    void test_allocate_1 () {
        A x;
        const difference_type s = 1;
        const value_type      v = 1;
        const pointer         p = x.allocate(s);
        int size = s * sizeof(value_type);
        x.construct(p, v);
        int* tp = (int*)p;
        CPPUNIT_ASSERT(int(*(tp - 1)) == -1 * size);
        CPPUNIT_ASSERT(int(*(tp + size / 4)) == -1 * size);
        CPPUNIT_ASSERT(*p == v);
        x.destroy(p);
        x.deallocate(p, s);
    }
    
    void test_allocate_2 () {
        A x;
        const difference_type s = 12;
        const value_type      v = 4;
        const pointer         p = x.allocate(s);
        int size = s * sizeof(value_type);
        x.construct(p, v);
        int* tp = (int*)p;
        CPPUNIT_ASSERT(int(*(tp - 1)) == -1 * size);
        CPPUNIT_ASSERT(int(*(tp + size / 4)) == -1 * size);
        CPPUNIT_ASSERT(*p == v);
        x.destroy(p);
        x.deallocate(p, s);
    }
    
    void test_allocate_3 () {
        A x;
        const difference_type s = 20;
        const value_type      v = 5;
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
    // test_deallocate
    // -----
    
    void test_deallocate_1 () {
        A x;
        const difference_type   s = 1;
        const pointer           p = x.allocate(s);
        int* ptr = (int*)p;
        x.deallocate(p, s);
        int availableBytes = x.byte_count() - 2 * sizeof(int);
        CPPUNIT_ASSERT(int(*(ptr - 1)) == availableBytes);
        CPPUNIT_ASSERT(int(*(ptr + availableBytes / 4)) == availableBytes);
    }
    
    void test_deallocate_2 () {
        A x;
        const difference_type   s = 12;
        const pointer           p = x.allocate(s);
        int* ptr = (int*)p;       
        x.deallocate(p, s);
        int availableBytes = x.byte_count() - 2 * sizeof(int);
        CPPUNIT_ASSERT(int(*(ptr - 1)) == availableBytes);
        CPPUNIT_ASSERT(int(*(ptr + availableBytes / 4)) == availableBytes);
    }
    
    void test_deallocate_3 () {
        A x;
        const difference_type   s = 22;
        const pointer           p = x.allocate(s);
        int* ptr = (int*)p;       
        x.deallocate(p, s);
        int availableBytes = x.byte_count() - 2 * sizeof(int);
        CPPUNIT_ASSERT(int(*(ptr - 1)) == availableBytes);
        CPPUNIT_ASSERT(int(*(ptr + availableBytes / 4)) == availableBytes);
    }
    
    // -----
    // suite
    // -----
    
    CPPUNIT_TEST_SUITE(TestMyAllocator);
    CPPUNIT_TEST(test_allocate_1);
    CPPUNIT_TEST(test_allocate_2);
    CPPUNIT_TEST(test_allocate_3);
    CPPUNIT_TEST(test_deallocate_1);
    CPPUNIT_TEST(test_deallocate_2);
    CPPUNIT_TEST(test_deallocate_3);
    CPPUNIT_TEST(test_constructor_1);
    CPPUNIT_TEST(test_constructor_2);
    CPPUNIT_TEST(test_constructor_3);
    CPPUNIT_TEST(test_construct_1);
    CPPUNIT_TEST(test_construct_2);
    CPPUNIT_TEST(test_construct_3);
    CPPUNIT_TEST(test_destroy_1);
    CPPUNIT_TEST(test_destroy_2);
    CPPUNIT_TEST(test_destroy_3);
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

    tr.addTest(TestMyAllocator< Allocator<int, 100> >::suite());
    tr.addTest(TestMyAllocator< Allocator<double, 200> >::suite());

    tr.run();

    cout << "Done." << endl;
    return 0;}

