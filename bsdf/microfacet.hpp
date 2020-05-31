//
//  microfacet.hpp
//  ahPathtracer
//
//  Created by Anand Hotwani on 25/05/2020.
//  Copyright © 2020 Anand Hotwani. All rights reserved.
//

#ifndef microfacet_hpp
#define microfacet_hpp

#include "math.hpp"

inline float chiGGX(float v) { return v > 0.0f ? 1.0f : 0.0f; }

inline float GGX_Distribution(Vector3f n, Vector3f h, float alpha) {
    float NoH = glm::dot(n, h);
    float alpha2 = alpha * alpha;
    float NoH2 = NoH * NoH;
    float den = NoH2 * alpha2 + (1.0f - NoH2);
    return (chiGGX(NoH) * alpha2) / (M_PI * den * den);
}

inline float GGX_PartialGeometryTerm(Vector3f v, Vector3f n, Vector3f h, float alpha) {
    float VoH2 = saturate(glm::dot(v, h));
    float chi = chiGGX(VoH2 / saturate(glm::dot(v, n)));
    VoH2 = VoH2 * VoH2;
    float tan2 = (1 - VoH2) / VoH2;
    return (chi * 2.0f) / (1 + sqrtf(1 + alpha * alpha * tan2));
}


inline float schlick(float r0, float r90, Vector3f v, Vector3f h) {
    float VoH = glm::dot(v, h);
    return r0 + (r90 - r0) * powf((1 - VoH), 5.0f);
}

#endif /* microfacet_hpp */
