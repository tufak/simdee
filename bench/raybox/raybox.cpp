// work around a STL bug in normal_distribution, where a double-to-float conversion produces a
// warning
#if defined(_MSC_VER)
#pragma warning(disable : 4244)
#endif

#define SIMDEE_NEED_INT 0
#include <simdee/simd_vectors/avx.hpp>
#include <simdee/util/allocator.hpp>

#include <chrono>
#include <functional>
#include <iostream>
#include <limits>
#include <random>
#include <vector>

const char* const hline =
    "===============================================================================\n";

auto now = []() { return std::chrono::high_resolution_clock::now(); };

template <typename Duration>
double to_ms(Duration dur) {
    using nanoseconds = std::chrono::nanoseconds;
    return static_cast<double>(std::chrono::duration_cast<nanoseconds>(dur).count()) / 1.e6;
}

double benchmark_ms(std::function<void()> func) {
    double best = std::numeric_limits<double>::infinity();
    for (int i = 0; i < 24; i++) {
        auto tp1 = now();
        func();
        auto tp2 = now();
        double time = to_ms(tp2 - tp1);
        best = std::min(best, time);
    }
    return best;
}

int main() {
    std::cout << hline << "Benchmark: Ray-box intersection\n";

    // allocate data
    struct RayBoxData1 {
        float minx, miny, minz, maxx, maxy, maxz;
    };
    struct alignas(__m256) RayBoxData8 {
        float minx[8], miny[8], minz[8], maxx[8], maxy[8], maxz[8];
    };
    struct alignas(__m256) RayBoxData8S {
        sd::avxf::storage_t minx, miny, minz, maxx, maxy, maxz;
    };
    const std::size_t dataSize8 = 1024 * 1024;
    const std::size_t dataSize1 = 8 * dataSize8;
    using vec8 = std::vector<RayBoxData8, sd::allocator<RayBoxData8>>;
    using vec1 = std::vector<RayBoxData1>;
    using vec8S = std::vector<RayBoxData8S, sd::allocator<RayBoxData8S>>;
    vec8 data8(dataSize8);
    vec1 data1(dataSize1);
    const vec8S& data8S = reinterpret_cast<const vec8S&>(data8);
    enum class Result : char { fail = 13, win = 42 };
    std::vector<Result> resultsNonSimd(dataSize1);
    std::vector<Result> resultsHandSimd(dataSize1);
    std::vector<Result> resultsSimdee(dataSize1);

    // fill data
    std::minstd_rand re(0x8a7ac012);
    std::normal_distribution<float> dist(0, 1);
    auto fill = [&dist, &re](float* data, std::size_t num) {
        for (size_t i = 0; i < num; ++i) { *(data++) = dist(re); }
    };
    const std::size_t numFloats = (sizeof(RayBoxData1) / sizeof(float)) * dataSize1;
    fill(reinterpret_cast<float*>(data8.data()), numFloats);

    {
        RayBoxData1* ptr = data1.data();
        for (auto i = 0U; i < dataSize8; ++i) {
            const auto& el = data8[i];
            for (int j = 0; j < 8; ++j) {
                ptr->minx = el.minx[j];
                ptr->miny = el.miny[j];
                ptr->minz = el.minz[j];
                ptr->maxx = el.maxx[j];
                ptr->maxy = el.maxy[j];
                ptr->maxz = el.maxz[j];
                ptr++;
            }
        }
    }

    // common pre-calculations
    auto gamma = [](int n) {
        double eps2 = 0.5 * static_cast<double>(std::numeric_limits<float>::epsilon());
        return static_cast<float>((n * eps2) / (1 - n * eps2));
    };
    struct Vec3f {
        float x, y, z;
    };
    const float robustFactor = 1 + 2 * gamma(3);
    const Vec3f invDir = {dist(re), dist(re), dist(re)};
    const Vec3f rayOrigin = {dist(re), dist(re), dist(re)};
    const float rayTMax = 100.f;
    const int dirIsNeg[3] = {(dist(re) > 0) ? 1 : 0, (dist(re) > 0) ? 1 : 0,
                             (dist(re) > 0) ? 1 : 0};

    // non-SIMD implementation (for correctness checking)
    auto nonSimd = [&]() {
        auto resIt = resultsNonSimd.begin();

        for (const auto& elem : data1) {
            float tmin = ((dirIsNeg[0] ? elem.maxx : elem.minx) - rayOrigin.x) * invDir.x;
            float tmax = ((dirIsNeg[0] ? elem.minx : elem.maxx) - rayOrigin.x) * invDir.x;
            float tminy = ((dirIsNeg[1] ? elem.maxy : elem.miny) - rayOrigin.y) * invDir.y;
            float tmaxy = ((dirIsNeg[1] ? elem.miny : elem.maxy) - rayOrigin.y) * invDir.y;
            tmax *= robustFactor;
            tmaxy *= robustFactor;
            if (tmin > tmaxy || tminy > tmax) {
                *(resIt++) = Result::fail;
                continue;
            }
            if (tminy > tmin) tmin = tminy;
            if (tmaxy < tmax) tmax = tmaxy;
            float tminz = ((dirIsNeg[2] ? elem.maxz : elem.minz) - rayOrigin.z) * invDir.z;
            float tmaxz = ((dirIsNeg[2] ? elem.minz : elem.maxz) - rayOrigin.z) * invDir.z;
            tmaxz *= robustFactor;
            if (tmin > tmaxz || tminz > tmax) {
                *(resIt++) = Result::fail;
                continue;
            }
            if (tminz > tmin) tmin = tminz;
            if (tmaxz < tmax) tmax = tmaxz;
            *(resIt++) = ((tmin < rayTMax) && (tmax > 0)) ? Result::win : Result::fail;
        }
    };

    // SIMD implementation by hand
    auto handSimd = [&]() {
        auto resIt = resultsHandSimd.begin();

        for (const auto& elem : data8) {
            uint32_t fail;
            __m256 tmin, tmax;
            {
                __m256 minx = _mm256_load_ps(elem.minx);
                __m256 maxx = _mm256_load_ps(elem.maxx);
                __m256 idirx = _mm256_broadcast_ss(&invDir.x);
                __m256 rayox = _mm256_broadcast_ss(&rayOrigin.x);
                tmin = _mm256_mul_ps(_mm256_sub_ps(dirIsNeg[0] ? maxx : minx, rayox), idirx);
                tmax = _mm256_mul_ps(_mm256_sub_ps(dirIsNeg[0] ? minx : maxx, rayox), idirx);
            }

            {
                __m256 tminy, tmaxy;
                {
                    __m256 miny = _mm256_load_ps(elem.miny);
                    __m256 maxy = _mm256_load_ps(elem.maxy);
                    __m256 idiry = _mm256_broadcast_ss(&invDir.y);
                    __m256 rayoy = _mm256_broadcast_ss(&rayOrigin.y);
                    tminy = _mm256_mul_ps(_mm256_sub_ps(dirIsNeg[1] ? maxy : miny, rayoy), idiry);
                    tmaxy = _mm256_mul_ps(_mm256_sub_ps(dirIsNeg[1] ? miny : maxy, rayoy), idiry);
                }

                __m256 factor = _mm256_broadcast_ss(&robustFactor);
                tmax = _mm256_mul_ps(tmax, factor);
                tmaxy = _mm256_mul_ps(tmaxy, factor);

                {
                    __m256 failcond = _mm256_or_ps(_mm256_cmp_ps(tmin, tmaxy, _CMP_GT_OQ),
                                                   _mm256_cmp_ps(tminy, tmax, _CMP_GT_OQ));
                    fail = uint32_t(_mm256_movemask_ps(failcond));

                    if (fail == 0xFF) {
                        for (int i = 0; i < 8; ++i) { *(resIt++) = Result::fail; }
                        continue;
                    }
                }

                tmin = _mm256_blendv_ps(tmin, tminy, _mm256_cmp_ps(tminy, tmin, _CMP_GT_OQ));
                tmax = _mm256_blendv_ps(tmax, tmaxy, _mm256_cmp_ps(tmaxy, tmax, _CMP_LT_OQ));
            }

            {
                __m256 tminz, tmaxz;
                {
                    __m256 minz = _mm256_load_ps(elem.minz);
                    __m256 maxz = _mm256_load_ps(elem.maxz);
                    __m256 idirz = _mm256_broadcast_ss(&invDir.z);
                    __m256 rayoz = _mm256_broadcast_ss(&rayOrigin.z);
                    tminz = _mm256_mul_ps(_mm256_sub_ps(dirIsNeg[2] ? maxz : minz, rayoz), idirz);
                    tmaxz = _mm256_mul_ps(_mm256_sub_ps(dirIsNeg[2] ? minz : maxz, rayoz), idirz);
                }

                __m256 factor = _mm256_broadcast_ss(&robustFactor);
                tmaxz = _mm256_mul_ps(tmaxz, factor);

                {
                    __m256 failcond = _mm256_or_ps(_mm256_cmp_ps(tmin, tmaxz, _CMP_GT_OQ),
                                                   _mm256_cmp_ps(tminz, tmax, _CMP_GT_OQ));
                    fail = fail | uint32_t(_mm256_movemask_ps(failcond));

                    if (fail == 0xFF) {
                        for (int i = 0; i < 8; ++i) { *(resIt++) = Result::fail; }
                        continue;
                    }
                }

                tmin = _mm256_blendv_ps(tmin, tminz, _mm256_cmp_ps(tminz, tmin, _CMP_GT_OQ));
                tmax = _mm256_blendv_ps(tmax, tmaxz, _mm256_cmp_ps(tmaxz, tmax, _CMP_LT_OQ));
            }

            __m256 wincond =
                _mm256_and_ps(_mm256_cmp_ps(tmin, _mm256_broadcast_ss(&rayTMax), _CMP_LT_OQ),
                              _mm256_cmp_ps(tmax, _mm256_setzero_ps(), _CMP_GT_OQ));
            uint32_t win = ~fail & uint32_t(_mm256_movemask_ps(wincond));

            for (int i = 0; i < 8; ++i) {
                *(resIt++) = ((win >> i) & 1) ? Result::win : Result::fail;
            }
        }
    };

    // implementation using Simdee
    auto simdee = [&]() {
        auto resIt = resultsSimdee.begin();

        for (const auto& elem : data8S) {
            auto tmin = ((dirIsNeg[0] ? elem.maxx : elem.minx) - rayOrigin.x) * invDir.x;
            auto tmax = ((dirIsNeg[0] ? elem.minx : elem.maxx) - rayOrigin.x) * invDir.x;
            auto tminy = ((dirIsNeg[1] ? elem.maxy : elem.miny) - rayOrigin.y) * invDir.y;
            auto tmaxy = ((dirIsNeg[1] ? elem.miny : elem.maxy) - rayOrigin.y) * invDir.y;

            sd::avxf factor(robustFactor);
            tmax *= factor;
            tmaxy *= factor;
            auto fail = mask((tmin > tmaxy) || (tminy > tmax));

            if (all(fail)) {
                for (int i = 0; i < 8; ++i) { *(resIt++) = Result::fail; }
                continue;
            }

            tmin = cond(tminy > tmin, tminy, tmin);
            tmax = cond(tmaxy < tmax, tmaxy, tmax);
            auto tminz = ((dirIsNeg[2] ? elem.maxz : elem.minz) - rayOrigin.z) * invDir.z;
            auto tmaxz = ((dirIsNeg[2] ? elem.minz : elem.maxz) - rayOrigin.z) * invDir.z;
            tmaxz *= factor;
            fail |= mask((tmin > tmaxz) || (tminz > tmax));

            if (all(fail)) {
                for (int i = 0; i < 8; ++i) { *(resIt++) = Result::fail; }
                continue;
            }

            tmin = cond(tminz > tmin, tminz, tmin);
            tmax = cond(tmaxz < tmax, tmaxz, tmax);
            auto win = ~fail & mask((tmin < rayTMax) && (tmax > sd::zero()));

            for (int i = 0; i < 8; ++i) { *(resIt++) = win[i] ? Result::win : Result::fail; }
        }
    };

    // check performance
    std::cout << "non-SIMD: " << benchmark_ms(nonSimd) << " ms\n";
    std::cout << "hand SIMD: " << benchmark_ms(handSimd) << " ms\n";
    std::cout << "Simdee: " << benchmark_ms(simdee) << " ms\n";

    // check correctness
    if (resultsNonSimd != resultsHandSimd) std::cerr << "hand SIMD results incorrect\n";
    if (resultsNonSimd != resultsSimdee) std::cerr << "Simdee results incorrect\n";
}
