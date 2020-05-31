//
//  camera.cpp
//  ahPathtracer
//
//  Created by Anand Hotwani on 04/04/2020.
//  Copyright © 2020 Anand Hotwani. All rights reserved.
//

#include "camera.hpp"

Camera::Camera(uint32_t _width, uint32_t _height, float _fov, Vector3f look_from, Vector3f look_at, float aperture, float focal_dist) {
    width = _width;
    height = _height;
    fov = _fov; // Vertical FOV
    
    lens_radius = aperture * 0.5f;
    focus_distance = focal_dist;
    
    // Calculate c2w from lookFrom and lookAt
    this->c2w = Matrix4x4f(    1.0f, 0.0f, 0.0f, 0.0f,
                                     0.0,  1.0f, 0.0f, 0.0f,
                                     0.0f, 0.0f, 1.0f, 0.0f,
                                     0.0f, 0.0f, 0.0f, 1.0f);   // Identity matrix
    
    
    Vector3f tmp = Vector3f (0.0f, 1.0f, 0.0f); // Generic up-vector
    
    Vector3f forward = glm::normalize(look_from - look_at);
    Vector3f right = glm::cross(glm::normalize(tmp), forward);
    Vector3f up = glm::cross(forward, right);
    c2w[0][0] = right.x;
    c2w[0][1] = right.y;
    c2w[0][2] = right.z;
    c2w[1][0] = up.x;
    c2w[1][1] = up.y;
    c2w[1][2] = up.z;
    c2w[2][0] = forward.x;
    c2w[2][1] = forward.y;
    c2w[2][2] = forward.z;
    c2w[3][0] = look_from.x;
    c2w[3][1] = look_from.y;
    c2w[3][2] = look_from.z;
    
    scale = tanf(deg2rad(fov * 0.5f));
    
    image_aspect_ratio = width / (float)height;
}

Ray Camera::get_ray(float u, float v) {
    
    // Maya-style
    float ndc_x = (2.0f * (u + random_float()) / (float)width - 1.0f) * scale;
    float ndc_y = (1.0f - 2.0f * (v + random_float()) / (float)height) * scale * 1.0f / image_aspect_ratio;
        
    Ray r;
    Vector3f orig = Vector3f(0.0f, 0.0f, 0.0f);
    Vector3f dir =  glm::normalize(Vector3f(ndc_x, ndc_y, -1.0f));
    
    // Reference: https://computergraphics.stackexchange.com/questions/8130/defocus-blur-computing-the-pixel-plane-distance
    if (lens_radius > 0.0f) {
        
        // Generate random 2D point in range [0,1]
        Vector2f lens = Vector2f(random_float(), random_float());
        // Scale to [-1, 1]
        lens = concentric_sample_disk(lens);
        // Scale by aperature radius
        lens *= lens_radius;
        
        orig.x = lens.x;
        orig.y = lens.y;

        // Compute point on plane of focus, intersection on z-axis
        float intersection = std::abs(focus_distance / dir.z);
        Vector3f focus_point = dir * intersection;
        dir = glm::normalize(focus_point - orig);
    }
    
    r.o = TransformPointMatrix(c2w, orig);
    r.d = TransformDirMatrix(c2w, dir);
    
    return r;
}
