//
//  bvh.hpp
//  ahPathtracer
//
//  Created by Anand Hotwani on 18/04/2020.
//  Copyright © 2020 Anand Hotwani. All rights reserved.
//

#ifndef bvh_hpp
#define bvh_hpp

#include "math.hpp"
#include "shape.hpp"
#include "scene.hpp"

class BVHNode : public Shape {
public:
    BVHNode();    
    BVHNode(Scene& list) : BVHNode(list.objects, 0, list.objects.size()) {}
    BVHNode(std::vector<std::shared_ptr<Shape>> &objects, size_t start, size_t end);
    
    virtual bool intersect(const Ray& r, float tmin, float tmax, SurfaceInteraction& rec) const;
    virtual bool get_bbox(AABBox &output_box) const;
public:
    std::shared_ptr<Shape> left;
    std::shared_ptr<Shape> right;
    AABBox box;
};

#endif /* bvh_hpp */
