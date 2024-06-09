#include "typelist.h"

#include <catch2/catch_test_macros.hpp>

#include <tuple>

namespace tl_test {

TEST_CASE("contains") {
  STATIC_REQUIRE(tl::contains<int, tl::type_list<int, double, float>>);
  STATIC_REQUIRE(tl::contains<double, tl::type_list<int, double, float>>);
  STATIC_REQUIRE(tl::contains<float, tl::type_list<int, double, float>>);
  STATIC_REQUIRE(tl::contains<int, tl::type_list<int>>);
  STATIC_REQUIRE(tl::contains<int, tl::type_list<int, int>>);

  STATIC_REQUIRE(!tl::contains<char, tl::type_list<int, double, float>>);
  STATIC_REQUIRE(!tl::contains<char, tl::type_list<int>>);
  STATIC_REQUIRE(!tl::contains<char, tl::type_list<>>);
  STATIC_REQUIRE(!tl::contains<char, tl::type_list<int, int>>);

  STATIC_REQUIRE(tl::contains<int, std::tuple<int, double, float>>);
  STATIC_REQUIRE(tl::contains<float, std::tuple<int, double, float>>);
  STATIC_REQUIRE(tl::contains<int, std::tuple<int>>);
  STATIC_REQUIRE(tl::contains<int, std::tuple<int, int>>);

  STATIC_REQUIRE(!tl::contains<char, std::tuple<int, double, float>>);
  STATIC_REQUIRE(!tl::contains<char, std::tuple<int>>);
  STATIC_REQUIRE(!tl::contains<char, std::tuple<>>);
  STATIC_REQUIRE(!tl::contains<char, std::tuple<int, int>>);
}

TEST_CASE("flip") {
  STATIC_REQUIRE(std::is_same_v<tl::flip<tl::type_list<>>, tl::type_list<>>);
  STATIC_REQUIRE(std::is_same_v<tl::flip<std::tuple<>>, std::tuple<>>);

  using l11 = tl::type_list<
      tl::type_list<int, double>,
      tl::type_list<double, int>,
      tl::type_list<int, float>,
      tl::type_list<char, void>>;

  using l12 = tl::type_list<
      tl::type_list<double, int>,
      tl::type_list<int, double>,
      tl::type_list<float, int>,
      tl::type_list<void, char>>;

  STATIC_REQUIRE(std::is_same_v<tl::flip<l11>, l12>);
  STATIC_REQUIRE(std::is_same_v<tl::flip<l12>, l11>);

  // clang-format off
  using l21 = std::tuple<
      tl::type_list<int, double>,
      tl::type_list<double, int>,
      std::tuple<int, float>,
      tl::type_list<char, void>>;

  using l22 = std::tuple<
      tl::type_list<double, int>,
      tl::type_list<int, double>,
      std::tuple<float, int>,
      tl::type_list<void, char>>;
  // clang-format on

  STATIC_REQUIRE(std::is_same_v<tl::flip<l21>, l22>);
  STATIC_REQUIRE(std::is_same_v<tl::flip<l22>, l21>);

  using l31 = std::pair<std::pair<int, double>, std::pair<long, float>>;
  using l32 = std::pair<std::pair<double, int>, std::pair<float, long>>;

  STATIC_REQUIRE(std::is_same_v<tl::flip<l31>, l32>);
  STATIC_REQUIRE(std::is_same_v<tl::flip<l32>, l31>);
}

TEST_CASE("index_of_unique") {
  STATIC_REQUIRE(tl::index_of_unique<int, tl::type_list<int, double, float>> == 0);
  STATIC_REQUIRE(tl::index_of_unique<double, tl::type_list<int, double, float>> == 1);
  STATIC_REQUIRE(tl::index_of_unique<float, tl::type_list<int, double, float>> == 2);
  STATIC_REQUIRE(tl::index_of_unique<int, tl::type_list<int>> == 0);

  STATIC_REQUIRE(tl::index_of_unique<int, std::tuple<int, double, float>> == 0);
  STATIC_REQUIRE(tl::index_of_unique<double, std::tuple<int, double, float>> == 1);
  STATIC_REQUIRE(tl::index_of_unique<float, std::tuple<int, double, float>> == 2);
  STATIC_REQUIRE(tl::index_of_unique<int, std::tuple<int>> == 0);
}

TEST_CASE("flatten") {
  STATIC_REQUIRE(std::is_same_v<tl::flatten<tl::type_list<int, double, float>>, tl::type_list<int, double, float>>);
  STATIC_REQUIRE(std::is_same_v<
                 tl::flatten<tl::type_list<tl::type_list<int>, tl::type_list<double>, tl::type_list<float>>>,
                 tl::type_list<int, double, float>>);
  STATIC_REQUIRE(std::is_same_v<
                 tl::flatten<tl::type_list<int, tl::type_list<double>, tl::type_list<float>>>,
                 tl::type_list<int, double, float>>);
  STATIC_REQUIRE(std::is_same_v<
                 tl::flatten<tl::type_list<tl::type_list<int, tl::type_list<double>>, float>>,
                 tl::type_list<int, double, float>>);
  STATIC_REQUIRE(std::is_same_v<tl::flatten<tl::type_list<>>, tl::type_list<>>);
  STATIC_REQUIRE(std::is_same_v<tl::flatten<tl::type_list<tl::type_list<tl::type_list<>>>>, tl::type_list<>>);

  STATIC_REQUIRE(std::is_same_v<tl::flatten<std::tuple<int, double, float>>, std::tuple<int, double, float>>);
  STATIC_REQUIRE(std::is_same_v<
                 tl::flatten<std::tuple<std::tuple<int>, std::tuple<double>, std::tuple<float>>>,
                 std::tuple<int, double, float>>);
  STATIC_REQUIRE(std::is_same_v<
                 tl::flatten<std::tuple<tl::type_list<int>, tl::type_list<double>, tl::type_list<float>>>,
                 std::tuple<int, double, float>>);
  STATIC_REQUIRE(std::is_same_v<
                 tl::flatten<tl::type_list<std::tuple<int>, double, std::tuple<float>>>,
                 tl::type_list<int, double, float>>);
}

template <int N>
struct int_wrapper {};

template <template <typename...> typename List, int... Ns>
using make_int_list = List<int_wrapper<Ns>...>;

template <typename Lhs, typename Rhs>
struct less_cmp;

template <int Lhs, int Rhs>
struct less_cmp<int_wrapper<Lhs>, int_wrapper<Rhs>> {
  static constexpr bool value = Lhs < Rhs;
};

template <typename Lhs, typename Rhs, typename SomeParam = void>
struct mod10_greater_cmp;

template <int Lhs, int Rhs>
struct mod10_greater_cmp<int_wrapper<Lhs>, int_wrapper<Rhs>> {
  static constexpr bool value = Lhs % 10 > Rhs % 10;
};

TEST_CASE("merge_sort") {
  STATIC_REQUIRE(std::is_same_v<tl::merge_sort<tl::type_list<>, less_cmp>, tl::type_list<>>);
  STATIC_REQUIRE(std::is_same_v<tl::merge_sort<std::tuple<>, less_cmp>, std::tuple<>>);

  using l1 = make_int_list<tl::type_list, 42>;
  STATIC_REQUIRE(std::is_same_v<tl::merge_sort<l1, less_cmp>, l1>);

  using l2 = make_int_list<tl::type_list, 42, 42>;
  STATIC_REQUIRE(std::is_same_v<tl::merge_sort<l2, less_cmp>, l2>);

  static_assert(std::is_same_v<tl::concat_by_comparator<
    tl::type_list,
    make_int_list<tl::type_list, 1>,
    make_int_list<tl::type_list, 0>,
    less_cmp>,
    make_int_list<tl::type_list, 0, 1>
    >);

  static_assert(std::is_same_v<tl::concat_by_comparator<
    tl::type_list,
    make_int_list<tl::type_list, 1, 2, 3>,
    make_int_list<tl::type_list, 0>,
    less_cmp>,
    make_int_list<tl::type_list, 0, 1, 2, 3>
    >);

  using l3 = make_int_list<tl::type_list, 1, 0>;
  using l3_sorted = make_int_list<tl::type_list, 0, 1>;
  STATIC_REQUIRE(std::is_same_v<tl::merge_sort<l3_sorted, less_cmp>, l3_sorted>);
  STATIC_REQUIRE(std::is_same_v<tl::merge_sort<l3, less_cmp>, l3_sorted>);

  using l4 = make_int_list<tl::type_list, 2, 1, 3>;
  using l4_sorted = make_int_list<tl::type_list, 1, 2, 3>;
  STATIC_REQUIRE(std::is_same_v<tl::merge_sort<l4_sorted, less_cmp>, l4_sorted>);
  STATIC_REQUIRE(std::is_same_v<tl::merge_sort<l4, less_cmp>, l4_sorted>);

  using l5 = make_int_list<std::tuple, 1, 3, 4, 2>;
  using l5_sorted = make_int_list<std::tuple, 1, 2, 3, 4>;
  STATIC_REQUIRE(std::is_same_v<tl::merge_sort<l5_sorted, less_cmp>, l5_sorted>);
  STATIC_REQUIRE(std::is_same_v<tl::merge_sort<l5, less_cmp>, l5_sorted>);

  using l6 = make_int_list<std::tuple, 5, 4, 3, 2, 1>;
  using l6_sorted = make_int_list<std::tuple, 1, 2, 3, 4, 5>;
  STATIC_REQUIRE(std::is_same_v<tl::merge_sort<l6_sorted, less_cmp>, l6_sorted>);
  STATIC_REQUIRE(std::is_same_v<tl::merge_sort<l6, less_cmp>, l6_sorted>);

  using l7 = make_int_list<tl::type_list, 3, 7, 2, 9, 5>;
  using l7_sorted = make_int_list<tl::type_list, 2, 3, 5, 7, 9>;
  STATIC_REQUIRE(std::is_same_v<tl::merge_sort<l7_sorted, less_cmp>, l7_sorted>);
  STATIC_REQUIRE(std::is_same_v<tl::merge_sort<l7, less_cmp>, l7_sorted>);

  using l8 = make_int_list<tl::type_list, 2, 11, 7, 14, 18, 13, 6, 1, 1, 8, 5, 6, 8, 14, 15, 1>;
  using l8_sorted = make_int_list<tl::type_list, 1, 1, 1, 2, 5, 6, 6, 7, 8, 8, 11, 13, 14, 14, 15, 18>;
  STATIC_REQUIRE(std::is_same_v<tl::merge_sort<l8_sorted, less_cmp>, l8_sorted>);
  STATIC_REQUIRE(std::is_same_v<tl::merge_sort<l8, less_cmp>, l8_sorted>);

  using l9 = make_int_list<tl::type_list, 12, 7, 14, 19, 13, 6, 1, 1, 8, 5, 6, 8, 14, 1>;
  using l9_sorted = make_int_list<tl::type_list, 19, 8, 8, 7, 6, 6, 5, 14, 14, 13, 12, 1, 1, 1>;
  STATIC_REQUIRE(std::is_same_v<tl::merge_sort<l9_sorted, mod10_greater_cmp>, l9_sorted>);
  STATIC_REQUIRE(std::is_same_v<tl::merge_sort<l9, mod10_greater_cmp>, l9_sorted>);
}

} // namespace tl_test
