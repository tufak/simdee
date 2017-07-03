# `SIMDVectorB` (concept)

Describes all SIMD vector types provided by Simdee that have a boolean underlying scalar type.

## Implementations

* [`sd::sseb`](sse.md), [`sd::avxb`](avx.md), [`sd::dumb`](dum.md)
* [`sd::dual<T>`](dual.md), where `T` satisfies `SIMDVectorB`
* [`sd::vec4b`](vec4.md), [`sd::vec8b`](vec8.md)

## Requirements

Type that satisfies `SIMDVectorB` must also satisfy [`SIMDVector`](SIMDVector.md).

### Member types

Additional requirements apply regarding member types of a type `T` that satisfies `SIMDVectorB`:

* `vec_b` is `T`
* `scalar_t` is `sd::bool32_t`
* `T` has an extra member type `mask_t`, which is an instantiation of `sd::impl::mask`

### Conversions

A type that satisfies `SIMDVectorB` must be explicitly convertible to [`vec_u`](SIMDVectorU.md).

### Operations

Additional operations must be provided for a type `T` that satisfies `SIMDVectorB`:

syntax         | result type | description
---------------|-------------|---------------------------------------------------------------
`!x`           | `T`         | scalar-wise logical not
`x && y`       | `T`         | scalar-wise logical and, short-circuit evaluation is disabled
`x \|\| y`     | `T`         | scalar-wise logical or, short-circuit evaluation is disabled
`andnot(x, y)` | `T`         | same as `x && !y`
`x == y`       | `T`         | scalar-wise equal
`x != y`       | `T`         | scalar-wise not equal
`mask(x)`      | `mask_t`    | provide a bit-mask of truth values
`any(x)`       | `bool`      | returns `true` if any of the scalars is true
`all(x)`       | `bool`      | returns `true` if all of the scalars are true

where `x`, `y` are values of type `T`.
