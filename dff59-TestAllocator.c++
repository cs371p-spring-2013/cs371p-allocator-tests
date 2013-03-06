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
        x.deallocate(b, s);}

    // -----
    // suite
    // -----

    CPPUNIT_TEST_SUITE(TestAllocator);
    CPPUNIT_TEST(test_one);
    CPPUNIT_TEST(test_ten);
    CPPUNIT_TEST_SUITE_END();};

template <typename A>
struct FailureCases : CppUnit::TestFixture
{
  void fail1 ()
  {
    A alligator;
    alligator.allocate (1);
  }

  CPPUNIT_TEST_SUITE (FailureCases);
  CPPUNIT_TEST_EXCEPTION (fail1, std::bad_alloc);
  CPPUNIT_TEST_SUITE_END ();
};

template <typename A>
struct MyTests : CppUnit::TestFixture
{
  typedef typename A::value_type      value_type;
  typedef typename A::difference_type difference_type;
  typedef typename A::pointer         pointer;
  const int MYSIZE = sizeof (value_type);

  void valid1 ()
  {
    A valid;
    int* addr = reinterpret_cast <int*> (valid.allocate (1) + 1);
    *addr = -MYSIZE;
    CPPUNIT_ASSERT (valid.isValid ());
  }

  void valid2 ()
  {
    A valid;
    int* addr1 = reinterpret_cast <int*> (valid.allocate (5) + 5);
    int* addr2 = reinterpret_cast <int*> (valid.allocate (3) + 3);
    *addr1 = -MYSIZE * 5;
    *addr2 = -MYSIZE * 3;
    CPPUNIT_ASSERT (valid.isValid ());
  }

  void valid3 ()
  {
    A invalid;
    int* badAddr = reinterpret_cast <int*> (invalid.allocate (1) + 1);
    *badAddr = 0;
    CPPUNIT_ASSERT (!invalid.isValid ());
  }

  void valid4 ()
  {
    A invalid;
    int* addr = reinterpret_cast <int*> (invalid.allocate (1) + 1);
    *addr = -MYSIZE;
    int* badAddr = reinterpret_cast <int*> (invalid.allocate (2) + 2);
    *badAddr = -MYSIZE;
    CPPUNIT_ASSERT (!invalid.isValid ());
  }

  void valid5 ()
  {
    A invalid;
    int* badAddr = reinterpret_cast <int*> (invalid.allocate (1) + 1);
    *badAddr = -MYSIZE;
    invalid.allocate (2);
    *badAddr = -MYSIZE * 2;
    CPPUNIT_ASSERT (!invalid.isValid ());
  }

  void valid6 ()
  {
    A invalid;
    int* badAddr1 = reinterpret_cast <int*> (invalid.allocate (5) + 5);
    int* badAddr2 = reinterpret_cast <int*> (invalid.allocate (3) + 3);
    *badAddr1 = -MYSIZE * 5;
    *badAddr2 = -MYSIZE * 5;
    CPPUNIT_ASSERT (!invalid.isValid ());
  }

  void test2 ()
  {
    A alligator;
    value_type* addr1 = alligator.allocate (5);
    for (int ndx0 = 0; ndx0 < 5; ndx0++)
    {
      alligator.construct (addr1 + ndx0, 1);
    }

    value_type* addr2 = alligator.allocate (3);
    for (int ndx1 = 0; ndx1 < 3; ndx1++)
    {
      alligator.construct (addr2 + ndx1, 1);
    }

    char* testAddr1 = reinterpret_cast <char*> (addr1);
    char* testAddr2 = reinterpret_cast <char*> (addr2);

    CPPUNIT_ASSERT ((testAddr1 + (5 * sizeof (value_type)) + 8) == testAddr2);

    for (int ndx2 = 0; ndx2 < 5; ndx2++)
    {
      alligator.destroy (addr1 + ndx2);
    }

    for (int ndx3 = 0; ndx3 < 3; ndx3++)
    {
      alligator.destroy (addr2 + ndx3);
    }

    alligator.deallocate (addr1, 0);
    alligator.deallocate (addr2, 0);
  }

