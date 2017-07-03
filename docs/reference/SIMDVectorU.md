# `SIMDVectorU` (concept)

Describes all SIMD vector types provided by Simdee that have a 32-bit unsigned integral underlying scalar type.

## Implementations

* [`sd::sseu`](sse.md), [`sd::avxu`](avx.md), [`sd::dumu`](dum.md)
* [`sd::dual<T>`](dual.md), where `T` satisfies `SIMDVectorU`
* [`sd::vec4u`](vec4.md), [`sd::vec8u`](vec8.md)

## Requirements

Type that satisfies `SIMDVectorU` must also satisfy [`SIMDVector`](SIMDVector.md).

### Member types

Additional requirements apply regarding member types of a type `T` that satisfies `SIMDVectorU`:

* `vec_u` is `T`
* `scalar_t` is `std::uint32_t`

### Conversions

A type that satisfies `SIMDVectorU` must be:
* explicitly convertible to and from [`vec_s`](SIMDVectorS.md).
* explicitly convertible from [`vec_b`](SIMDVectorB.md).

### Operations

Additional operations must be provided for a type `T` that satisfies `SIMDVectorU`, unless the macro `SIMDEE_NEED_INT` is set to `0`:

syntax         | result type               | description
---------------|---------------------------|-------------------------------------------------------
`x == y`       | [`vec_b`](SIMDVectorB.md) | scalar-wise equal
`x != y`       | [`vec_b`](SIMDVectorB.md) | scalar-wise not equal
`~x`           | `T`                       | bit-wise not
`x & y`        | `T`                       | bit-wise and
`x \| y`       | `T`                       | bit-wise or
`x ^ y`        | `T`                       | bit-wise xor
`x &= y`       | `T`                       | same as `x = x & y`
`x \|= y`      | `T`                       | same as `x = x | y`
`x ^= y`       | `T`                       | same as `x = x ^ y`
`andnot(x, y)` | `T`                       | same as `x & ~y`

where `x`, `y` are values of type `T`.
