#include "bootstrapper.hpp"

int main() {
  Bootstrapper::Bootstrap();
  static IoC::Container& container = IoC::Container::Get();
  container.Resolve<Engine>()->Run();
  return 0;
}
