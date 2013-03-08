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

	void test_valid1()
	{
		//test with no blocks allocated

		Allocator<double, 100> x;

		//is it valid?
		CPPUNIT_ASSERT(x.valid());
	}
	void test_valid2()
	{
		//test with just one isolated block allocated

		Allocator<double, 100> x;
		double *p1, *p2;

		//alloc a1
		p1 = x.allocate(3);

		//alloc a2
		p2 = x.allocate(3);

		//dealloc a1
		x.deallocate(p1);

		//is it valid?
		CPPUNIT_ASSERT(x.valid());
	}
	void test_valid3()
	{
		//test with multiple blocks allocated

		Allocator<double, 500> x;
		double* a[10];

		//alloc a1-a10
		for(int i=0;i<10;++i)
			a[i] = x.allocate(5);

		//is it valid?
		CPPUNIT_ASSERT(x.valid());
	}
	void test_valid4()
	{
		//test with a custom array

		Allocator<int, 100> x;

		x.view(x.a[0]) = x.view(x.a[44]) = 40;
		x.view(x.a[48]) = x.view(x.a[96]) = 44;

		//is it valid?
		CPPUNIT_ASSERT(x.valid());
	}
	void test_valid5()
	{
		//test with a broken array

		Allocator<int, 100> x;

		x.view(x.a[0]) = 0;

		//is it valid? - well, it shouldn't be
		CPPUNIT_ASSERT(!x.valid());
	}

	void test_allocate1()
	{
		//test when there is no free space

		Allocator<char, 100> x;

		//alloc a1 - take all of the space
		x.allocate(92);

		//alloc a2
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
	void test_allocate2()
	{
		//test when there isn't enough space

		Allocator<bool, 100> x;

		//alloc a1 - take more space than there is
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
	void test_allocate3()
	{
		//test when there is a perfect fit for a block

		Allocator<double, 200> x;
		double *p1, *p2;

		//alloc a1
		x.allocate(7);

		//alloc a2
		p1 = x.allocate(11);

		//alloc a3
		x.allocate(3);

		//dealloc a2
		x.deallocate(p1);

		//alloc a2
		p2 = x.allocate(11);

		CPPUNIT_ASSERT(p1 == p2);
	}
	void test_allocate4()
	{
		//test when there is an imperfect fit for a block(causing a block to split)

		Allocator<int, 200> x;
		int *p1, *p2;

		//alloc a1
		x.allocate(3);

		//alloc a2
		p1 = x.allocate(11);

		//alloc a3
		x.allocate(9);

		//dealloc a2
		x.deallocate(p1);

		//alloc a4 - a4<a2
		p2 = x.allocate(7);

		CPPUNIT_ASSERT(p1 == p2);
	}
	void test_allocate5()
	{
		//test when taking up the entire allocator

		Allocator<char, 10> x;

		//alloc a1 - take all of the space
		try
		{
			x.allocate(2);
			CPPUNIT_ASSERT(true);
		}
		catch(std::bad_alloc&)
		{
			CPPUNIT_ASSERT(false);
		}
	}
	void test_allocate6()
	{
		//test when there is not enough space to do another allocate

		Allocator<int, 100> x;
		int *p1, *p2;

		//alloc a1
		p1 = x.allocate(10);

		//alloc a2 - causing an overalloc
		p2 = x.allocate(10);

		CPPUNIT_ASSERT(*(p1-1)==-40 && *(p2-1)==-44);
	}

	void test_dealloc1()
	{
		//test when a freed block would be merged with the one behind it

		Allocator<double, 104> x;
		double *p1, *p2;

		//alloc a1
		p1 = x.allocate(5);

		//alloc a2 - taking up the rest of the allocator
		p2 = x.allocate(6);

		//dealloc a1
		x.deallocate(p1);

		//dealloc a2
		x.deallocate(p2);

		CPPUNIT_ASSERT(*(reinterpret_cast<int*>(p1)-1) == 96);
	}
	void test_dealloc2()
	{
		//test when a freed block would be merged with the one in front of it
		Allocator<bool, 99> x;
		bool *p;

		//alloc a1
		p = x.allocate(23);

		//dealloc a1
		x.deallocate(p);

		CPPUNIT_ASSERT(*(reinterpret_cast<int*>(p)-1) == 91);
	}
	void test_dealloc3()
	{
		//test when a freed block would be merged with the ones in front and back of it
		Allocator<float, 200> x;
		float *p1, *p2;

		//alloc a1
		p1 = x.allocate(4);

		//alloc a2 - leaving a free block
		p2 = x.allocate(7);

		//dealloc a1
		x.deallocate(p1);

		//dealloc a2
		x.deallocate(p2);

		CPPUNIT_ASSERT(*(reinterpret_cast<int*>(p1)-1) == 192);
	}
	template <typename T>
	void test_chaos()
	{
		for(int num=0;num<100;++num)
		{
			//test random chaotic use of the allocator
			Allocator<T, 20000> x;
			T *p[100];
			int r;

			srand(time(NULL));

			try
			{
				//alloc 100 blocks
				for(int i=0;i<100;++i)
				{
					p[i] = x.allocate((rand()%10)+1);
					x.construct(p[i], (T)(rand()%10));
				}
				//dealloc and realloc randomly
				for(int i=0;i<1000;++i)
				{
					r = rand()%100;
					x.destroy(p[i*2]);
					x.deallocate(p[r]);
					p[r] = x.allocate((rand()%10)+1);
				}
				//clear out every other block
				for(int i=0;i<50;++i)
				{
					x.deallocate(p[i*2]);
				}
				//do some more chaos
				for(int i=0;i<1000;++i)
				{
					r = (2*(rand()%50))+1;
					x.deallocate(p[r]);
					p[r] = x.allocate((rand()%10)+1);
					x.construct(p[r], (T)(rand()%10));
				}
				//deallocate the remaining blocks
				for(int i=0;i<50;++i)
				{
					x.deallocate(p[(i*2)+1]);
				}
				//validate that allocator is entirely free
				CPPUNIT_ASSERT(x.view(x.a[0]) == 19992);
			}
			catch(std::bad_alloc&)
			{
				CPPUNIT_ASSERT(false);
			}
		}
	}

	// -----
	// suite
	// -----

	CPPUNIT_TEST_SUITE(CustomAllocator);
	CPPUNIT_TEST(test_valid1);
	CPPUNIT_TEST(test_valid2);
	CPPUNIT_TEST(test_valid3);
	CPPUNIT_TEST(test_valid4);
	CPPUNIT_TEST(test_valid5);
	CPPUNIT_TEST(test_allocate1);
	CPPUNIT_TEST(test_allocate2);
	CPPUNIT_TEST(test_allocate3);
	CPPUNIT_TEST(test_allocate4);
	CPPUNIT_TEST(test_allocate5);
	CPPUNIT_TEST(test_allocate6);
	CPPUNIT_TEST(test_dealloc1);
	CPPUNIT_TEST(test_dealloc2);
	CPPUNIT_TEST(test_dealloc3);
	CPPUNIT_TEST(test_chaos<int>);
	CPPUNIT_TEST(test_chaos<float>);
	CPPUNIT_TEST(test_chaos<double>);
	CPPUNIT_TEST(test_chaos<char>);
	CPPUNIT_TEST(test_chaos<bool>);
	CPPUNIT_TEST(test_chaos<short>);
	CPPUNIT_TEST(test_chaos<long>);
	CPPUNIT_TEST(test_chaos<long double>);
	CPPUNIT_TEST(test_chaos<wchar_t>);
	CPPUNIT_TEST(test_chaos<unsigned int>);
	CPPUNIT_TEST(test_chaos<long long>);
	CPPUNIT_TEST(test_chaos<unsigned long>);
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
