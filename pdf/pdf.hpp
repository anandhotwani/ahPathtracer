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

/*
float3 Integrator::EstimateDirect(Light *light, UniformSampler *sampler, SurfaceInteraction &interaction, BSDF *bsdf) const {
    float3 directLighting = float3(0.0f);
    float3 f;
    float lightPdf, scatteringPdf;


    // Sample lighting with multiple importance sampling
    // Only sample if the BRDF is non-specular
    if ((bsdf->SupportedLobes & ~BSDFLobe::Specular) != 0) {
        float3 Li = light->SampleLi(sampler, m_scene, interaction, &lightPdf);

        // Make sure the pdf isn't zero and the radiance isn't black
        if (lightPdf != 0.0f && !all(Li)) {
            // Calculate the brdf value
            f = bsdf->Eval(interaction);
            scatteringPdf = bsdf->Pdf(interaction);

            if (scatteringPdf != 0.0f && !all(f)) {
                float weight = PowerHeuristic(1, lightPdf, 1, scatteringPdf);
                directLighting += f * Li * weight / lightPdf;
            }
        }
    }


    // Sample brdf with multiple importance sampling
    bsdf->Sample(interaction, sampler);
    f = bsdf->Eval(interaction);
    scatteringPdf = bsdf->Pdf(interaction);
    if (scatteringPdf != 0.0f && !all(f)) {
        lightPdf = light->PdfLi(m_scene, interaction);
        if (lightPdf == 0.0f) {
            // We didn't hit anything, so ignore the brdf sample
            return directLighting;
        }

        float weight = PowerHeuristic(1, scatteringPdf, 1, lightPdf);
        float3 Li = light->Le();
        directLighting += f * Li * weight / scatteringPdf;
    }

    return directLighting;
}
*/

#endif /* pdf_hpp */
