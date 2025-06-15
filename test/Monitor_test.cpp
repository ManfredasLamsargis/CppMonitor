#include "Monitor.hpp"

#include <gtest/gtest.h>

using mem::Monitor;

TEST(SingleThreaded, ResourceValueIsPersistent) {
  constexpr int set_value{1};
  Monitor<int> monitor{};

  monitor.lock().execute([&](int &value) { value = set_value; });

  monitor.lock().check([&](const int &value) { EXPECT_EQ(value, set_value); });
}

TEST(SingleThreaded, ChainingMethods_ThenModificationsAreSequential) {
  constexpr int expected{10};
  Monitor<int> monitor{};

  monitor.lock()
      .then([](int &v) { v = 8; })
      .then([](int &v) { v /= 2; })
      .then([](int &v) { v += 6; })  // if sequential v=8/2+6=10
      .release();

  monitor.lock().check([&](const int &actual) { EXPECT_EQ(actual, expected); });
}

TEST(SignleThreaded, ChainingMethods_MixedChainedModificationsAreSequential) {
  Monitor<int> monitor{};

  monitor.lock()
      .then_check([](const int &v) { EXPECT_EQ(v, 0); })
      .then([](int &v) { v += 10; })
      .then_check([](const int &v) { EXPECT_EQ(v, 10); })
      .then([](int &v) { v /= 2; })
      .then([](int &v) { v += 5; })
      .then_check([](const int &v) { EXPECT_EQ(v, 10); })
      .before_release([](int &v) { v -= 10; });

  monitor.lock().check([](const int &v) { EXPECT_EQ(v, 0); });
}

TEST(Constructor, InitializesFromSingleValue) {
  constexpr std::string_view initial_value{"test"};

  Monitor<std::string> monitor{initial_value};

  monitor.lock().check(
      [&](const std::string &value) { EXPECT_EQ(value, initial_value); });
}

TEST(Constructor, InitializesFromMultipleArgs) {
  constexpr std::array<int, 3> expected_value{1, 2, 3};

  Monitor<std::array<int, 3>> monitor{1, 2, 3};

  monitor.lock().check([&](const std::array<int, 3> &value) {
    EXPECT_EQ(value, expected_value);
  });
}

TEST(Constructor, InitializesWithDefaultNotifyPolicy) {
  constexpr auto expected_policy{
      Monitor<int>::AccessGuard::NotifyPolicy::notify_all};

  const Monitor<int> monitor{};

  EXPECT_EQ(monitor.default_notify_policy(), expected_policy);
}

TEST(Constructor, InitializesWithProvidedDefaultPolicy) {
  constexpr auto expected_policy{Monitor<int>::AccessGuard::NotifyPolicy::none};

  const Monitor<int> monitor{expected_policy};

  EXPECT_EQ(monitor.default_notify_policy(), expected_policy);
}