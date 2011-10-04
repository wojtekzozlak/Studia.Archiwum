#ifndef _IMAGES_HH_
#define _IMAGES_HH_

#include <utility>
#include <boost/function.hpp>

typedef unsigned int color_t;
typedef double coord_t;
typedef std::pair<coord_t, coord_t> point_t;

template<typename P = point_t, typename C = color_t>
class Image
{
  public:
    typedef boost::function<C (P)> type;
};

typedef Image<point_t, color_t>::type image_t;

image_t circle(const point_t p, const coord_t r);

image_t translate(const image_t &im, const point_t x);

image_t checker(const coord_t d);

point_t to_polar(const point_t p);

image_t rings(const point_t p, const coord_t d);

#endif
