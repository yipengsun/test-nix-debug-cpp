#include <exception>
#include <iostream>
#include <string>
#include <variant>


/////////////////
// definitions //
/////////////////

struct Test {};
using Object = std::variant<int, Test>;

struct TestException : public std::exception {
  TestException(const std::string &message) : message(message) {};
  std::string message;
};


////////////////////////////
// func: Object -> Object //
////////////////////////////

template <class... Ts> struct Collector : Ts... {
  using Ts::operator()...;
};

// to make ccls happy
template <class... Ts> Collector(Ts...) -> Collector<Ts...>;


// for the 5th case, delete this function
Object func(Object n) {
  auto funcInt = [](int) -> int { return 233; };
  auto funcTest = [](Test) -> int {
    throw TestException{"Throw in func"};
  };

  return std::visit(Collector{funcInt, funcTest}, n);
}


/*
  What I've tried so far:

  | idx | status | pre-condition | throw            | catch            | handler       |
  |-----|--------|---------------|------------------|------------------|---------------|
  | 1   | ok     | N/A           | `std::exception` | `std::exception` | `e.what()`    |
  | 2   | not ok | N/A           | `TestException`  | `std::exception` | `e.what()`    |
  | 3   | not ok | N/A           | `TestException`  | `TestException`  | `e.what()`    |
  | 4   | not ok | N/A           | `TestException`  | `TestException`  | `e.message()` |
  | 5   | ok     | delete func   | `TestException`  | `TestException`  | `e.message()` |

  In the 2nd and 3rd case, `Corrupted shared library list 0x0 != 0x6b636f4c5f323178`

  In the 4th case, Segmentation fault with:
  ```
  Program received signal SIGSEGV, Segmentation fault.
  0x00007ffff7fd15ab in _dl_lookup_symbol_x ()
  from /nix/store/3dyw8dzj9ab4m8hv5dpyx7zii8d0w6fi-glibc-2.39-52/lib/ld-linux-x86-64.so.2
  ```

  In all 5 cases, the program runs normally, w/o a debugger attached.
  In all 5 cases, a Windows VS Code session attached with WSL can debug the
  code w/o problem.
*/

int main() {
  try {
    // throw std::exception();
    throw TestException{"Just throw'in"};
    //func();
  // } catch (const std::exception &e) {
  } catch (const TestException &e) {
    // std::cout << "Caught exception: " << e.what() << std::endl;
    std::cout << "Caught exception: " << e.message << std::endl;
  }

  return 0;
}
