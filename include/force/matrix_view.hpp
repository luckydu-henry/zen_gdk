///
/// \file      matrix_view.hpp
/// \brief     Mapping 1D data to 2D.
/// \details   ~
/// \author    HenryDu
/// \date      11.06.2024
/// \copyright © HenryDu 2024. All right reserved.
///
#pragma once
#include <iterator>

#include "primary.hpp"
#include "vector.hpp"
#include "force/vector_view.hpp"
namespace force {
    ///
    /// \class   matrix_view
    /// \brief   A view to map 1D data to 2D matrix. Index follows row major access.
    /// \details ~
    /// \tparam  Ty - 
    ///
    template <typename Ty>
    class matrix_view {
    public:
        using value_type         = Ty;
        using reference          = value_type&;
        using const_reference    = const value_type&;
        using pointer            = value_type*;
        using const_pointer      = const value_type*;

        // Point type is for locate.
        using point_type         = const vector_view<std::ptrdiff_t>;
        using row_coord          = point_type::value_type;
        using col_coord          = point_type::value_type;

        // Matrix view does not support linear iteration.
        using row_view           = vector_view<value_type>;
        using const_row_view     = const vector_view<const value_type>;
        using col_view           = vector_view<value_type>;
        using const_col_view     = const vector_view<const value_type>;
        using row_iterator       = vector_iterator<value_type>;
        using const_row_iterator = const vector_iterator<const value_type>;
        using col_iterator       = vector_iterator<value_type>;
        using const_col_iterator = const vector_iterator<const value_type>;

        constexpr matrix_view() = default;
        /// \brief matrix_view object constructor
        /// \param p  - Pointer to data.
        /// \param x  - x coordinate of linear data use p as origin.
        /// \param y  - y coordinate of linear data use p as origin.
        /// \param w  - width
        /// \param h  - height
        /// \param dy - How many elements between two row.
        /// \param dx - How many elements between two column.
        constexpr matrix_view(const_pointer p, 
                              const row_coord x,       const col_coord y, 
                              const std::size_t w,     const std::size_t h, 
                              const std::ptrdiff_t dy, const std::ptrdiff_t dx = 1) : mDeltaY(dy), mDeltaX(dx), mPtr(const_cast<pointer>(&p[y * dy + x * dx])), mWidth(w), mHeight(h) {}
        constexpr matrix_view(const matrix_view&) = default;
        constexpr matrix_view(matrix_view&&)      = default;

        constexpr matrix_view& operator=(const matrix_view&) = default;
        constexpr matrix_view& operator=(matrix_view&&)      = default;

        /// \brief   Iterate a matrix_view with index mode
        /// \example
        /// // Standard access sequence.
        /// matrix_view<...> view(...);
        /// for (auto y = 0; y != view.height(); ++y) {
        ///     for (auto x = 0; x != view.width(); ++x) {
        ///        view[y * view.row_delta() + x * view.col_delta()] = <value>;
        ///     }
        /// }
        constexpr std::ptrdiff_t  row_delta()  const { return mDeltaY; }
        constexpr std::ptrdiff_t  col_delta()  const { return mDeltaX; }
        constexpr std::size_t     width()      const { return mWidth; }
        constexpr std::size_t     height()     const { return mHeight; }
        constexpr pointer         data()             { return mPtr; }
        constexpr const_pointer   data()       const { return mPtr; }
        constexpr std::size_t     size()       const { return mWidth * mHeight; }

        constexpr reference        operator[](std::ptrdiff_t i)       { return mPtr[i]; }
        constexpr const_reference  operator[](std::ptrdiff_t i) const { return mPtr[i]; }
        constexpr reference        operator[](const point_type p)       { return mPtr[p[1] * mDeltaY + p[0] * mDeltaX]; }
        constexpr const_reference  operator[](const point_type p) const { return mPtr[p[1] * mDeltaY + p[0] * mDeltaX]; }
        // With C++26 comes in future the second-dimensional operator[] will be provided as soon as it can.

        // Get one row or a column.
        constexpr row_view         row_at(row_coord i)       { return row_view(mPtr + i * mDeltaY, 0, mWidth, mDeltaX); }
        constexpr const_row_view   row_at(row_coord i) const { return const_row_view(mPtr + i * mDeltaY, 0, mWidth, mDeltaX); }
        constexpr col_view         col_at(col_coord i)       { return col_view(mPtr + i * mDeltaX, 0, mHeight, mDeltaY); }
        constexpr const_col_view   col_at(col_coord i) const { return const_col_view(mPtr + i * mDeltaX, 0, mHeight, mDeltaY); }

