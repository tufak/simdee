#ifndef SIMDIFY_ARRAY_OF_VECTORS
#define SIMDIFY_ARRAY_OF_VECTORS

#include "containers_common.hpp"

namespace simd {

    template <typename Simd_t, typename Ids, typename Sequence>
    struct array_of_vectors_impl;

    template <typename Simd_t, id... Ids, std::size_t... I>
    struct array_of_vectors_impl<Simd_t, id_sequence<Ids...>, sequence<I...>> {
        static_assert(sizeof...(Ids) == sizeof...(I), "array_of_vectors_impl: sequence size mismatch");

        using self_t = array_of_vectors_impl;
        using simd_t = Simd_t;
        using f_t = typename simd_t::f_t;
        using mm_t = typename simd_t::mm_t;

        static_assert(std::is_trivial<f_t>::value, "array_of_vectors_impl: f_t not trivial");

        enum : std::size_t { N = sizeof...(Ids), W = simd_t::W };

        using value_type = named_array<f_t, Ids...>;
        using value_type_vector = named_array<simd_t, Ids...>;
        using reference = named_array<simd::reference<simd::storage<f_t>>, Ids...>;
        using const_reference = named_array<simd::const_reference<simd::storage<f_t>>, Ids...>;
        using reference_vector = value_type_vector&;
        using const_reference_vector = const value_type_vector&;

        SIMDIFY_CONTAINERS_COMMON_CONSTRUCTION(array_of_vectors_impl);

        void reserve(std::size_t count) {
            if (count <= m_cap) return;

            std::size_t new_cap = m_cap;
            if (new_cap == 0) {
                new_cap = div_ceil_shift<W>(count);
            }
            else do { new_cap *= 2; } while (new_cap < count);

            decltype(m_data) new_data(aligned_malloc<f_t, alignof(mm_t)>(N * new_cap), aligned_deleter{});

            if (!new_data) throw std::bad_alloc{};

            if (m_sz != 0) {
                std::memcpy(new_data.get(), m_data.get(), N*sizeof(f_t)*m_sz);
            }

            std::swap(m_data, new_data);
            m_cap = new_cap;
        }

        void fill(const value_type& val, std::size_t from = 0) {
            auto first = begin();
            auto last = end();
            first += from;
            
            for (; first < last; ++first) {
                detail::no_op(simd::get<I>(*first) = simd::get<I>(val)...);
            }
        }

        SIMIDFY_CONTAINERS_COMMON_SIZE_MANAGEMENT;

        reference operator[](std::size_t i) {
            reference res;
            auto base = m_data.get() + (N*W)*div_floor<W>(i) + mod<W>(i);
            detail::no_op(simd::get<I>(res).reset(base + I*W)...);
            return res;
        }

        const_reference operator[](std::size_t i) const {
            const_reference res;
            auto base = m_data.get() + (N*W)*div_floor<W>(i) + mod<W>(i);
            detail::no_op(simd::get<I>(res).reset(base + I*W)...);
            return res;
        }

        SIMDIFY_CONTAINERS_COMMON_ACCESS("structure_of_arrays");

        void push_back(const value_type& val) {
            reserve(m_sz + 1);
            auto base = m_data.get() + (N*W)*div_floor<W>(m_sz) + mod<W>(m_sz);
            detail::no_op(*(base + I*W) = simd::get<I>(val)...);
            ++m_sz;
        }

        SIMDIFY_CONTAINERS_COMMON_POP_BACK("structure_of_arrays");

        template <typename Val>
        struct value_iterator : std::iterator<std::forward_iterator_tag, Val> {
            value_iterator(const self_t& self, std::size_t idx) :
                m_ptr(self.data_as_value_vector_type_ptr() + (idx / W)) {}

            value_iterator& operator++() { ++m_ptr; return *this; }

            bool operator==(const value_iterator& rhs) const { return m_ptr == rhs.m_ptr; }
            bool operator!=(const value_iterator& rhs) const { return m_ptr != rhs.m_ptr; }
            Val& operator*() { return *m_ptr; }
            Val* operator->() { return m_ptr; }

