// JiNP 4 - Wojciech Żółtak - 292583
#ifndef _SI_UNITS_H_
#define _SI_UNITS_H_

#include <boost/units/static_rational.hpp>
#include <boost/preprocessor/seq.hpp>
#include <boost/mpl/plus.hpp>
#include <boost/mpl/minus.hpp>
#include <boost/mpl/times.hpp>
#include <boost/mpl/divides.hpp>
#include <ostream>
#include <iostream>
#include <string>
using namespace boost::units;
using namespace boost;

#define zero static_rational<0>
#define one static_rational<1>
#define BOOST_TT_units (met)(kil)(sec)(amp)(kel)(mol)(can)

std::ostream& write(std::ostream &pOstream, const char s[],
  const integer_type &n, const integer_type &d)
{
  if(n == 0)
    return pOstream;
  pOstream << " " << s;
  if(n != 1 || d != 1)
  {
    pOstream << "^";
    if(d > 1)
      pOstream << "(" << n << "/" << d << ")";
    else
      pOstream << n;
  }
  return pOstream;
}


template<typename Met, typename Kil, typename Sec, typename Amp,
         typename Kel, typename Mol, typename Can>
class Unit {
  template<class Unit1, class Unit2> friend struct multiply_si_units;
  template<class Unit1, class Unit2> friend struct divide_si_units;
  template<class Unit, int N> friend struct power;
  template<class Unit, int N> friend struct root;

  public:
    typedef Met met;
    typedef Kil kil;
    typedef Sec sec;
    typedef Amp amp;
    typedef Kel kel;
    typedef Mol mol;
    typedef Can can;

  friend std::ostream& operator<<(std::ostream& os,
      const Unit<met, kil, sec, amp, kel, mol, can>& unit ){
    write(os, "m", met::numerator(), met::denominator());
    write(os, "kg", kil::numerator(), kil::denominator());
    write(os, "s", sec::numerator(), sec::denominator());
    write(os, "A", amp::numerator(), amp::denominator());
    write(os, "K", kel::numerator(), kel::denominator());
    write(os, "mol", mol::numerator(), mol::denominator());
    write(os, "cd", can::numerator(), can::denominator());
  }
};

template<class Unit1, class Unit2> struct multiply_si_units {    
  public:
    #define BOOST_TT_plus(r, data, t) \
      typedef typename mpl::plus<typename Unit1::t, typename Unit2::t>::type t;
    BOOST_PP_SEQ_FOR_EACH(BOOST_TT_plus, _, BOOST_TT_units)
    #undef BOOST_TT_plus
    
    typedef Unit<met, kil, sec, amp, kel, mol, can> type;
};

template<class Unit1, class Unit2> struct divide_si_units {
  public:
    #define BOOST_TT_minus(r, data, t) \
      typedef typename mpl::minus<typename Unit1::t, typename Unit2::t>::type t;
    BOOST_PP_SEQ_FOR_EACH(BOOST_TT_minus, _, BOOST_TT_units)
    #undef BOOST_TT_minus
    
    typedef Unit<met, kil, sec, amp, kel, mol, can> type;
};

template<class Unit1, int N> struct power {
  public:
    #define BOOST_TT_times(r, data, t) \
      typedef typename mpl::times<typename Unit1::t, \
                                  static_rational<N> >::type t;
    BOOST_PP_SEQ_FOR_EACH(BOOST_TT_times, _, BOOST_TT_units)
    #undef BOOST_TT_times

    typedef Unit<met, kil, sec, amp, kel, mol, can> type;
};

template<class Unit1, int N> struct root {
  public:
    #define BOOST_TT_divides(r, data, t) \
      typedef typename mpl::divides<typename Unit1::t, \
                                    static_rational<N> >::type t;
    BOOST_PP_SEQ_FOR_EACH(BOOST_TT_divides, _, BOOST_TT_units)
    #undef BOOST_TT_divides

    typedef Unit<met, kil, sec, amp, kel, mol, can> type;
};

typedef Unit<one, zero, zero, zero, zero, zero, zero > Meter;
typedef Unit<zero, one, zero, zero, zero, zero, zero> Kilogram;
typedef Unit<zero, zero, one, zero, zero, zero, zero> Second;
typedef Unit<zero, zero, zero, one, zero, zero, zero> Ampere;
typedef Unit<zero, zero, zero, zero, one, zero, zero> Kelvin;
typedef Unit<zero, zero, zero, zero, zero, one, zero> Mole;
typedef Unit<zero, zero, zero, zero, zero, zero, one> Candela;
typedef Unit<zero, zero, zero, zero, zero, zero, zero> UnitLess;

#endif
