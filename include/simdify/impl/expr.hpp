#ifndef SIMDIFY_EXPR
#define SIMDIFY_EXPR

#include "../util/inline.hpp"
#include <type_traits>

namespace simd {
    namespace expr {
        template <typename T>
        struct aligned {
            SIMDIFY_FORCE_INLINE explicit aligned(T* const& r) : ptr(r) {}

            template <typename Simd_t>
            SIMDIFY_FORCE_INLINE void operator=(const Simd_t& r) {
                static_assert(!std::is_const<T>::value, "Storing into a const pointer via aligned()");
                using f_t = typename Simd_t::f_t;
                r.store(reinterpret_cast<f_t*>(ptr));
            }

            template <typename F_t>
            SIMDIFY_FORCE_INLINE const F_t* get_load() const {
                return reinterpret_cast<const F_t*>(ptr);
            }

            // data
            T* const& ptr;
        };

        template <typename T>
        struct unaligned {
            SIMDIFY_FORCE_INLINE explicit unaligned(T* const& r) : ptr(r) {}

            template <typename Simd_t>
            SIMDIFY_FORCE_INLINE void operator=(const Simd_t& r) {
                static_assert(!std::is_const<T>::value, "Storing into a const pointer via unaligned()");
                using f_t = typename Simd_t::f_t;
                r.store(reinterpret_cast<f_t*>(ptr));
            }

            template <typename F_t>
            SIMDIFY_FORCE_INLINE const F_t* get_load() const {
                return reinterpret_cast<const F_t*>(ptr);
            }

            // data
            T* const& ptr;
        };

        template <typename T>
        struct uint {
            SIMDIFY_FORCE_INLINE explicit uint(T&& r) : ref(std::forward<T>(r)) {}

            SIMDIFY_FORCE_INLINE explicit operator float() {
                using u_t = simd::select_uint_t<sizeof(float)>;
                return reinterpret_cast<const float&>(static_cast<u_t>(ref));
            }

            SIMDIFY_FORCE_INLINE explicit operator double() {
                using u_t = simd::select_uint_t<sizeof(double)>;
                return reinterpret_cast<const double&>(static_cast<u_t>(ref));
            }

            // data
            T&& ref;
        };

        template <typename T>
        struct sint {
            SIMDIFY_FORCE_INLINE explicit sint(T&& r) : ref(std::forward<T>(r)) {}

            SIMDIFY_FORCE_INLINE explicit operator float() {
                using u_t = simd::select_sint_t<sizeof(float)>;
                return reinterpret_cast<const float&>(static_cast<u_t>(ref));
            }

            SIMDIFY_FORCE_INLINE explicit operator double() {
                using u_t = simd::select_sint_t<sizeof(double)>;
                return reinterpret_cast<const double&>(static_cast<u_t>(ref));
            }

            // data
            T&& ref;
        };

        template <typename T>
        struct utof {
            SIMDIFY_FORCE_INLINE explicit utof(T&& r) : ref(std::forward<T>(r)) {}

            template <typename F_t>
            F_t to() {
                static_assert(std::is_floating_point<F_t>::value, "utof::to used to convert to a non-floating point type");
                using u_t = simd::select_uint_t<sizeof(F_t)>;
                return reinterpret_cast<const F_t&>(static_cast<u_t>(ref));
            }

            // data
            T&& ref;
        };

        template <typename T>
        struct stof {
            SIMDIFY_FORCE_INLINE explicit stof(T&& r) : ref(std::forward<T>(r)) {}

            template <typename F_t>
            F_t to() {
                static_assert(std::is_floating_point<F_t>::value, "stof::to() used to convert to a non-floating point type");
                using s_t = simd::select_sint_t<sizeof(F_t)>;
                return reinterpret_cast<const F_t&>(static_cast<s_t>(ref));
            }

            // data
            T&& ref;
        };

        template <typename T>
        struct tou {
            using f_t = typename std::decay<T>::type;
            using u_t = simd::select_uint_t<sizeof(f_t)>;
            static_assert(std::is_floating_point<f_t>::value, "tou() must be used to convert a floating-point value");

            SIMDIFY_FORCE_INLINE explicit tou(T&& r) : ref(std::forward<T>(r)) {}

            SIMDIFY_FORCE_INLINE operator u_t() {
                return reinterpret_cast<const u_t&>(ref);
            }

            // data
            T&& ref;
        };

        template <typename T>
        struct tos {
            using f_t = typename std::decay<T>::type;
            using s_t = simd::select_sint_t<sizeof(f_t)>;
            static_assert(std::is_floating_point<f_t>::value, "tos() must be used to convert a floating-point value");

            SIMDIFY_FORCE_INLINE explicit tos(T&& r) : ref(std::forward<T>(r)) {}

            SIMDIFY_FORCE_INLINE operator s_t() {
                return reinterpret_cast<const s_t&>(ref);
            }

            // data
            T&& ref;
        };
    }

    template <typename T>
    SIMDIFY_FORCE_INLINE expr::aligned<T> aligned(T* const& r) { return expr::aligned<T>(r); }
    template <typename T>
    SIMDIFY_FORCE_INLINE expr::unaligned<T> unaligned(T* const& r) { return expr::unaligned<T>(r); }
    template <typename T>
    SIMDIFY_FORCE_INLINE expr::uint<T&&> uint(T&& r) { return expr::uint<T&&>(std::forward<T>(r)); }
    template <typename T>
    SIMDIFY_FORCE_INLINE float uintf(T&& r) { return float(expr::uint<T&&>(std::forward<T>(r))); }
    template <typename T>
    SIMDIFY_FORCE_INLINE double uintd(T&& r) { return double(expr::uint<T&&>(std::forward<T>(r))); }
    template <typename T>
    SIMDIFY_FORCE_INLINE expr::sint<T&&> sint(T&& r) { return expr::sint<T&&>(std::forward<T>(r)); }
    template <typename T>
    SIMDIFY_FORCE_INLINE float sintf(T&& r) { return float(expr::sint<T&&>(std::forward<T>(r))); }
    template <typename T>
    SIMDIFY_FORCE_INLINE double sintd(T&& r) { return double(expr::sint<T&&>(std::forward<T>(r))); }
    template <typename T>
    SIMDIFY_FORCE_INLINE expr::utof<T&&> utof(T&& r) { return expr::utof<T&&>(std::forward<T>(r)); }
    template <typename T>
    SIMDIFY_FORCE_INLINE expr::stof<T&&> stof(T&& r) { return expr::stof<T&&>(std::forward<T>(r)); }
    template <typename T>
    SIMDIFY_FORCE_INLINE expr::tou<T&&> tou(T&& r) { return expr::tou<T&&>(std::forward<T>(r)); }
    template <typename T>
    SIMDIFY_FORCE_INLINE expr::tos<T&&> tos(T&& r) { return expr::tos<T&&>(std::forward<T>(r)); }
}

#endif // SIMDIFY_EXPR
