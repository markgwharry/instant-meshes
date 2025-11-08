/*
    tbb_compat.h -- helpers to provide backwards-compatible shims for
    Intel/oneAPI TBB interfaces that changed in newer releases.

    This header provides minimal replacements for APIs that Instant Meshes
    relies on but that were removed from recent versions of TBB.  The
    shims are only enabled when compiling against a TBB release that no
    longer ships the legacy symbols.
*/

#pragma once

#include <memory>

#if defined(TBB_INTERFACE_VERSION) && (TBB_INTERFACE_VERSION >= 12000)
#  include <tbb/global_control.h>
#  include <tbb/info.h>

namespace tbb {

class task_scheduler_init {
public:
    static const int automatic = -1;

    explicit task_scheduler_init(int max_threads = automatic) {
        int desired_threads = max_threads == automatic
            ? static_cast<int>(tbb::info::default_concurrency())
            : max_threads;
        if (desired_threads < 1)
            desired_threads = 1;

        m_control.reset(new tbb::global_control(
            tbb::global_control::max_allowed_parallelism,
            static_cast<std::size_t>(desired_threads)));
    }

private:
    std::unique_ptr<tbb::global_control> m_control;
};

template <typename Range, typename Value, typename Map, typename Reduce>
Value parallel_deterministic_reduce(const Range &range, Value identity,
                                    const Map &map, const Reduce &reduce) {
    return tbb::parallel_reduce(range, identity, map, reduce);
}

} // namespace tbb

#endif // TBB_INTERFACE_VERSION >= 12000

