#include "rational.h"
#include <ostream>
#include <iostream>

Rational::Rational(long n, long d) :
  numerator(d > 0 ? n : -n), denominator(d > 0 ? d : -d)
{
  if(denominator != 0)
    adjust();
};

bool Rational::isNumber() const
{
  return denominator != 0;
}

long Rational::n() const
{
  return numerator;
}

long Rational::d() const
{
  return denominator;
}

long Rational::gcd(long a, long b)
{
  long c;
  if(a < 0) a = -a;
  if(b < 0) b = -b;
  while(b != 0)
  {
    c = a % b;
    a = b;
    b = c;
  }
  return a;
}

long Rational::lcm(long a, long b)
{
  return (a / gcd(a, b)) * b;
}

void Rational::adjust()
{
  long div = Rational::gcd(numerator, denominator);
  numerator /= div;
  denominator /= div;
  return ;
}

const Rational Rational::operator-() const
{
  return Rational(-numerator, denominator);
}

Rational& Rational::operator=(Rational &r)
{
  numerator = r.numerator;
  denominator = r.denominator;
  return *this;
}

Rational& Rational::operator=(long &n)
{
  numerator = n;
  denominator = 1;
  return *this;
}

Rational& Rational::operator+=(const Rational &r)
{
  if(!isNumber() || !r.isNumber())
    denominator = 0;
  else
  {
    long div = Rational::gcd(denominator, r.denominator);
    numerator *= r.denominator / div;
    numerator += r.numerator * (denominator / div);
    denominator *= r.denominator / div;
    adjust();
  }
  return *this;
}

Rational& Rational::operator-=(const Rational &r)
{
  if(!isNumber() || !r.isNumber())
    denominator = 0;
  else
  {
    long div = Rational::gcd(denominator, r.denominator);
    numerator *= r.denominator / div;
    numerator -= r.numerator * (denominator / div);
    denominator *= r.denominator / div;
    adjust();
  }
  return *this;
}

Rational& Rational::operator*=(const Rational &r)
{
  if(!isNumber() || !r.isNumber())
    denominator = 0;
  else
  {
    long div1 = Rational::gcd(numerator, r.denominator);
    long div2 = Rational::gcd(r.numerator, denominator);
    numerator = (numerator / div1) * (r.numerator / div2);
    denominator = (denominator / div2) * (r.denominator / div1);
  }
  return *this;
}

Rational& Rational::operator/=(const Rational &r)
{
  if(!isNumber() || !r.isNumber())
    denominator = 0;
  else
  {
    long div1 = Rational::gcd(numerator, r.numerator);
    long div2 = Rational::gcd(r.denominator, denominator);
    numerator = (numerator / div1) * (r.denominator / div2);
    denominator = (denominator / div2) * (r.numerator / div1);
    if(denominator < 0)
    {
      numerator = -numerator;
      denominator = -denominator;
    }
  }
  return *this;
}

bool Rational::operator<(const Rational &r) const
{
  if(!isNumber() || !r.isNumber())
    return false;
  const long new_d = lcm(denominator, r.denominator);
  return (new_d / denominator) * numerator <
    (new_d / r.denominator) * r.numerator;
}

bool Rational::operator==(const Rational &r) const
{
  return (isNumber() && r.isNumber() &&
    numerator == r.numerator && denominator == r.denominator);
}

bool Rational::operator!=(const Rational &r) const
{
  return (isNumber() && r.isNumber() &&
    numerator != r.numerator || denominator != r.denominator);
}

Rational::operator bool_type() const {
  return isNumber() == true && numerator != 0 ?
    &Rational::this_type_does_not_support_comparisons : 0;
}

const Rational Zero()
{
  static Rational r;
  return r;
}

const Rational One()
{
  static Rational r(1);
  return r;
}

std::ostream& operator<<(std::ostream &out, const Rational &r)
{
  if(r.isNumber())
  {
    long int_part = r.n() / r.d();
    long frac_part = r.n() - r.d() * int_part;
    if(int_part != 0 || int_part == 0 && frac_part == 0)
      out << int_part;
    if(int_part > 0 && frac_part > 0)
      out << "+";
    if(frac_part != 0)
      out << frac_part << "/" << r.d();
   }
   else out << "NaN";
}
