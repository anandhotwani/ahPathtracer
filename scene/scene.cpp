//
//  scene.cpp
//  ahPathtracer
//
//  Created by Anand Hotwani on 03/04/2020.
//  Copyright © 2020 Anand Hotwani. All rights reserved.
//

#include "scene.hpp"

void Scene::Clear() {
    objects.clear();
}

void Scene::Add(std::shared_ptr<Shape> object) {
    objects.push_back(object);
}

void Scene::AddLight(std::shared_ptr<Shape> light) {
    lights.push_back(light);
}

bool Scene::intersect(const Ray &r, float t_min, float t_max, SurfaceInteraction &interaction) const {
    
    SurfaceInteraction temp;
    bool any_hit = false;
    float t_near = t_max;
    
    for (const auto& object : objects) {
        if (object->intersect(r, t_min, t_near, temp)) {
            any_hit = true;
            t_near = temp.t;
            interaction = temp;
        }
    }
    return any_hit;
}

bool Scene::get_bbox(AABBox &output_box) const {
    if (this->objects.empty()) return false;
    
    AABBox temp;
    bool first_box = true;
    
    for (const auto& object : objects) {
        if (!object->get_bbox(temp)) return false;
        output_box = first_box ? temp : expand_by(output_box, temp);
        first_box = false;
    }
    return true;
}
