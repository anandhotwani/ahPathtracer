//
//  scene.hpp
//  ahPathtracer
//
//  Created by Anand Hotwani on 03/04/2020.
//  Copyright © 2020 Anand Hotwani. All rights reserved.
//

#ifndef scene_hpp
#define scene_hpp

#include "shape.hpp"
#include <memory>
#include <vector>

class Scene : public Shape {
public:
    Scene() {}
    Scene(std::shared_ptr<Shape> object) { Add(object); }
    
    void Clear();
    void Add(std::shared_ptr<Shape> object);
    void AddLight(std::shared_ptr<Shape> light);
    
    virtual bool intersect(const Ray& r, float t_min, float t_max, SurfaceInteraction &interaction) const;
    virtual bool get_bbox(AABBox& output_box) const;
    
public:
    std::vector<std::shared_ptr<Shape>> objects;
    std::vector<std::shared_ptr<Shape>> lights;
};

#endif /* scene_hpp */
