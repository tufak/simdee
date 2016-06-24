//
// common SIMD type tests
//
// following macros are expected to be defined
// SIMD_TYPE -- name of the SIMD type as a string
// SIMD_TEST_TAG -- catch tests tag(s) as a string
// SIMD_WIDTH -- expected W
// SIMD_LOAD_AF -- expression that extracts vector_t from bufAF
// SIMD_LOAD_AU -- expression that extracts vector_t from bufAU
// SIMD_LOAD_AS -- expression that extracts vector_t from bufAS
//

TEST_CASE(SIMD_TYPE " basic guarantees", SIMD_TEST_TAG) {
    REQUIRE(F::W == SIMD_WIDTH);
    REQUIRE(U::W == SIMD_WIDTH);
    REQUIRE(S::W == SIMD_WIDTH);
    REQUIRE((std::is_same<F::scalar_t, float>::value));
    REQUIRE((std::is_same<U::scalar_t, uint32_t>::value));
    REQUIRE((std::is_same<S::scalar_t, int32_t>::value));
    REQUIRE((std::is_same<F::array_t, std::array<F::scalar_t, F::W>>::value));
    REQUIRE((std::is_same<U::array_t, std::array<U::scalar_t, U::W>>::value));
    REQUIRE((std::is_same<S::array_t, std::array<S::scalar_t, S::W>>::value));
    REQUIRE(sizeof(F) == sizeof(F::vector_t));
    REQUIRE(sizeof(U) == sizeof(U::vector_t));
    REQUIRE(sizeof(S) == sizeof(S::vector_t));
    REQUIRE(alignof(F) == alignof(F::vector_t));
    REQUIRE(alignof(U) == alignof(U::vector_t));
    REQUIRE(alignof(S) == alignof(S::vector_t));
    REQUIRE(sizeof(F::scalar_t) * F::W == sizeof(F));
    REQUIRE(sizeof(U::scalar_t) * U::W == sizeof(U));
    REQUIRE(sizeof(S::scalar_t) * S::W == sizeof(S));
    REQUIRE((std::is_trivially_copyable<F>::value));
    REQUIRE((std::is_trivially_copyable<U>::value));
    REQUIRE((std::is_trivially_copyable<S>::value));
}

