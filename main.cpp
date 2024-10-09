#include <iostream>
#include <string>
#include <variant>


/////////////////////
// fib: int -> int //
/////////////////////

int fib(int n) {
  if (n == 0) return 0;
  if (n == 1) return 1;
  return fib(n - 1) + fib(n - 2);
}


//////////////////////////
// definition of Object //
//////////////////////////

struct Test {
  int toNumber() const { return 10; };
  std::string toString() const { return "I'm a test struct"; };
};

std::ostream& operator<<(std::ostream &os, const Test &t) {
  os << t.toString();
  return os;
}


using Object = std::variant<int, double, Test>;

std::ostream& operator<<(std::ostream &os, const Object &n) {
  auto helper = [&os](auto &&arg) -> std::ostream & {
    os << arg;
    return os;
  };

  return std::visit(helper, n);
}


///////////////////////////
// fib: Object -> Object //
///////////////////////////

template <class... Ts> struct Collector : Ts... {
  using Ts::operator()...;
};

// to make ccls happy
template <class... Ts> Collector(Ts...) -> Collector<Ts...>;

Object fib(Object n) {
  auto fibInt = [](int n) -> int {
    return fib(n);
  };
  auto fibDouble = [](double n) -> int {
    return fib(static_cast<int>(n));
  };
  auto fibTest = [](Test n) -> int {
    return fib(n.toNumber());
  };

  return std::visit(Collector{fibInt, fibDouble, fibTest}, n);
}


int main() {
  using namespace std;

  int n = 30;
  cout << "Fibonacci of " << n << " is " << fib(n) << endl;

  Object num{30.13};
  Object result = fib(num);
  cout << "Fibonacci of " << num << " is " << result << endl;

  Object test{Test{}};
  cout << "Fibonacci of " << test << " is " << fib(test) << endl;

  return 0;
}
