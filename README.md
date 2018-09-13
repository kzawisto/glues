
# Glues 
## Low overhead Scala/FP like interface for C++ containers. 
No indirect calls, no unneeded objects, compile time code generation.

```c++
  std::vector<int> v{0, 1, 2, 3, 4, 5};
  auto cumulated_product = mon(v.begin(), v.end())
                    .map([](int i) { return i + 1; }) // lazy evaluation
                    .scan_left([](double a, int b) { return a * b; }, 1.0); // sort of cumsum function as in scala
  // result is std::vector<int>{1, 2, 6, 24, 120, 720};

  auto result = glues::mon(v.begin(), v.end())
                    .map([](int i){return i * i;}) // lazy
                    .filter([](int i) {return i < 10;}) // lazy
                    .accumulate([](std::string s, // computation here, one loop
                                   int a) { return s + std::to_string(a); },
                                std::string("foo")); // 
  // result is foo149

    std::vector<double> numbers{10, -1, 1000, 100};
      auto logarithms = glues::mon(numbers.begin(), numbers.end()).flat_map([](double i) {
          return i > 0 ?
              glues::some(log10(i)) : glues::none<double>(); // any iterable can be used here as return type
      }).run<std::deque<double>>(); // trigger execution and create a deque<double> for result
  // result is  std::deque<double> {1.0, 3.0, 2.0};

  // see unit tests for more examples
```
## Requirements:

C++11 or greater needed, Boost::container::small_vector, Boost::optional (tested with G++ 6.2, Boost 1.62).
Header only library.

 

## Installation

Header-only library, download from git and copy includes to desired directory.
```bash
git clone http://github.com/kzawisto/glues
cd glues
sudo rm -r /usr/include/glues
sudo cp include/glues /usr/include
```

## Tests

Clone repo, checkout desired version.
Then:
```
cd glues/test
make 
./test
```
GoogleTest (GTest) v. 1.8 is needed for test
