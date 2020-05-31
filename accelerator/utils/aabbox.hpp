//
//  aabbox.hpp
//  ahPathtracer
//
//  Created by Anand Hotwani on 17/04/2020.
//  Copyright © 2020 Anand Hotwani. All rights reserved.
//

#ifndef aabbox_hpp
#define aabbox_hpp

#include "math.hpp"

class AABBox {
public:
    AABBox(const Vector3f bl = Vector3f(-0.0001f), const Vector3f tr = Vector3f(0.0001f)) { bounds[0] = bl, bounds[1] = tr; }
    bool intersect(const Ray& r, float t_min, float t_max) const;
    int longest_axis();
    float surface_area() const;
    Vector3f centroid() const;
    void expand_by(const AABBox &b);
    void expand_by(const Vector3f &v);
    
public:
    Vector3f bounds[2];
};

inline AABBox expand_by(AABBox& b0, AABBox& b1) {
    
    Vector3f bl = Vector3f(
                           std::min(b0.bounds[0].x, b1.bounds[0].x),
                           std::min(b0.bounds[0].y, b1.bounds[0].y),
                           std::min(b0.bounds[0].z, b1.bounds[0].z));
    
    Vector3f tr = Vector3f(std::max(b0.bounds[1].x, b1.bounds[1].x),
                           std::max(b0.bounds[1].y, b1.bounds[1].y),
                           std::max(b0.bounds[1].z, b1.bounds[1].z));
    
    return AABBox(bl, tr);
}

inline AABBox expand_by(AABBox& b0, const Vector3f &v) {
    
    Vector3f bl = Vector3f(
                           std::min(b0.bounds[0].x, v.x),
                           std::min(b0.bounds[0].y, v.y),
                           std::min(b0.bounds[0].z, v.z));
    
    Vector3f tr = Vector3f(std::max(b0.bounds[1].x, v.x),
                           std::max(b0.bounds[1].y, v.y),
                           std::max(b0.bounds[1].z, v.z));
    
    return AABBox(bl, tr);
}

#endif /* aabbox_hpp */
