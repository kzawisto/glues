#ifndef INCLUDE_GLUES_GLUE_H_
#define INCLUDE_GLUES_GLUE_H_

#include <glues/utility.h>
#include <type_traits>
#include <vector>

#include <boost/container/small_vector.hpp>
#include <boost/optional/optional.hpp>
namespace glues {
template <typename T>
using Vector =
    boost::container::small_vector<T, 1, boost::container::new_allocator<T>>;

template <typename T> auto some(const T &t) -> Vector<T> {
  return Vector<T>{t};
}

template <typename T> auto none() -> Vector<T> { return Vector<T>{}; }

template <typename T, typename Op> struct MonadicGlueMap;
template <typename, typename> struct MonadicGlueFilter;
template <typename, typename> struct MonadicGlueFlatMap;

template <typename K> struct Data {
  K begin;
  K end;
};

template <typename ThisGlue, typename T> struct MonadicTrait : public ThisGlue {
  Data<T> data;
  using This = MonadicTrait<ThisGlue, T>;

  template <typename... Args>
  MonadicTrait(const Data<T> &data, Args... args)
      : data(data), ThisGlue(args...) {}

  template <typename Op2, typename K = ThisGlue>
  boost::optional<ReturnTypeBinary<Op2, typename K::output_type>>
  reduce(Op2 op2) {
    using Out = typename ThisGlue::output_type;
    boost::optional<Out> result;
    for (auto it = data.begin; it != data.end; ++it) {
      auto results_prelim = static_cast<ThisGlue *>(this)->execute_monadic(*it);
      for (auto i : results_prelim) {
        if (result) {
          result = op2(result.get(), i);
        } else {
          result = i;
        }
      }
    }
    return result;
  }

  template <typename Container> Container run() {
    Container result;
    for (auto it = data.begin; it != data.end; ++it) {
      auto results_prelim = static_cast<ThisGlue *>(this)->execute_monadic(*it);
      for (auto i : results_prelim) {
        insert_generic(result, i);
      }
    }
    return result;
  }

  template <typename Op2, typename C> C accumulate(Op2 p2, C initial) {
    for (auto it = data.begin; it != data.end; ++it) {
      auto results_prelim = static_cast<ThisGlue *>(this)->execute_monadic(*it);

      for (auto i : results_prelim) {
        initial = p2(initial, i);
      }
    }
    return initial;
  }

  template <typename Op2, typename C> C fold_left(Op2 p2, C initial) {
    return accumulate(p2, initial);
  }

  template <typename Op2, typename C>
  std::vector<typename ThisGlue::output_type> scan_left(Op2 p2, C initial) {
    std::vector<typename ThisGlue::output_type> result;
    for (auto it = data.begin; it != data.end; ++it) {
      auto results_prelim = static_cast<ThisGlue *>(this)->execute_monadic(*it);

      for (auto i : results_prelim) {
        if (result.empty()) {
          result.push_back(p2(initial, i));
        } else {
          result.push_back(p2(result.back(), i));
        }
      }
    }
    return result;
  }

  long count() {
    long result = 0;
    for (auto it = data.begin; it != data.end; ++it) {
      auto results_prelim = static_cast<ThisGlue *>(this)->execute_monadic(*it);
      result += results_prelim.size();
    }
    return result;
  }

  template <typename Op2>
  MonadicTrait<MonadicGlueMap<This, Op2>, T> map(Op2 op2) {
    return MonadicTrait<MonadicGlueMap<This, Op2>, T>(data, op2, *this);
  }

  template <typename Op2>
  MonadicTrait<MonadicGlueFilter<This, Op2>, T> filter(Op2 op2) {
    return MonadicTrait<MonadicGlueFilter<This, Op2>, T>(data, op2, *this);
  }

  template <typename Op2>
  MonadicTrait<MonadicGlueFlatMap<This, Op2>, T> flat_map(Op2 op2) {
    return MonadicTrait<MonadicGlueFlatMap<This, Op2>, T>(data, op2, *this);
  }
};
template <typename T> struct MonadicGlueTrivial {
  MonadicGlueTrivial() {}
  typedef T input_type;
  typedef T output_type;
  static Vector<output_type> execute_monadic(input_type k) {
    return Vector<output_type>{k};
  }
};

template <typename T>
MonadicTrait<MonadicGlueTrivial<typename std::remove_reference<
                 decltype(*std::declval<T>())>::type>,
             T>
mon(T begin, T end) {
  return MonadicTrait<MonadicGlueTrivial<typename std::remove_reference<
                          decltype(*std::declval<T>())>::type>,
                      T>(Data<T>{begin, end});
}

template <typename PrevGlue, typename Op> struct MonadicGlueMap {
  typedef typename PrevGlue::input_type input_type;
  typedef typename PrevGlue::output_type this_input_type;
  typedef ReturnType<Op, this_input_type> output_type;
  PrevGlue prev;
  Op op;

  MonadicGlueMap(Op op, PrevGlue prev) : op(op), prev(prev) {}

  Vector<output_type> execute_monadic(input_type k) {
    Vector<output_type> out;
    for (auto a : prev.execute_monadic(k)) {
      auto b = op(a);
      out.push_back(b);
    }
    return out;
  }
};

template <typename PrevGlue, typename Op> struct MonadicGlueFilter {
  typedef typename PrevGlue::input_type input_type;
  typedef typename PrevGlue::output_type this_input_type;
  typedef this_input_type output_type;
  PrevGlue prev;
  Op op;

  MonadicGlueFilter(Op op, PrevGlue prev) : op(op), prev(prev) {}

  Vector<output_type> execute_monadic(input_type k) {
    Vector<output_type> out;
    for (auto a : prev.execute_monadic(k)) {
      if (op(a)) {
        out.push_back(a);
      }
    }
    return out;
  }
};

template <typename PrevGlue, typename Op> struct MonadicGlueFlatMap {
  typedef typename PrevGlue::input_type input_type;
  typedef typename PrevGlue::output_type this_input_type;
  typedef
      typename container_val<ReturnType<Op, this_input_type>>::type output_type;
  PrevGlue prev;
  Op op;

  MonadicGlueFlatMap(Op op, PrevGlue prev) : op(op), prev(prev) {}
  Vector<output_type> execute_monadic(input_type k) {
    Vector<output_type> out;
    for (auto a : prev.execute_monadic(k)) {
      for (auto b : op(a)) {
        out.push_back(b);
      }
    }
    return out;
  }
};

}

#endif /* INCLUDE_GLUES_GLUE_H_ */
