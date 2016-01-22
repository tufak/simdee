#ifndef SIMDIFY_NAMED_ARRAY
#define SIMDIFY_NAMED_ARRAY

#include "expr.hpp"
#include "../util/inline.hpp"
#include <tuple>
#include <array>
#include <type_traits>

///////////////////////////////////////////////////////////////////////////////////////////////////
#define SIMDIFY_ADD_IDENTIFIER(ID)                                                                \
                                                                                                  \
namespace simd {                                                                                  \
    namespace id {                                                                                \
        struct ID {                                                                               \
            enum : std::size_t { size = 1 };                                                      \
            using reverse = ID;                                                                   \
        };                                                                                        \
    }                                                                                             \
                                                                                                  \
    namespace sub {                                                                               \
        template <typename... T>                                                                  \
        struct ID;                                                                                \
    }                                                                                             \
                                                                                                  \
    namespace detail {                                                                            \
        template <typename... T1, typename... T2>                                                 \
        struct join<sub::ID<T1...>, sub::ID<T2...>> {                                             \
            using type = sub::ID<T1..., T2...>;                                                   \
        };                                                                                        \
    }                                                                                             \
                                                                                                  \
    namespace sub {                                                                               \
        template <>                                                                               \
        struct ID<> {                                                                             \
            enum : std::size_t { size = 0 };                                                      \
            using reverse = ID<>;                                                                 \
        };                                                                                        \
                                                                                                  \
        template <typename T1, typename... T>                                                     \
        struct ID<T1, T...> {                                                                     \
            enum : std::size_t { size = T1::size + ID<T...>::size };                              \
            using reverse = detail::join_t<typename ID<T...>::reverse, ID<typename T1::reverse>>; \
        };                                                                                        \
    }                                                                                             \
                                                                                                  \
    namespace detail {                                                                            \
        template <int D, typename T, typename... Ids>                                             \
        struct pack<D, T, group<id::ID, Ids...>> : pack<D, T, group<Ids...>> {                    \
            using base_t = pack<D, T, group<Ids...>>;                                             \
                                                                                                  \
            SIMDIFY_INL constexpr pack() = default;                                      \
            SIMDIFY_INL constexpr pack(const pack&) = default;                           \
            SIMDIFY_INL constexpr pack(pack&&) = default;                                \
            SIMDIFY_INL pack& operator=(const pack&) = default;                          \
            SIMDIFY_INL pack& operator=(pack&&) = default;                               \
                                                                                                  \
            template <typename... Args>                                                           \
            SIMDIFY_INL constexpr pack(const std::tuple<Args...>& t) :                   \
                base_t(t),                                                                        \
                ID(std::get<sizeof...(Ids)>(t)) {}                                                \
                                                                                                  \
            template <typename... Args>                                                           \
            SIMDIFY_INL pack& operator=(const std::tuple<Args...>& t) {                  \
                base_t::operator=(t);                                                             \
                ID = std::get<sizeof...(Ids)>(t);                                                 \
                return *this;                                                                     \
            }                                                                                     \
                                                                                                  \
            T ID;                                                                                 \
        };                                                                                        \
                                                                                                  \
        template <int D, typename T, typename... MyNames, typename... Ids>                        \
        struct pack<D, T, group<sub::ID<MyNames...>, Ids...>> : pack<D, T, group<Ids...>> {       \
            using base_t = pack<D, T, group<Ids...>>;                                             \
                                                                                                  \
            SIMDIFY_INL constexpr pack() = default;                                      \
            SIMDIFY_INL constexpr pack(const pack&) = default;                           \
            SIMDIFY_INL constexpr pack(pack&&) = default;                                \
            SIMDIFY_INL pack& operator=(const pack&) = default;                          \
            SIMDIFY_INL pack& operator=(pack&&) = default;                               \
                                                                                                  \
            template <typename... Args>                                                           \
            SIMDIFY_INL constexpr pack(const std::tuple<Args...>& t) :                   \
                base_t(t),                                                                        \
                ID(std::get<sizeof...(Ids)>(t)) {}                                                \
                                                                                                  \
            template <typename... Args>                                                           \
            SIMDIFY_INL pack& operator=(const std::tuple<Args...>& t) {                  \
                base_t::operator=(t);                                                             \
                ID = std::get<sizeof...(Ids)>(t);                                                 \
                return *this;                                                                     \
            }                                                                                     \
                                                                                                  \
            pack<D + 1, T, group<MyNames...>> ID;                                                 \
        };                                                                                        \
    }                                                                                             \
}                                                                                                 \
                                                                                                  \
