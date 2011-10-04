// JiNP 4 - Wojciech Żółtak - 292583
#include <boost/operators.hpp> 
#include <cmath>
#include "si_units.hh"

template<class Unit, class Y = double>
class Quantity : boost::additive <Quantity <Unit, Y> >,
                 boost::multiplicative <Quantity <Unit, Y>, Y>
{
  public:
    typedef Unit unit_type;
    typedef Y value_type;
    typedef Quantity<unit_type, value_type> this_type;
  
    Quantity() : value(Y()) {};
    
    explicit Quantity(Y const &val) : value(val) {};

    template<typename YY>
    Quantity(Quantity<Unit, YY> const &pQuantity)
    {
      value = pQuantity.get_value();
    }

    template<typename YY>
    Quantity<Unit, Y>& operator=(Quantity<Unit, YY> const &pQuantity)
    {
      value = pQuantity.get_value();
      return *this;
    }
    
    template<typename YY>
    Quantity<Unit, Y>& operator+=(Quantity<Unit, YY> const &pQuantity)
    {
      value += pQuantity.get_value();
      return *this;      
    }

    template<typename YY>
    Quantity<Unit, Y>& operator-=(Quantity<Unit, YY> const &pQuantity)
    {
      value -= pQuantity.get_value();
      return *this;
    }

    Quantity<Unit, Y>& operator*=(value_type const &val)
    {
      value *= val;
      return *this;
    }

    Quantity<Unit, Y>& operator/=(value_type const &val)
    {
      value /= val;
      return *this;
    }

    value_type const& get_value() const
    {
      return value;
    }

    void swap(Quantity<Unit, Y> &pQuantity)
    {
      Y temp;
      temp = pQuantity.value;
      pQuantity.value = value;
      value = temp;
      return ;
    }
    
    friend std::ostream& operator<<
      (std::ostream &pOstream, const Quantity<Unit, Y> &pQuantity)
    {
      pOstream << pQuantity.get_value() << Quantity::unit_type();
      return pOstream;
    } 
    
  private:
    value_type value;
};

template <class UnitT, class Y>
void swap(Quantity<UnitT, Y> &pA, Quantity<UnitT, Y> &pB)
{
  pA.swap(pB);
}

template<typename UnitT, typename Y>
const Quantity<UnitT, Y> operator+
  (Quantity<UnitT, Y> const &pFirst, Quantity<UnitT, Y> const &pSecond)
{
  typedef Quantity<UnitT, Y> retType;
  return Quantity<retType, Y>(pFirst.get_value() + pSecond.get_value());
}

template<typename UnitT, typename Y>
const Quantity<UnitT, Y> operator-
  (Quantity<UnitT, Y> const &pFirst,Quantity<UnitT, Y> const &pSecond)
{
  typedef Quantity<UnitT, Y> retType;
  return Quantity<retType, Y>(pFirst.get_value() - pSecond.get_value());
}

template<typename UnitT1, typename UnitT2, typename Y>
const Quantity<typename multiply_si_units<UnitT1, UnitT2>::type, Y> operator*
  (Quantity<UnitT1, Y> const &pFirst, Quantity<UnitT2, Y> const &pSecond)
{
  typedef typename multiply_si_units<UnitT1, UnitT2>::type retType;
  return Quantity<retType, Y>(pFirst.get_value() * pSecond.get_value());
}

template<typename UnitT1, typename UnitT2, typename Y>
const Quantity<typename divide_si_units<UnitT1, UnitT2>::type, Y> operator/
  (Quantity<UnitT1, Y> const &pFirst, Quantity<UnitT2, Y> const &pSecond)
{
  typedef typename divide_si_units<UnitT1, UnitT2>::type retType;
  return Quantity<retType, Y>(pFirst.get_value() / pSecond.get_value());
}

template<typename UnitT, typename Y, typename YY>
bool operator==
  (Quantity<UnitT, Y> const &pFirst, Quantity<UnitT, YY> const &pSecond)
{
  return pFirst.get_value() == pSecond.get_value();
}

template<typename UnitT, typename Y, typename YY>
bool operator!=
  (Quantity<UnitT, Y> const &pFirst, Quantity<UnitT, YY> const &pSecond)
{
  return pFirst.get_value() != pSecond.get_value();
}

template<typename UnitT, typename Y, typename YY>
bool operator<
  (Quantity<UnitT, Y> const &pFirst, Quantity<UnitT, YY> const &pSecond)
{
  return pFirst.get_value() < pSecond.get_value();
}

template<typename UnitT, typename Y, typename YY>
bool operator>
  (Quantity<UnitT, Y> const &pFirst, Quantity<UnitT, YY> const &pSecond)
{
  return pFirst.get_value() > pSecond.get_value();
}

template<typename UnitT, typename Y, typename YY>
bool operator<=
  (Quantity<UnitT, Y> const &pFirst, Quantity<UnitT, YY> const &pSecond)
{
  return pFirst.get_value() <= pSecond.get_value();
}

template<typename UnitT, typename Y, typename YY>
bool operator>=
  (Quantity<UnitT, Y> const &pFirst, Quantity<UnitT, YY> const &pSecond)
{
  return pFirst.get_value() >= pSecond.get_value();
}

template<typename UnitT, typename Y>
const Quantity<typename power<UnitT, 2>::type, Y> sqr
  (Quantity<UnitT, Y> const &a)
{
  typedef typename power<UnitT, 2>::type retType;
  return Quantity<retType, Y>(a.get_value() * a.get_value());
}

template<typename UnitT, typename Y>
const Quantity<typename root<UnitT, 2>::type, Y> sqrt
  (Quantity<UnitT, Y> const &a)
{
  typedef typename root<UnitT, 2>::type retType;
  return Quantity<retType, Y>(sqrt(a.get_value()));
}
