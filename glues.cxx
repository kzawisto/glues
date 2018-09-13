//
//This is shared under MIT-LICENSE
//K. Zawistowski, 2018
//
#include<bits/stdc++.h>

template<typename K>
struct container_val {

    typedef typename std::remove_reference<decltype(*std::declval<K>().begin())>::type type;
};
template<typename K>
struct has_push_back{
    typedef typename container_val<K>::type ValT;
    template<typename L>
    static auto constexpr has_it(int) -> decltype( std::declval<L>().push_back(std::declval<ValT>()), bool()) {
        return true;
    }
    template<typename L>
    static auto constexpr has_it(...) -> bool {
        return false;
    }
    static constexpr bool value = has_it<K>(1);
};
template<typename K>
struct has_insert{
    typedef typename container_val<K>::type ValT;
    template<typename L>
    static auto  constexpr has_it(int) ->decltype( std::declval<L>().insert(std::declval<ValT>()), bool()) {
        return true;
    }
    template<typename L>
    static auto constexpr has_it(...) -> bool {
        return false;
    }
    static constexpr bool value = has_it<K>(1);
};



template<typename K>
auto insert_generic(K & k, const typename container_val<K>::type & elem) -> typename std::enable_if<
    has_push_back<K>::value, void>::type {
        k.push_back(elem);
}
template<typename K>
auto insert_generic(K & k, const typename container_val<K>::type & elem) -> typename std::enable_if<
    !has_push_back<K>::value && has_insert<K>::value, void>::type {
        k.insert(elem);
}

template<typename PrevGlue, typename Op>
struct Glue  {
    
    typedef typename PrevGlue::input_type input_type;
    typedef typename PrevGlue::iter_type iter_type;
    iter_type begin,  end;
    Op op;
    PrevGlue prev;
    typedef Glue<PrevGlue, Op> ThisT;
    typedef decltype(
            std::declval<Op>()(std::declval<input_type>())) output_type;
    output_type execute(input_type k) {
        return op(prev.execute(k));
    }
    template<typename Op2>
    Glue<ThisT, Op2> map( Op2 op2) {
        return Glue<ThisT, Op2> {begin, end, op2, *this};
    }

    template<typename Container = std::vector<output_type>>
    Container run()  {
        Container ret;
        for(auto k = begin; k!= end; k++) {
            insert_generic(ret, execute(*k));
        }
        return ret;
    }
};

template<typename K, typename T = decltype(*std::declval<K>())>
struct GlueTrivial {
    K begin; K end;
    typedef K iter_type;
    typedef T input_type;
    typedef T output_type;
    typedef GlueTrivial<K, T> ThisT;
    output_type execute(input_type k) {
        return k;
    }
    template<typename Op2>
    Glue<ThisT, Op2> map( Op2 op2) {
        return Glue<ThisT, Op2> {begin, end, op2, *this};
    }
};
template<typename T>
GlueTrivial<T, decltype(*std::declval<T>())> glue(T begin, T end) {
    return GlueTrivial<T, decltype(*std::declval<T>())>{begin, end};
}


int main() {
    std::vector<int> vec {1,2,3,4,5,6, 2, 4, 5, 6 ,1};
    auto vec3 = glue(vec.begin(), vec.end())
        .map( [] ( int i) -> int { return i *2;})
        .map( [] ( int i) -> double { return i +2.5;})
        .map( [] ( double i) -> std::string {
                std::stringstream ss;
                ss << i;
                return ss.str() + " kg";
        });
		auto vec2 = vec3
        .run<std::set<std::string>>();

    for(auto a: vec2) {
        std::cout<<a<<" "; 
    }
}