///////////////////////////////////////////////////////////////////////////////////////////////////

namespace simd {

    namespace detail {
        template <typename T1, typename T2>
        struct join;

        template <typename T1, typename T2>
        using join_t = typename join<T1, T2>::type;

        template <typename... T>
        struct group;

        template <typename... T1, typename... T2>
        struct join<group<T1...>, group<T2...>> {
            using type = group<T1..., T2...>;
        };

        template <>
        struct group<> {
            enum : std::size_t { size = 0 };
            using reverse = group<>;
        };

        template <typename T1, typename... T>
        struct group<T1, T...> {
            enum : std::size_t { size = T1::size + group<T...>::size };
            using reverse = detail::join_t<typename group<T...>::reverse, group<typename T1::reverse>>;
        };

        template <typename... T>
        using reverse_group = typename group<T...>::reverse;

        template <int D, typename T, typename Group>
        struct pack;

        template <int D, typename T>
        struct pack<D, T, group<>> {
            SIMDIFY_INL constexpr pack() = default;
            SIMDIFY_INL constexpr pack(const pack&) = default;
            SIMDIFY_INL constexpr pack(pack&&) = default;
            SIMDIFY_INL pack& operator=(const pack&) = default;
            SIMDIFY_INL pack& operator=(pack&&) = default;

            template <typename... Args>
            SIMDIFY_INL constexpr pack(const std::tuple<Args...>&) {}

            template <typename... Args>
            SIMDIFY_INL pack& operator=(const std::tuple<Args...>&) { return *this; }
        };
    }
}

SIMDIFY_ADD_IDENTIFIER(a);
SIMDIFY_ADD_IDENTIFIER(b);
SIMDIFY_ADD_IDENTIFIER(c);
SIMDIFY_ADD_IDENTIFIER(d);
SIMDIFY_ADD_IDENTIFIER(e);
SIMDIFY_ADD_IDENTIFIER(f);
SIMDIFY_ADD_IDENTIFIER(g);
SIMDIFY_ADD_IDENTIFIER(h);
SIMDIFY_ADD_IDENTIFIER(i);
SIMDIFY_ADD_IDENTIFIER(j);
SIMDIFY_ADD_IDENTIFIER(k);
SIMDIFY_ADD_IDENTIFIER(l);
SIMDIFY_ADD_IDENTIFIER(m);
SIMDIFY_ADD_IDENTIFIER(n);
SIMDIFY_ADD_IDENTIFIER(o);
SIMDIFY_ADD_IDENTIFIER(p);
SIMDIFY_ADD_IDENTIFIER(q);
SIMDIFY_ADD_IDENTIFIER(r);
SIMDIFY_ADD_IDENTIFIER(s);
SIMDIFY_ADD_IDENTIFIER(t);
SIMDIFY_ADD_IDENTIFIER(u);
SIMDIFY_ADD_IDENTIFIER(v);
SIMDIFY_ADD_IDENTIFIER(w);
SIMDIFY_ADD_IDENTIFIER(x);
SIMDIFY_ADD_IDENTIFIER(y);
SIMDIFY_ADD_IDENTIFIER(z);

namespace simd {
    namespace detail {
        template <typename... Args>
        SIMDIFY_INL constexpr int no_op(Args&&...) { return 0; }
    }

