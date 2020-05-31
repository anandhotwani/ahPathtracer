//
//  onb.hpp
//  ahPathtracer
//
//  Created by Anand Hotwani on 10/05/2020.
//  Copyright © 2020 Anand Hotwani. All rights reserved.
//

#ifndef onb_hpp
#define onb_hpp

#include "vector.hpp"

class ONB {
public:
    ONB() {}
    Vector3f local(float a, float b, float c) const { return a * this->u + b * this->v + c * this->w; }
    Vector3f local(const Vector3f& v) const { return v.x * this->u + v.y * this->v + v.z * this->w; }
    void build_from_w(const Vector3f& n) {
        w = glm::normalize(n);  // Along z-axis
        Vector3f a = (abs(w.x) > 0.9f) ? Vector3f(0.0f, 1.0f, 0.0f) : Vector3f(1.0f, 0.0f, 0.0f);
        v = glm::normalize(glm::cross(w, a));
        u = glm::cross(w, v);
    }
public:
    Vector3f u, v, w;
};



#endif /* onb_hpp */