  void test3 ()
  {
    A alligator;
    value_type* addr1 = alligator.allocate (5);
    for (int ndx0 = 0; ndx0 < 5; ndx0++)
    {
      alligator.construct (addr1 + ndx0, 1);
    }

    value_type* addr2 = alligator.allocate (3);
    for (int ndx1 = 0; ndx1 < 3; ndx1++)
    {
      alligator.construct (addr2 + ndx1, 1);
    }

    alligator.deallocate (addr1, 0);
    alligator.deallocate (addr2, 0);

    value_type* addr3 = alligator.allocate (5);
    value_type* addr4 = alligator.allocate (3);

    CPPUNIT_ASSERT (addr1 == addr3);
    CPPUNIT_ASSERT (addr2 == addr4);

    for (int ndx2 = 0; ndx2 < 5; ndx2++)
    {
      alligator.destroy (addr1 + ndx2);
    }

    for (int ndx3 = 0; ndx3 < 3; ndx3++)
    {
      alligator.destroy (addr2 + ndx3);
    }

    alligator.deallocate (addr1, 0);
    alligator.deallocate (addr2, 0);
  }

  void test4 ()
  {
    A alligator;
    value_type* addr1 = alligator.allocate (5);
    for (int ndx0 = 0; ndx0 < 5; ndx0++)
    {
      alligator.construct (addr1 + ndx0, 1);
    }

    value_type* addr2 = alligator.allocate (3);
    for (int ndx1 = 0; ndx1 < 3; ndx1++)
    {
      alligator.construct (addr2 + ndx1, 1);
    }

    alligator.deallocate (addr1, 0);
    alligator.deallocate (addr2, 0);

    value_type* addr3 = alligator.allocate (3);
    value_type* addr4 = alligator.allocate (5);

    CPPUNIT_ASSERT (addr1 == addr3);
    CPPUNIT_ASSERT (addr2 == (addr4 + 2));

    for (int ndx2 = 0; ndx2 < 5; ndx2++)
    {
      alligator.destroy (addr1 + ndx2);
    }

    for (int ndx3 = 0; ndx3 < 3; ndx3++)
    {
      alligator.destroy (addr2 + ndx3);
    }

    alligator.deallocate (addr1, 0);
    alligator.deallocate (addr2, 0);
  }

  void test5 ()
  {
    A alligator;
    alligator.allocate (100 / sizeof (value_type));
  }

  void test6 ()
  {
    int test = 0;
    A alligator;
    int* addr = 0;
    try
    {
      addr = reinterpret_cast <int*> (alligator.allocate
                                                (92 / sizeof (value_type)));
    }
    catch (...)
    {
      ++test;
    }

    CPPUNIT_ASSERT (!test);
    CPPUNIT_ASSERT (*(addr - 1) == -92);
  }

  void test7 ()
  {
    A alligator;
    value_type* valAddr = alligator.allocate (5);
    int* intAddr = reinterpret_cast <int*> (valAddr);
    intAddr -= 1;
    CPPUNIT_ASSERT (*intAddr == -((int) sizeof (value_type) * 5));
    alligator.deallocate (valAddr);
    CPPUNIT_ASSERT (*intAddr == 92);
  }

  void test8 ()
  {
    A alligator;
    value_type* valAddr1 = alligator.allocate (5);
    value_type* valAddr2 = alligator.allocate (3);
    int* intAddr1 = reinterpret_cast <int*> (valAddr1);
    int* intAddr2 = reinterpret_cast <int*> (valAddr2);
    intAddr1 -= 1;
    intAddr2 -= 1;
    CPPUNIT_ASSERT (*intAddr1 == -((int) sizeof (value_type) * 5));
    CPPUNIT_ASSERT (*intAddr2 == -((int) sizeof (value_type) * 3));
    alligator.deallocate (valAddr2);
    CPPUNIT_ASSERT (*intAddr1 == -((int) sizeof (value_type) * 5));
    CPPUNIT_ASSERT (*intAddr2 == (84 - (sizeof (value_type) * 5)));
    alligator.deallocate (valAddr1);
    CPPUNIT_ASSERT (*intAddr1 == 92);
  }

