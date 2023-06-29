#pragma once
#include "basic_vector.hpp"
namespace force {

    template <typename Ty>
    class vector2;

    template <>
    class vector2<float> : public basic_vec128<float> {
    public:
        static constexpr unsigned int dimension = 2;
        explicit vector2()                                 :basic_vec128<float>() {}
        explicit vector2(const basic_vec128<float>& right) :basic_vec128<float>(right) {}
        explicit vector2(basic_vec128<float>&& right)      :basic_vec128<float>(right) {}
        vector2(float x, float y)                          :basic_vec128<float>() { 
            m_data[0] = x; m_data[1] = y; m_data[2] = m_data[3] = 0; 
        }
        const float& operator[](unsigned int i) const { if (i > 1) throw "vector2 subscript out of range"; return m_data[i]; }
        float&       operator[](unsigned int i)       { if (i > 1) throw "vector2 subscript out of range"; return m_data[i]; }
        ~vector2() override = default;
    };

    template <typename Ty>
    class vector3;

    template <>
    class vector3<float> : public basic_vec128<float> {
    public:
        static constexpr unsigned int dimension = 3;
        explicit vector3() :basic_vec128<float>() {}
        explicit vector3(const basic_vec128<float>& right) :basic_vec128<float>(right) {}
        explicit vector3(basic_vec128<float>&& right)      :basic_vec128<float>(right) {}
        vector3(float x, float y, float z)                 :basic_vec128<float>() {
            m_data[0] = x; m_data[1] = y; m_data[2] = z; m_data[3] = 0;
        }
        const float& operator[](unsigned int i) const { if (i > 2) throw "vector3 subscript out of range"; return m_data[i]; }
        float&       operator[](unsigned int i)       { if (i > 2) throw "vector3 subscript out of range"; return m_data[i]; }
        ~vector3() override = default;
    };

    template <typename Ty>
    class vector4;

    template <>
    class vector4<float> : public basic_vec128<float> {
    public:
        static constexpr unsigned int dimension = 4;
        explicit vector4() :basic_vec128<float>() {}
        explicit vector4(const basic_vec128<float>& right) :basic_vec128<float>(right) {}
        explicit vector4(basic_vec128<float>&& right)      :basic_vec128<float>(right) {}
        vector4(float x, float y, float z, float w)        :basic_vec128<float>() {
            m_data[0] = x; m_data[1] = y; m_data[2] = z; m_data[3] = w;
        }
        const float& operator[](unsigned int i) const { return m_data[i]; }
        float&       operator[](unsigned int i)       { return m_data[i]; }
        ~vector4() override = default;
    };

    template <>
    class vector2<double> : public basic_vec128<double> {
    public:
        vector2()                                  :basic_vec128<double>() {}
        vector2(const basic_vec128<double>& right) :basic_vec128<double>(right) {}
        vector2(basic_vec128<double>&& right)      :basic_vec128<double>(right) {}
        vector2(double x, double y)                :basic_vec128<double>() { m_data[0] = x; m_data[1] = y; }

        const double& operator[](unsigned int i) const { if (i > 1) throw "vector2 subscript out of range"; return m_data[i]; }
        double& operator[](unsigned int i)             { if (i > 1) throw "vector2 subscript out of range"; return m_data[i]; }

        ~vector2() override = default;
    };
    // Use these definitions instead of using basic_vector.
    // These defs are simd optimized if you turn on simd.
    using vec2  = vector2<float>;
    using vec3  = vector3<float>;
    using vec4  = vector4<float>;

    // Define type_traits and concepts.
    template <typename Ty>
    struct vector_traits : public std::false_type {};
    template <>
    struct vector_traits<vec2> : public std::true_type {};
    template <>
    struct vector_traits<vec3> : public std::true_type {};
    template <>
    struct vector_traits<vec4> : public std::true_type {};
    template <typename Ty>
    concept vector = vector_traits<Ty>::value;

#if !defined FORCE_USE_SIMD_VECTOR
    // Define functions for vectors.
    template <vector Vec>
    Vec::value_type dot(const Vec& v1, const Vec& v2) {
        auto sum = static_cast<Vec::value_type>(0);
        for (std::size_t i = 0; i < Vec::dimension; ++i) sum += v1[i] * v2[i];
        return sum;
    }
    template <vector Vec>
    Vec::value_type modulus(const Vec& v) {
        auto sum = static_cast<Vec::value_type>(0);
        for (std::size_t i = 0; i < Vec::dimension; ++i) sum += v[i] * v[i];
        return static_cast<Vec::value_type>(::force::sqrt(sum));
    }
    template <vector Vec>
    const bool operator==(const Vec& v1, const Vec& v2) {
        return std::memcmp(v1.data(), v2.data(), Vec::memsize) == 0;
    }
// #else // This will avalible when simd supports.
#endif
}