TEST_CASE(SIMD_TYPE " explicit construction", SIMD_TEST_TAG) {
    alignas(F)F::array_t rf = bufZF;
    alignas(U)U::array_t ru = bufZU;
    alignas(S)S::array_t rs = bufZS;
    auto tor = [&rf, &ru, &rs](const F& tf, const U& tu, const S& ts) {
        tf.aligned_store(rf.data());
        tu.aligned_store(ru.data());
        ts.aligned_store(rs.data());
    };

    SECTION("from scalar_t") {
        F tf(1.2345678f);
        U tu(123456789U);
        S ts(-123456789);
        tor(tf, tu, ts);
        for (auto val : rf) REQUIRE(val == 1.2345678f);
        for (auto val : ru) REQUIRE(val == 123456789U);
        for (auto val : rs) REQUIRE(val == -123456789);
    }
    SECTION("from vector_t") {
        F tf(SIMD_LOAD_AF);
        U tu(SIMD_LOAD_AU);
        S ts(SIMD_LOAD_AS);
        tor(tf, tu, ts);
        REQUIRE(rf == bufAF);
        REQUIRE(ru == bufAU);
        REQUIRE(rs == bufAS);
    }
    SECTION("from aligned pointer") {
        F tf(simd::aligned(bufAF.data()));
        U tu(simd::aligned(bufAU.data()));
        S ts(simd::aligned(bufAS.data()));
        tor(tf, tu, ts);
        REQUIRE(rf == bufAF);
        REQUIRE(ru == bufAU);
        REQUIRE(rs == bufAS);
    }
    SECTION("from unaligned pointer") {
        F tf(simd::unaligned(bufAF.data()));
        U tu(simd::unaligned(bufAU.data()));
        S ts(simd::unaligned(bufAS.data()));
        tor(tf, tu, ts);
        REQUIRE(rf == bufAF);
        REQUIRE(ru == bufAU);
        REQUIRE(rs == bufAS);
    }
    SECTION("from simd::storage") {
        simd::storage<F> storf;
        simd::storage<U> storu;
        simd::storage<S> stors;
        storf = bufAF;
        storu = bufAU;
        stors = bufAS;
        F tf(storf);
        U tu(storu);
        S ts(stors);
        tor(tf, tu, ts);
        REQUIRE(rf == bufAF);
        REQUIRE(ru == bufAU);
        REQUIRE(rs == bufAS);
    }
    SECTION("from uval, sval, zero, etc. (simd::init family)") {
        {
            F tf(simd::fval(1.2345678f));
            U tu(simd::fval(1.2345678f));
            S ts(simd::fval(1.2345678f));
            tor(tf, tu, ts);
            for (auto val : rf) REQUIRE(simd::tof(val) == 1.2345678f);
            for (auto val : ru) REQUIRE(simd::tof(val) == 1.2345678f);
            for (auto val : rs) REQUIRE(simd::tof(val) == 1.2345678f);
        }
        {
            F tf(simd::uval(0xdeadbeef));
            U tu(simd::uval(0xdeadbeef));
            S ts(simd::uval(0xdeadbeef));
            tor(tf, tu, ts);
            for (auto val : rf) REQUIRE(simd::tou(val) == 0xdeadbeef);
            for (auto val : ru) REQUIRE(simd::tou(val) == 0xdeadbeef);
            for (auto val : rs) REQUIRE(simd::tou(val) == 0xdeadbeef);
        }
        {
            F tf(simd::sval(-123456789));
            U tu(simd::sval(-123456789));
            S ts(simd::sval(-123456789));
            tor(tf, tu, ts);
            for (auto val : rf) REQUIRE(simd::tos(val) == -123456789);
            for (auto val : ru) REQUIRE(simd::tos(val) == -123456789);
            for (auto val : rs) REQUIRE(simd::tos(val) == -123456789);
        }
        {
            F tf(simd::zero());
            U tu(simd::zero());
            S ts(simd::zero());
            tor(tf, tu, ts);
            for (auto val : rf) REQUIRE(simd::tou(val) == 0x00000000);
            for (auto val : ru) REQUIRE(simd::tou(val) == 0x00000000);
            for (auto val : rs) REQUIRE(simd::tou(val) == 0x00000000);
        }
        {
            F tf(simd::all_bits());
            U tu(simd::all_bits());
            S ts(simd::all_bits());
            tor(tf, tu, ts);
            for (auto val : rf) REQUIRE(simd::tou(val) == 0xffffffff);
            for (auto val : ru) REQUIRE(simd::tou(val) == 0xffffffff);
            for (auto val : rs) REQUIRE(simd::tou(val) == 0xffffffff);
        }
        {
            F tf(simd::sign_bit());
            U tu(simd::sign_bit());
            S ts(simd::sign_bit());
            tor(tf, tu, ts);
            for (auto val : rf) REQUIRE(simd::tou(val) == 0x80000000);
            for (auto val : ru) REQUIRE(simd::tou(val) == 0x80000000);
            for (auto val : rs) REQUIRE(simd::tou(val) == 0x80000000);
        }
        {
            F tf(simd::abs_mask());
            U tu(simd::abs_mask());
            S ts(simd::abs_mask());
            tor(tf, tu, ts);
            for (auto val : rf) REQUIRE(simd::tou(val) == 0x7fffffff);
            for (auto val : ru) REQUIRE(simd::tou(val) == 0x7fffffff);
            for (auto val : rs) REQUIRE(simd::tou(val) == 0x7fffffff);
        }
    }
}


