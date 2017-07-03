# `sd::vec4_` (type family)

Defined in header `<simdee/vec4.hpp>`

`sd::vec4_` is an architecture-independent type family of vectors that contain 4 scalars.

type        | `width` | `scalar_t`      | satisfies concepts
------------|---------|-----------------|----------------------------------------------------------------
`sd::vec4b` | 4       | `sd::bool32_t`  | [`SIMDVector`](SIMDVector.md), [`SIMDVectorB`](SIMDVectorB.md)
`sd::vec4f` | 4       | `float`         | [`SIMDVector`](SIMDVector.md), [`SIMDVectorF`](SIMDVectorF.md)
`sd::vec4u` | 4       | `std::uint32_t` | [`SIMDVector`](SIMDVector.md), [`SIMDVectorU`](SIMDVectorU.md)
`sd::vec4s` | 4       | `std::int32_t`  | [`SIMDVector`](SIMDVector.md), [`SIMDVectorS`](SIMDVectorS.md)

The `vec4` family is an alias for another type family, based on supported instruction sets:

type                        | if [`sse`](sse.md) is supported | otherwise
----------------------------|---------------------------------|-------------------------------------------                        
`sd::vec4b` is an alias for | [`sd::sseb`](sse.md)            | [`sd::dual<sd::dual<sd::dumb>>`](dual.md)                        
`sd::vec4f` is an alias for | [`sd::ssef`](sse.md)            | [`sd::dual<sd::dual<sd::dumf>>`](dual.md)                        
`sd::vec4u` is an alias for | [`sd::sseu`](sse.md)            | [`sd::dual<sd::dual<sd::dumu>>`](dual.md)                        
`sd::vec4s` is an alias for | [`sd::sses`](sse.md)            | [`sd::dual<sd::dual<sd::dums>>`](dual.md)
