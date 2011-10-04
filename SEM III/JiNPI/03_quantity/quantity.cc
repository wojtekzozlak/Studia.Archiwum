/**
* Copyright 2010
*
* Author: Łukasz Bieniasz-Krzywiec
*/

#include <cassert>
#include <cstdio>
#include <typeinfo>
#include <iostream>

#include "si_units.hh"
#include "quantity.hh"

typedef multiply_si_units<Meter, Kilogram>::type m_times_kg;

void test_this_type() {
  assert(typeid(Quantity<Meter>::this_type) == typeid(Quantity<Meter>));
}

void test_value_type() {
  assert(typeid(Quantity<Meter>::value_type) == typeid(double));
  assert(typeid(Quantity<Meter, int>::value_type) == typeid(int));
  assert(typeid(Quantity<Meter, bool>::value_type) == typeid(bool));
}

void test_unit_type() {
  assert(typeid(Quantity<UnitLess>::unit_type) == typeid(UnitLess));
  assert(typeid(Quantity<Meter>::unit_type) == typeid(Meter));
  assert(typeid(Quantity<m_times_kg>::unit_type) == typeid(m_times_kg));
}

void test_default_constructor() {
  Quantity<UnitLess> ();
  Quantity<UnitLess, int> ();
  Quantity<UnitLess, bool> ();
  Quantity<Meter> ();
  Quantity<Meter, int> ();
  Quantity<Meter, bool> ();
  Quantity<m_times_kg> ();
  Quantity<m_times_kg, int> ();
  Quantity<m_times_kg, bool> ();
}

void test_construct_from_value() {
  Quantity<UnitLess> q1(1);
  assert(q1.get_value() == 1.0);
  Quantity<UnitLess, int> q2(1.5);
  assert(q2.get_value() == 1);
  Quantity<UnitLess, bool> q3(1.0);
  assert(q3.get_value() == true);
  Quantity<UnitLess, bool> q4(0.0);
  assert(q4.get_value() == false);

  Quantity<Meter> q5(1);
  assert(q5.get_value() == 1.0);
  Quantity<Meter, int> q6(1.5);
  assert(q6.get_value() == 1);
  Quantity<Meter, bool> q7(1.0);
  assert(q7.get_value() == true);
  Quantity<Meter, bool> q8(0.0);
  assert(q8.get_value() == false);

  Quantity<m_times_kg> q9(1);
  assert(q9.get_value() == 1.0);
  Quantity<m_times_kg, int> q10(1.5);
  assert(q10.get_value() == 1);
  Quantity<m_times_kg, bool> q11(1.0);
  assert(q11.get_value() == true);
  Quantity<m_times_kg, bool> q12(0.0);
  assert(q12.get_value() == false);
}

void test_copy_construct() {
  Quantity<Meter, int> from(11);
  Quantity<Meter> to(from);
  assert(to.get_value() == 11.0);

  // Quantity<Kilogram> to2(from); // compilation should fail
}

void test_assignment() {
  Quantity<Meter, int> from(11);
  Quantity<Meter> to;
  to = from;
  assert(to.get_value() == 11.0);

  // Quantity<Kilogram> to2(from);
  // to2 = from; // compilation should fail
}

void test_operator_plus_assign() {
  Quantity<Meter, int> q1(3);
  Quantity<Meter, int> q2(5);
  q1 += q2;
  assert(q1.get_value() == 8);
  assert(q2.get_value() == 5);

  Quantity<Kilogram> q3(3.3);
  Quantity<Kilogram> q4(5.5);
  q3 += q4;
  assert(q3.get_value() == 8.8);
  assert(q4.get_value() == 5.5);

  Quantity<Meter> q5(56.7);
  q1 += q5;
  assert(q1.get_value() == 64);
  assert(q5.get_value() == 56.7);

  // q1 += q3; // compilation should fail
  // q3 += q1; // compilation should fail
}

void test_operator_minus_assign() {
  Quantity<Meter, int> q1(3);
  Quantity<Meter, int> q2(5);
  q1 -= q2;
  assert(q1.get_value() == -2);
  assert(q2.get_value() == 5);

  Quantity<Kilogram> q3(3.3);
  Quantity<Kilogram> q4(5.5);
  q3 -= q4;
  assert(q3.get_value() == -2.2);
  assert(q4.get_value() == 5.5);

  Quantity<Meter> q5(56.7);
  q1 -= q5;
  assert(q1.get_value() == -58);
  assert(q5.get_value() == 56.7);

  // q1 -= q3; // compilation should fail
  // q3 -= q1; // compilation should fail
}

void test_operator_times_assign() {
  Quantity<Meter, int> q1(3);
  q1 *= 5;
  assert(q1.get_value() == 3 * 5);

  Quantity<Kilogram> q2(3.3);
  q2 *= 5.5;
  assert(q2.get_value() == 3.3 * 5.5);

  q1 *= 56.7;
  assert(q1.get_value() == 15 * 56);

  // q1 *= Quantity<Meter, int>(2); // compilation should fail
  // q2 *= Quantity<Kilogram>(2.0); // compilation should fail
}