TEST_CASE(SIMD_TYPE " implicit construction", SIMD_TEST_TAG) {
    alignas(F)F::array_t rf = bufZF;
    alignas(U)U::array_t ru = bufZU;
    alignas(S)S::array_t rs = bufZS;
    auto implicit_test = [&rf, &ru, &rs](const F& tf, const U& tu, const S& ts) {
        tf.aligned_store(rf.data());
        tu.aligned_store(ru.data());
        ts.aligned_store(rs.data());
    };

    SECTION("from scalar_t") {
        implicit_test(1.2345678f, 123456789U, -123456789);
        for (auto val : rf) REQUIRE(val == 1.2345678f);
        for (auto val : ru) REQUIRE(val == 123456789U);
        for (auto val : rs) REQUIRE(val == -123456789);
    }
    SECTION("from vector_t") {
        implicit_test(
            SIMD_LOAD_AF,
            SIMD_LOAD_AU,
            SIMD_LOAD_AS);
        REQUIRE(rf == bufAF);
        REQUIRE(ru == bufAU);
        REQUIRE(rs == bufAS);
    }
    SECTION("from aligned pointer") {
        implicit_test(
            simd::aligned(bufAF.data()),
            simd::aligned(bufAU.data()),
            simd::aligned(bufAS.data()));
        REQUIRE(rf == bufAF);
        REQUIRE(ru == bufAU);
        REQUIRE(rs == bufAS);
    }
    SECTION("from unaligned pointer") {
        implicit_test(
            simd::unaligned(bufAF.data()),
            simd::unaligned(bufAU.data()),
            simd::unaligned(bufAS.data()));
        REQUIRE(rf == bufAF);
        REQUIRE(ru == bufAU);
        REQUIRE(rs == bufAS);
    }
    SECTION("from simd::storage") {
        simd::storage<F> storf;
        simd::storage<U> storu;
        simd::storage<S> stors;
        storf = bufAF;
        storu = bufAU;
        stors = bufAS;
        implicit_test(storf, storu, stors);
        REQUIRE(rf == bufAF);
        REQUIRE(ru == bufAU);
        REQUIRE(rs == bufAS);
    }
    SECTION("from uval, sval, zero, etc. (simd::init family)") {
        implicit_test(simd::fval(1.2345678f), simd::fval(1.2345678f), simd::fval(1.2345678f));
        for (auto val : rf) REQUIRE(simd::tof(val) == 1.2345678f);
        for (auto val : ru) REQUIRE(simd::tof(val) == 1.2345678f);
        for (auto val : rs) REQUIRE(simd::tof(val) == 1.2345678f);
        implicit_test(simd::uval(0xdeadbeef), simd::uval(0xdeadbeef), simd::uval(0xdeadbeef));
        for (auto val : rf) REQUIRE(simd::tou(val) == 0xdeadbeef);
        for (auto val : ru) REQUIRE(simd::tou(val) == 0xdeadbeef);
        for (auto val : rs) REQUIRE(simd::tou(val) == 0xdeadbeef);
        implicit_test(simd::sval(-123456789), simd::sval(-123456789), simd::sval(-123456789));
        for (auto val : rf) REQUIRE(simd::tos(val) == -123456789);
        for (auto val : ru) REQUIRE(simd::tos(val) == -123456789);
        for (auto val : rs) REQUIRE(simd::tos(val) == -123456789);
        implicit_test(simd::zero(), simd::zero(), simd::zero());
        for (auto val : rf) REQUIRE(simd::tou(val) == 0x00000000);
        for (auto val : ru) REQUIRE(simd::tou(val) == 0x00000000);
        for (auto val : rs) REQUIRE(simd::tou(val) == 0x00000000);
        implicit_test(simd::all_bits(), simd::all_bits(), simd::all_bits());
        for (auto val : rf) REQUIRE(simd::tou(val) == 0xffffffff);
        for (auto val : ru) REQUIRE(simd::tou(val) == 0xffffffff);
        for (auto val : rs) REQUIRE(simd::tou(val) == 0xffffffff);
        implicit_test(simd::sign_bit(), simd::sign_bit(), simd::sign_bit());
        for (auto val : rf) REQUIRE(simd::tou(val) == 0x80000000);
        for (auto val : ru) REQUIRE(simd::tou(val) == 0x80000000);
        for (auto val : rs) REQUIRE(simd::tou(val) == 0x80000000);
        implicit_test(simd::abs_mask(), simd::abs_mask(), simd::abs_mask());
        for (auto val : rf) REQUIRE(simd::tou(val) == 0x7fffffff);
        for (auto val : ru) REQUIRE(simd::tou(val) == 0x7fffffff);
        for (auto val : rs) REQUIRE(simd::tou(val) == 0x7fffffff);
    }
}

