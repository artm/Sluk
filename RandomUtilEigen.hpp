#pragma once

namespace Eigen { namespace internal {
    template<>
    struct functor_traits< NormalRnd >
    {
        enum {
            Cost = 5 * NumTraits<double>::MulCost,
            PacketAccess = false,
            IsRepeatable = false
        };
    };
}}