  void test9 ()
  {
    A alligator;
    value_type* valAddr1 = alligator.allocate (5);
    value_type* valAddr2 = alligator.allocate (3);
    int* intAddr1 = reinterpret_cast <int*> (valAddr1);
    int* intAddr2 = reinterpret_cast <int*> (valAddr2);
    intAddr1 -= 1;
    intAddr2 -= 1;
    CPPUNIT_ASSERT (*intAddr1 == -((int) sizeof (value_type) * 5));
    CPPUNIT_ASSERT (*intAddr2 == -((int) sizeof (value_type) * 3));
    alligator.deallocate (valAddr1);
    CPPUNIT_ASSERT (*intAddr1 == (int) sizeof (value_type) * 5);
    CPPUNIT_ASSERT (*intAddr2 == -((int) sizeof (value_type) * 3));
    alligator.deallocate (valAddr2);
    CPPUNIT_ASSERT (*intAddr1 == 92);
  }

  void test10 ()
  {
    A alligator;
    value_type* valAddr1 = alligator.allocate (2);
    value_type* valAddr2 = alligator.allocate (2);
    value_type* valAddr3 = alligator.allocate (2);
    int* intAddr1 = reinterpret_cast <int*> (valAddr1);
    int* intAddr2 = reinterpret_cast <int*> (valAddr2);
    int* intAddr3 = reinterpret_cast <int*> (valAddr3);
    intAddr1 -= 1;
    intAddr2 -= 1;
    intAddr3 -= 1;
    CPPUNIT_ASSERT (*intAddr1 == -((int) sizeof (value_type) * 2));
    CPPUNIT_ASSERT (*intAddr2 == -((int) sizeof (value_type) * 2));
    CPPUNIT_ASSERT (*intAddr3 == -((int) sizeof (value_type) * 2));
    alligator.deallocate (valAddr3);
    CPPUNIT_ASSERT (*intAddr1 == -((int) sizeof (value_type) * 2));
    CPPUNIT_ASSERT (*intAddr2 == -((int) sizeof (value_type) * 2));
    CPPUNIT_ASSERT (*intAddr3 == (76 - (sizeof (value_type) * 4)));
    alligator.deallocate (valAddr1);
    CPPUNIT_ASSERT (*intAddr1 == (int) sizeof (value_type) * 2);
    CPPUNIT_ASSERT (*intAddr2 == -((int) sizeof (value_type) * 2));
    CPPUNIT_ASSERT (*intAddr3 == (76 - (sizeof (value_type) * 4)));
    alligator.deallocate (valAddr2);
    CPPUNIT_ASSERT (*intAddr1 == 92);
  }

  void test11 ()
  {
    A alligator;
    value_type* valAddr1 = alligator.allocate (2);
    value_type* valAddr2 = alligator.allocate (2);
    value_type* valAddr3 = alligator.allocate (2);
    int* intAddr1 = reinterpret_cast <int*> (valAddr1);
    int* intAddr2 = reinterpret_cast <int*> (valAddr2);
    int* intAddr3 = reinterpret_cast <int*> (valAddr3);
    intAddr1 -= 1;
    intAddr2 -= 1;
    intAddr3 -= 1;
    CPPUNIT_ASSERT (*intAddr1 == -((int) sizeof (value_type) * 2));
    CPPUNIT_ASSERT (*intAddr2 == -((int) sizeof (value_type) * 2));
    CPPUNIT_ASSERT (*intAddr3 == -((int) sizeof (value_type) * 2));
    alligator.deallocate (valAddr1);
    CPPUNIT_ASSERT (*intAddr1 == (int) sizeof (value_type) * 2);
    CPPUNIT_ASSERT (*intAddr2 == -((int) sizeof (value_type) * 2));
    CPPUNIT_ASSERT (*intAddr3 == -((int) sizeof (value_type) * 2));
    alligator.deallocate (valAddr2);
    CPPUNIT_ASSERT (*intAddr1 == (int) (sizeof (value_type) * 4) + 8);
    CPPUNIT_ASSERT (*intAddr3 == -((int) sizeof (value_type) * 2));
    alligator.deallocate (valAddr3);
    CPPUNIT_ASSERT (*intAddr1 == 92);
  }

