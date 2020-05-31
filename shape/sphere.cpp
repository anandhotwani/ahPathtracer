//
//  sphere.cpp
//  ahPathtracer
//
//  Created by Anand Hotwani on 03/04/2020.
//  Copyright © 2020 Anand Hotwani. All rights reserved.
//

#include "sphere.hpp"
#include <iostream>

inline Vector2f calculate_spherical_UV(const Vector3f& p) {
    Vector2f out_uv = Vector2f(0.0f);
    auto phi = atan2(p.z, p.x);
    auto theta = asin(p.y);
    out_uv[0] = 1.0f - (phi + M_PI) / (2.0f * M_PI);
    out_uv[1] = (theta + M_PI/2) / M_PI;
    return out_uv;
}

bool Sphere::intersect(const Ray &r, float t_min, float t_max, SurfaceInteraction &interaction) const {
    
    Vector3f oc = r.o - this->center;
    float a = glm::length2(r.d);
    float half_b = glm::dot(oc, r.d);
    float c = glm::length2(oc) - radius * radius;
    float discriminant = half_b * half_b - a * c;
    
    if (discriminant > 0.0f) {
        float root = sqrt(discriminant);
        float temp = (-half_b - root) / a;
        if (temp < t_max && temp > t_min) {
            interaction.t = temp;
            interaction.p = r.o + interaction.t * r.d;
            Vector3f outward_normal = (interaction.p - center) / radius;  // Normalized by diving by radius
            interaction.set_face_normal(r, outward_normal);
            interaction.BSDF_ptr = BSDF_ptr;
            Vector3f unit_origin_sphere = (interaction.p - center) / radius;
            interaction.uv = calculate_spherical_UV(unit_origin_sphere);
            return true;
        }
        temp = (-half_b + root) / a;
        if (temp < t_max && temp > t_min) {
            interaction.t = temp;
            interaction.p = r.o + interaction.t * r.d;
            Vector3f outward_normal = (interaction.p - center) / radius;  // Normalized by diving by radius
            interaction.set_face_normal(r, outward_normal);
            interaction.BSDF_ptr = BSDF_ptr;
            Vector3f unit_origin_sphere = (interaction.p - center) / radius;
            interaction.uv = calculate_spherical_UV(unit_origin_sphere);
            return true;
        }
    }
    // No solutions
    return false;
}

bool Sphere::get_bbox(AABBox& output_box) const {
    output_box = AABBox(
                        center - Vector3f(radius),
                        center + Vector3f(radius));
    return true;
}
