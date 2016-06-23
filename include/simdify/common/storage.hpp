#ifndef SIMDIFY_COMMON_STORAGE_HPP
#define SIMDIFY_COMMON_STORAGE_HPP

#include "expr.hpp"
#include "../simd_types/common.hpp"
#include "../util/inline.hpp"

namespace simd {

    //
    // storage for SIMD or floating-point types
    //
    template <typename T, typename Enable = void>
    struct storage;

    //
    // storage replacement for loading SIMD vectors from interleaved (AOS) data
    //
    template <typename Simd_t, std::size_t N>
    struct aos_storage;

    //
    // specialized storage for SIMD types
    //
    template <typename Simd_t>
    struct alignas(Simd_t)storage<Simd_t, typename std::enable_if<is_simd_type<Simd_t>::value>::type> {
        using stored_t = Simd_t;
        using scalar_t = typename Simd_t::scalar_t;
        using data_t = std::array<scalar_t, Simd_t::W>;

        SIMDIFY_INL constexpr storage() = default;
        SIMDIFY_INL constexpr storage(const storage&) = default;

        SIMDIFY_INL explicit storage(const Simd_t& rhs) { rhs.aligned_store(data()); }

        SIMDIFY_INL constexpr explicit storage(const data_t& rhs) : m_data(rhs) {}

        template <std::size_t N>
        SIMDIFY_INL storage(const aos_storage<Simd_t, N>& rhs) { operator=(rhs); }

        SIMDIFY_INL storage& operator=(const storage&) = default;

        SIMDIFY_INL storage& operator=(const Simd_t& rhs) {
            rhs.aligned_store(data());
            return *this;
        }

        SIMDIFY_INL storage& operator=(const data_t& rhs) {
            m_data = rhs;
            return *this;
        }

        template <std::size_t N>
        SIMDIFY_INL storage& operator=(const aos_storage<Simd_t, N>& rhs) {
            auto lp = data();
            auto rp = rhs.data();
            for (std::size_t i = 0; i < Simd_t::W; ++i, ++lp, rp += N) {
                *lp = *rp;
            }
            return *this;
        }

        SIMDIFY_INL scalar_t* data() { return m_data.data(); }
        SIMDIFY_INL const scalar_t* data() const { return m_data.data(); }
        SIMDIFY_INL scalar_t& operator[](std::size_t i) { return m_data[i]; }
        SIMDIFY_INL const scalar_t& operator[](std::size_t i) const { return m_data[i]; }

        // implicit conversion to Simd_t
        SIMDIFY_INL operator Simd_t() const { Simd_t s; s.aligned_load(data()); return s; }

        // data
        data_t m_data;
    };

    //
    // specialized storage for arithmetic types
    //
    template <typename Scalar_t>
    struct storage<Scalar_t, typename std::enable_if<std::is_arithmetic<Scalar_t>::value>::type> {
        using stored_t = Scalar_t;
        using scalar_t = Scalar_t;
        using data_t = Scalar_t;

        SIMDIFY_INL constexpr storage() = default;
        SIMDIFY_INL constexpr storage(const storage&) = default;
        SIMDIFY_INL explicit storage(const Scalar_t& rhs) : m_data(rhs) {}

        SIMDIFY_INL storage& operator=(const storage&) = default;

        SIMDIFY_INL storage& operator=(const Scalar_t& rhs) {
            m_data = rhs;
            return *this;
        }

        SIMDIFY_INL Scalar_t* data() { return &m_data; }
        SIMDIFY_INL const Scalar_t* data() const { return &m_data; }
        SIMDIFY_INL scalar_t& operator[](std::size_t i) { return m_data; }
        SIMDIFY_INL const scalar_t& operator[](std::size_t i) const { return m_data; }

        // implicit conversion to Scalar_t
        SIMDIFY_INL operator Scalar_t() const { return m_data; }

        // data
        data_t m_data;
    };

    //
    // storage replacement for loading SIMD vectors from interleaved (AOS) data
    //
    template <typename Simd_t, std::size_t N>
    struct aos_storage {
        using stored_t = Simd_t;
        enum : std::size_t { W = Simd_t::W };
        using scalar_t = typename Simd_t::scalar_t;
        using data_t = std::array<scalar_t, N * W>;

        SIMDIFY_INL constexpr aos_storage() = default;
        SIMDIFY_INL aos_storage(const aos_storage& rhs) { operator=(rhs); }
        SIMDIFY_INL explicit aos_storage(const Simd_t& rhs) { operator=(rhs); }
        SIMDIFY_INL explicit aos_storage(const storage<Simd_t>& rhs) { operator=(rhs); }

        SIMDIFY_INL aos_storage& operator=(const aos_storage& rhs) {
            auto lp = data();
            auto rp = rhs.data();
            for (std::size_t i = 0; i < W; ++i, lp += N, rp += N) {
                *lp = *rp;
            }
            return *this;
        }

        SIMDIFY_INL aos_storage& operator=(const Simd_t& rhs) {
            rhs.interleaved_store(data(), N);
            return *this;
        }

        SIMDIFY_INL aos_storage& operator=(const storage<Simd_t>& rhs) {
            auto lp = data();
            auto rp = rhs.data();
            for (std::size_t i = 0; i < W; ++i, lp += N, ++rp) {
                *lp = *rp;
            }
            return *this;
        }

        SIMDIFY_INL scalar_t* data() { return m_data.data(); }
        SIMDIFY_INL const scalar_t* data() const { return m_data.data(); }
        SIMDIFY_INL scalar_t& operator[](std::size_t i) { return m_data[i * N]; }
        SIMDIFY_INL const scalar_t& operator[](std::size_t i) const { return m_data[i * N]; }

        // implicit conversion to Simd_t
        SIMDIFY_INL operator Simd_t() const {
            Simd_t s;
            s.interleaved_load(data(), N);
            return s;
        }

        // data
        data_t m_data;
    };

    template <typename T>
    SIMDIFY_INL constexpr auto tof(storage<T> r) -> decltype(tof(T(r))) { return tof(T(r)); }
    template <typename T, std::size_t N>
    SIMDIFY_INL constexpr auto tof(aos_storage<T, N> r) -> decltype(tof(T(r))) { return tof(T(r)); }

    template <typename T>
    SIMDIFY_INL constexpr auto tou(storage<T> r) -> decltype(tou(T(r))) { return tou(T(r)); }
    template <typename T, std::size_t N>
    SIMDIFY_INL constexpr auto tou(aos_storage<T, N> r) -> decltype(tou(T(r))) { return tou(T(r)); }

    template <typename T>
    SIMDIFY_INL constexpr auto tos(storage<T> r) -> decltype(tos(T(r))) { return tos(T(r)); }
    template <typename T, std::size_t N>
    SIMDIFY_INL constexpr auto tos(aos_storage<T, N> r) -> decltype(tos(T(r))) { return tos(T(r)); }
}

#endif // SIMDIFY_COMMON_STORAGE_HPP
