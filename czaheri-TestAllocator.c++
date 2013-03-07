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

#define private public
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
        try {
            A x;
            const difference_type s = 1;
            const value_type      v = 2;
            const pointer         p = x.allocate(s);
            if(p != 0) {
                x.construct(p, v);
                CPPUNIT_ASSERT(*p == v);
                x.destroy(p);
                x.deallocate(p, s);
            }
        } catch(bad_alloc& e) {
            CPPUNIT_ASSERT(true);
        }
    }

    void test_two() {
        try {
            A x;
            const difference_type   s1 = 2;
            const difference_type   s2 = 2;
            const difference_type   s3 = 2;
            const pointer           p1 = x.allocate(s1);
            const pointer           p2 = x.allocate(s2);
            const pointer           p3 = x.allocate(s3);
            if(p1 && p2 && p3) {
                x.deallocate(p1, s1);
                x.deallocate(p3, s3);
                x.deallocate(p2, s2);
            }     
        } catch(bad_alloc& e) {
            CPPUNIT_ASSERT(true);
        } 
    }

    // --------
    // test_ten
    // --------

    void test_ten () {
        try {
            A x;
            const difference_type s = 10;
            const value_type      v = 2;
            const pointer         b = x.allocate(s);
                  pointer         e = b + s;
                  pointer         p = b;

            if(p != 0) {
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
        } catch(bad_alloc& e) {
            CPPUNIT_ASSERT(true);
        }
    }

    // -----
    // suite
    // -----

    CPPUNIT_TEST_SUITE(TestAllocator);

    CPPUNIT_TEST(test_one);
    CPPUNIT_TEST(test_ten);
    CPPUNIT_TEST(test_two);


    CPPUNIT_TEST_SUITE_END();
};

template<typename A>
struct TestAllocator_OurAllocator : CppUnit::TestFixture {

    typedef typename A::value_type      value_type;
    typedef typename A::difference_type difference_type;
    typedef typename A::pointer         pointer;
    typedef typename A::sentinel        sentinel;

    // -----------
    // Constructor
    // ----------

    void test_constructor_1() {
        try {
            A x;
            int index = 0;
            const sentinel first_sentinel = *reinterpret_cast<int *>(const_cast<char*>(&x.a[index]));
            index += sizeof(sentinel);
            index += abs(first_sentinel);
            const sentinel second_sentinel = *reinterpret_cast<int *>(const_cast<char*>(&x.a[index]));
            CPPUNIT_ASSERT(first_sentinel == second_sentinel);
        } catch(bad_alloc& e) {
            CPPUNIT_ASSERT(true);
        }
        
    }

    void test_constructor_2() {
        try {
            A x;
            int index = 0;
            const sentinel first_sentinel = *reinterpret_cast<int *>(const_cast<char*>(&x.a[index]));
            const int heap_size = x.capacity - 2 * sizeof(sentinel);
            CPPUNIT_ASSERT(first_sentinel == heap_size);
        } catch(bad_alloc& e) {
            CPPUNIT_ASSERT(true);
        }
    }

    void test_constructor_3() {
        try {
            A x;
            int index = 0;
            const sentinel first_sentinel = *reinterpret_cast<int *>(const_cast<char*>(&x.a[index]));
            index += sizeof(sentinel);
            index += abs(first_sentinel);
            const sentinel second_sentinel = *reinterpret_cast<int *>(const_cast<char*>(&x.a[index]));
            const int heap_size = x.capacity - 2 * sizeof(sentinel);
            CPPUNIT_ASSERT(first_sentinel == second_sentinel);
            CPPUNIT_ASSERT(first_sentinel == heap_size);
            CPPUNIT_ASSERT(second_sentinel == heap_size);
        } catch(bad_alloc& e) {
            CPPUNIT_ASSERT(true);
        }
    }

    // -----------
    // Allocate
    // ----------

    void test_allocate_1() {
        try {
            A x;
            x.allocate(-1);
            CPPUNIT_ASSERT(false);
        } catch(bad_alloc& e) {
            CPPUNIT_ASSERT(true);
        }
    }

