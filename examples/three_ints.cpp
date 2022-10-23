#include "../restrict.hpp"

void three_ints_no_restrict(
        int& x, int& y, int& z)
{
    x = 1;
    y = 2;
    z = x;
}

void three_ints_restrict(
        restrict::ref<int> x, restrict::ref<int> y, restrict::ref<int> z)
{
    x.get() = 1;
    y.get() = 2;
    z.get() = x.get();
}
