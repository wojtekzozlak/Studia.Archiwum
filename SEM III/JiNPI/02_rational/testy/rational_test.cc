#include "rational.h"

#include <cassert>
#include <sstream>
#include <iostream>
#include <climits>


#if TEST_NUM == 144 || TEST_NUM == 145
void test_fun_1(Rational r)
{
    r+r;
}

void test_fun_2(Rational const r)
{
    r+r;
}

void test_fun_3(Rational const & r)
{
    r+r;
}
#endif



int main()
{
#if TEST_NUM == 0
    {
        ::std::clog << "Test." << ::std::endl;
    }
#endif

// testy konstruktorow
#if TEST_NUM == 101
    {
        ::std::clog << "Rational - konstruktor domyslny i destruktor." << ::std::endl;
        Rational r;
    }
#endif
#if TEST_NUM == 102
    {
        ::std::clog << "Rational - odczyt skladowych." << ::std::endl;
        Rational r;
        r.n();
        r.d();
    }
#endif
#if TEST_NUM == 103
    {
        ::std::clog << "Rational - obiektu domyslny rowny zero." << ::std::endl;
        Rational r;
        assert(r == Zero());
    }
#endif
#if TEST_NUM == 104
    {
        ::std::clog << "Rational - konstruktor jednoargumentowy." << ::std::endl;
        Rational r(5);
	Rational t(5,1);
        assert(r == t);
    }
#endif
#if TEST_NUM == 105
    {
        ::std::clog << "Rational - konstruktor dwuargumentowy, poprawna liczba." << ::std::endl;
        Rational r(3, 4);
        assert(r.n() == 3);
        assert(r.d() == 4);
    }
#endif
#if TEST_NUM == 106
    {
        ::std::clog << "Rational - konstruktor dwuargumentowy, niepoprawna liczba." << ::std::endl;
        Rational r(3, 0);
	assert(!r.isNumber());
    }
#endif
#if TEST_NUM == 107
    {
        ::std::clog << "Rational - konstruktor kopiujacy, poprawna liczba." << ::std::endl;
        Rational r1(7,13);
        Rational r2(r1);
        assert(r1 == r2);
    }
#endif
#if TEST_NUM == 108
    {
        ::std::clog << "Rational - konstruktor kopiujacy, niepoprawna liczba." << ::std::endl;
        Rational r1(200,0);
        Rational r2(r1);
        assert(!r2.isNumber());
    }
#endif

// testy operatorow przypisania
#if TEST_NUM == 109
    {
        ::std::clog << "Rational - operator przypisania, poprawna liczba." << ::std::endl;
        Rational r1(7,13);
        Rational r2;
        r2 = r1;
        assert(r1 == r2);
    }
#endif
#if TEST_NUM == 110
    {
      ::std::clog << "Rational - operator przypisania, niepoprawna liczba." << ::std::endl;
      Rational r1(200,0);
      Rational r2;
      r2 = r1;
      assert(!r2.isNumber());
    }
#endif
#if TEST_NUM == 111
    {
      ::std::clog << "Rational - operator +=, poprawna liczba." << ::std::endl;
      Rational r1(LONG_MAX, LONG_MAX);
      Rational r2(LONG_MIN + 1, LONG_MAX);
      Rational t;
      r2 += r1;
      assert(r2 == t);
    }
#endif
#if TEST_NUM == 112
    {
      ::std::clog << "Rational - operator +=, niepoprawna liczba po lewej stronie." << ::std::endl;
      Rational r1(1,0);
      Rational r2(1,1);
      r1 += r2;
      assert(!r1.isNumber());
    }
#endif
#if TEST_NUM == 113
    {
      ::std::clog << "Rational - operator +=, niepoprawna liczba po prawej stronie." << ::std::endl;
      Rational r1(1,1);
      Rational r2(0,0);
      r1 += r2;
      assert(!r1.isNumber());
    }
#endif
#if TEST_NUM == 114
    {
      ::std::clog << "Rational - operator -=, poprawna liczba." << ::std::endl;
      Rational r1(LONG_MAX, LONG_MAX);
      Rational r2(LONG_MAX, LONG_MAX);
      Rational t;
      r2 -= r1;
      assert(r2 == t);
    }
#endif
#if TEST_NUM == 115
    {
      ::std::clog << "Rational - operator -=, niepoprawna liczba po lewej stronie." << ::std::endl;
      Rational r1(1,0);
      Rational r2(1,1);
      r1 -= r2;
      assert(!r1.isNumber());
    }
#endif
#if TEST_NUM == 116
    {
      ::std::clog << "Rational - operator -=, niepoprawna liczba po prawej stronie." << ::std::endl;
      Rational r1(1,1);
      Rational r2(0,0);
      r1 -= r2;
      assert(!r1.isNumber());
    }
    

#endif
#if TEST_NUM == 117
    {
      ::std::clog << "Rational - operator *=, poprawna liczba." << ::std::endl;
      Rational r1(LONG_MAX, LONG_MAX);
      Rational r2(LONG_MAX, LONG_MAX);
      Rational t(1);
      r2 *= r1;
      assert(r2 == t);
    }
#endif
#if TEST_NUM == 118
    {
      ::std::clog << "Rational - operator *=, niepoprawna liczba po lewej stronie." << ::std::endl;
      Rational r1(1,0);
      Rational r2(1,1);
      r1 *= r2;
      assert(!r1.isNumber());
    }
#endif
#if TEST_NUM == 119
    {
      ::std::clog << "Rational - operator *=, niepoprawna liczba po prawej stronie." << ::std::endl;
      Rational r1(1,1);
      Rational r2(0,0);
      r1 *= r2;
      assert(!r1.isNumber());
    }
#endif
#if TEST_NUM == 120
    {
      ::std::clog << "Rational - operator /=, poprawna liczba." << ::std::endl;
      Rational r1(LONG_MAX, LONG_MAX);
      Rational r2(LONG_MAX, LONG_MAX);
      Rational t(1);
      r2 /= r1;
      assert(r2 == t);
    }
#endif
#if TEST_NUM == 121
    {
      ::std::clog << "Rational - operator /=, dzielenie przez 0." << ::std::endl;
      Rational r1(1,0);
      Rational r2(0,1);
      r1 /= r2;
      assert(!r1.isNumber());
    }
#endif
#if TEST_NUM == 122
    {
      ::std::clog << "Rational - operator /=, niepoprawna liczba po lewej stronie." << ::std::endl;
      Rational r1(1,0);
      Rational r2(1,1);
      r1 /= r2;
      assert(!r1.isNumber());
    }

#endif
#if TEST_NUM == 123
    {
      ::std::clog << "Rational - operator /=, niepoprawna liczba po prawej stronie." << ::std::endl;
      Rational r1(1,1);
      Rational r2(0,0);
      r1 /= r2;
      assert(!r1.isNumber());
    }
#endif

//operatory jedno i dwuargumentowe
#if TEST_NUM == 124
    {
        ::std::clog << "Rational - element przeciwny." << ::std::endl;
        Rational r1(3,4);
        Rational r2(-r1);
	Rational t(-3,4);
        assert(r2 == t);
	Rational r4(0,0);
	Rational r5(-r4);
        assert(!r5.isNumber());
    }
#endif
#if TEST_NUM == 126
    {
        ::std::clog << "Rational - dodawanie." << ::std::endl;
        Rational r1(1,4);
        Rational r2(3,4);
        Rational r3(r2 + r1);
	Rational t(1);
        assert(r3 == t);
        Rational r4(0,0);
	Rational r5(r1 + r4);
        assert(!r5.isNumber());
	Rational r6(r4 + r2);
        assert(!r6.isNumber());
    }
#endif
#if TEST_NUM == 129
    {
        ::std::clog << "Rational - odejmowanie." << ::std::endl;
        Rational r1(1,4);
        Rational r2(5,4);
        Rational r3(r2 - r1);
	Rational t(1);
        assert(r3 == t);
        Rational r4(3,0);
        Rational r5(r1 - r4);
        assert(!r5.isNumber());
        Rational r6(r4 - r2);
        assert(!r6.isNumber());
    }
#endif
#if TEST_NUM == 132
    {
        ::std::clog << "Rational - mnożenie." << ::std::endl;
        Rational r1(1,4);
        Rational r2(4,1);
        Rational r3(r2 * r1);
	Rational t(1);
        assert(r3 == t);
        Rational r4(-1,0);
        Rational r5(r1 * r4);
        assert(!r5.isNumber());
        Rational r6(r4 * r2);
        assert(!r6.isNumber());
    }
#endif
#if TEST_NUM == 133
    {
        ::std::clog << "Rational - dzielenie." << ::std::endl;
        Rational r1(1,4);
        Rational r2(1,4);
        Rational r3(r2 / r1);
	Rational t(1);
        assert(r3 == t);
        Rational r4(-1,0);
        Rational r5(r1 / r4);
        assert(!r5.isNumber());
        Rational r6(r4 / r2);
        assert(!r6.isNumber());
    }
#endif

// operatory porownania, funkcje n i d, Zero i One
#if TEST_NUM == 134
    {
        ::std::clog << "Rational - operator != ." << ::std::endl;
        Rational r1(7,6);
        Rational r2(7,6);
        assert(!(r1 != r2));
        assert(!(r2 != r1));
        Rational r3(6,7);
        assert(r1 != r3);
        assert(r3 != r1);
    }
#endif
#if TEST_NUM == 135
    {
        ::std::clog << "Rational - operator <." << ::std::endl;
        Rational r1(7,6);
        Rational r2(6,5);
        assert(!(r2 < r1));
        assert(r1 < r2);
    }
#endif
#if TEST_NUM == 136
    {
        ::std::clog << "Rational - operator <=." << ::std::endl;
        Rational r1(7,6);
        Rational r2(7,6);
        assert(r1 <= r2);
    }
#endif
#if TEST_NUM == 137
    {
        ::std::clog << "Rational - operator >." << ::std::endl;
        Rational r1(7,6);
        Rational r2(6,5);
        assert(!(r1 > r2));
        assert(r2 > r1);
    }
#endif
#if TEST_NUM == 138
    {
        ::std::clog << "Rational - operator >=." << ::std::endl;
        Rational r1(7,6);
        Rational r2(7,6);
        assert(r2 <= r1);
    }
#endif
#if TEST_NUM == 139
    {
        ::std::clog << "Rational - funkcje n i d." << ::std::endl;
        Rational r1(6,8);
        assert(r1.n() == 3);
        assert(r1.d() == 4);
    }
#endif
#if TEST_NUM == 140
    {
        ::std::clog << "Rational - funkcja Zero." << ::std::endl;
        assert(Zero().n() == 0.0);
    }
#endif
#if TEST_NUM == 141
    {
        ::std::clog << "Rational - funkcja One." << ::std::endl;
        assert(One().n() == One().d());
    }
#endif

// aksjomaty addytywne i multiplikatywne
#if TEST_NUM == 142
    {
        ::std::clog << "Rational - aksjomaty addytywne." << ::std::endl;
        Rational zero;
        Rational r1(1,4);
        Rational r2(-4,5);
        Rational r3(154,665);
        assert(r1 + r2 == r2 + r1);
        assert((r1 + (r2 + r3)) == ((r1 + r2) + r3));
        assert(r1 + (-r1) == zero);
        assert(r2 + (-r2) == zero);
        assert(r3 + (-r3) == zero);
        assert(r1 - r1 == zero);
        assert(r2 - r2 == zero);
        assert(r3 - r3 == zero);
        assert(r1 + zero == r1);
        assert(zero + r1 == r1);
        assert(r2 + zero == r2);
        assert(zero + r2 == r2);
        assert(r1 - zero == r1);
        assert(zero - r1 == -r1);
        assert(r2 - zero == r2);
        assert(zero - r2 == -r2);
        assert(r1 + (-r2) == r1 - r2);
        assert(r1 - r2 == -(r2 - r1));
    }
#endif
#if TEST_NUM == 143
    {
        ::std::clog << "Rational - aksjomaty multiplikatywne." << ::std::endl;
        Rational zero;
        Rational one(1);
        Rational r1(99,5);
        Rational r2(-11,34);
        Rational r3(44,12);
        assert(r1 * r2 == r2 * r1);
        assert((r1 * (r2 * r3)) == ((r1 * r2) * r3));
        assert(r1 * zero == zero);
        assert(zero * r1 == zero);
        assert(r2 * zero == zero);
        assert(zero * r2 == zero);
	assert(r1 * one == r1);
	assert(r2 * one == r2);
	assert(r1 / r1 == one);
	Rational rr(r2/r2);
	assert(r2 / r2 == one);
    }
#endif
#if TEST_NUM == 144
    {
        ::std::clog << "Rational - implicit konwersje." << ::std::endl;
        Rational r1(1, 2);
        Rational r2(r1 + 3);
        Rational r3(r2 - 4);
        Rational r4(r3 * 'a');
        Rational r5 = 3;
        Rational r6 = 'b';
        Rational r7 = 1 + 2*One() + 3*Zero();
        test_fun_1(3);
        test_fun_2(3);
        test_fun_3(3);
        test_fun_1('a');
        test_fun_2('a');
        test_fun_3('a');
    }
#endif
#if TEST_NUM == 145
    {
        ::std::clog << "Rational - const correctness." << ::std::endl;
        Rational const r1(1, 2);
        Rational const r2(3, 7);
        r1.n();
        r1.d();
        r1 + r2;
        r1 - r2;
        r1 * r2;
        r1 / r2;
        (r1 + r2).n();
        (r1 - r2).d();
        (-r1).n();
        Rational const & r3 = r1 + r2;
        test_fun_1(r1 + r2 * r3);
        test_fun_2(r1 + r2 + r3);
        test_fun_3(r1 + r2 - r3);
        ::std::ostringstream oss;
        oss << r3;
        oss << r1 + r2;
    }
#endif
#if TEST_NUM == 146
    {
        ::std::clog << "Rational - laczenie operatorow." << ::std::endl;
        Rational r1(1, 2);
        Rational r2(3, 4);
        Rational r3 = r1 * r2;
        r1 += r2 += r3;
        r1 -= r2 -= r3;
        r1 *= r2 *= r3;
        r1 = r2 /= r3;
        ::std::ostringstream oss;
        oss << r1 << r2 << (r1 + r2) << r3;
        r1 = r2 = r3;
        r1 += r2 -= r3 *= r1 /= r2;
        r1 + r2 + r3;
    }
#endif
#if TEST_NUM == 147
    {
        ::std::clog << "Rational - konstrukcja z typow całkowitoliczbowych." << ::std::endl;
        Rational r1(1, 2);
        Rational r2('a', 'b');
        Rational r3(true, false);
    }
#endif
#if TEST_NUM == 148
    {
        ::std::clog << "Rational - operator << ." << ::std::endl;
        Rational r1(8,16);
        ::std::clog << "Rational(8,16): " <<  r1 << ::std::endl;
        Rational r2(16,8);
        ::std::clog << "Rational(16,8): " <<  r2 << ::std::endl;
        Rational r3(16,10);
        ::std::clog << "Rational(16,10): " << r3 << ::std::endl;
        Rational r4(0,10);
        ::std::clog << "Rational(0,10): " << r4 << ::std::endl;
        Rational r5(0,0);
        ::std::clog << "Rational(0,0): " << r5 << ::std::endl;
        Rational r6(-1,0);
        ::std::clog << "Rational(-1,0): " << r6 << ::std::endl;
    }
#endif

// liczby wymierne w wyrazeniach logicznych
#if TEST_NUM == 149
    {
        ::std::clog << "Rational - liczby wymierne w wyrażeniach logicznych, poprawne liczby." << ::std::endl;
        {
            Rational r1;
            if(r1)
                assert(false);
            Rational r2(1);
            if(r2)
                ;
            else
                assert(false);
            assert(!r1);
            assert(r2);
            assert(r1 || r2);
            assert(r2 && r2);
            bool b = r1;
            if(b);
        }
    }
#endif
#if TEST_NUM == 150
    {
        ::std::clog << "Rational - liczby wymierne w wyrażeniach logicznych, niepoprawne liczby." << ::std::endl;
        {
            Rational r1(0,0);
	    Rational r2(-1,0);
            if(r1)
                assert(false);
	    if (!r1)
		;
	    else
	        assert(false);
            assert(!r1);
            assert(!r2);
            assert(!r1 || !r2);
            assert(!r2 && !r2);
            bool b = r1;
            if(b);
        }
    }
#endif
#if TEST_NUM == 151
    {
        ::std::clog << "Rational - safe bool." << ::std::endl;
        Rational r1(0,0);
        int i = r1; //  to sie nie powinno skompilowac!
	assert(false);
    }
#endif
    ::std::clog << "OK." << ::std::endl;
    return 0;
}
