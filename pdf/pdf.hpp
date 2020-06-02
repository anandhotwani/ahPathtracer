//
//  pdf.hpp
//  ahPathtracer
//
//  Created by Anand Hotwani on 10/05/2020.
//  Copyright © 2020 Anand Hotwani. All rights reserved.
//

#ifndef pdf_hpp
#define pdf_hpp

#include "math.hpp"
#include <vector>
#include <memory>

class PDF  {
    public:
        virtual ~PDF() {}

        virtual float value(const Vector3f& direction) const = 0;
        virtual Vector3f generate() const = 0;
};

class CosinePDF : public PDF {
    public:
        CosinePDF(const Vector3f& w) { uvw.build_from_w(w); }

        virtual float value(const Vector3f& direction) const {
            auto cosine = dot(glm::normalize(direction), uvw.w);
            return (cosine <= 0.0f ) ? 0.0f : cosine / M_PI;
        }

        virtual Vector3f generate() const  {
            return uvw.local(random_cosine_direction());
        }

    public:
        ONB uvw;
};


class LightPDF : public PDF {
    public:
        LightPDF(std::shared_ptr<Shape> p, const Vector3f& origin) : ptr(p), o(origin) {}

        virtual float value(const Vector3f& direction) const {
            return ptr->pdf_value(o, direction);
        }

        virtual Vector3f generate() const {
            return ptr->rand_direction_towards(o);
        }

    public:
        Vector3f o;
        std::shared_ptr<Shape> ptr;
};


class MixedPDF : public PDF {
    public:
    MixedPDF() {}
        MixedPDF(PDF* p0, PDF* p1) {
            p[0] = p0;
            p[1] = p1;
        }

        virtual float value(const Vector3f& direction) const {
        
            // Naive 50/50 for now
            //std::cout<<"mixed PDF value is "<<0.5f * p[0]->value(direction) + 0.5 *p[1]->value(direction)<<"\n\n";
            return 0.5f * p[0]->value(direction) + 0.5 *p[1]->value(direction);
        }

        virtual Vector3f generate() const {
            if (random_float() < 0.5f)
                return p[0]->generate();
            else
                return p[1]->generate();
        }

    public:
        PDF* p[2];
};

#endif /* pdf_hpp */