TEST_CASE(SIMD_TYPE " assignment", SIMD_TEST_TAG) {
    alignas(F)F::array_t rf = bufZF;
    alignas(U)U::array_t ru = bufZU;
    alignas(S)S::array_t rs = bufZS;
    F tf;
    U tu;
    S ts;
    auto tor = [&rf, &tf, &ru, &tu, &rs, &ts]() {
        tf.aligned_store(rf.data());
        tu.aligned_store(ru.data());
        ts.aligned_store(rs.data());
    };

    SECTION("from scalar_t") {
        tf = 1.2345678f;
        tu = 123456789U;
        ts = -123456789;
        tor();
        for (auto val : rf) REQUIRE(val == 1.2345678f);
        for (auto val : ru) REQUIRE(val == 123456789U);
        for (auto val : rs) REQUIRE(val == -123456789);
    }
    SECTION("from vector_t") {
        tf = SIMD_LOAD_AF;
        tu = SIMD_LOAD_AU;
        ts = SIMD_LOAD_AS;
        tor();
        REQUIRE(rf == bufAF);
        REQUIRE(ru == bufAU);
        REQUIRE(rs == bufAS);
    }
    SECTION("from aligned pointer") {
        tf = simd::aligned(bufAF.data());
        tu = simd::aligned(bufAU.data());
        ts = simd::aligned(bufAS.data());
        tor();
        REQUIRE(rf == bufAF);
        REQUIRE(ru == bufAU);
        REQUIRE(rs == bufAS);
    }
    SECTION("from unaligned pointer") {
        tf = simd::unaligned(bufAF.data());
        tu = simd::unaligned(bufAU.data());
        ts = simd::unaligned(bufAS.data());
        tor();
        REQUIRE(rf == bufAF);
        REQUIRE(ru == bufAU);
        REQUIRE(rs == bufAS);
    }
    SECTION("from simd::storage") {
        simd::storage<F> storf;
        simd::storage<U> storu;
        simd::storage<S> stors;
        storf = bufAF;
        storu = bufAU;
        stors = bufAS;
        tf = storf;
        tu = storu;
        ts = stors;
        tor();
        REQUIRE(rf == bufAF);
        REQUIRE(ru == bufAU);
        REQUIRE(rs == bufAS);
    }
    SECTION("from uval, sval, zero, etc. (simd::init family)") {
        tf = simd::fval(1.2345678f);
        tu = simd::fval(1.2345678f);
        ts = simd::fval(1.2345678f);
        tor();
        for (auto val : rf) REQUIRE(simd::tof(val) == 1.2345678f);
        for (auto val : ru) REQUIRE(simd::tof(val) == 1.2345678f);
        for (auto val : rs) REQUIRE(simd::tof(val) == 1.2345678f);
        tf = simd::uval(0xdeadbeef);
        tu = simd::uval(0xdeadbeef);
        ts = simd::uval(0xdeadbeef);
        tor();
        for (auto val : rf) REQUIRE(simd::tou(val) == 0xdeadbeef);
        for (auto val : ru) REQUIRE(simd::tou(val) == 0xdeadbeef);
        for (auto val : rs) REQUIRE(simd::tou(val) == 0xdeadbeef);
        tf = simd::sval(-123456789);
        tu = simd::sval(-123456789);
        ts = simd::sval(-123456789);
        tor();
        for (auto val : rf) REQUIRE(simd::tos(val) == -123456789);
        for (auto val : ru) REQUIRE(simd::tos(val) == -123456789);
        for (auto val : rs) REQUIRE(simd::tos(val) == -123456789);
        tf = simd::zero();
        tu = simd::zero();
        ts = simd::zero();
        tor();
        for (auto val : rf) REQUIRE(simd::tou(val) == 0x00000000);
        for (auto val : ru) REQUIRE(simd::tou(val) == 0x00000000);
        for (auto val : rs) REQUIRE(simd::tou(val) == 0x00000000);
        tf = simd::all_bits();
        tu = simd::all_bits();
        ts = simd::all_bits();
        tor();
        for (auto val : rf) REQUIRE(simd::tou(val) == 0xffffffff);
        for (auto val : ru) REQUIRE(simd::tou(val) == 0xffffffff);
        for (auto val : rs) REQUIRE(simd::tou(val) == 0xffffffff);
        tf = simd::sign_bit();
        tu = simd::sign_bit();
        ts = simd::sign_bit();
        tor();
        for (auto val : rf) REQUIRE(simd::tou(val) == 0x80000000);
        for (auto val : ru) REQUIRE(simd::tou(val) == 0x80000000);
        for (auto val : rs) REQUIRE(simd::tou(val) == 0x80000000);
        tf = simd::abs_mask();
        tu = simd::abs_mask();
        ts = simd::abs_mask();
        tor();
        for (auto val : rf) REQUIRE(simd::tou(val) == 0x7fffffff);
        for (auto val : ru) REQUIRE(simd::tou(val) == 0x7fffffff);
        for (auto val : rs) REQUIRE(simd::tou(val) == 0x7fffffff);
    }
}