        private:
            Val* m_ptr;
        };

        template <typename Ref>
        struct reference_iterator : std::iterator<std::forward_iterator_tag, Ref> {
            reference_iterator(const reference_iterator&) = default;
            reference_iterator& operator=(const reference_iterator&) = default;

            reference_iterator(const self_t& self, std::size_t idx) {
                auto base = self.m_data.get() + (N*W)*div_floor<W>(idx) + mod<W>(idx);
                detail::no_op(simd::get<I>(m_ref).reset(base + I*W)...);
                pos = idx;
            }

            reference_iterator& operator++() {
                auto incr = (mod<W>(pos) == W - 1) ? ((N - 1)*W + 1) : 1;
                detail::no_op(simd::get<I>(m_ref).ptr() += incr...);
                ++pos;
                return *this;
            }

            reference_iterator& operator--() {
                auto decr = (mod<W>(pos) == 0) ? ((N - 1)*W + 1) : 1;
                detail::no_op(simd::get<I>(m_ref).ptr() -= decr...);
                --pos;
                return *this;
            }

            reference_iterator& operator+=(std::ptrdiff_t add) {
                enum : std::ptrdiff_t { sigW = W, sigN = N };
                auto tail = static_cast<std::ptrdiff_t>(mod<W>(pos));
                tail = (add >= 0) ? tail : (tail - sigW + 1);
                auto ta = (tail + add) / sigW;
                auto nw = ((sigN - 1)*sigW);
                auto incr = add + ta*nw;
                detail::no_op(simd::get<I>(m_ref).ptr() += incr...);
                pos += add;
                return *this;
            }

            std::ptrdiff_t operator-(const reference_iterator& rhs) {
                return static_cast<std::ptrdiff_t>(pos - rhs.pos);
            }

            bool operator<(const reference_iterator& rhs) const { return pos < rhs.pos; }
            bool operator<=(const reference_iterator& rhs) const { return pos <= rhs.pos; }
            bool operator==(const reference_iterator& rhs) const { return pos == rhs.pos; }
            Ref& operator*() { return m_ref; }
            Ref* operator->() { return &m_ref; }

            ////

            reference_iterator& operator-=(std::ptrdiff_t sub) {
                return operator+=(-sub);
            }

            reference_iterator operator+(std::ptrdiff_t add) {
                reference_iterator res(*this);
                res += add;
                return res;
            }

            friend reference_iterator operator+(std::ptrdiff_t add, const reference_iterator& rhs) {
                return rhs + add;
            }

            reference_iterator operator++(int) {
                reference_iterator res(*this);
                operator++();
                return res;
            }

            reference_iterator operator--(int) {
                reference_iterator res(*this);
                operator--();
                return res;
            }

            bool operator>=(const reference_iterator& rhs) const { return !operator<(rhs); }
            bool operator>(const reference_iterator& rhs) const { return !operator<=(rhs); }
            bool operator!=(const reference_iterator& rhs) const { return !operator==(rhs); }

            ////

        private:
            Ref m_ref;
            std::size_t pos;
        };

        using iterator = reference_iterator<reference>;
        using const_iterator = reference_iterator<const_reference>;
        using iterator_vector = value_iterator<value_type_vector>;
        using const_iterator_vector = value_iterator<const value_type_vector>;

        SIMDIFY_CONTAINERS_COMMON_ITERATION;

    private:
        value_type_vector* data_as_value_vector_type_ptr() const {
            return static_cast<value_type_vector*>(static_cast<void*>(m_data.get()));
        }

        SIMDIFY_CONTAINERS_COMMON_DATA;
    };

    template <typename Simd_t, id... Ids>
    using array_of_vectors = array_of_vectors_impl<Simd_t, id_sequence<Ids...>, make_sequence_t<0, sizeof...(Ids)>>;

}

#endif // SIMDIFY_ARRAY_OF_VECTORS