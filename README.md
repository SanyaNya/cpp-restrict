This is experimental implementation of restrict keyword from C in standard c++.

Look at example comparison: https://godbolt.org/z/4P7onsdoq

Requirements: C++11

How it works:
<ol>
<li>restrict::ref reminds reference to object and moves object to temporary raw_storage(with it no destructor of temporary will be called)</li>
<li>You modificating temporary using restrict::ref::get and because temporary can`t be aliased there is no extra load</li>
<li>On scope end temporary moves back to object</li>
</ol>
