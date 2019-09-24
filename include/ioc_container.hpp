#ifndef _IOC_CONTAINER_HPP_
#define _IOC_CONTAINER_HPP_

#include <unordered_map>
#include <typeindex>
#include <memory>

namespace IoC {

// Dependency Injection Item.
struct DiItem {
  std::type_info const* type;
  std::shared_ptr<void> value;

  DiItem() : type(&typeid(nullptr)) {}

  template<class T>
  DiItem(std::shared_ptr<T> value) : type(&typeid(T)), value(value) {}
};

class Container {
  using FactoryFn = std::function<DiItem()>;
  typedef std::unordered_map<std::type_index, FactoryFn> ObjectMap;
  typedef std::unordered_map<std::type_index, DiItem> SingletonMap;
  ObjectMap items_;
  SingletonMap singletons_;

 public:
  template<class T>
  std::shared_ptr<T> Resolve() {
    auto it = items_.find(typeid(T));
    if (it == items_.end()) return nullptr;

    DiItem item = it->second();
    if (typeid(T) != *item.type) return nullptr;
    return std::static_pointer_cast<T>(item.value);
  }

  template<class T, class I, class ...Args>
  FactoryFn CreateFactory() {
    return [=]() {
      std::shared_ptr<I> i = std::make_shared<T>(Resolve<Args>()...);
      return DiItem(i);
    };
  }

  template<class T, class I, class ...Args>
  void RegisterType() {
    std::type_index type = std::type_index(typeid(I));
    items_.erase(type);
    items_.insert(make_pair(type, CreateFactory<T, I, Args...>()));
  }

  template<class T, class I, class ...Args>
  void RegisterInstance() {
    std::type_index type = std::type_index(typeid(I));
    items_.erase(type);

    singletons_.insert(make_pair(type, CreateFactory<T, I, Args...>()()));
    auto factory = [=]() {
      auto it = singletons_.find(typeid(T));
      if (it == singletons_.end()) throw;

      DiItem item = it->second;
      if (typeid(T) != *item.type) throw;
      return item;
    };
    items_.insert(make_pair(type, factory));
  }

  static Container& Get() {
    static auto container = std::make_shared<Container>();
    return *container;
  }
};

} // End of namespace.

#endif
