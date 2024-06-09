#pragma once

#include <type_traits>

namespace tl {

template <typename... Types>
struct type_list;

// Contains
template <typename Type, typename List>
struct contains_impl;

template <template <typename...> typename List, typename... Types, typename Type>
struct contains_impl<Type, List<Types...>> {
  static constexpr bool value = (std::is_same_v<Type, Types> || ...);
};

template <typename T, typename List>
static constexpr bool contains = contains_impl<T, List>::value;

// Flip

template <typename List>
struct simple_flip_impl;

template <template <typename...> typename List, typename X, typename Y>
struct simple_flip_impl<List<X, Y>> {
  using type = List<Y, X>;
};

template <template <typename...> typename List>
struct simple_flip_impl<List<>> {
  using type = List<>;
};

template <typename List>
using simple_flip = simple_flip_impl<List>::type;

template <typename List>
struct flip_impl;

template <template <typename...> typename List, typename... Lists>
struct flip_impl<List<Lists...>> {
  using type = List<simple_flip<Lists>...>;
};

template <template <typename...> typename List>
struct flip_impl<List<>> {
  using type = List<>;
};

template <typename List>
using flip = flip_impl<List>::type;

// index_of_unique
template <typename Type, typename List>
struct index_of_unique_impl;

template <typename Type, template <typename...> typename List, typename... Rest>
struct index_of_unique_impl<Type, List<Type, Rest...>> {
  static constexpr std::size_t value = 0;
};

template <typename Type, template <typename...> typename List, typename Head, typename... Rest>
struct index_of_unique_impl<Type, List<Head, Rest...>> {
  static constexpr std::size_t value = 1 + index_of_unique_impl<Type, List<Rest...>>::value;
};

template <typename Type, typename List>
static constexpr std::size_t index_of_unique = index_of_unique_impl<Type, List>::value;

// concat

template <typename T>
struct identity_impl {
  using type = T;
};

template <typename T>
using identity = identity_impl<T>;

template <
    template <typename...>
    typename List1,
    template <typename...>
    typename List2,
    typename... Types1,
    typename... Types2>
identity<List1<Types1..., Types2...>> operator+(identity<List1<Types1...>>, identity<List2<Types2...>>);

template <typename... Lists>
using concat_fast = typename decltype((std::declval<identity<Lists>>() + ... + identity<type_list<>>{}))::type;

// flatten

template <typename List>
struct flatten2_impl {
  using type = type_list<List>;
};

template <template <typename...> typename List1, typename... Types>
struct flatten2_impl<List1<Types...>> {
  using type = concat_fast<typename flatten2_impl<Types>::type...>;
};

template <typename List>
struct unpack;

template <typename... Types>
struct unpack<type_list<Types...>> {
  template <template <typename...> typename List>
  using type = List<Types...>;
};

template <typename List>
struct flatten_impl;

template <template <typename...> typename List, typename... Types>
struct flatten_impl<List<Types...>> {
  using type = typename unpack<typename flatten2_impl<List<Types...>>::type>::template type<List>;
};

template <typename List>
using flatten = typename flatten_impl<List>::type;

// merge-sort

template <typename List1, typename List2, typename List3, template <typename...> typename Comparator>
struct concat_by_comparator_impl;

template <
    template <typename...>
    typename List1,
    typename... Types1,
    template <typename...>
    typename List2,
    typename Head2,
    typename... Types2,
    template <typename...>
    typename List3,
    typename Head3,
    typename... Types3,
    template <typename...>
    typename Comparator>
struct concat_by_comparator_impl<List1<Types1...>, List2<Head2, Types2...>, List3<Head3, Types3...>, Comparator> {
  using type = std::conditional_t<
      Comparator<Head2, Head3>::value,
      typename concat_by_comparator_impl<
          List1<Types1..., Head2>,
          List2<Types2...>,
          List3<Head3, Types3...>,
          Comparator>::type,
      typename concat_by_comparator_impl<
          List1<Types1..., Head3>,
          List2<Head2, Types2...>,
          List3<Types3...>,
          Comparator>::type>;
};

template <
    typename List,
    template <typename...>
    typename List2,
    template <typename...>
    typename List3,
    template <typename...>
    typename Comparator>
struct concat_by_comparator_impl<List, List2<>, List3<>, Comparator> {
  using type = List;
};

template <
    template <typename...>
    typename List1,
    typename... Types1,
    template <typename...>
    typename List2,
    template <typename...>
    typename List3,
    typename Head3,
    typename... Types3,
    template <typename...>
    typename Comparator>
struct concat_by_comparator_impl<List1<Types1...>, List2<>, List3<Head3, Types3...>, Comparator> {
  using type = typename concat_by_comparator_impl<List1<Types1..., Head3>, List2<>, List3<Types3...>, Comparator>::type;
};

template <
    template <typename...>
    typename List1,
    typename... Types1,
    template <typename...>
    typename List2,
    typename Head2,
    typename... Types2,
    template <typename...>
    typename List3,
    template <typename...>
    typename Comparator>
struct concat_by_comparator_impl<List1<Types1...>, List2<Head2, Types2...>, List3<>, Comparator> {
  using type = typename concat_by_comparator_impl<List1<Types1..., Head2>, List2<Types2...>, List3<>, Comparator>::type;
};

template <
    template <typename...>
    typename List,
    typename List1,
    typename List2,
    template <typename...>
    typename Comparator>
using concat_by_comparator = typename concat_by_comparator_impl<List<>, List1, List2, Comparator>::type;

template <typename List, template <typename...> typename Comparator>
struct merge_sort_impl;

template <
    template <typename...>
    typename List,
    typename H1,
    typename H2,
    typename... Types,
    template <typename...>
    typename Comparator>
struct merge_sort_impl<List<H1, H2, Types...>, Comparator> {
  using temp_list = List<
      typename concat_by_comparator_impl<List<>, H1, H2, Comparator>::type,
      typename merge_sort_impl<List<Types...>, Comparator>::type>;
  using type = typename merge_sort_impl<temp_list, Comparator>::type;
};

template <template <typename...> typename List, template <typename...> typename Comparator>
struct merge_sort_impl<List<>, Comparator> {
  using type = List<>;
};

template <template <typename...> typename List, typename H, template <typename...> typename Comparator>
struct merge_sort_impl<List<H>, Comparator> {
  using type = H;
};

template <template <typename...> typename List, typename H1, typename H2, template <typename...> typename Comparator>
struct merge_sort_impl<List<H1, H2>, Comparator> {
  using type = typename concat_by_comparator_impl<List<>, H1, H2, Comparator>::type;
};

template <typename T>
struct wrapper;

template <template <typename...> typename List, typename... Types>
struct wrapper<List<Types...>> {
  using type = List<type_list<Types>...>;
};

template <typename List, template <typename...> typename Comparator>
using merge_sort = typename merge_sort_impl<typename wrapper<List>::type, Comparator>::type;

} // namespace tl
