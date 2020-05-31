//
//  file_texture.cpp
//  ahPathtracer
//
//  Created by Anand Hotwani on 24/04/2020.
//  Copyright © 2020 Anand Hotwani. All rights reserved.
//

#include "file_texture.hpp"

Vector3f File_Texture::value(Vector2f uv, const Vector3f &p) const {
    // If no texture data, output debug color
    if (data == nullptr)
        return Vector3f(0.0f, 1.0f, 1.0f);
    
    int i = static_cast<int>(uv[0] * width);
    int j = static_cast<int>((1 - uv[1]) * height - 0.001f);
    
    if (i < 0) i = 0;
    if (i > width - 1) i = width - 1;
    if (j < 0) j = 0;
    if (j > height - 1) j = height - 1;
    
    float r = static_cast<int>(data[3 * i + 3 * width * j + 0]) / 255.0f;
    float g = static_cast<int>(data[3 * i + 3 * width * j + 1]) / 255.0f;
    float b = static_cast<int>(data[3 * i + 3 * width * j + 2]) / 255.0f;
    
    // Gamma correct
    r = powf(r, 2.2f);
    g = powf(g, 2.2f);
    b = powf(b, 2.2f);
    
    return Vector3f(r, g, b);
}

Vector3f EXR_Texture::value(Vector2f uv, const Vector3f &p) const {
    // If no texture data, output debug color
    if (rgba == nullptr)
        return Vector3f(1.0f, 0.0f, 1.0f);
    
    int i = static_cast<int>(uv[0] * width);
    int j = static_cast<int>((1 - uv[1]) * height - 0.001f);
    
    if (i < 0) i = 0;
    if (i > width - 1) i = width - 1;
    if (j < 0) j = 0;
    if (j > height - 1) j = height - 1;
    
    float r = rgba[4 * i + 4 * width * j + 0];
    float g = rgba[4 * i + 4 * width * j + 1];
    float b = rgba[4 * i + 4 * width * j + 2];
    
    return powf(2.0f, exposure) * Vector3f(r, g, b);
}
