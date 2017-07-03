# `sd::vec8_` (type family)

Defined in header `<simdee/vec8.hpp>`

`sd::vec8_` is an architecture-independent type family of vectors that contain 8 scalars.

type        | `width` | `scalar_t`      | satisfies concepts
------------|---------|-----------------|----------------------------------------------------------------
`sd::vec8b` | 8       | `sd::bool32_t`  | [`SIMDVector`](SIMDVector.md), [`SIMDVectorB`](SIMDVectorB.md)
`sd::vec8f` | 8       | `float`         | [`SIMDVector`](SIMDVector.md), [`SIMDVectorF`](SIMDVectorF.md)
`sd::vec8u` | 8       | `std::uint32_t` | [`SIMDVector`](SIMDVector.md), [`SIMDVectorU`](SIMDVectorU.md)
`sd::vec8s` | 8       | `std::int32_t`  | [`SIMDVector`](SIMDVector.md), [`SIMDVectorS`](SIMDVectorS.md)

The `vec8` family is an alias for another type family, based on supported instruction sets:

type                        | if [`avx`](avx.md) is supported | otherwise
----------------------------|---------------------------------|----------------------------------                       
`sd::vec8b` is an alias for | [`sd::avxb`](avx.md)            | [`sd::dual<sd::vec4b>`](dual.md)                        
`sd::vec8f` is an alias for | [`sd::avxf`](avx.md)            | [`sd::dual<sd::vec4f>`](dual.md)                        
`sd::vec8u` is an alias for | [`sd::avxu`](avx.md)            | [`sd::dual<sd::vec4u>`](dual.md)                        
`sd::vec8s` is an alias for | [`sd::avxs`](avx.md)            | [`sd::dual<sd::vec4s>`](dual.md)
