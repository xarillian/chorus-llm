#pragma once

#include "chorus/runtime/runtime_loading.hpp"

#include <cstddef>
#include <limits>
#include <mutex>

namespace Chorus {

struct ChorusRuntimeTestHooks {
    static void fail_next_load_setup(ChorusRuntime& runtime) {
        runtime.assert_host_thread();
        runtime._test_fail_next_load_setup = true;
    }

    static void fail_next_worker_start(ChorusRuntime& runtime) {
        runtime.assert_host_thread();
        runtime._test_fail_next_worker_start = true;
    }

    static void fail_next_preparation_worker_start(ChorusRuntime& runtime) {
        runtime.assert_host_thread();
        runtime._test_fail_next_preparation_worker_start = true;
    }

    static void use_preparation_workers(ChorusRuntime& runtime, size_t count) {
        runtime.assert_host_thread();
        runtime._test_preparation_workers = count;
    }

    static bool load_parked(const ChorusRuntime& runtime, LoadId id) {
        runtime.assert_host_thread();
        if (!runtime._loading)
            return false;
        std::lock_guard lock(runtime._loading->mutex);
        const auto& attempt = runtime._loading->attempt;
        return attempt && attempt->id == id && attempt->parked && !attempt->committed;
    }

    static bool load_committed(const ChorusRuntime& runtime, LoadId id) {
        runtime.assert_host_thread();
        if (!runtime._loading)
            return false;
        std::lock_guard lock(runtime._loading->mutex);
        const auto& attempt = runtime._loading->attempt;
        return attempt && attempt->id == id && attempt->committed;
    }

    static void exhaust_load_ids(ChorusRuntime& runtime) {
        runtime.assert_host_thread();
        runtime._next_load_id = std::numeric_limits<LoadId>::max();
    }

    static void hold_next_retirement(ChorusRuntime& runtime) {
        runtime.assert_host_thread();
        runtime._test_hold_next_retirement = true;
    }

    static bool retirement_held(const ChorusRuntime& runtime) {
        runtime.assert_host_thread();
        if (!runtime._loading)
            return false;
        std::lock_guard lock(runtime._loading->mutex);
        return runtime._loading->attempt && runtime._loading->attempt->test_retirement_held;
    }

    static void release_retirement(ChorusRuntime& runtime) {
        runtime.assert_host_thread();
        if (!runtime._loading)
            return;
        {
            std::lock_guard lock(runtime._loading->mutex);
            if (runtime._loading->attempt)
                runtime._loading->attempt->test_hold_retirement = false;
        }
        runtime._loading->cv.notify_all();
    }
};

} // namespace Chorus