        /// \brief  
        /// \example
        /// matrix_view<...> view(...);
        /// for (auto i = view.row_begin(); i != view.row_end(); ++i) {
        ///     for (auto j = view.col_begin(i); j != view.col_end(i); ++j) {
        ///        *j = value();
        ///     }
        /// }
        /// // column first access is similar.
        constexpr row_iterator     row_begin()               { return row_iterator(mPtr, mDeltaY); }
        constexpr row_iterator     row_end  ()               { return row_begin() + mHeight; }
        constexpr col_iterator     col_begin(row_iterator i) { return col_iterator(&i[0], mDeltaX); }
        constexpr col_iterator     col_end  (row_iterator i) { return col_begin(i) + mWidth; }
        constexpr col_iterator     col_begin()               { return col_iterator(mPtr, mDeltaX); }
        constexpr col_iterator     col_end  ()               { return col_begin() + mWidth; }
        constexpr row_iterator     row_begin(col_iterator i) { return row_iterator(&i[0], mDeltaY); }
        constexpr row_iterator     row_end  (col_iterator i) { return row_begin(i) + mHeight; }

        constexpr const_row_iterator     row_begin()                     const { return const_row_iterator(mPtr, mDeltaY); }
        constexpr const_row_iterator     row_end  ()                     const { return row_begin() + mHeight; }
        constexpr const_col_iterator     col_begin(const_row_iterator i) const { return const_col_iterator(&i[0], mDeltaX); }
        constexpr const_col_iterator     col_end  (const_row_iterator i) const { return col_begin(i) + mWidth; }
        constexpr const_col_iterator     col_begin()                     const { return const_col_iterator(mPtr, mDeltaX); }
        constexpr const_col_iterator     col_end  ()                     const { return col_begin() + mWidth; }
        constexpr const_row_iterator     row_begin(const_col_iterator i) const { return const_row_iterator(&i[0], mDeltaY); }
        constexpr const_row_iterator     row_end  (const_col_iterator i) const { return row_begin(i) + mHeight; }

        constexpr matrix_view view(std::ptrdiff_t tx, std::ptrdiff_t ty, std::size_t w, std::size_t h) const {
            return matrix_view(mPtr, tx, ty, w, h, mDeltaY, mDeltaX);
        }

        // Only these two can have by a matrix.
        constexpr matrix_view& operator*=(const value_type v) {
            for (std::size_t i = 0; i != mHeight; ++i) { std::ranges::for_each(row_at(i), [v](auto& k) {k *= v; }); }
            return *this;
        }
        constexpr matrix_view& operator/=(const value_type v) {
            for (std::size_t i = 0; i != mHeight; ++i) { std::ranges::for_each(row_at(i), [v](auto& k) {k /= v; }); }
            return *this;
        }
        constexpr bool         operator==(const matrix_view& view) const {
            for (std::size_t i = 0; i != mHeight; ++i) {
                if (row_at(i) != view.row_at(i))  return false;
            }
            return true;
        }
    private:
        std::ptrdiff_t  mDeltaY, mDeltaX;
        pointer         mPtr;
        std::size_t     mWidth, mHeight;
    };

    // Since matrix_view can't have a proper iterator so for_each_view and copy_view are especially for users
    // to play with it easier.

    /// \brief You should use this instead of writing your own loop.
    /// \tparam Src  - Your view's value type.
    /// \tparam F1   - Inner function, you should do your all modification here.
    /// \tparam F2   - Not related to specific data so you can do print("\n") or add padding here.
    /// \example
    /// // This example shows how to use a super generic second order loop output value as integer and truncate all values.
    /// matrix_view<float> view(...);
    /// for_each_view(view, [](auto& p) {
    ///     auto integer_value = static_cast<int>(p);
    ///     std::cout << value;
    ///     p = static_cast<float>(integer_value);
    /// },
    /// [](auto&) { std::cout << std::endl; } ); // Output with std::endl.
    /// \retval      - End pointer.
    template <typename Src, typename F1, typename F2>
    constexpr decltype(auto) for_each_view(matrix_view<Src> view, F1 fi, F2 fo) {
        auto i = view.row_begin();
        for (; i != view.row_end(); ++i) {
            auto   j = view.col_begin(i);
            for (; j != view.col_end(i); ++j) {
                // Accept *j as a reference.
                std::invoke(fi, *j);
            }
            // Also accept *i as a reference.
            std::invoke(fo, *i);
        }
        return i;
    }
    template <typename Src, typename F>
    constexpr decltype(auto) for_each_view(matrix_view<Src> view, F f) {
        return for_each_view(view, f, [](auto&) {});
    }
    /// \brief  Copy a view with desired copy rule.
    /// \tparam OutIt - Output iterator.
    /// \tparam Src   - View type.
    /// \tparam RuleF - Type of rule function
    /// \param  view  - matrix_view
    /// \param  dest  - destination output iterator.
    /// \param  f     - copy rule function which takes in destination pointer and view position pointer as arguments.
    ///
    /// \example
    /// // This example shows how to copy a 2D rgb view to 1D array -- a super generic 2D copy.
    /// matrix_view<std::uint8_t> view(...)
    /// auto* destination = new std::uint8_t[ w * h * bpp ];
    /// copy_view(view, destination, [](std::uint8_t* dest, std::uint8_t& src) {
    ///    *dest++ = (&src)[0];
    ///    *dest++ = (&src)[1];
    ///    *dest++ = (&src)[2];
    /// });
    ///
    /// \retval       - Return the end of destination pointer.
    template <typename OutIt, typename Src, typename RuleF>
    constexpr OutIt copy_view(const matrix_view<Src> view, OutIt dest, RuleF f) {
        for (auto i = view.row_begin(); i != view.row_end(); ++i) {
            for (auto j = view.col_begin(i); j != view.col_end(i); ++j) {
                // Accept dest as OutIt& and *j as Src&.
                std::invoke(f, dest, *j);
            }
        }
        return dest;
    }
    /// \brief A specialization when Dst and Src are the same.
    template <typename OutIt, typename Ty> requires std::is_convertible_v<std::iter_value_t<OutIt>, Ty>
    constexpr decltype(auto) copy_view(const matrix_view<Ty> view, OutIt dest) {
        return copy_view(view, dest, [](OutIt& d, const Ty& v) { *d++ = v; });
    }

