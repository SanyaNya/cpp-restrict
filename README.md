This is experimental implementation of restrict keyword from C in standard c++.

Look at example comparison: https://godbolt.org/z/13s9d7M5n

Requirements: C++11

How it works:
  1)restrict::ref reminds reference to object and moves object to temporary raw_storage(with it no destructor of temporary will be called)
  2)You modificating temporary using restrict::ref::get and because temporary can`t be aliased there is no extra load
  3)On scope end temporary moves back to object
