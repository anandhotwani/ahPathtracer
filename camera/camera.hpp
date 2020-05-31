//
//  camera.hpp
//  ahPathtracer
//
//  Created by Anand Hotwani on 04/04/2020.
//  Copyright © 2020 Anand Hotwani. All rights reserved.
//

#ifndef camera_hpp
#define camera_hpp

#include "math.hpp"
#include <iostream>

class Camera {
public:
    Camera(uint32_t _width, uint32_t _height, float _fov, Vector3f look_from, Vector3f look_at, float aperture, float focal_dist);
    Ray get_ray(float u, float v);
    
public:
    uint32_t width;
    uint32_t height;
    float fov;
    Matrix4x4f c2w;
    float scale;
    float image_aspect_ratio;
    
    float lens_radius;
    float focus_distance;
};

#endif /* camera_hpp */