TEST_CASE(SIMD_TYPE " type conversion", SIMD_TEST_TAG) {
    SECTION("int to float") {
        simd::storage<F> expected, result;
        std::transform(begin(bufAS), end(bufAS), begin(expected), [](S::scalar_t a) {
            return static_cast<F::scalar_t>(a);
        });
        S in = simd::aligned(bufAS.data());
        F(in).aligned_store(result.data());
        REQUIRE(result == expected);
    }
    SECTION("float to int") {
        simd::storage<S> expected, result;
        std::transform(begin(bufAF), end(bufAF), begin(expected), [](F::scalar_t a) {
            return simd::round_to_int32(a);
        });
        F in = simd::aligned(bufAF.data());
        S(in).aligned_store(result.data());
        REQUIRE(result == expected);
    }
    SECTION("int to uint") {
        simd::storage<U> expected, result;
        std::transform(begin(bufAS), end(bufAS), begin(expected), [](S::scalar_t a) {
            return static_cast<U::scalar_t>(a);
        });
        S in = simd::aligned(bufAS.data());
        U(in).aligned_store(result.data());
        REQUIRE(result == expected);
    }
    SECTION("uint to int") {
        simd::storage<S> expected, result;
        std::transform(begin(bufAU), end(bufAU), begin(expected), [](U::scalar_t a) {
            return simd::round_to_int32(a);
        });
        U in = simd::aligned(bufAU.data());
        S(in).aligned_store(result.data());
        REQUIRE(result == expected);
    }
}

