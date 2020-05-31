//
//  rect.hpp
//  ahPathtracer
//
//  Created by Anand Hotwani on 28/04/2020.
//  Copyright © 2020 Anand Hotwani. All rights reserved.
//

#ifndef rect_hpp
#define rect_hpp

#include "triangle.hpp"

class Rect : public Shape {
public:
    Rect() {}
    Rect(Matrix4x4f o2w, Vector3f v00, Vector3f v10, Vector3f v11, Vector3f v01, std::shared_ptr<BSDF> m) {
        
        Vector3f v00new, v10new, v11new, v01new;
        
        v00new = TransformPointMatrix(o2w, v00);
        v10new = TransformPointMatrix(o2w, v10);
        v11new = TransformPointMatrix(o2w, v11);
        v01new = TransformPointMatrix(o2w, v01);
        
        p00 = v00new;
        p10 = v10new;
        p11 = v11new;
        p01 = v01new;
        
        BSDF_ptr = m;
        Vector2f uv00 = Vector2f(1.0, 0.0f);
        Vector2f uv10 = Vector2f(0.0, 0.0f);
        Vector2f uv01 = Vector2f(0.0, 1.0f);
        Vector2f uv11 = Vector2f(1.0, 1.0f);
        tri0 = std::make_shared<Triangle>(v00new, v10new, v11new, uv00, uv10, uv11, BSDF_ptr);
        tri1 = std::make_shared<Triangle>(v01new, v11new, v10new, uv00, uv11, uv01, BSDF_ptr);
        
    }
    
    virtual bool intersect(const Ray& r, float t_min, float t_max, SurfaceInteraction &interaction) const {
        if (tri0->intersect(r, t_min, t_max, interaction))
            return true;
        if (tri1->intersect(r, t_min, t_max, interaction))
            return true;
        return false;
    }
    
    virtual bool get_bbox(AABBox &output_box) const {
        AABBox temp0, temp1;
        tri0->get_bbox(temp0);
        tri1->get_bbox(temp1);
        temp0.expand_by(temp1);
        temp0.bounds[0].y = -0.0001f;
        temp0.bounds[1].y = 0.0001f;
        output_box = temp0;
        return true;
    }
    
    virtual float pdf_value(const Vector3f &o, const Vector3f& v) const {
        SurfaceInteraction interaction;
        if (!this->intersect(Ray(o, v), 0.0001f, MaxFloat, interaction))
            return 0.0f;
        // Assuming square
        float area = fabs(glm::length(p10 - p00)) * fabs(glm::length(p11- p01));
        float distance_squared = interaction.t * interaction.t * glm::length2(v);
        float cosine = fabs(glm::dot(v, interaction.n) / glm::length(v));
        
        return distance_squared / (cosine * area);
    }
    
    virtual Vector3f rand_direction_towards(const Vector3f& o) const {
        float seed = random_float();
        Vector3f rand = Vector3f(0.0f);
        
        if (seed < 0.5f) {
            rand = tri0->rand_direction_towards(o);
        }
        else {
            rand = tri1->rand_direction_towards(o);
        }
        
        //std::cout<<"Random point on tri is "<<glm::to_string(rand)<<"\n";
        
        return rand;
    }
    
public:
    std::shared_ptr<Triangle> tri0;
    std::shared_ptr<Triangle> tri1;
    std::shared_ptr<BSDF> BSDF_ptr;
    Vector3f p00, p10, p01, p11;
};

#endif /* rect_hpp */
