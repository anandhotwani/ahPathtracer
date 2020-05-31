//
//  BSDF.hpp
//  ahPathtracer
//
//  Created by Anand Hotwani on 04/04/2020.
//  Copyright © 2020 Anand Hotwani. All rights reserved.
//

#ifndef BSDF_hpp
#define BSDF_hpp

#include "math.hpp"
#include "shape.hpp"
#include "texture.hpp"
#include "file_texture.hpp"

#include "microfacet.hpp"


class PDF;

struct BSDFData {
    Ray speculuar_ray;
    bool is_specular;
    Vector3f albedo;
    PDF* pdf_ptr;
};

class BSDF {
public:
    virtual Vector3f emitted(const Ray& r_in, const SurfaceInteraction& interaction, Vector2f uv, Vector3f& p) const { return Vector3f(0.0f); }
    virtual Vector3f sample(const Vector3f& w) const {return Vector3f(0.0f); }
    virtual bool evaluate(const Ray& wi, const SurfaceInteraction& interaction, BSDFData& bsdf_data) const { return false; };
    virtual float pdf(const Ray& r_in, const SurfaceInteraction& interaction, const Ray& scattered) const { return 0; }
    
};

class Diffuse : public BSDF {
public:
    Diffuse(std::shared_ptr<Texture> a) : color(a) {}
    virtual Vector3f sample(const Vector3f& w) const;
    virtual bool evaluate(const Ray& wi, const SurfaceInteraction &interaction, BSDFData& bsdf_data) const;
    virtual float pdf(const Ray& r_in, const SurfaceInteraction& interaction, const Ray& scattered) const;
public:
    std::shared_ptr<Texture> color;
};

class Metal : public BSDF {
public:
    Metal(const Vector3f &a, float r) : albedo(a), roughness(saturate(r)) {}
    virtual bool evaluate(const Ray &wi, const SurfaceInteraction &interaction, BSDFData& bsdf_data) const;
public:
    Vector3f albedo;
    float roughness;
    
};

class Glass : public BSDF {
public:
    Glass(float ri) : ior(ri) {}
    virtual bool evaluate(const Ray &wi, Ray &wo, const SurfaceInteraction &interaction, Vector3f &albedo) const;
public:
    float ior;
};

class Emissive : public BSDF {
public:
    Emissive(std::shared_ptr<Texture> t) : incandescant(t) {}
    Vector3f emitted(const Ray& r_in, const SurfaceInteraction& interaction, Vector2f uv, Vector3f &p) const {
        if (!interaction.front_facing)
            return incandescant->value(uv, p);
        else
            return Vector3f(0.0f);
    }
    virtual bool evaluate(const Ray &wi, Ray &wo, const SurfaceInteraction &interaction, Vector3f &albedo) const { return false; }
public:
    std::shared_ptr<Texture> incandescant;
};

class Microfacet : public BSDF {
public:
    Microfacet(std::shared_ptr<Texture> a, float r) : color(a), roughness(r) {}
    virtual Vector3f sample(const Vector3f& w) const;
    virtual bool evaluate(const Ray& wi, const SurfaceInteraction &interaction, BSDFData& bsdf_data) const;
    virtual float pdf(const Ray& r_in, const SurfaceInteraction& interaction, const Ray& scattered) const;
public:
    std::shared_ptr<Texture> color;
    float roughness;
};

#endif /* BSDF_hpp */
