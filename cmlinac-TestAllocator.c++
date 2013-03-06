// -----------------------------------------------------------------------------
// projects/allocator/TestAllocator.c++
// Project Completed by: Mitch Stephan (mjs4373) & Chris Mlinac (ccm854)
// Due: 03/07/13
// Professor: Glenn P. Downing
// ----------------------------------------------------------------------------

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

// -------
// defines
// -------

#define private public
#define protected public
#define class struct


// -------------
// TestAllocator
// -------------

template <typename A>
struct TestAllocator : CppUnit::TestFixture {
    // --------
    // typedefs
    // --------

    typedef typename A::value_type  		value_type;
    typedef typename A::difference_type	difference_type;
    typedef typename A::pointer				pointer;


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
                ++p;
			}
		}
        catch (...) {
            while (b != p) {
                --p;
                x.destroy(p);}
            x.deallocate(b, s);
            throw;
		}
        CPPUNIT_ASSERT(std::count(b, e, v) == s);
        while (b != e) {
            --e;
            x.destroy(e);
		}
        x.deallocate(b, s);
	}
	
	CPPUNIT_TEST_SUITE(TestAllocator);
		CPPUNIT_TEST(test_one);
		CPPUNIT_TEST(test_ten);
    CPPUNIT_TEST_SUITE_END();};

	template <typename A>
	struct TestOurAllocator : CppUnit::TestFixture {
    // --------
    // typedefs
    // --------

    typedef typename A::value_type			value_type;
    typedef typename A::difference_type	difference_type;
    typedef typename A::pointer				pointer;
	// ---------
	
	// test view
	// ---------

	void test_view_1 () {
		A x;
		char temp[50];
		x.view(*(temp+4)) = 1;
		CPPUNIT_ASSERT(x.view(*(temp+4)) == 1);
	}
	
	void test_view_2 () {
		A x;
		char temp[50];
		x.view(*(temp+5)) = -2;
		CPPUNIT_ASSERT(x.view(*(temp+5)) == -2); 
	}

	void test_view_3 () {
		A x;
		char temp[50];
		x.view(*temp) = -2;
		CPPUNIT_ASSERT(x.view(*temp) == -2);
		x.view(*temp) = 2;
		CPPUNIT_ASSERT(x.view(*temp) == 2);
	}

	// ----------
	// test valid
	//-----------

	void test_valid_1 () {
		A x;
		char* p = x.start;
		CPPUNIT_ASSERT(x.view(*p) == x.b_size - INT_SIZE * 2);
		p = x.end - 4;
		CPPUNIT_ASSERT(x.view(*p) == x.b_size - INT_SIZE * 2);
		CPPUNIT_ASSERT(x.valid());
	}

	void test_valid_2 () {
		A x;
		char* p = x.start;
		CPPUNIT_ASSERT(x.view(*p) == x.b_size - INT_SIZE * 2);
		p = x.end - 4;
		CPPUNIT_ASSERT(x.view(*p) == x.b_size - INT_SIZE * 2);
		CPPUNIT_ASSERT(x.valid());
	}

	void test_valid_3 () {
		A x;
		char* p = x.start;
		CPPUNIT_ASSERT(x.view(*p) == x.b_size - INT_SIZE * 2);
		p = x.end - 4;
		CPPUNIT_ASSERT(x.view(*p) == x.b_size - INT_SIZE * 2);
		CPPUNIT_ASSERT(x.valid());
	}

	// ----------------
	// test write_block
	// ----------------

	void test_write_block_1 () {
		A x;
		x.write_block(x.start, x.b_size, x.b_size, -1); // allocating entire array excluding sentinels
		char* p = x.start;
		CPPUNIT_ASSERT(x.view(*p) == (-1 * (x.b_size - (INT_SIZE * 2))));
		p = x.end - 4;
		CPPUNIT_ASSERT(x.view(*p) == (-1 * (x.b_size - (INT_SIZE * 2))));
		CPPUNIT_ASSERT(x.valid());
	}

	void test_write_block_2 () {
		A x; 
		x.write_block(x.start, x.b_size, x.b_size, -1); // allocating entire array excluding sentinels
		x.write_block(x.start, x.b_size, x.b_size, 1); // deallocating everything
		char* p = x.start;
		CPPUNIT_ASSERT(x.view(*p) == (x.b_size - (INT_SIZE * 2)));
		p = x.end - 4;
		CPPUNIT_ASSERT(x.view(*p) == (x.b_size - (INT_SIZE * 2)));
		CPPUNIT_ASSERT(x.valid());
	}

	void test_write_block_3 () {
		A x;
		x.write_block(x.start, 10 + INT_SIZE * 2, x.b_size, -1); 
		char* p = x.start;
		CPPUNIT_ASSERT(x.view(*p) == -10);
		p += 10 + INT_SIZE;
		CPPUNIT_ASSERT(x.view(*p) == -10);
		p += INT_SIZE;
		CPPUNIT_ASSERT(x.view(*p) == x.b_size - 10 - INT_SIZE * 4);
		p = x.end - 4;
		CPPUNIT_ASSERT(x.view(*p) == x.b_size - 10 - INT_SIZE * 4);
		CPPUNIT_ASSERT(x.valid());
	}

	// -------------
	// test allocate
	// -------------

	void test_allocate_1 () {
		A x; 
		x.allocate(1);
		char* p = x.start;
		CPPUNIT_ASSERT(x.view(*p) == -1 * (signed)sizeof(value_type));
		CPPUNIT_ASSERT(x.valid());
	}

	void test_allocate_2 () {
		A x; 
		x.allocate(3);
		x.allocate(1);
		char* p = x.start;
		CPPUNIT_ASSERT(x.view(*p) == -3 * (signed)sizeof(value_type));
		CPPUNIT_ASSERT(x.valid());
	}

	void test_allocate_3 () {
		A x; 
		try {
			x.allocate(x.b_size);
			CPPUNIT_ASSERT(false);
		}
		catch (...) {
			CPPUNIT_ASSERT(true);
		}
	}

	
	// ---------------
	// test deallocate
	// ---------------

	void test_deallocate_1 () {
		A x; 
		pointer p = x.allocate(2);
		x.deallocate(p);
		CPPUNIT_ASSERT(x.valid());
	}

	void test_deallocate_2 () {
		A x;
		pointer p = x.allocate(20);
		x.allocate(7);
		x.deallocate(p);
		CPPUNIT_ASSERT(x.valid());
	}

	void test_deallocate_3 () {
		A x;
		pointer p = x.allocate(3); 
		pointer pp = x.allocate(3);
		assert(x.valid());
		pointer ppp = x.allocate(3);
		assert(x.valid());
		x.deallocate(ppp); // deallocate 3rd block
		assert(x.valid());
		x.deallocate(p); // deallocate 1st block
		assert(x.valid());
		x.deallocate(pp); // deallocate 2nd block
		assert(x.valid());
		CPPUNIT_ASSERT(x.valid());
	}
	
	// --------------------------
	// Not required to test: 
	// Construct, Destory, ==, !=
	// --------------------------

    // -----
    // suite
    // -----

    CPPUNIT_TEST_SUITE(TestOurAllocator);
		CPPUNIT_TEST(test_view_1);
		CPPUNIT_TEST(test_view_2);
		CPPUNIT_TEST(test_view_3);
		CPPUNIT_TEST(test_valid_1);
		CPPUNIT_TEST(test_valid_2);
		CPPUNIT_TEST(test_valid_3);
		CPPUNIT_TEST(test_write_block_1);
		CPPUNIT_TEST(test_write_block_2);
		CPPUNIT_TEST(test_write_block_3);
		CPPUNIT_TEST(test_allocate_1);
		CPPUNIT_TEST(test_allocate_2);
		CPPUNIT_TEST(test_allocate_3);
		CPPUNIT_TEST(test_deallocate_1);
		CPPUNIT_TEST(test_deallocate_2);
		CPPUNIT_TEST(test_deallocate_3);
    CPPUNIT_TEST_SUITE_END();};


