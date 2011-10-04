/*
  JiNP 3 - wz292593, Wojciech Żółtak

  Rational class implementation
*/
#include "rational.h"
#include <ostream>

Rational::Rational(long n, long d) : numerator(n), denominator(d)
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
  if(a < 0) a = -a;
  if(b < 0) b = -b;
  return (a / gcd(a, b)) * b;
}

void Rational::adjust()
{
  long div = Rational::gcd(numerator, denominator);
  if(denominator > 0)
  {
    numerator /= div;
    denominator /= div;
  }
  else
  {
    numerator /= -div;
    denominator /= -div;  
  }
  return ;
}

const Rational Rational::operator-() const
{
  return Rational(-numerator, denominator);
}

Rational& Rational::operator=(const Rational &r)
{
  numerator = r.numerator;
  denominator = r.denominator;
  return *this;
}

Rational& Rational::operator=(const long &n)
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
    long new_n = numerator * (r.denominator / div) + r.numerator * (denominator / div);
    long new_d = denominator * (r.denominator / div);
    numerator = new_n;
    denominator = new_d;
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
    long new_n = numerator * (r.denominator / div) - r.numerator * (denominator / div);
    long new_d = denominator * (r.denominator / div);
    numerator = new_n;
    denominator = new_d;
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
    long new_n = (numerator / div1) * (r.numerator / div2);
    long new_d = (denominator / div2) * (r.denominator / div1);
    numerator = new_n;
    denominator = new_d;
  }
  return *this;
}

Rational& Rational::operator/=(const Rational &r)
{
  if(!isNumber() || !r.isNumber() || r.numerator == 0)
    denominator = 0;
  else
  {
    long div1 = Rational::gcd(numerator, r.numerator);
    long div2 = Rational::gcd(r.denominator, denominator);
    long new_n = (numerator / div1) * (r.denominator / div2);
    long new_d = (denominator / div2) * (r.numerator / div1);
    numerator = new_n;
    denominator = new_d;
    if(denominator < 0)
      adjust();
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

bool operator==(const Rational &r1, const Rational &r2)
{
  return (r1.isNumber() && r2.isNumber() &&
    r1.n() == r2.n() && r1.d() == r2.d());
}

bool operator!=(const Rational &r1, const Rational &r2)
{
  return (r1.isNumber() && r2.isNumber() &&
    (r1.n() != r2.n() || r1.d() != r2.d()));
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
