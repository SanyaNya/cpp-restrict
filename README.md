# cpp-restrict

This is experimental implementation of restrict keyword from C in standard c++.

## Example: Look at example comparison: https://godbolt.org/z/4P7onsdoq

## Requirements: C++11

## How it works:
 * restrict::ref reminds reference to object and moves object to temporary raw_storage(with it no destructor of temporary will be called)
 * You modificating temporary using restrict::ref::get and because temporary cant be aliased there is no extra load
 * On scope end temporary moves back to object