    template <typename T, typename... Ids>
    struct named_array : detail::pack<0, T, detail::reverse_group<Ids...>> {
        using group_t = detail::reverse_group<Ids...>;
        using base_t = detail::pack<0, T, group_t>;
        enum : std::size_t { N = group_t::size };

        constexpr named_array() = default;
        constexpr named_array(const named_array&) = default;
        constexpr named_array(named_array&&) = default;
        named_array& operator=(const named_array&) = default;
        named_array& operator=(named_array&&) = default;

        template <typename U>
        named_array(const named_array<U, Ids...>& rhs) {
            auto& la = as_array();
            auto& ra = rhs.as_array();

            for (int i = 0; i < N; ++i) {
                new (&la[i]) T(ra[i]);
            }
        }

        template <typename U>
        named_array(named_array<U, Ids...>& rhs) {
            auto& la = as_array();
            auto& ra = rhs.as_array();

            for (int i = 0; i < N; ++i) {
                new (&la[i]) T(ra[i]);
            }
        }

        template <typename U>
        named_array& operator=(const named_array<U, Ids...>& rhs) {
            auto& la = as_array();
            auto& ra = rhs.as_array();

            for (int i = 0; i < N; ++i) {
                la[i] = ra[i];
            }

            return *this;
        }

        template <typename U>
        named_array& operator=(named_array<U, Ids...>& rhs) {
            auto& la = as_array();
            auto& ra = rhs.as_array();

            for (int i = 0; i < N; ++i) {
                la[i] = ra[i];
            }

            return *this;
        }

        template <typename... Args>
        SIMDIFY_INL constexpr named_array(const std::tuple<Args...>& t) : base_t(t) {
            static_assert(sizeof...(Args) == sizeof...(Ids), "named_array: incorrect number of parameters");
        }

        template <typename... Args>
        SIMDIFY_INL named_array& operator=(const std::tuple<Args...>& t) {
            static_assert(sizeof...(Args) == sizeof...(Ids), "named_array operator=: incorrect number of parameters");
            base_t::operator=(t);
            return *this;
        }

        SIMDIFY_INL T& operator[](std::size_t i) {
            return as_array()[i];
        }

        SIMDIFY_INL const T& operator[](std::size_t i) const {
            return as_array()[i];
        }

        template <std::size_t I = 0>
        SIMDIFY_INL T& get() {
            return std::get<I>(as_array());
        }

        template <std::size_t I = 0>
        SIMDIFY_INL const T& get() const {
            return std::get<I>(as_array());
        }

        SIMDIFY_INL void swap(named_array& rhs) {
            if (std::is_trivial<T>::value) {
                std::swap(*this, rhs);
            }
            else {
                as_array().swap(rhs.as_array());
            }
        }

        SIMDIFY_INL std::array<T, N>& as_array() {
            return reinterpret_cast<std::array<T, N>&>(*this);
        }
        SIMDIFY_INL const std::array<T, N>& as_array() const {
            return reinterpret_cast<const std::array<T, N>&>(*this);
        }
    };

    template <typename T, typename... Ids>
    SIMDIFY_INL void swap(named_array<T, Ids...>& lhs, named_array<T, Ids...>& rhs) {
        return lhs.swap(rhs);
    }

    template <size_t I, typename T, typename... Ids>
    SIMDIFY_INL constexpr T& get(simd::named_array<T, Ids...>& a) {
        return a.template get<I>();
    }
    template <size_t I, typename T, typename... Ids>
    SIMDIFY_INL constexpr T&& get(simd::named_array<T, Ids...>&& a) {
        return std::move(a.template get<I>());
    }
    template <size_t I, typename T, typename... Ids>
    SIMDIFY_INL constexpr const T& get(const simd::named_array<T, Ids...>& a) {
        return a.template get<I>();
    }
    template <size_t I, typename T, typename... Ids>
    SIMDIFY_INL constexpr const T&& get(const simd::named_array<T, Ids...>&& a) {
        return std::move(a.template get<I>());
    }
}

#endif // SIMDIFY_NAMED_ARRAY
