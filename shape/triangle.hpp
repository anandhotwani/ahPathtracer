//
//  triangle.hpp
//  ahPathtracer
//
//  Created by Anand Hotwani on 10/04/2020.
//  Copyright © 2020 Anand Hotwani. All rights reserved.
//

#ifndef triangle_hpp
#define triangle_hpp

#include "shape.hpp"
#include "bsdf.hpp"
#include "bvh.hpp"
#include <iostream>

class Triangle : public Shape {
public:
    Triangle(Vector3f _v0, Vector3f _v1, Vector3f _v2, Vector2f _uv0, Vector2f _uv1, Vector2f _uv2, std::shared_ptr<BSDF> _m) : BSDF_ptr(_m) {
        
        v0 = _v0;
        v1 = _v1;
        v2 = _v2;
        e1 = v1 - v0;
        e2 = v2 - v0;
        UV0 = _uv0;
        UV1 = _uv1;
        UV2 = _uv2;
        n = glm::normalize(glm::cross(e1, e2));
    }
    
    virtual bool intersect(const Ray& r, float t_min, float t_max, SurfaceInteraction &interaction) const;
    virtual bool get_bbox(AABBox& output_box) const;
    
    Vector3f get_barycentric(Vector3f &p) const;
    
    virtual Vector3f rand_direction_towards(const Vector3f &o) const;
    
public:
    Vector3f v0, v1, v2;    // Vertex coordinates
    Vector3f e1, e2;        // Edge 1 and 2
    Vector3f n;             // Geometric normal
    Vector2f UV0, UV1, UV2; // UV coordinates for each vertex
    std::shared_ptr<BSDF> BSDF_ptr;

};

#endif /* triangle_hpp */
