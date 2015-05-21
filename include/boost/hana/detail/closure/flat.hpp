/*!
@file
Defines `boost::hana::detail::flat_closure`.

@copyright Louis Dionne 2015
Distributed under the Boost Software License, Version 1.0.
(See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)
 */

#ifndef BOOST_HANA_DETAIL_CLOSURE_FLAT_HPP
#define BOOST_HANA_DETAIL_CLOSURE_FLAT_HPP

#include <boost/hana/detail/std/declval.hpp>
#include <boost/hana/detail/std/forward.hpp>
#include <boost/hana/detail/std/integer_sequence.hpp>
#include <boost/hana/detail/std/size_t.hpp>


namespace boost { namespace hana { namespace detail {
    template <detail::std::size_t n, typename Xn>
    struct element {
        Xn get;
        using get_type = Xn;
    };

    void swallow(...);

    // This type is only used for pattern matching.
    template <typename ...Xs>
    struct flat_closure_impl : Xs... {
        flat_closure_impl() = default;
        flat_closure_impl(flat_closure_impl&&) = default;
        flat_closure_impl(flat_closure_impl const&) = default;
        flat_closure_impl(flat_closure_impl&) = default;

        // Make sure the constructor is SFINAE-friendly.
        template <typename ...Ys, typename = decltype(swallow(
            (Xs{detail::std::declval<Ys>()}, void(), 0)...
        ))>
        constexpr flat_closure_impl(Ys&& ...y)
            : Xs{static_cast<Ys&&>(y)}...
        { }
    };

    template <typename Indices, typename ...Xs>
    struct make_flat_closure_impl;

    template <detail::std::size_t ...n, typename ...Xn>
    struct make_flat_closure_impl<detail::std::index_sequence<n...>, Xn...> {
        using type = flat_closure_impl<element<n, Xn>...>;
    };

    //! @ingroup group-details
    //! Minimalistic closure implemented through multiple inheritance.
    //!
    //! This is intended to be used as a building block for other more complex
    //! data structures. The following is guaranteed by a `flat_closure`:
    //! `flat_closure<X1, ..., Xn>` is a type representing a closure holding
    //! objects of types `X1, ..., Xn` (basically a tuple). More specifically,
    //! `flat_closure<X1, ..., Xn>` is an alias to
    //!
    //! @code
    //!     flat_closure_impl<
    //!         element<0, X0>,
    //!         element<1, X1>,
    //!         ...,
    //!         element<n, Xn>
    //!     >
    //! @endcode
    //!
    //! Note that this makes `flat_closure<X1, ..., Xn>` a dependent type,
    //! which means that pattern matching is not allowed on it.
    //! `flat_closure_impl` inherits publicly from each of these `element`s,
    //! which makes it possible to retrieve an object from the closure based
    //! on its index. Also note that `element<n, Xn>` is guaranteed to have a
    //! nested `get_type` alias equivalent to `Xn`, which is handy to retrieve
    //! that `Xn` without having to pattern match.
    template <typename ...Xs>
    using flat_closure = typename make_flat_closure_impl<
        detail::std::make_index_sequence<sizeof...(Xs)>, Xs...
    >::type;

    //! @ingroup group-details
    //! Get the nth element of a `flat_closure`.
    template <detail::std::size_t n, typename Xn>
    static constexpr Xn const&
    flat_closure_get(element<n, Xn> const& x)
    { return x.get; }

    template <detail::std::size_t n, typename Xn>
    static constexpr Xn&
    flat_closure_get(element<n, Xn>& x)
    { return x.get; }

    template <detail::std::size_t n, typename Xn>
    static constexpr Xn&&
    flat_closure_get(element<n, Xn>&& x)
    { return static_cast<element<n, Xn>&&>(x).get; }
}}} // end namespace boost::hana::detail

#endif // !BOOST_HANA_DETAIL_CLOSURE_FLAT_HPP
