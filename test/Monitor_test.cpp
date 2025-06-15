#include "Monitor.hpp"

#include <gtest/gtest.h>

#include <future>
#include <thread>

using mem::Monitor;

TEST(SingleThreaded, Resource_ValueIsPersistent) {
  constexpr int set_value{1};
  Monitor<int> monitor{};

  monitor.acquire().execute([&](int &value) { value = set_value; });

  monitor.acquire().check(
      [&](const int &value) { EXPECT_EQ(value, set_value); });
}

TEST(SingleThreaded, ChainingMethods_ThenModificationsAreSequential) {
  constexpr int expected{10};
  Monitor<int> monitor{};

  monitor.acquire()
      .then([](int &v) { v = 8; })
      .then([](int &v) { v /= 2; })
      .then([](int &v) { v += 6; })  // if sequential v=8/2+6=10
      .release();

  monitor.acquire().check(
      [&](const int &actual) { EXPECT_EQ(actual, expected); });
}

TEST(SignleThreaded, ChainingMethods_MixedChainedModificationsAreSequential) {
  Monitor<int> monitor{};

  monitor.acquire()
      .then_check([](const int &v) { EXPECT_EQ(v, 0); })
      .then([](int &v) { v += 10; })
      .then_check([](const int &v) { EXPECT_EQ(v, 10); })
      .then([](int &v) { v /= 2; })
      .then([](int &v) { v += 5; })
      .then_check([](const int &v) { EXPECT_EQ(v, 10); })
      .before_release([](int &v) { v -= 10; });

  monitor.acquire().check([](const int &v) { EXPECT_EQ(v, 0); });
}

TEST(SingleThreaded, AccessGuard_MoveTransfersOwnership) {
  using StringMonitor = Monitor<std::string>;
  using AccessGuard = StringMonitor::AccessGuard;

  // --- SETUP ---
  StringMonitor monitor{"test"};

  // --- ACTION ---
  AccessGuard old_guard{monitor.acquire()};
  AccessGuard new_guard{std::move(old_guard)};

  // --- ASSERT ---
  EXPECT_FALSE(old_guard.owns_resource());
  EXPECT_TRUE(new_guard.owns_resource());
}

TEST(SingleThreaded, AccessGuard_FunctionReturnTransfersOwnership) {
  using VecMonitor = Monitor<std::vector<int>>;
  using AccessGuard = VecMonitor::AccessGuard;

  // --- SETUP ---
  VecMonitor monitor{};
  constexpr int expected_size{1};
  constexpr int expected_value{1};
  auto return_guard = [&] -> AccessGuard {
    return monitor.acquire().then(
        [&](std::vector<int> &vec) { vec.push_back(expected_value); });
  };

  // --- ACTION ---
  AccessGuard guard{return_guard()};

  // --- ASSERT ---
  EXPECT_TRUE(guard.owns_resource());
  ASSERT_EQ(guard.resource().size(), expected_size);
  EXPECT_EQ(guard.resource().front(), expected_value);
}

TEST(Constructor, InitializesFromSingleValue) {
  constexpr std::string_view initial_value{"test"};

  Monitor<std::string> monitor{initial_value};

  monitor.acquire().check(
      [&](const std::string &value) { EXPECT_EQ(value, initial_value); });
}

TEST(Constructor, InitializesFromMultipleArgs) {
  constexpr std::array<int, 3> expected_value{1, 2, 3};

  Monitor<std::array<int, 3>> monitor{1, 2, 3};

  monitor.acquire().check([&](const std::array<int, 3> &value) {
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

TEST(MultiThreaded, AcquireWhen_BlocksUntilNotified) {
  using namespace std::chrono_literals;
  using IntegerMonitor = Monitor<int>;
  using NotifyPolicy = IntegerMonitor::AccessGuard::NotifyPolicy;

  // --- SETUP ---
  std::promise<bool> listener_completed_promise;
  std::future<bool> listener_completed_future{
      listener_completed_promise.get_future()};
  IntegerMonitor monitor{NotifyPolicy::notify_one};

  // -- ACTION --
  const std::jthread listener{[&] {
    monitor.acquire_when([](const int &value) { return value == 1; })
        .then_check([](const int &value) { EXPECT_EQ(value, 1); })
        .execute([&](int &) { listener_completed_promise.set_value(true); });
  }};

  std::this_thread::sleep_for(50ms);

  monitor.acquire().execute([](int &val) { val = 1; });

  // -- ASSERT --
  const std::future_status listener_future_status{
      listener_completed_future.wait_for(1s)};

  ASSERT_EQ(listener_future_status, std::future_status::ready)
      << "Test timed out: reader thread did not complete. "
      << "Likely reader thread was never notified by the writer thread.";

  EXPECT_TRUE(listener_completed_future.get());
}

TEST(MultiThreaded, NotifyPolicyNone_DoesNotWakeWaitingThread) {
  using namespace std::chrono_literals;
  using IntegerMonitor = Monitor<int>;
  using NotifyPolicy = IntegerMonitor::AccessGuard::NotifyPolicy;

  // --- SETUP ---
  std::promise<void> listener_woke_up_promise;
  std::future<void> listener_woke_up_future{
      listener_woke_up_promise.get_future()};

  IntegerMonitor monitor{NotifyPolicy::none};

  // --- ACTION ---
  const std::jthread listener{[&] {
    monitor.acquire_when([](const int &value) { return value == 1; })
        .execute([&](auto &) { listener_woke_up_promise.set_value(); });
  }};

  std::this_thread::sleep_for(50ms);

  monitor.acquire().execute([](int &value) { value = 1; });

  // --- ASSERT ---
  const std::future_status listener_future_status{
      listener_woke_up_future.wait_for(100ms)};

  ASSERT_EQ(listener_future_status, std::future_status::timeout)
      << "Listener thread woke up when should not have with "
         "NotifyPolicy::none.";

  // --- CLEANUP ---
  monitor.acquire(NotifyPolicy::notify_one).release();
}