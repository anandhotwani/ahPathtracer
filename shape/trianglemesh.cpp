//
//  trianglemesh.cpp
//  ahPathtracer
//
//  Created by Anand Hotwani on 10/04/2020.
//  Copyright © 2020 Anand Hotwani. All rights reserved.
//

#include "trianglemesh.hpp"

TriangleMesh::TriangleMesh(Matrix4x4f o2w, const char *filepath, std::shared_ptr<BSDF> m) {
    
    object2world = o2w;
    BSDF_ptr = m;
    std::string inputfile = filepath;
    unsigned long pos = inputfile.find_last_of("/");
    std::string mtlbasepath = inputfile.substr(0, pos + 1);

    std::cout<<"> Attempting to open  "<<inputfile<<" ... \n";
    std::string err = tinyobj::LoadObj(shapes, materials, inputfile.c_str(), mtlbasepath.c_str());

    if (!err.empty()) {
        std::cout<<err<<std::endl;
    }

    long shapes_size, indices_size;
    shapes_size = shapes.size();
    std::cout<<"> Number of polygon objects found in the .obj file : "<<shapes.size()<<"\n";

    // Load triangles
    for (uint32_t i = 0; i < shapes_size; ++i) {
        indices_size = shapes[i].mesh.indices.size() / 3;
        std::cout<<"> Triangles found in mesh : "<<indices_size<<"\n";
        for (size_t f = 0; f < indices_size; ++f) {
            
            // Vertices
            Vector3f v0 = Vector3f(
                                    shapes[i].mesh.positions[shapes[i].mesh.indices[3 * f] * 3],
                                    shapes[i].mesh.positions[shapes[i].mesh.indices[3 * f] * 3 + 1],
                                    shapes[i].mesh.positions[shapes[i].mesh.indices[3 * f] * 3 + 2]
                                    );
            Vector3f v1 = Vector3f(
                                    shapes[i].mesh.positions[shapes[i].mesh.indices[3 * f + 1] * 3],
                                    shapes[i].mesh.positions[shapes[i].mesh.indices[3 * f + 1] * 3 + 1],
                                    shapes[i].mesh.positions[shapes[i].mesh.indices[3 * f + 1] * 3 + 2]
                                    );
            Vector3f v2 = Vector3f(
                                    shapes[i].mesh.positions[shapes[i].mesh.indices[3 * f + 2] * 3],
                                    shapes[i].mesh.positions[shapes[i].mesh.indices[3 * f + 2] * 3 + 1],
                                    shapes[i].mesh.positions[shapes[i].mesh.indices[3 * f + 2] * 3 + 2]
                                    );
            
            // Move to world-space
            v0 = TransformPointMatrix(object2world, v0);
            v1 = TransformPointMatrix(object2world, v1);
            v2 = TransformPointMatrix(object2world, v2);
            
            Vector2f UV0, UV1, UV2;
            
            // Attempt to load texture coordinates, UV
            if (shapes[i].mesh.indices[3 * f + 2] * 2 + 1 < shapes[i].mesh.texcoords.size()) {
                UV0 = Vector2f(
                    shapes[i].mesh.texcoords[shapes[i].mesh.indices[3 * f] * 2],
                    shapes[i].mesh.texcoords[shapes[i].mesh.indices[3 * f] * 2 + 1]
                );

                UV1 = Vector2f(
                    shapes[i].mesh.texcoords[shapes[i].mesh.indices[3 * f + 1] * 2],
                    shapes[i].mesh.texcoords[shapes[i].mesh.indices[3 * f + 1] * 2 + 1]
                );

                UV2 = Vector2f(
                    shapes[i].mesh.texcoords[shapes[i].mesh.indices[3 * f + 2] * 2],
                    shapes[i].mesh.texcoords[shapes[i].mesh.indices[3 * f + 2] * 2 + 1]
                );
                
                // Ensure UVs are in [0,1] range
                UV0.x = saturate(UV0.x);
                UV0.y = saturate(UV0.y);
                UV1.x = saturate(UV1.x);
                UV1.y = saturate(UV1.y);
                UV2.x = saturate(UV2.x);
                UV2.y = saturate(UV2.y);
                
//                std::cout<<"UV Coordinates are : \n"<<"v0 : "<<glm::to_string(UV0)<<"\nv1 : "<<glm::to_string(UV1)<<"\nv2 : "<<glm::to_string(UV2)<<"\n";
            }
            else {
                UV0 = Vector2f(0.0f);
                UV1 = Vector2f(0.0f);
                UV2 = Vector2f(0.0f);
            }

            tris.push_back(std::make_shared<Triangle>(v0, v1, v2, UV0, UV1, UV2, BSDF_ptr));
        }
    }
    std::cout<<"> Successfully opened "<<inputfile<<"! \n";
    
    Scene scene;
    for (int i = 0; i < tris.size(); ++i) {
        scene.Add(tris[i]);
    }
    
    std::cout<<"> Constructing mesh acceleration structure (BVH)... \n";
    triangle_bvh = std::make_unique<BVHNode>(scene);
    std::cout<<"> Successfully acceleration structure! \n\n";
    shapes.clear();
    materials.clear();
}

bool TriangleMesh::intersect(const Ray &r, float t_min, float t_max, SurfaceInteraction &interaction) const {
    
    return triangle_bvh->intersect(r, t_min, t_max, interaction);
    
//    bool hit_tri = false;
//    long tri_idx;
//    float t = t_max;
//    SurfaceInteraction temp;
//    long triangles_size = tris.size();
//    for (long i = 0; i < triangles_size; ++i) {
//        if (tris[i]->intersect(r, t_min, t, temp)) {
//            hit_tri = true;
//            tri_idx = i;
//            t = temp.t;
//            interaction = temp;
//        }
//    }
//    return hit_tri;
}

bool TriangleMesh::get_bbox(AABBox &output_box) const {
    
    // Just making the aabbox shift towards the actual mesh location instead of world-space origin
    AABBox box;
    tris[0]->get_bbox(box);
    
//    std::cout<<"Tris size is "<<tris.size()<<"\n";
    
    // Expand the bbox to include all triangles
    for (size_t i = 1; i < tris.size(); ++i) {
        AABBox compare;
        tris[i]->get_bbox(compare);
        box = expand_by(box, compare);
//    std::cout<<"Mesh Bounding box extents are \nbl: "<<glm::to_string(box.bounds[0])<<"\ntr "<<glm::to_string(box.bounds[1])<<"\n\n";
    }
    output_box = box;
//    std::cout<<"Mesh Bounding box extents are \nbl: "<<glm::to_string(box.bounds[0])<<"\ntr "<<glm::to_string(box.bounds[1])<<"\n\n";
    return true;
}
