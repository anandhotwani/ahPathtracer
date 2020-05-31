//
//  shape.hpp
//  ahPathtracer
//
//  Created by Anand Hotwani on 03/04/2020.
//  Copyright © 2020 Anand Hotwani. All rights reserved.
//

#ifndef shape_hpp
#define shape_hpp

#include "math.hpp"
#include "aabbox.hpp"

class BSDF;

struct SurfaceInteraction {
    Vector3f p;         // Point of intersection
    Vector3f n;         // Geometric normal
    Vector3f wo;
    std::shared_ptr<BSDF> BSDF_ptr;
    float    t;         // Length along ray for intersection
    bool front_facing;  // Determine if the normal is aligned with the ray or against
    Vector2f uv;
    Vector3f AOV;     // To write arbitrary values out to for debugging (shader normals, barycentric coordinates etc)
    
    inline void set_face_normal(const Ray& r, const Vector3f &outward_normal) {
        front_facing = glm::dot(r.d, outward_normal) < 0.0f;
        n = front_facing ? outward_normal : -outward_normal;
    }
};

class Shape {
public:
    virtual bool intersect(const Ray& r, float t_min, float t_max, SurfaceInteraction &interaction) const = 0;
    virtual bool get_bbox(AABBox& output_box) const = 0;
    virtual float pdf_value(const Vector3f& o, const Vector3f& v) const { return 0.0f; }
    virtual Vector3f rand_direction_towards(const Vector3f& o) const { return Vector3f(1.0f, 0.0f, 0.0f); }
};

#endif /* shape_hpp */
