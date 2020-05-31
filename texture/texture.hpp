//
//  texture.hpp
//  ahPathtracer
//
//  Created by Anand Hotwani on 24/04/2020.
//  Copyright © 2020 Anand Hotwani. All rights reserved.
//

#ifndef texture_hpp
#define texture_hpp

#include "math.hpp"

class Texture {
    public:
        virtual Vector3f value(Vector2f uv, const Vector3f& p) const = 0;
};

class Checkered_Texture : public Texture {
public:
    Checkered_Texture() {}
    Checkered_Texture(std::shared_ptr<Texture> t0, std::shared_ptr<Texture> t1) : even(t0), odd(t1) {}
    virtual Vector3f value(Vector2f uv, const Vector3f &p) const;
public:
    std::shared_ptr<Texture> odd;
    std::shared_ptr<Texture> even;
};

class Constant_Texture : public Texture {
    public:
        Constant_Texture() {}
        Constant_Texture(Vector3f c) : color(c) {}

        virtual Vector3f value(Vector2f uv, const Vector3f& p) const {
            return color;
        }

    public:
        Vector3f color;
};

#endif /* texture_hpp */