TEST_CASE(SIMD_TYPE " arithmetic", SIMD_TEST_TAG) {
    simd::storage<F> rf, ef;
    F a = simd::aligned(bufAF.data());
    F b = simd::aligned(bufBF.data());

    SECTION("unary plus") {
        std::transform(begin(bufAF), end(bufAF), begin(ef), [](F::scalar_t a) { return +a; });
        rf = +a;
        REQUIRE(rf == ef);
    }
    SECTION("unary minus") {
        std::transform(begin(bufAF), end(bufAF), begin(ef), std::negate<F::scalar_t>());
        rf = -a;
        REQUIRE(rf == ef);
    }
    SECTION("plus") {
        std::transform(begin(bufAF), end(bufAF), begin(bufBF), begin(ef), std::plus<F::scalar_t>());
        rf = a + b;
        REQUIRE(rf == ef);
        a += b;
        rf = a;
        REQUIRE(rf == ef);
    }
    SECTION("minus") {
        std::transform(begin(bufAF), end(bufAF), begin(bufBF), begin(ef), std::minus<F::scalar_t>());
        rf = a - b;
        REQUIRE(rf == ef);
        a -= b;
        rf = a;
        REQUIRE(rf == ef);
    }
    SECTION("multiplies") {
        std::transform(begin(bufAF), end(bufAF), begin(bufBF), begin(ef), std::multiplies<F::scalar_t>());
        rf = a * b;
        REQUIRE(rf == ef);
        a *= b;
        rf = a;
        REQUIRE(rf == ef);
    }
    SECTION("divides") {
        std::transform(begin(bufAF), end(bufAF), begin(bufBF), begin(ef), std::divides<F::scalar_t>());
        rf = a / b;
        REQUIRE(rf == ef);
        a /= b;
        rf = a;
        REQUIRE(rf == ef);
    }
    SECTION("absolute value") {
        std::transform(begin(bufAF), end(bufAF), begin(ef), [](F::scalar_t a) { return std::abs(a); });
        rf = abs(a);
        REQUIRE(rf == ef);
    }
    SECTION("signum") {
        std::transform(begin(bufAF), end(bufAF), begin(ef), [](F::scalar_t a) { return std::signbit(a) ? F::scalar_t(-1) : F::scalar_t(1); });
        rf = signum(a);
        REQUIRE(rf == ef);
    }
    SECTION("square root") {
        std::transform(begin(bufAF), end(bufAF), begin(ef), [](F::scalar_t a) { return std::sqrt(std::abs(a)); });
        rf = sqrt(abs(a));
        for (int i = 0; i < F::W; ++i) REQUIRE(rf[i] == Approx(ef[i]));
    }
    SECTION("fast reciprocal") {
        std::transform(begin(bufAF), end(bufAF), begin(ef), [](F::scalar_t a) { return 1 / a; });
        rf = rcp(a);
        for (int i = 0; i < F::W; ++i) REQUIRE(rf[i] == Approx(ef[i]).epsilon(0.001));
    }
    SECTION("fast reciprocal square root") {
        std::transform(begin(bufAF), end(bufAF), begin(ef), [](F::scalar_t a) { return 1 / std::sqrt(std::abs(a)); });
        rf = rsqrt(abs(a));
        for (int i = 0; i < F::W; ++i) REQUIRE(rf[i] == Approx(ef[i]).epsilon(0.001));
    }
}

TEST_CASE(SIMD_TYPE " float comparison", SIMD_TEST_TAG) {
    auto if_ = [](bool in) { return in ? 0xffffffffU : 0x00000000U; };
    simd::storage<U> r, e;
    F a = simd::aligned(bufAF.data());
    F b = simd::aligned(bufBF.data());

    SECTION("equal to") {
        std::transform(begin(bufAF), end(bufAF), begin(bufBF), begin(e), [&if_](F::scalar_t a, F::scalar_t b) {
            return if_(a == b);
        });
        r = a == b;
        REQUIRE(r == e);
    }
    SECTION("not equal to") {
        std::transform(begin(bufAF), end(bufAF), begin(bufBF), begin(e), [&if_](F::scalar_t a, F::scalar_t b) {
            return if_(a != b);
        });
        r = a != b;
        REQUIRE(r == e);
    }
    SECTION("greater") {
        std::transform(begin(bufAF), end(bufAF), begin(bufBF), begin(e), [&if_](F::scalar_t a, F::scalar_t b) {
            return if_(a > b);
        });
        r = a > b;
        REQUIRE(r == e);
    }
    SECTION("less") {
        std::transform(begin(bufAF), end(bufAF), begin(bufBF), begin(e), [&if_](F::scalar_t a, F::scalar_t b) {
            return if_(a < b);
        });
        r = a < b;
        REQUIRE(r == e);
    }
    SECTION("greater equal") {
        std::transform(begin(bufAF), end(bufAF), begin(bufBF), begin(e), [&if_](F::scalar_t a, F::scalar_t b) {
            return if_(a >= b);
        });
        r = a >= b;
        REQUIRE(r == e);
    }
    SECTION("less equal") {
        std::transform(begin(bufAF), end(bufAF), begin(bufBF), begin(e), [&if_](F::scalar_t a, F::scalar_t b) {
            return if_(a <= b);
        });
        r = a <= b;
        REQUIRE(r == e);
    }
}

