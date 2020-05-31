//
//  triangle.cpp
//  ahPathtracer
//
//  Created by Anand Hotwani on 10/04/2020.
//  Copyright © 2020 Anand Hotwani. All rights reserved.
//

#include "triangle.hpp"

bool Triangle::intersect(const Ray &r, float t_min, float t_max, SurfaceInteraction &interaction) const {
    
    float u, v, t_temp = 0.0f;

     Vector3f pvec = glm::cross(r.d, e2);
     float det = glm::dot(e1, pvec);
     if (det ==  0.0f) return false;
     float inv_det = 1.0f / det;
     Vector3f tvec = r.o - v0;
     u = glm::dot(tvec, pvec) * inv_det;
     if (u < 0.0f || u > 1.0f ) return false;
     Vector3f qvec = glm::cross(tvec, e1);
     v = glm::dot(r.d, qvec) * inv_det;
     if (v < 0.0f || u + v > 1.0f) return false;
     t_temp = glm::dot(e2, qvec) * inv_det;
     if (t_temp < t_max) {
         if (t_temp > t_min) {
            interaction.t = t_temp;
            interaction.p = r.o + interaction.t * r.d;
             Vector3f outward_normal = this->n;  // Normalized by diving by radius
             interaction.set_face_normal(r, outward_normal);
             interaction.BSDF_ptr = this->BSDF_ptr;
             
             interaction.n = glm::dot(r.d, this->n) < 0.0f ? this->n : this->n * -1.0f;

             Vector3f barycentric = get_barycentric(interaction.p);
             interaction.uv = (UV0 * barycentric.x) + (UV1 * barycentric.y) + (UV2 * barycentric.z);
             return true;
         }
     }
     return false;
}

bool Triangle::get_bbox(AABBox &output_box) const {
    Vector3f bl = Vector3f(
                           std::min(std::min(v0.x, v1.x), v2.x),
                           std::min(std::min(v0.y, v1.y), v2.y),
                           std::min(std::min(v0.z, v1.z), v2.z)
                           );
    Vector3f tr = Vector3f(
                           std::max(std::max(v0.x, v1.x), v2.x),
                           std::max(std::max(v0.y, v1.y), v2.y),
                           std::max(std::max(v0.z, v1.z), v2.z)
                           );
    
    output_box = AABBox(bl, tr);
    return true;
}

Vector3f Triangle::get_barycentric(Vector3f &p) const {
    Vector3f v2_ = p - v0;
    float d00 = glm::dot(e1, e1);
    float d01 = glm::dot(e1, e2);
    float d11 = glm::dot(e2, e2);
    float d20 = glm::dot(v2_, e1);
    float d21 = glm::dot(v2_, e2);
    float d = d00 * d11 - d01 * d01;
    float v = (d11 * d20 - d01 * d21) / d;
    float w = (d00 * d21 - d01 * d20) / d;
    float u = 1 - v - w;
    return Vector3f(u, v, w);
}

Vector3f Triangle::rand_direction_towards(const Vector3f& o) const {
    Vector3f rand;
    float s = random_float();
    float t = random_float();
    
    if (s < t) {
        rand = Vector3f(s * v0.x + (t - s) * v1.x + (1 - t) * v2.x,
                        s * v0.y + (t - s) * v1.y + (1 - t) * v2.y,
                        s * v0.z + (t - s) * v1.z + (1 - t) * v2.z);
    }
    else {
        rand = Vector3f(t * v0.x + (s - t) * v1.x + (1 - s) * v2.x,
                        t * v0.y + (s - t) * v1.y + (1 - s) * v2.y,
                        t * v0.z + (s - t) * v1.z + (1 - s) * v2.z);
        
    }

    // We have a random point, now we need the vector towards it
    return rand - o;
}
