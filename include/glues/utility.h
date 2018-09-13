#ifndef INCLUDE_GLUES_UTILITY_H_
#define INCLUDE_GLUES_UTILITY_H_


#include<type_traits>

template<typename Op, typename Arg>
using ReturnType = decltype(std::declval<Op>()(std::declval<Arg>()));

template<typename Op, typename Arg>
using ReturnTypeBinary = decltype(std::declval<Op>()(
		std::declval<Arg>(),
		std::declval<Arg>()
		));


#endif /* INCLUDE_GLUES_UTILITY_H_ */
