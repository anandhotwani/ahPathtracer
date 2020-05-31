//
//  aabbox.cpp
//  ahPathtracer
//
//  Created by Anand Hotwani on 17/04/2020.
//  Copyright © 2020 Anand Hotwani. All rights reserved.
//

#include "aabbox.hpp"

bool AABBox::intersect(const Ray &r, float t_min, float t_max) const {
    Vector3f inv_d = Vector3f(1.0f / r.d.x,
                              1.0f / r.d.y,
                              1.0f / r.d.z);
    
    float tx1 = (bounds[0].x - r.o.x) * inv_d.x;
    float tx2 = (bounds[1].x - r.o.x) * inv_d.x;
    
    float tmin_temp = std::min(tx1, tx2);
    float tmax_temp = std::max(tx1, tx2);
    
    float ty1 = (bounds[0].y - r.o.y) * inv_d.y;
    float ty2 = (bounds[1].y - r.o.y) * inv_d.y;
    
    tmin_temp = std::max(tmin_temp, std::min(ty1, ty2));
    tmax_temp = std::min(tmax_temp, std::max(ty1, ty2));
    
    float tz1 = (bounds[0].z - r.o.z) * inv_d.z;
    float tz2 = (bounds[1].z - r.o.z) * inv_d.z;
    
    tmin_temp = std::max(tmin_temp, std::min(tz1, tz2));
    tmax_temp = std::min(tmax_temp, std::max(tz1, tz2));
    
    t_min = tmin_temp;
    t_max = tmax_temp;
    
    return tmax_temp >= tmin_temp;
}

void AABBox::expand_by(const AABBox &b) {
    
    bounds[0].x = std::min(bounds[0].x, b.bounds[0].x);
    bounds[0].y = std::min(bounds[0].y, b.bounds[0].y);
    bounds[0].z = std::min(bounds[0].z, b.bounds[0].z);
      
    bounds[1].x = std::max(bounds[1].x, b.bounds[1].x);
    bounds[1].y = std::max(bounds[1].y, b.bounds[1].y);
    bounds[1].z = std::max(bounds[1].z, b.bounds[1].z);
}

void AABBox::expand_by(const Vector3f &v) {
    
    bounds[0].x = std::min(bounds[0].x, v.x);
    bounds[0].y = std::min(bounds[0].y, v.y);
    bounds[0].z = std::min(bounds[0].z, v.z);
      
    bounds[1].x = std::max(bounds[1].x, v.x);
    bounds[1].y = std::max(bounds[1].y, v.y);
    bounds[1].z = std::max(bounds[1].z, v.z);
}

int AABBox::longest_axis() {
    int axis = -1;
    float max_extent = MinFloat;
    for (int i = 0; i < 3; ++i) {
        if (bounds[1][i] - bounds[0][i] > MinFloat) {
            max_extent = bounds[1][i] - bounds[0][i];
            axis = i;
        }
    }
    return axis;
}

float AABBox::surface_area() const {
    float side_x = bounds[1].x - bounds[0].x;
    float side_y = bounds[1].y - bounds[0].y;
    float side_z = bounds[1].z - bounds[0].z;
    return 2.0f * (side_x * side_y + side_x * side_z + side_y * side_z);
}

Vector3f AABBox::centroid() const {
    return 0.5f * (bounds[1] + bounds[0]);
}