    void test_allocate_2() {
        try {
            A x;
            const pointer p = x.allocate(1);
            if(p != 0) {
                CPPUNIT_ASSERT(x.view(x.a[0]) == -(signed int)sizeof(value_type));
            }
        } catch(bad_alloc& e) {
            CPPUNIT_ASSERT(true);
        }
    }

    void test_allocate_3() {
        try {
            A x;
            x.allocate(0);
            CPPUNIT_ASSERT(false);
        } catch(bad_alloc& e) {
            CPPUNIT_ASSERT(true);
        }
    }

    void test_allocate_4() {
        try {
            A x;
            const pointer p = x.allocate(5);
            if(p == 0) {
                int requested_block_size = 5 * sizeof(value_type) + 2 * sizeof(sentinel);
                int available_block_size = x.capacity - 2 * sizeof(sentinel);
                CPPUNIT_ASSERT(requested_block_size > available_block_size);
            } else {
                CPPUNIT_ASSERT(p == (pointer)(x.a + sizeof(sentinel)));
                CPPUNIT_ASSERT(x.view(x.a[0]) == 5 * -(signed int)sizeof(value_type));
                CPPUNIT_ASSERT(x.view(x.a[sizeof(sentinel) + 5 * sizeof(value_type)]) == 5 * -(signed int)sizeof(value_type));
            }
        } catch(bad_alloc& e) {
            CPPUNIT_ASSERT(true);
        }
    }


    // -----------
    // Deallocate
    // ----------

    void test_deallocate_1() {
        try {
            A x;
            const pointer p = x.allocate(1);
            if(p != 0) {
                CPPUNIT_ASSERT(x.view(x.a[0]) == -(signed int)sizeof(value_type));
                x.deallocate(p);
            }
            CPPUNIT_ASSERT(x.valid());
        } catch (bad_alloc& e) {
            CPPUNIT_ASSERT(true);
        }
    }

    void test_deallocate_2() {
        try {
            A x;
            const pointer p = x.allocate(5);
            if(p == 0) {
                CPPUNIT_ASSERT(true);
            } else {
                CPPUNIT_ASSERT(x.view(x.a[0]) == 5 * -(signed int)sizeof(value_type));
                x.deallocate(p);
                CPPUNIT_ASSERT(x.valid());
            }
        } catch(bad_alloc& e) {
            CPPUNIT_ASSERT(true);
        }
    }

    void test_deallocate_3() {
        try {
            A x;
            const pointer p = x.allocate(2);
            const pointer q = x.allocate(3);
            if(p != 0) {
                CPPUNIT_ASSERT(x.view(x.a[0]) == 2 * -(signed int)sizeof(value_type));
                x.deallocate(p);
                CPPUNIT_ASSERT(x.valid()); 

                if(q != 0) {
                    x.deallocate(q);
                    CPPUNIT_ASSERT(x.valid());
                }
            } 
        } catch(bad_alloc& e) {
            CPPUNIT_ASSERT(true);
        }
    }

    void test_valid_1() {
        try {
            A x;
            CPPUNIT_ASSERT(x.valid());
        } catch(bad_alloc& e) {
            CPPUNIT_ASSERT(true);
        }
    }

    void test_valid_2() {
        try {
            A x;
            x.view(x.a[x.capacity - sizeof(sentinel) - 1]) = 1;
            CPPUNIT_ASSERT(!x.valid()); 
        } catch(bad_alloc& e) {
            CPPUNIT_ASSERT(true);
        }
    }

    void test_valid_3() {
        try {
            A x;
            pointer p = x.allocate(1);
            if(p!= 0) {
                x.view(x.a[sizeof(sentinel) + sizeof(value_type)]) = 1;
                CPPUNIT_ASSERT(!x.valid());
            }
        } catch(bad_alloc& e) {
            CPPUNIT_ASSERT(true);
        }
    }