  void test12 ()
  {
    A alligator;
    value_type* valAddr1 = alligator.allocate (2);
    value_type* valAddr2 = alligator.allocate (2);
    value_type* valAddr3 = alligator.allocate (2);
    int* intAddr1 = reinterpret_cast <int*> (valAddr1);
    int* intAddr2 = reinterpret_cast <int*> (valAddr2);
    int* intAddr3 = reinterpret_cast <int*> (valAddr3);
    intAddr1 -= 1;
    intAddr2 -= 1;
    intAddr3 -= 1;
    CPPUNIT_ASSERT (*intAddr1 == -((int) sizeof (value_type) * 2));
    CPPUNIT_ASSERT (*intAddr2 == -((int) sizeof (value_type) * 2));
    CPPUNIT_ASSERT (*intAddr3 == -((int) sizeof (value_type) * 2));
    alligator.deallocate (valAddr1);
    CPPUNIT_ASSERT (*intAddr1 == (int) sizeof (value_type) * 2);
    CPPUNIT_ASSERT (*intAddr2 == -((int) sizeof (value_type) * 2));
    CPPUNIT_ASSERT (*intAddr3 == -((int) sizeof (value_type) * 2));
    alligator.deallocate (valAddr3);
    CPPUNIT_ASSERT (*intAddr1 == (int) sizeof (value_type) * 2);
    CPPUNIT_ASSERT (*intAddr2 == -((int) sizeof (value_type) * 2));
    CPPUNIT_ASSERT (*intAddr3 == (76 - (sizeof (value_type) * 4)));
    alligator.deallocate (valAddr2);
    CPPUNIT_ASSERT (*intAddr1 == 92);
  }

  void test13 ()
  {
    A alligator;
    value_type* valAddr1 = alligator.allocate (2);
    value_type* valAddr2 = alligator.allocate (2);
    value_type* valAddr3 = alligator.allocate (2);
    int* intAddr1 = reinterpret_cast <int*> (valAddr1);
    int* intAddr2 = reinterpret_cast <int*> (valAddr2);
    int* intAddr3 = reinterpret_cast <int*> (valAddr3);
    intAddr1 -= 1;
    intAddr2 -= 1;
    intAddr3 -= 1;
    CPPUNIT_ASSERT (*intAddr1 == -((int) sizeof (value_type) * 2));
    CPPUNIT_ASSERT (*intAddr2 == -((int) sizeof (value_type) * 2));
    CPPUNIT_ASSERT (*intAddr3 == -((int) sizeof (value_type) * 2));
    alligator.deallocate (valAddr2);
    CPPUNIT_ASSERT (*intAddr1 == -((int) sizeof (value_type) * 2));
    CPPUNIT_ASSERT (*intAddr2 == (int) sizeof (value_type) * 2);
    CPPUNIT_ASSERT (*intAddr3 == -((int) sizeof (value_type) * 2));
    alligator.deallocate (valAddr1);
    CPPUNIT_ASSERT (*intAddr1 == (sizeof (value_type) * 4) + 8);
    CPPUNIT_ASSERT (*intAddr3 == -((int) sizeof (value_type) * 2));
    alligator.deallocate (valAddr3);
    CPPUNIT_ASSERT (*intAddr1 == 92);
  }

