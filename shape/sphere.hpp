//
//  sphere.hpp
//  ahPathtracer
//
//  Created by Anand Hotwani on 03/04/2020.
//  Copyright © 2020 Anand Hotwani. All rights reserved.
//

#ifndef sphere_hpp
#define sphere_hpp

#include "shape.hpp"
#include "BSDF.hpp"

class Sphere: public Shape {
public:
    Sphere() {}
    Sphere(Matrix4x4f o2w, float r, std::shared_ptr<BSDF> m) : object2world(o2w), center(TransformPointMatrix(o2w, Vector3f(0.0f))), radius(r), BSDF_ptr(m)  {};
    
    virtual bool intersect(const Ray& r, float t_min, float t_max, SurfaceInteraction &interaction) const;
    virtual bool get_bbox(AABBox& output_box) const;
    
public:
    Matrix4x4f object2world;
    Vector3f center;
    float radius;
    std::shared_ptr<BSDF> BSDF_ptr;
};

#endif /* sphere_hpp */
