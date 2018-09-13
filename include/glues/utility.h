#ifndef INCLUDE_GLUES_UTILITY_H_
#define INCLUDE_GLUES_UTILITY_H_

#include <type_traits>
namespace glues {

template <typename Op, typename Arg>
using ReturnType = decltype(std::declval<Op>()(std::declval<Arg>()));

template <typename Op, typename Arg>
using ReturnTypeBinary =
    decltype(std::declval<Op>()(std::declval<Arg>(), std::declval<Arg>()));

template <typename K> struct container_val {
  typedef
      typename std::remove_reference<decltype(*std::declval<K>().begin())>::type
          type;
};

template <typename K> struct has_push_back {
  typedef typename container_val<K>::type ValT;
  template <typename L>
  static auto constexpr has_it(int)
      -> decltype(std::declval<L>().push_back(std::declval<ValT>()), bool()) {
    return true;
  }
  template <typename L> static auto constexpr has_it(...) -> bool {
    return false;
  }
  static constexpr bool value = has_it<K>(1);
};

template <typename K> struct has_insert {
  typedef typename container_val<K>::type ValT;
  template <typename L>
  static auto constexpr has_it(int)
      -> decltype(std::declval<L>().insert(std::declval<ValT>()), bool()) {
    return true;
  }
  template <typename L> static auto constexpr has_it(...) -> bool {
    return false;
  }
  static constexpr bool value = has_it<K>(1);
};

template <typename K>
auto insert_generic(K &k, const typename container_val<K>::type &elem) ->
    typename std::enable_if<has_push_back<K>::value, void>::type {
  k.push_back(elem);
}

template <typename K>
auto insert_generic(K &k, const typename container_val<K>::type &elem) ->
    typename std::enable_if<!has_push_back<K>::value && has_insert<K>::value,
                            void>::type {
  k.insert(elem);
}
}
#endif /* INCLUDE_GLUES_UTILITY_H_ */
