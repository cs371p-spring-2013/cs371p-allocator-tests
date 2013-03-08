// --------
// includes
// --------

#include <algorithm> // count
#include <iostream>  // ios_base
#include <memory>    // allocator
#include <climits>   // INT_MAX

#include "cppunit/extensions/HelperMacros.h" // CPPUNIT_TEST, CPPUNIT_TEST_SUITE, CPPUNIT_TEST_SUITE_END
#include "cppunit/TestFixture.h"             // TestFixture
#include "cppunit/TextTestRunner.h"          // TestRunner

#define private public  //defined so valid() and a[] are reachable

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
        x.deallocate(b, s);}

    // -----
    // suite
    // -----

    CPPUNIT_TEST_SUITE(TestAllocator);
    CPPUNIT_TEST(test_one);
    CPPUNIT_TEST(test_ten);
    CPPUNIT_TEST_SUITE_END();};


struct CustomAllocator : CppUnit::TestFixture {

    //------
    // valid
    //------

    void valid1()
    {
	Allocator<int, 100> x;
	CPPUNIT_ASSERT(x.valid());
    }

    void valid2()
    {
	Allocator<double, 100> x;

	double* p1 = x.allocate(1);
	double* p2 = x.allocate(2);

	CPPUNIT_ASSERT(x.valid());
    }

    void valid3()
    {
	Allocator<long, 100> x;

	long* p1 = x.allocate(1);

	CPPUNIT_ASSERT(x.valid());
    }

    // --------
    // allocate
    // --------

    void allocate1()
    {
	Allocator<int, 100> x;

	try {
	    x.allocate(124789213);
	    CPPUNIT_ASSERT(false);
	}
	catch (std::bad_alloc&) {
	    CPPUNIT_ASSERT(true);
	}
    }

    void allocate2()
    {
	Allocator<int, 100> x;
	
	try{
	    x.allocate(23);
	    CPPUNIT_ASSERT(true);
	}
	catch (std::bad_alloc&) {
	    CPPUNIT_ASSERT(false);
	}
    } 

    void allocate3()
    {
	Allocator<char, 100> x;
	
	try{
	    x.allocate(92);
	    CPPUNIT_ASSERT(true);
	}
	catch (std::bad_alloc&) {
	    CPPUNIT_ASSERT(false);
	}
    } 

    // ----------
    // deallocate
    // ----------

    void deallocate1()
    {
	Allocator<char, 100> x;
	
	char* p1 = x.allocate(6);

	x.deallocate(p1);

	CPPUNIT_ASSERT(x.valid());
    } 

    void deallocate2()
    {
	Allocator<char, 100> x;
	
	char* p1 = x.allocate(6);

	x.deallocate(p1);

	CPPUNIT_ASSERT(x.valid());
    } 

    void deallocate3()
    {
	Allocator<int, 100> x;
	
	int* p1 = x.allocate(8);

	x.deallocate(p1);

	CPPUNIT_ASSERT(x.valid());
    } 

    // -----
    // suite
    // -----

    CPPUNIT_TEST_SUITE(CustomAllocator);
    CPPUNIT_TEST(valid1);
    CPPUNIT_TEST(valid2);
    CPPUNIT_TEST(valid3);
    CPPUNIT_TEST(allocate1);
    CPPUNIT_TEST(allocate2);
    CPPUNIT_TEST(allocate3);
    CPPUNIT_TEST(deallocate1);
    CPPUNIT_TEST(deallocate2);
    CPPUNIT_TEST(deallocate3);
    CPPUNIT_TEST_SUITE_END();
};


// ----
// main
// ----

int main () {
    using namespace std;
    ios_base::sync_with_stdio(false);    // turn off synchronization with C I/O
    cout << "TestAllocator.c++" << endl;

    //comparing the std allocator with the custon one

    CppUnit::TextTestRunner tr;

    tr.addTest(TestAllocator< std::allocator<int> >::suite());
	tr.addTest(TestAllocator< Allocator<int, 100> >::suite());

    tr.addTest(TestAllocator< std::allocator<double> >::suite());
	tr.addTest(TestAllocator< Allocator<double, 100> >::suite());

    tr.addTest(TestAllocator< std::allocator<float> >::suite());
	tr.addTest(TestAllocator< Allocator<float, 100> >::suite());

    tr.addTest(TestAllocator< std::allocator<char> >::suite());
	tr.addTest(TestAllocator< Allocator<char, 100> >::suite());

    tr.addTest(TestAllocator< std::allocator<bool> >::suite());
	tr.addTest(TestAllocator< Allocator<bool, 100> >::suite());

    //custom allocator tests

    tr.addTest(CustomAllocator::suite());

    tr.run();

    cout << "Done." << endl;
    return 0;}
