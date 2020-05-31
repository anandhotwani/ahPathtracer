//
//  texture.cpp
//  ahPathtracer
//
//  Created by Anand Hotwani on 24/04/2020.
//  Copyright © 2020 Anand Hotwani. All rights reserved.
//

#include "texture.hpp"

Vector3f Checkered_Texture::value(Vector2f uv, const Vector3f &p) const {
    float sines = sinf(100.0f * p.x) * sin(100.0f *p.y) * sin(100.0f * p.z);
    if (sines < 0.0f)
        return odd->value(uv, p);
    else
        return even->value(uv, p);
}
