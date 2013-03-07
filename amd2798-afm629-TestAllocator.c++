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

#define private public	//defined so valid() and a[] are reachable

#include "Allocator.h"

// -------------
// TestStdAllocator
// -------------

template <typename A>
struct TestStdAllocator : CppUnit::TestFixture {
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

    CPPUNIT_TEST_SUITE(TestStdAllocator);
    CPPUNIT_TEST(test_one);
    CPPUNIT_TEST(test_ten);
    CPPUNIT_TEST_SUITE_END();};


// ---------------------
// TestAllocatorMethods
// ---------------------

struct TestAllocatorMethods : CppUnit::TestFixture {

	//No blocks allocated
	void test_valid_1()
	{
		Allocator<double, 100> x;
		CPPUNIT_ASSERT(x.valid());
	}

	//Two allocated blocks then merged when deallocated
	void test_valid_2()
	{
		Allocator<double, 100> x;
		double *p1, *p2;

		p1 = x.allocate(3);
		p2 = x.allocate(3);

		x.deallocate(p1);

		CPPUNIT_ASSERT(x.valid());
	}

	//Five allocated blocks
	void test_valid_3()
	{
		Allocator<double, 300> x;
		double* a[5];

		a[0] = x.allocate(5);
		a[1] = x.allocate(5);
		a[2] = x.allocate(5);
		a[3] = x.allocate(5);
		a[4] = x.allocate(5);
		
		CPPUNIT_ASSERT(x.view(x.a[0]) = -40);
		CPPUNIT_ASSERT(x.view(x.a[296]) = 52);
		CPPUNIT_ASSERT(x.valid());
	}

	//Test by manually specifying sentinnels
	void test_valid_4()
	{
		Allocator<int, 100> x;

		x.view(x.a[0]) = x.view(x.a[8]) = 4;
		x.view(x.a[12]) = x.view(x.a[96]) = 80;

		CPPUNIT_ASSERT(x.valid());
	}

	//Test by specifying invalid sentinnels
	void test_valid_5()
	{
		Allocator<int, 10> x;

		x.view(x.a[0]) = 0;

		CPPUNIT_ASSERT(!x.valid());
	}

	//Allocate whole block, then another for 1 value_type == bad_alloc
	void test_allocate_1()
	{
		Allocator<char, 100> x;

		x.allocate(92);

		try
		{
			x.allocate(1);
			CPPUNIT_ASSERT(false);
		}
		catch(std::bad_alloc&)
		{
			CPPUNIT_ASSERT(true);
		}
	}

	//Allocate more than available space == bad_alloc
	void test_allocate_2()
	{
		Allocator<bool, 100> x;

		try
		{
			x.allocate(1000);
			CPPUNIT_ASSERT(false);
		}
		catch(std::bad_alloc&)
		{
			CPPUNIT_ASSERT(true);
		}
	}

	//Allocate blocks to fit perfectly
	void test_allocate_3()
	{
		Allocator<char, 50> x;
		char *p1, *p2;

		x.allocate(5);

		p1 = x.allocate(5);

		x.allocate(8);

		x.deallocate(p1);

		p2 = x.allocate(5);

		CPPUNIT_ASSERT(p1 == p2);
	}

	//Allocate one block to fit the whole space
	void test_allocate_4()
	{
		Allocator<double, 40> x;

		try
		{
			x.allocate(4);
			CPPUNIT_ASSERT(true);
		}
		catch(std::bad_alloc&)
		{
			CPPUNIT_ASSERT(false);
		}
	}

	//Overallocate second block
	void test_allocate_5()
	{
		Allocator<int, 100> x;
		int *p1, *p2;

		p1 = x.allocate(10);
		p2 = x.allocate(10);

		CPPUNIT_ASSERT(*(p1-1)==-40 && *(p2-1)==-44);
	}

	//Deallocate and merge only the left free block
	void test_dealloc_1()
	{
		Allocator<double, 104> x;
		double *p1, *p2;

		p1 = x.allocate(5);
		p2 = x.allocate(6);

		x.deallocate(p1);
		x.deallocate(p2);

		CPPUNIT_ASSERT(*(reinterpret_cast<int*>(p1)-1) == 96);
	}

	//Deallocate and merge only the right free block
	void test_dealloc_2()
	{
		Allocator<bool, 99> x;
		bool *p;

		p = x.allocate(23);

		x.deallocate(p);

		CPPUNIT_ASSERT(*(reinterpret_cast<int*>(p)-1) == 91);
	}

	//Deallocate and merge both adjacent blocks
	void test_dealloc_3()
	{

		Allocator<float, 200> x;
		float *p1, *p2;

		CPPUNIT_ASSERT(x.view(x.a[0]) = 192);
		CPPUNIT_ASSERT(x.view(x.a[196]) = 192);

		p1 = x.allocate(4);
		p2 = x.allocate(7);

		x.deallocate(p1);
		x.deallocate(p2);

		CPPUNIT_ASSERT(*(reinterpret_cast<int*>(p1)-1) == 192);
	}

	// -----
	// suite
	// -----

	CPPUNIT_TEST_SUITE(TestAllocatorMethods);
	CPPUNIT_TEST(test_valid_1);
	CPPUNIT_TEST(test_valid_2);
	CPPUNIT_TEST(test_valid_3);
	CPPUNIT_TEST(test_valid_4);
	CPPUNIT_TEST(test_valid_5);
	CPPUNIT_TEST(test_allocate_1);
	CPPUNIT_TEST(test_allocate_2);
	CPPUNIT_TEST(test_allocate_3);
	CPPUNIT_TEST(test_allocate_4);
	CPPUNIT_TEST(test_allocate_5);
	CPPUNIT_TEST(test_dealloc_1);
	CPPUNIT_TEST(test_dealloc_2);
	CPPUNIT_TEST(test_dealloc_3);
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

//------------------------------------------------------------------
// Comparison Tests between standard allocator and custom allocator
//------------------------------------------------------------------

    tr.addTest(TestStdAllocator< std::allocator<int> >::suite());
    tr.addTest(TestStdAllocator< Allocator<int, 100> >::suite());

    tr.addTest(TestStdAllocator< std::allocator<double> >::suite());
    tr.addTest(TestStdAllocator< Allocator<double, 100> >::suite());

    tr.addTest(TestStdAllocator< std::allocator<float> >::suite());
    tr.addTest(TestStdAllocator< Allocator<float, 100> >::suite());

    tr.addTest(TestStdAllocator< std::allocator<char> >::suite());
    tr.addTest(TestStdAllocator< Allocator<char, 100> >::suite());

    tr.addTest(TestStdAllocator< std::allocator<bool> >::suite());
    tr.addTest(TestStdAllocator< Allocator<bool, 100> >::suite());

//-------------------------------------------------
// Tests for custom allocator methods in isolation
//-------------------------------------------------
    tr.addTest(TestAllocatorMethods::suite());

    tr.run();

    cout << "Done." << endl;
    return 0;}