    void test_view_1() {
        try {
            A x;
            pointer p = x.allocate(1);
            if(p != 0) {
                CPPUNIT_ASSERT(x.view(x.a[0]) == -(signed int)sizeof(value_type));
            }
        } catch(bad_alloc& e) {
            CPPUNIT_ASSERT(true);
        }
    }

    void test_view_2() {
        try {
            A x;
            pointer p = x.allocate(1);
            if(p != 0) {
                CPPUNIT_ASSERT(x.view(x.a[sizeof(sentinel) + sizeof(value_type)]) == -(signed int)sizeof(value_type));
            }
        } catch(bad_alloc& e) {
            CPPUNIT_ASSERT(true);
        }
    }

    void test_view_3() {
        try {
            A x;
            pointer p = x.allocate(1);
            if(p != 0) {
                CPPUNIT_ASSERT(x.view(x.a[0]) == -(signed int)sizeof(value_type));
                CPPUNIT_ASSERT(x.view(x.a[sizeof(sentinel) + sizeof(value_type)]) == -(signed int)sizeof(value_type));
            }
        } catch(bad_alloc& e) {
            CPPUNIT_ASSERT(true);
        }
    }


    // -----
    // suite
    // -----

    CPPUNIT_TEST_SUITE(TestAllocator_OurAllocator);

    CPPUNIT_TEST(test_constructor_1);
    CPPUNIT_TEST(test_constructor_2);
    CPPUNIT_TEST(test_constructor_3);

    CPPUNIT_TEST(test_allocate_1);
    CPPUNIT_TEST(test_allocate_2);
    CPPUNIT_TEST(test_allocate_3);
    CPPUNIT_TEST(test_allocate_4);

    CPPUNIT_TEST(test_deallocate_1);
    CPPUNIT_TEST(test_deallocate_2);
    CPPUNIT_TEST(test_deallocate_3);

    CPPUNIT_TEST(test_valid_1);
    CPPUNIT_TEST(test_valid_2);
    CPPUNIT_TEST(test_valid_3);

    CPPUNIT_TEST(test_view_1);
    CPPUNIT_TEST(test_view_2);
    CPPUNIT_TEST(test_view_3);

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
    tr.addTest(TestAllocator< Allocator<int, 12> >::suite()); 
    tr.addTest(TestAllocator< Allocator<int, 200> >::suite()); 

    tr.addTest(TestAllocator< std::allocator<double> >::suite());
    tr.addTest(TestAllocator< Allocator<double, 100> >::suite());
    tr.addTest(TestAllocator< Allocator<double, 12> >::suite()); 
    tr.addTest(TestAllocator< Allocator<double, 200> >::suite()); 

    tr.addTest(TestAllocator< std::allocator<bool> >::suite());
    tr.addTest(TestAllocator< Allocator<bool, 100> >::suite());
    tr.addTest(TestAllocator< Allocator<bool, 12> >::suite());
    tr.addTest(TestAllocator< Allocator<bool, 200> >::suite()); 

    /// Our allocator
    tr.addTest(TestAllocator_OurAllocator< Allocator<int, 100> >::suite());
    tr.addTest(TestAllocator_OurAllocator< Allocator<int, 12> >::suite()); 
    tr.addTest(TestAllocator_OurAllocator< Allocator<int, 200> >::suite()); 

    tr.addTest(TestAllocator_OurAllocator< Allocator<double, 100> >::suite());
    tr.addTest(TestAllocator_OurAllocator< Allocator<double, 12> >::suite()); 
    tr.addTest(TestAllocator_OurAllocator< Allocator<double, 200> >::suite()); 

    tr.addTest(TestAllocator_OurAllocator< Allocator<bool, 100> >::suite());
    tr.addTest(TestAllocator_OurAllocator< Allocator<bool, 12> >::suite()); 
    tr.addTest(TestAllocator_OurAllocator< Allocator<bool, 200> >::suite());  
    tr.run();

    cout << "Done." << endl;
    return 0;
}