    // Matrix rigid transformation -- these transformation won't change the size of raw data
    // But some of them will modify content.

    /// \brief transpose
    template <typename Ty>
    constexpr decltype(auto) transpose_view(const matrix_view<Ty> view) {
        return matrix_view<Ty>(view.data(), 0, 0, view.height(), view.width(), view.col_delta(), view.row_delta());
    }
    /// \brief  Horizontal flip
    template <typename Ty>
    constexpr decltype(auto) reverse_row_view(const matrix_view<Ty> view) {
        return matrix_view<Ty>(view.data(), 1 - view.width(), 0, view.width(), view.height(), view.row_delta(), -view.col_delta());
    }
    /// \brief  Vertical flip.
    template <typename Ty>
    constexpr decltype(auto) reverse_col_view(const matrix_view<Ty> view) {
        return matrix_view<Ty>(view.data(), 0, 1 - view.height(), view.width(), view.height(), -view.row_delta(), view.col_delta());
    }
    /// \brief  Rotate the view 90 degrees counterclockwise
    template <typename Ty>
    constexpr decltype(auto) rotate_view_half_pi(const matrix_view<Ty> view) {
        return matrix_view<Ty>(view.data(), 0, 1 - view.width(), view.height(), view.width(), -view.col_delta(), view.row_delta());
    }
    /// \brief  Rotate the view 90 degrees clockwise or rotate the view 270 degrees counterclockwise
    template <typename Ty>
    constexpr decltype(auto) rotate_view_neg_half_pi(const matrix_view<Ty> view) {
        return matrix_view<Ty>(view.data(), 1 - view.height(), 0, view.height(), view.width(), view.col_delta(), -view.row_delta());
    }
    /// \brief  Rotate the view 180 degrees or central symmetry
    template <typename Ty>
    constexpr decltype(auto) rotate_view_pi(const matrix_view<Ty> view) {
        return matrix_view<Ty>(view.data(), 1 - view.width(), 1 - view.height(), view.width(), view.height(), -view.row_delta(), -view.col_delta());;
    }

    // Projection transformation will need a new destination to store matrix after transformation.

    template <typename Ty>
    constexpr decltype(auto) index_scaled_view(const matrix_view<Ty> src, const matrix_view<Ty> dest, vector_view<std::ptrdiff_t> p) {
        using real_t = float64_t;
        auto x = static_cast<std::ptrdiff_t>(force::round(static_cast<real_t>(p[0]) * static_cast<real_t>(dest.width())  / static_cast<real_t>(src.width())));
        auto y = static_cast<std::ptrdiff_t>(force::round(static_cast<real_t>(p[1]) * static_cast<real_t>(dest.height()) / static_cast<real_t>(src.height())));
        return std::make_pair(
            force::clamp<std::ptrdiff_t>(x, 0, src.width() - 1),
            force::clamp<std::ptrdiff_t>(y, 0, src.height() - 1)
        );
    }
    // Nearest interpolation is good when you are trying to scale down an image -- lower its resolution.
    template <typename Ty>
    constexpr decltype(auto) scale_view_nearest(const matrix_view<Ty> src, matrix_view<Ty> dest) {
        using point = vector<std::ptrdiff_t, 2>;
        for (std::ptrdiff_t dy = 0; dy != dest.height(); ++dy) {
            for (std::ptrdiff_t dx = 0; dx != dest.width(); ++dx) {
                auto [sx, sy] = index_scaled_view(src, dest, point(dx, dy));
                dest[point(dx, dy)] = src[point(sx, sy)];
            }
        }
    }
}