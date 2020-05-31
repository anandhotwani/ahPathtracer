//
//  math_utils.hpp
//  ahPathtracer
//
//  Created by Anand Hotwani on 29/03/2020.
//  Copyright © 2020 Anand Hotwani. All rights reserved.
//

#ifndef math_utils_hpp
#define math_utils_hpp

#include "vector.hpp"
#include <functional>
#include <random>

const float MaxFloat = std::numeric_limits<float>::infinity();
const float MinFloat = std::numeric_limits<float>::epsilon();
const float ShadowBias = 1e-4;

#ifndef M_PI
#define M_PI 3.1415926535897932385;
#endif

// Common math functions

inline Vector3f lerp(const Vector3f& a, const Vector3f& b, const float t) {
    return a * (1.0f - t) + b * t;
}

inline float deg2rad(const float& deg) { return deg * M_PI / 180.0f; }

inline float saturate(float x) {
    if (x < 0.0f) return 0.0f;
    if (x > 0.999f) return 1.0f;
    return x;
}

inline float clamp(float& x, float& lo, float& hi) {
    if (x < lo) return lo;
    if (x > hi) return hi;
    return x;
}

inline Vector3f reflect(const Vector3f& v, const Vector3f& n) {
    return v - 2.0f * glm::dot(v, n) * n;
}

inline Vector3f refract(const Vector3f& uv, const Vector3f& n, float etai_over_etat) {
    float cos_theta = glm::dot(-uv, n);
    Vector3f r_out_parallel = etai_over_etat * (uv + cos_theta * n);
    Vector3f r_out_perp = -sqrtf(1.0f - glm::length2(r_out_parallel)) * n;
    return r_out_parallel + r_out_perp;
}

inline float fr_schlick(float cosine, float ior) {
    float r0 = (1.0f - ior) / (1.0f + ior);
    r0 = r0 * r0;
    return r0 + (1.0f - r0) * powf((1.0f - cosine), 5.0f);
}

// Random functions


// Float in range [0, 1]
inline float random_float() {
    static std::default_random_engine generator;
    std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
    return distribution(generator);
}

// Float in arbitrary range
inline float random_float(float lo, float hi) {
    return lo + (hi - lo) * random_float();
}

// Random integer
inline uint32_t random_int(uint32_t min, uint32_t max) {
    // Returns a random integer in [min, max]
    return static_cast<int>(random_float(min, max+1));
}

inline Vector3f random_vector() { return Vector3f(random_float(-1.0f, 1.0f), random_float(-1.0f, 1.0f), random_float(-1.0f, 1.0f)); }

inline Vector3f random_in_unit_sphere() {
    while (true) {
        Vector3f p = Vector3f(random_float(-1.0f, 1.0f), random_float(-1.0f, 1.0f), random_float(-1.0f, 1.0f));
        if (glm::length2(p) >= 1.0f ) continue;
        return p;
    }
}

inline Vector3f random_cosine_direction() {
    float r1 = random_float();
    float r2 = random_float();
    float z = sqrtf(1.0f - r2);
    float phi = 2.0f * M_PI * r1;
    float x = cos(phi) * sqrtf(r2);
    float y = sin(phi) * sqrtf(r2);

    return Vector3f(x, y, z);
}

inline Vector3f random_unit_vector() {
    float a = random_float(0.0f, 2.0f * M_PI);
    float z = random_float(-1.0f, 1.0f);
    float r = sqrtf(1.0f - z * z);
    return Vector3f(r * cos(a), r * sin(a), z);
}

inline Vector3f uniform_sample_hemisphere(const Vector3f& normal) {
    
    Vector3f Nt, Nb;
    float r1 = 2.0 * M_PI * drand48();
    float r2 = drand48();
    float r2s = sqrt(r2);
    
    if (std::fabs(normal.x) > std::fabs(normal.y))
        Nt = Vector3f(normal.z, 0, -normal.x) / sqrt(normal.x * normal.x + normal.z * normal.z);
    else
        Nt = Vector3f(0, -normal.z, normal.y) / sqrt(normal.y * normal.y + normal.z * normal.z);

    Nb = glm::cross(normal, Nt);
    
    return Vector3f(glm::normalize((Nt * cos(r1) * r2s + Nb * sin(r1) * r2s + normal * sqrt(1 - r2))));
}

inline Vector2f concentric_sample_disk(const Vector2f &u) {
    // Map uniform random numbers to [-1,1]
    Vector2f u_offset = 2.0f * u - Vector2f(1, 1);
    
    // Handle degeneracy at origin
    if (u_offset.x == 0 && u_offset.y == 0) return Vector2f(0.0f, 0.0f);
    
    // Apply concentric mapping to point
    float theta, r;
    if (std::abs(u_offset.x) > std::abs(u_offset.y)) {
        r = u_offset.x;
        theta = (M_PI / 4.0f) * (u_offset.y / u_offset.x);
    } else {
        r = u_offset.y;
        theta = (M_PI / 2.0f) - (M_PI / 4.0f) * (u_offset.x / u_offset.y);
    }
    
    return r * Vector2f(std::cos(theta), std::sin(theta));
}

#endif /* math_utils_hpp */