TEST_CASE(SIMD_TYPE " int comparison", SIMD_TEST_TAG) {
    auto if_ = [](bool in) { return in ? 0xffffffffU : 0x00000000U; };
    simd::storage<U> r, e;
    S a = simd::aligned(bufAS.data());
    S b = simd::aligned(bufBS.data());

    SECTION("equal to") {
        std::transform(begin(bufAS), end(bufAS), begin(bufBS), begin(e), [&if_](S::scalar_t a, S::scalar_t b) {
            return if_(a == b);
        });
        r = a == b;
        REQUIRE(r == e);
    }
    SECTION("not equal to") {
        std::transform(begin(bufAS), end(bufAS), begin(bufBS), begin(e), [&if_](S::scalar_t a, S::scalar_t b) {
            return if_(a != b);
        });
        r = a != b;
        REQUIRE(r == e);
    }
    SECTION("greater") {
        std::transform(begin(bufAS), end(bufAS), begin(bufBS), begin(e), [&if_](S::scalar_t a, S::scalar_t b) {
            return if_(a > b);
        });
        r = a > b;
        REQUIRE(r == e);
    }
    SECTION("less") {
        std::transform(begin(bufAS), end(bufAS), begin(bufBS), begin(e), [&if_](S::scalar_t a, S::scalar_t b) {
            return if_(a < b);
        });
        r = a < b;
        REQUIRE(r == e);
    }
    SECTION("greater equal") {
        std::transform(begin(bufAS), end(bufAS), begin(bufBS), begin(e), [&if_](S::scalar_t a, S::scalar_t b) {
            return if_(a >= b);
        });
        r = a >= b;
        REQUIRE(r == e);
    }
    SECTION("less equal") {
        std::transform(begin(bufAS), end(bufAS), begin(bufBS), begin(e), [&if_](S::scalar_t a, S::scalar_t b) {
            return if_(a <= b);
        });
        r = a <= b;
        REQUIRE(r == e);
    }
}

TEST_CASE(SIMD_TYPE " bitwise", SIMD_TEST_TAG) {
    simd::storage<U> r, e;
    U a = simd::aligned(bufAU.data());
    U b = simd::aligned(bufBU.data());

    SECTION("bit and") {
        std::transform(begin(bufAU), end(bufAU), begin(bufBU), begin(e), [](U::scalar_t a, U::scalar_t b) {
            return simd::tou(a) & simd::tou(b);
        });
        r = a & b;
        REQUIRE(r == e);
        a &= b;
        r = a;
        REQUIRE(r == e);
    }
    SECTION("bit or") {
        std::transform(begin(bufAU), end(bufAU), begin(bufBU), begin(e), [](U::scalar_t a, U::scalar_t b) {
            return simd::tou(a) | simd::tou(b);
        });
        r = a | b;
        REQUIRE(r == e);
        a |= b;
        r = a;
        REQUIRE(r == e);
    }
    SECTION("bit xor") {
        std::transform(begin(bufAU), end(bufAU), begin(bufBU), begin(e), [](U::scalar_t a, U::scalar_t b) {
            return simd::tou(a) ^ simd::tou(b);
        });
        r = a ^ b;
        REQUIRE(r == e);
        a ^= b;
        r = a;
        REQUIRE(r == e);
    }
    SECTION("bit not") {
        std::transform(begin(bufAU), end(bufAU), begin(e), [](U::scalar_t a) {
            return ~simd::tou(a);
        });
        r = ~a;
        REQUIRE(r == e);
    }
    SECTION("complex expr") {
        std::transform(begin(bufAU), end(bufAU), begin(bufBU), begin(e), [](U::scalar_t af, U::scalar_t bf) {
            auto a = simd::tou(af);
            auto b = simd::tou(bf);
            return ~((~a & ~b) | (~a ^ ~b));
        });
        r = ~((~a & ~b) | (~a ^ ~b));
        REQUIRE(r == e);
    }
}

