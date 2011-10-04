/*
  Rational class
  - sign of a Rational is a sign of a numerator
  - NaN have 0 in the denominator
*/
#ifndef RATIONAL_H
#define RATIONAL_H

#include <iostream>
#include <boost/operators.hpp>

class Rational : boost::arithmetic<Rational,
boost::arithmetic<Rational, long,
boost::partially_ordered<Rational,
boost::partially_ordered<Rational, long> > > >{
  private:
    typedef void(Rational::*bool_type)() const;
    void this_type_does_not_support_comparisons() const {}
    long numerator, denominator;
    static long gcd(long a, long b);
    static long lcm(long a, long b);
    void adjust();

  public:
    Rational(long n = 0, long d = 1);

    const Rational operator-() const;
    Rational& operator=(Rational &r);
    Rational& operator=(long &r);
    Rational& operator+=(const Rational &r);
    Rational& operator-=(const Rational &r);
    Rational& operator*=(const Rational &r);
    Rational& operator/=(const Rational &r);
    bool operator<(const Rational &r) const;
    bool operator==(const Rational &r) const;
    bool operator!=(const Rational &r) const;
    operator bool_type() const;

    bool isNumber() const;
    long n() const;
    long d() const;
};

std::ostream& operator<<(std::ostream &out, const Rational &r);

const Rational Zero();

const Rational One();

#endif
