#include "catch.hpp"

#include <array>
#include <type_traits>

#define SIMDIFY_NEED_AVX
#include <simdify/simdify.hpp>

using T = simd::avx;

alignas(T) const T::array_f bufA = {
    -0.27787193f, +0.70154146f, -2.05181630f, -0.35385000f,
    -0.82358653f, -1.57705702f, +0.50797465f, +0.28198406f,
};
alignas(T) const T::array_f bufB = {
    -0.23645458f, +2.02369089f, -2.25835397f, +2.22944568f,
    +0.33756370f, +1.00006082f, -1.66416447f, -0.59003456f,
};
alignas(T) const T::array_f bufC = {
    +0.42862268f, -1.03598478f, +1.87786546f, +0.94070440f,
    +0.78734578f, -0.87587426f, +0.31994913f, -0.55829428f,
};

TEST_CASE("AVX basic guarantees", "[simd_t][x86][avx]") {
    REQUIRE(T::W == 8);
    REQUIRE((std::is_same<T::mm_t, __m256>::value));
    REQUIRE((std::is_same<T::f_t, float>::value));
    REQUIRE((std::is_same<T::u_t, uint32_t>::value));
    REQUIRE((std::is_same<T::i_t, int32_t>::value));
    REQUIRE((std::is_same<T::array_f, std::array<T::f_t, T::W>>::value));
    REQUIRE((std::is_same<T::array_u, std::array<T::u_t, T::W>>::value));
    REQUIRE((std::is_same<T::array_i, std::array<T::i_t, T::W>>::value));
    REQUIRE(sizeof(T) == sizeof(T::mm_t));
    REQUIRE(alignof(T) == alignof(T::mm_t));
    REQUIRE(sizeof(T::f_t) * T::W == sizeof(T));
    REQUIRE(sizeof(T::f_t) == sizeof(T::u_t));
    REQUIRE(std::is_integral<T::u_t>::value);
    REQUIRE(std::is_unsigned<T::u_t>::value);
}

TEST_CASE("AVX construction", "[simd_t][x86][avx]") {
    alignas(T)T::array_f res_f;
    alignas(T)T::array_u res_u;
    alignas(T)T::array_i res_i;

    SECTION("default") {
        T t;
    }
    SECTION("from mm") {
        T::mm_t mm = _mm256_load_ps(bufA.data());
        T t(mm);
        t.store(res_f.data()); REQUIRE(res_f == bufA);
    }
    SECTION("from a pointer") {
        T t(simd::aligned(bufB.data()));
        simd::aligned(res_f.data()) = t; REQUIRE(res_f == bufB);
    }
    SECTION("from special values") {
        T t1(simd::ZERO);
        T t2(simd::ALL_BITS);
        T t3(simd::ABS_MASK);
        T t4(simd::SIGN_BIT);

        simd::aligned(res_u.data()) = t1; for (auto val : res_u) REQUIRE(val == 0x00000000);
        simd::aligned(res_u.data()) = t2; for (auto val : res_u) REQUIRE(val == 0xffffffff);
        simd::aligned(res_u.data()) = t3; for (auto val : res_u) REQUIRE(val == 0x7fffffff);
        simd::aligned(res_u.data()) = t4; for (auto val : res_u) REQUIRE(val == 0x80000000);
    }
    SECTION("from wrappers") {
        T tf(T::F(1.2345678f));
        T tu(T::U(0xdeadbeef));
        T ti(T::I(-123456789));
        simd::aligned(res_f.data()) = tf; for (auto val : res_f) REQUIRE(val == 1.2345678f);
        simd::aligned(res_u.data()) = tu; for (auto val : res_u) REQUIRE(val == 0xdeadbeef);
        simd::aligned(res_i.data()) = ti; for (auto val : res_i) REQUIRE(val == -123456789);
    }
}

TEST_CASE("AVX assignment", "[simd_t][x86][avx]") {

}

TEST_CASE("AVX arithmetic", "[simd_t][x86][avx]") {

}

TEST_CASE("AVX logical", "[simd_t][x86][avx]") {

}

TEST_CASE("AVX bitwise", "[simd_t][x86][avx]") {

}