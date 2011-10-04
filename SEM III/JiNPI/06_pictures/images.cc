#include "images.hh"
#include <iostream>
#include <cmath>
#include <utility>
#include <complex>
#include <boost/function.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
using namespace boost::lambda;

#define WHITE 0xffffff
#define BLACK 0x000000

point_t subtract_pairs(const point_t p1, const point_t p2)
{
  return point_t(p1.first - p2.first, p1.second - p2.second);
}

image_t translate(const image_t &im, const point_t x)
{
  return bind(im, bind(subtract_pairs, _1, x));
}

color_t circle_f(point_t p, const coord_t r)
{
  coord_t dist = p.first * p.first + p.second * p.second;
  return (dist <= r * r ? BLACK : WHITE);
}

color_t checker_f(const point_t p, const coord_t d)
{
  return fabs(fmod(floor(p.first / d) + floor(p.second / d), 2.0)) < 1.0 ?
    BLACK : WHITE;
}

point_t to_polar(const point_t p)
{
  const std::complex<coord_t> c(p.first, p.second);
  return point_t(abs(c), std::arg(c));
}

image_t checker(const coord_t d)
{
  return bind(checker_f, _1, d);
}

image_t circle(const point_t p, const coord_t r)
{
  return bind(circle_f, bind(subtract_pairs, _1, p), r);
}

point_t absY(const point_t p)
{
  return point_t(p.first, abs(p.second));
}

image_t rings(const point_t p, const coord_t d)
{
  image_t f = checker(d);
  f = bind(f, bind(to_polar, _1));
  f = bind(f, bind(absY, _1));
  return bind(f, bind(subtract_pairs, _1, p));
}