TEST_CASE(SIMD_TYPE " horizontal operations", SIMD_TEST_TAG) {
    F a = simd::aligned(bufAF.data());
    auto max = [](F::scalar_t l, F::scalar_t r) { return std::max(l, r); };
    auto min = [](F::scalar_t l, F::scalar_t r) { return std::min(l, r); };
    F::scalar_t inf = std::numeric_limits<F::scalar_t>::infinity();
    F::scalar_t v, v0;

    SECTION("max") {
        v0 = std::accumulate(begin(bufAF), end(bufAF), -inf, max);
        v = a.reduce(simd::max).first_element();
        REQUIRE(v == v0);
        v = max_mask(a, simd::zero()).reduce(simd::max).first_element();
        REQUIRE(v == -inf);
    }
    SECTION("min") {
        v0 = std::accumulate(begin(bufAF), end(bufAF), inf, min);
        v = a.reduce(simd::min).first_element();
        REQUIRE(v == v0);
        v = min_mask(a, simd::zero()).reduce(simd::min).first_element();
        REQUIRE(v == inf);
    }
    SECTION("sum") {
        v0 = std::accumulate(begin(bufAF), end(bufAF), F::scalar_t(0));
        v = a.reduce(simd::operator+).first_element();
        REQUIRE(v == Approx(v0));
        v = sum_mask(a, simd::zero()).reduce(simd::operator+).first_element();
        REQUIRE(v == 0);
    }
    SECTION("product") {
        v0 = std::accumulate(begin(bufAF), end(bufAF), F::scalar_t(1), std::multiplies<F::scalar_t>());
        v = a.reduce(simd::operator*).first_element();
        REQUIRE(v == Approx(v0));
        v = product_mask(a, simd::zero()).reduce(simd::operator*).first_element();
        REQUIRE(v == 1);
    }
}

TEST_CASE(SIMD_TYPE " conditional", SIMD_TEST_TAG) {
    F af = simd::aligned(bufAF.data());
    U au = simd::aligned(bufAU.data());
    S as = simd::aligned(bufAS.data());
    F bf = simd::aligned(bufBF.data());
    U bu = simd::aligned(bufBU.data());
    S bs = simd::aligned(bufBS.data());

    U sel = af >= bf;
    bool sel0[F::W];
    for (auto& val : sel0) val = false;
    for (auto idx : sel) sel0[idx] = true;

    simd::storage<F> rf(cond(sel, af, bf));
    simd::storage<U> ru(cond(sel, au, bu));
    simd::storage<S> rs(cond(sel, as, bs));

    for (int i = 0; i < F::W; ++i) {
        REQUIRE((rf[i]) == (sel0[i] ? bufAF[i] : bufBF[i]));
        REQUIRE((ru[i]) == (sel0[i] ? bufAU[i] : bufBU[i]));
        REQUIRE((rs[i]) == (sel0[i] ? bufAS[i] : bufBS[i]));
    }
}

TEST_CASE(SIMD_TYPE " expression template compatibility", SIMD_TEST_TAG) {
    U in = simd::aligned(bufAU.data());
    U vec1 = ~in;
    auto vec2 = ~in;

    REQUIRE(vec1.front() == vec2.front());
    REQUIRE(vec1.begin().mask == vec2.begin().mask);
    REQUIRE(vec1.end().mask == vec2.end().mask);
    REQUIRE(vec1.any() == vec2.any());
    REQUIRE(vec1.all() == vec2.all());
    REQUIRE(vec1.first_element() == vec2.first_element());
}
