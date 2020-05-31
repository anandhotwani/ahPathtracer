//
//  file_texture.hpp
//  ahPathtracer
//
//  Created by Anand Hotwani on 24/04/2020.
//  Copyright © 2020 Anand Hotwani. All rights reserved.
//

#ifndef file_texture_hpp
#define file_texture_hpp

#include "texture.hpp"

class File_Texture : public Texture {
public:
    File_Texture() {}
    File_Texture(unsigned char* pixels, int w, int h) : data(pixels), width(w), height(h) {}
    ~File_Texture() { delete data; }
    
    virtual Vector3f value(Vector2f uv, const Vector3f& p) const;
public:
    unsigned char* data;
    int width, height;
};

class EXR_Texture : public Texture {
public:
    EXR_Texture() {}
    EXR_Texture(float e, float* pixels, int w, int h) : exposure(e), rgba(pixels), width(w), height(h) {}
    ~EXR_Texture() { delete rgba; }
    virtual Vector3f value(Vector2f uv, const Vector3f& p) const;
public:
    float* rgba;
    int width, height;
    float exposure;
};

#endif /* file_texture_hpp */
