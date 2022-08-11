#include <vector>
#include "restrict.hpp"

void assign_vector_front_no_restrict(
        std::vector<int>& x, std::vector<int>& y, std::vector<int>& z)
{
    x.front() = 11;
    y.front() = 333;
    z.front() = x.front();
}

void assign_vector_front_restrict(
        std::vector<int>& x, std::vector<int>& y, std::vector<int>& z)
{
    restrict::ref<int> xfr(x.front());
    restrict::ref<int> yfr(y.front());
    restrict::ref<int> zfr(z.front());

    xfr.get() = 11;
    yfr.get() = 333;
    zfr.get() = xfr.get();
}
