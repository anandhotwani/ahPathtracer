//
//  bvh.cpp
//  ahPathtracer
//
//  Created by Anand Hotwani on 18/04/2020.
//  Copyright © 2020 Anand Hotwani. All rights reserved.
//

#include "bvh.hpp"
#include <iostream>
#include <algorithm>

inline bool compare_bbox(const std::shared_ptr<Shape> a, const std::shared_ptr<Shape> b, int axis) {
    AABBox box_a;
    AABBox box_b;

    if (!a->get_bbox(box_a) || !b->get_bbox(box_b))
        std::cerr<<"No bounding box in bvh constructor! \n";

    return box_a.bounds[0][axis] < box_b.bounds[0][axis];
}

inline bool compare_bbox_x (const std::shared_ptr<Shape> a, const std::shared_ptr<Shape> b) {
    return compare_bbox(a, b, 0);
}

inline bool compare_bbox_y (const std::shared_ptr<Shape> a, const std::shared_ptr<Shape> b) {
    return compare_bbox(a, b, 1);
}

inline bool compare_bbox_z (const std::shared_ptr<Shape> a, const std::shared_ptr<Shape> b) {
    return compare_bbox(a, b, 2);
}

bool BVHNode::get_bbox(AABBox &output_box) const {
    output_box = this->box;
    return true;
}

bool BVHNode::intersect(const Ray& r, float t_min, float t_max, SurfaceInteraction& rec) const {
    
        if (box.intersect(r, t_min, t_max)) {
        SurfaceInteraction left_rec, right_rec;
        bool hit_left = left->intersect(r, t_min, t_max, left_rec);
        bool hit_right = right->intersect(r, t_min, t_max, right_rec);

        if (hit_left && hit_right) {
            if (left_rec.t < right_rec.t) {
                rec = left_rec;
            } else {
                rec = right_rec;
            }
            return true;
        } else if (hit_left) {
            rec = left_rec;
            return true;
        } else if (hit_right) {
            rec = right_rec;
            return true;
        } else {
            return false;
        }
    }

    return false;
}

BVHNode::BVHNode(std::vector<std::shared_ptr<Shape>> &objects, size_t start, size_t end) {
    
    // Using surface area heuristic 
    assert(objects.size() > 0);
    if (objects.size() == 1) {
        left = objects[0];
        right = objects[0];
        assert(left->get_bbox(box));
        return;
    } else if (objects.size() == 2) {
        left = objects[0];
        right = objects[1];
        AABBox left_box;
        AABBox right_box;
        assert(left->get_bbox(left_box));
        assert(right->get_bbox(right_box));
        box = expand_by(left_box, right_box);
        return;
    }
    float best_cost = MaxFloat;
    int best_axis = -1;
    int best_split_index = -1;
    const float T_aabb = 1.0;
    const float T_triangle = 1.0;
    for (int axis = 0; axis < 3; axis++) {
        std::sort(objects.begin(), objects.end(),
                [axis](std::shared_ptr<Shape>& h0, std::shared_ptr<Shape>& h1){
                AABBox aabb0, aabb1;
                h0->get_bbox(aabb0);
                h1->get_bbox(aabb1);
                return (aabb0.centroid()[axis] < aabb1.centroid()[axis]);
                });
        double all_surface_area;
        std::vector<float> surfaces_area_0(objects.size()-1);
        std::vector<int> triangle_size_0(objects.size()-1);
        std::vector<float> surfaces_area_1(objects.size()-1);
        std::vector<int> triangle_size_1(objects.size()-1);

        objects[0]->get_bbox(box);
        surfaces_area_0[0] = box.surface_area();
        triangle_size_0[0] = 1;
        for (int i = 1; i < objects.size()-1; i++) {
            AABBox tmp_box;
            objects[i]->get_bbox(tmp_box);
            box = expand_by(box, tmp_box);
            surfaces_area_0[i] = box.surface_area();
            triangle_size_0[i] = triangle_size_0[i-1]+1;
        }

        objects[objects.size()-1]->get_bbox(box);
        surfaces_area_1[objects.size()-2] = box.surface_area();
        triangle_size_1[objects.size()-2] = 1;
        for (int i = objects.size() - 3; i >= 0; i--) {
            AABBox tmp_box;
            objects[i]->get_bbox(tmp_box);
            box = expand_by(box, tmp_box);
            surfaces_area_1[i] = box.surface_area();
            triangle_size_1[i] = triangle_size_1[i+1] + 1;
        }
        {
            AABBox tmp_box;
            objects[0]->get_bbox(tmp_box);
            this->box = expand_by(box, tmp_box);
            all_surface_area = box.surface_area();
        }

        for (int i = 0; i < objects.size()-1; i++) {
            double cost = T_aabb +
                surfaces_area_0[i]/all_surface_area * triangle_size_0[i] * T_triangle +
                surfaces_area_1[i]/all_surface_area * triangle_size_1[i] * T_triangle;
            if (cost < best_cost) {
                best_cost = cost;
                best_axis = axis;
                best_split_index = i;
            }
        }


    }
    std::vector<std::shared_ptr<Shape>> polygons0, polygons1;
    if (best_axis == -1) {
        assert(false);
    }
    std::sort(objects.begin(), objects.end(),
            [best_axis](std::shared_ptr<Shape>& h0, std::shared_ptr<Shape>& h1){
            AABBox aabb0, aabb1;
            h0->get_bbox(aabb0);
            h1->get_bbox(aabb1);
            return (aabb0.centroid()[best_axis] < aabb1.centroid()[best_axis]);
            });
    std::copy(objects.begin(), objects.begin() + best_split_index+1, std::back_inserter(polygons0));
    std::copy(objects.begin()+best_split_index+1, objects.end(), std::back_inserter(polygons1));


    Scene polygons0_scene;
    Scene polygons1_scene;

    for (int i = 0; i < polygons0.size(); ++i) {
        polygons0_scene.Add(polygons0[i]);
    }

    for (int i = 0; i < polygons1.size(); ++i) {
        polygons1_scene.Add(polygons1[i]);
    }

    left = std::shared_ptr<Shape>(new BVHNode(polygons0_scene));
    right = std::shared_ptr<Shape>(new BVHNode(polygons1_scene));

    AABBox box_left, box_right;
    assert(left->get_bbox(box_left));
    assert(right->get_bbox(box_right));
    box = expand_by(box_left, box_right);
}