  void test14 ()
  {
    A alligator;
    value_type* valAddr1 = alligator.allocate (2);
    value_type* valAddr2 = alligator.allocate (2);
    value_type* valAddr3 = alligator.allocate (2);
    int* intAddr1 = reinterpret_cast <int*> (valAddr1);
    int* intAddr2 = reinterpret_cast <int*> (valAddr2);
    int* intAddr3 = reinterpret_cast <int*> (valAddr3);
    intAddr1 -= 1;
    intAddr2 -= 1;
    intAddr3 -= 1;
    CPPUNIT_ASSERT (*intAddr1 == -((int) sizeof (value_type) * 2));
    CPPUNIT_ASSERT (*intAddr2 == -((int) sizeof (value_type) * 2));
    CPPUNIT_ASSERT (*intAddr3 == -((int) sizeof (value_type) * 2));
    alligator.deallocate (valAddr2);
    CPPUNIT_ASSERT (*intAddr1 == -((int) sizeof (value_type) * 2));
    CPPUNIT_ASSERT (*intAddr2 == (int) sizeof (value_type) * 2);
    CPPUNIT_ASSERT (*intAddr3 == -((int) sizeof (value_type) * 2));
    alligator.deallocate (valAddr3);
    CPPUNIT_ASSERT (*intAddr1 == -((int) sizeof (value_type) * 2));
    CPPUNIT_ASSERT (*intAddr2 == (84 - sizeof (value_type) * 2));
    alligator.deallocate (valAddr1);
    CPPUNIT_ASSERT (*intAddr1 == 92);
  }

  void test15 ()
  {
    A alligator;
    value_type* valAddr1 = alligator.allocate (2);
    value_type* valAddr2 = alligator.allocate (2);
    value_type* valAddr3 = alligator.allocate (2);
    int* intAddr1 = reinterpret_cast <int*> (valAddr1);
    int* intAddr2 = reinterpret_cast <int*> (valAddr2);
    int* intAddr3 = reinterpret_cast <int*> (valAddr3);
    intAddr1 -= 1;
    intAddr2 -= 1;
    intAddr3 -= 1;
    CPPUNIT_ASSERT (*intAddr1 == -((int) sizeof (value_type) * 2));
    CPPUNIT_ASSERT (*intAddr2 == -((int) sizeof (value_type) * 2));
    CPPUNIT_ASSERT (*intAddr3 == -((int) sizeof (value_type) * 2));
    alligator.deallocate (valAddr3);
    CPPUNIT_ASSERT (*intAddr1 == -((int) sizeof (value_type) * 2));
    CPPUNIT_ASSERT (*intAddr2 == -((int) sizeof (value_type) * 2));
    CPPUNIT_ASSERT (*intAddr3 == (76 - sizeof (value_type) * 4));
    alligator.deallocate (valAddr2);
    CPPUNIT_ASSERT (*intAddr1 == -((int) sizeof (value_type) * 2));
    CPPUNIT_ASSERT (*intAddr2 == (84 - sizeof (value_type) * 2));
    alligator.deallocate (valAddr1);
    CPPUNIT_ASSERT (*intAddr1 == 92);
  }

  CPPUNIT_TEST_SUITE (MyTests);
  CPPUNIT_TEST (valid1);
  CPPUNIT_TEST (valid2);
  CPPUNIT_TEST (valid3);
  CPPUNIT_TEST (valid4);
  CPPUNIT_TEST (valid5);
  CPPUNIT_TEST (valid6);
  CPPUNIT_TEST (test2);
  CPPUNIT_TEST (test3);
  CPPUNIT_TEST (test4);
  CPPUNIT_TEST_EXCEPTION (test5, std::bad_alloc);
  CPPUNIT_TEST (test6);
  CPPUNIT_TEST (test7);
  CPPUNIT_TEST (test8);
  CPPUNIT_TEST (test9);
  CPPUNIT_TEST (test10);
  CPPUNIT_TEST (test11);
  CPPUNIT_TEST (test12);
  CPPUNIT_TEST (test13);
  CPPUNIT_TEST (test14);
  CPPUNIT_TEST (test15);
  CPPUNIT_TEST_SUITE_END ();
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

    tr.addTest (FailureCases <Allocator <int, 1>>::suite ());
    tr.addTest (FailureCases <Allocator <double, 1>>::suite ());
    tr.addTest (FailureCases <Allocator <int, 11>>::suite ());
    tr.addTest (FailureCases <Allocator <double, 15>>::suite ());

    tr.addTest (MyTests <Allocator <int, 100>>::suite ());
    tr.addTest (MyTests <Allocator <double, 100>>::suite ());

    tr.run();

    cout << "Done." << endl;
    return 0;}
