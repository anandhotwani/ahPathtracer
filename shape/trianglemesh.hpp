//
//  trianglemesh.hpp
//  ahPathtracer
//
//  Created by Anand Hotwani on 10/04/2020.
//  Copyright © 2020 Anand Hotwani. All rights reserved.
//

#ifndef trianglemesh_hpp
#define trianglemesh_hpp

#include "triangle.hpp"
#include "tiny_obj_loader.h"
#include <iostream>
#include <vector>

class TriangleMesh : public Shape {
public:
    TriangleMesh(Matrix4x4f o2w, const char *filepath, std::shared_ptr<BSDF> m);
    bool intersect(const Ray& r, float t_min, float t_max, SurfaceInteraction &interaction) const override;
    bool get_bbox(AABBox &output_box) const override;
public:
    Matrix4x4f object2world;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::vector<std::shared_ptr<Triangle>> tris;
    std::shared_ptr<BSDF> BSDF_ptr;
    std::shared_ptr<BVHNode> triangle_bvh;
};

#endif /* trianglemesh_hpp */
