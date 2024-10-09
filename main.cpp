#include <exception>
#include <iostream>
#include <string>
#include <variant>


//////////////////////////
// definition of Object //
//////////////////////////

struct Test {};
using Object = std::variant<int, Test>;


////////////////////////////
// func: Object -> Object //
////////////////////////////

template <class... Ts> struct Collector : Ts... {
  using Ts::operator()...;
};

// to make ccls happy
template <class... Ts> Collector(Ts...) -> Collector<Ts...>;


struct TestException : public std::exception {
  TestException(const std::string &message) : message(message) {};
  std::string message;
};


Object func(Object n) {
  auto funcInt = [](int) -> int { return 233; };
  auto funcTest = [](Test) -> int {
    throw TestException{"'Test' struct has no business doing fib!"};
  };

  return std::visit(Collector{funcInt, funcTest}, n);
}


int main() {
  using namespace std;

  Object test{Test{}};
  try {
    func(test);
  } catch (const TestException &e) {
    cout << "Caught exception: " << e.message << endl;
  }

  return 0;
}
