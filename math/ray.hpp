//
//  ray.hpp
//  ahPathtracer
//
//  Created by Anand Hotwani on 29/03/2020.
//  Copyright © 2020 Anand Hotwani. All rights reserved.
//

#ifndef ray_hpp
#define ray_hpp

#include "vector.hpp"

class Ray {
public:
    Ray() {}
    Ray(const Vector3f& orig, const Vector3f& dir) : o(orig), d(dir) {}
public:
    Vector3f o;
    Vector3f d;
};

#endif /* ray_hpp */