void test_operator_divide_assign() {
  Quantity<Meter, int> q1(3);
  q1 /= 5;
  assert(q1.get_value() == 3 / 5);

  Quantity<Kilogram> q2(3.3);
  q2 /= 5.5;
  assert(q2.get_value() == 3.3 / 5.5);

  q1 /= 56.7;
  assert(q1.get_value() == 3 / 5 / 56);

  // q1 /= Quantity<Meter, int>(2); // compilation should fail
  // q2 /= Quantity<Kilogram>(2.0); // compilation should fail
}

void test_swap() {
  Quantity<Meter> q1(1), q2(2);
  q1.swap(q2);
  assert(q1.get_value() == 2 && q2.get_value() == 1);
  swap(q1, q2);
  assert(q1.get_value() == 1 && q2.get_value() == 2);
}

void test_operator_plus() {
  Quantity<UnitLess, int> q1(3);
  Quantity<UnitLess, int> q2(5);
  assert((q1 + q2).get_value() == 3 + 5);

  Quantity<UnitLess> q3(3.3);
  Quantity<UnitLess> q4(5.5);
  assert((q3 + q4).get_value() == 3.3 + 5.5);

  Quantity<Kilogram> q5(56.7);

  // q1 + q3; // compilation should fail
  // q3 + q5; // compilation should fail
}

void test_operator_minus() {
  Quantity<UnitLess, int> q1(3);
  Quantity<UnitLess, int> q2(5);
  assert((q1 - q2).get_value() == 3 - 5);

  Quantity<UnitLess> q3(3.3);
  Quantity<UnitLess> q4(5.5);
  assert((q3 - q4).get_value() == 3.3 - 5.5);

  Quantity<Kilogram> q5(56.7);

  // q1 - q3; // compilation should fail
  // q3 - q5; // compilation should fail
}

void test_operator_times() {
  Quantity<UnitLess, int> q1(3);
  Quantity<UnitLess, int> q2(5);
  assert((q1 * q2).get_value() == 3 * 5);

  Quantity<UnitLess> q3(3.3);
  Quantity<UnitLess> q4(5.5);
  assert((q3 * q4).get_value() == 3.3 * 5.5);

  Quantity<Kilogram> q5(56.7);
  assert((q3 * q5).get_value() == 3.3 * 56.7);

  // q1 * q3; // compilation should fail
}

void test_operator_divide() {
  Quantity<UnitLess, int> q1(3);
  Quantity<UnitLess, int> q2(5);
  assert((q1 / q2).get_value() == 3 / 5);

  Quantity<UnitLess> q3(3.3);
  Quantity<UnitLess> q4(5.5);
  assert((q3 / q4).get_value() == 3.3 / 5.5);

  Quantity<Kilogram> q5(56.7);
  assert((q3 / q5).get_value() == 3.3 / 56.7);

  // q1 * q3; // compilation should fail
}

void test_sqr() {
  Quantity<Meter> q1(2);
  Quantity<power<Meter, 2>::type> q2 = sqr(q1);
  assert(q2.get_value() == 4.0);
}

void test_sqrt() {
  Quantity<Meter> q1(4);
  Quantity<root<Meter, 2>::type> q2 = sqrt(q1);
  std::cout << q2 << std::endl;
  assert(q2.get_value() == 2.0);
}

void test_comparisons() {
  Quantity<Meter> l(-1), r(1);
  assert(l < r);
  assert(l <= r);
  assert(!(l == r));
  assert(!(l >= r));
  assert(!(l > r));
  assert(l != r);
}

/**
* This function should print:

L              = 2 m
L+L            = 4 m
L-L            = 0 m
L*L            = 4 m^2
L/L            = 1
L*m            = 2 m^2
kg*(L/s)*(L/s) = 4 m^2 kg s^-2
kg*(L/s)^2    = 4 m^2 kg s^-2
L^3            = 8 m^3
L^(3/2)        = 2.82843 m^(3/2)

*/
void test_print() {
  Quantity<Meter> const m(1);
  Quantity<Kilogram> const kg(1);
  Quantity<Second> const s(1);

  typedef multiply_si_units<
      Kilogram, power<divide_si_units<Meter, Second>::type, 2>::type>::type
      Energy;

  Quantity<Meter>  L = Quantity<UnitLess>(2.0) * m; // quantity of length
  Quantity<Energy> E = kg * sqr(L / s);            // quantity of energy

  std::cout
    << "L              = " << L << std::endl
    << "L+L            = " << L+L << std::endl
    << "L-L            = " << L-L << std::endl
    << "L*L            = " << L*L << std::endl
    << "L/L            = " << L/L << std::endl
    << "L*m            = " << L*m << std::endl
    << "kg*(L/s)*(L/s) = " << kg*(L/s)*(L/s) << std::endl
    << "kg*(L/s)^2    = " << kg*sqr(L/s) << std::endl
    << "L^3            = " << L*L*L << std::endl
    << "L^(3/2)        = " << sqrt(L*L*L) << std::endl;
}

int main() {
  Quantity<Meter, long> z;
  std::cout << z << std::endl;
  test_this_type();
  test_value_type();
  test_unit_type();

  test_default_constructor();
  test_construct_from_value();
  test_copy_construct();
  test_assignment();

  test_operator_plus_assign();
  test_operator_minus_assign();
  test_operator_times_assign();
  test_operator_divide_assign();

  test_swap();

  test_operator_plus();
  test_operator_minus();
  test_operator_times();
  test_operator_divide();

  test_sqr();
  test_sqrt();

  test_comparisons();

  test_print();

  return 0;
}