// ----
// main
// ----

int main () {
    using namespace std;
    ios_base::sync_with_stdio(false);    // turn off synchronization with C I/O
    cout << "TestAllocator.c++" << endl;

    CppUnit::TextTestRunner tr;

	// char
	tr.addTest(TestAllocator< std::allocator<char> >::suite());
	tr.addTest(TestAllocator< Allocator<char, 500> >::suite());
	tr.addTest(TestOurAllocator< Allocator<char, 500> >::suite());
	// bool
	tr.addTest(TestAllocator< std::allocator<bool> >::suite());
	tr.addTest(TestAllocator< Allocator<bool, 500> >::suite());
	tr.addTest(TestOurAllocator< Allocator<bool, 500> >::suite());
	// short
	tr.addTest(TestAllocator< std::allocator<short> >::suite());
	tr.addTest(TestAllocator< Allocator<short, 500> >::suite());
	tr.addTest(TestOurAllocator< Allocator<short, 500> >::suite());
	// int
    tr.addTest(TestAllocator< std::allocator<int> >::suite());
	tr.addTest(TestAllocator< Allocator<int, 500> >::suite());
	tr.addTest(TestOurAllocator< Allocator<int, 500> >::suite());
	// double
    tr.addTest(TestAllocator< std::allocator<double> >::suite());
	tr.addTest(TestAllocator< Allocator<double, 500> >::suite());
	tr.addTest(TestOurAllocator< Allocator<double, 500> >::suite());
	// long
	tr.addTest(TestAllocator< std::allocator<long> >::suite());
	tr.addTest(TestAllocator< Allocator<long, 500> >::suite());
	tr.addTest(TestOurAllocator< Allocator<long, 500> >::suite());
	// float
	tr.addTest(TestAllocator< std::allocator<float> >::suite());
	tr.addTest(TestAllocator< Allocator<float, 500> >::suite());
	tr.addTest(TestOurAllocator< Allocator<float, 500> >::suite());

    tr.run();

    cout << "Done." << endl;
    return 0;
}
