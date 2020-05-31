//
//  matrix4x4.hpp
//  ahPathtracer
//
//  Created by Anand Hotwani on 07/04/2020.
//  Copyright © 2020 Anand Hotwani. All rights reserved.
//

#ifndef matrix4x4_hpp
#define matrix4x4_hpp

#include "vector.hpp"
#include <glm/gtc/matrix_transform.hpp>

typedef glm::mat4 Matrix4x4f;

inline Vector3f TransformDirMatrix(const Matrix4x4f &x, const Vector3f &src) {
    
    Vector3f dst;
    Vector4f temp = glm::normalize(x * Vector4f(src, 0.0f));
    
    dst.x = temp.x;
    dst.y = temp.y;
    dst.z = temp.z;
    
    return dst;
}

inline Vector3f TransformPointMatrix(const Matrix4x4f &x, const Vector3f &src) {
    
    Vector3f dst;
    Vector4f temp = x * Vector4f(src, 1.0f);
    
    dst.x = temp.x;
    dst.y = temp.y;
    dst.z = temp.z;
    
    return dst;
}

#endif /* matrix4x4_hpp */
