#include "calc/calc.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <random>

using ::testing::ElementsAre;
using ::testing::ElementsAreArray;

template <typename T> void print(const T &vec) {
    for (const auto &elem : vec) {
        std::cout << elem << ", ";
    }
    std::cout << std::endl;
}

using DedupClasses = ::testing::Types<UseSet, UseRuntimeConfigurableSet>;
class NameGenerator {
 public:
  template <typename T>
  static std::string GetName(int) {
    if constexpr (std::is_same_v<T, UseSet>) return "UseSet";
    if constexpr (std::is_same_v<T, UseRuntimeConfigurableSet>) return "UseRuntimeConfigurableSet";
  }
};

template <typename T>
class DeduplicateTest : public ::testing::Test {};

TYPED_TEST_SUITE(DeduplicateTest, DedupClasses, NameGenerator);

TYPED_TEST(DeduplicateTest, Works) {
    std::vector<std::string> input = {"John",  "Paul", "Ringo",  "Paul", "Paul",
                                      "Ringo", "John", "George", "Paul"};

    auto ret = TypeParam::dedupe(input);

    EXPECT_THAT(ret, ElementsAre("Ringo", "John", "George", "Paul"));
    print(ret);
}