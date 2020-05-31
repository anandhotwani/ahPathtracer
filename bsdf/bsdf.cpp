//
//  BSDF.cpp
//  ahPathtracer
//
//  Created by Anand Hotwani on 04/04/2020.
//  Copyright © 2020 Anand Hotwani. All rights reserved.
//

#include "bsdf.hpp"
#include "pdf.hpp"

Vector3f Diffuse::sample(const Vector3f& w) const{
    ONB uvw;
    uvw.build_from_w(w);
    return uvw.local(random_cosine_direction());
}

bool Diffuse::evaluate(const Ray& wi, const SurfaceInteraction& interaction, BSDFData& bsdf_data) const {
    bsdf_data.is_specular = false;
    bsdf_data.albedo = this->color->value(interaction.uv, interaction.p);
    bsdf_data.pdf_ptr = new CosinePDF(interaction.n);
    return true;
}

float Diffuse::pdf(const Ray &r_in, const SurfaceInteraction &interaction, const Ray &scattered) const {
    float cosine = glm::dot(interaction.n, glm::normalize(scattered.d));
    return cosine < 0.0f ? 0.0f : cosine / M_PI;
}

bool Metal::evaluate(const Ray &wi,  const SurfaceInteraction &interaction, BSDFData& bsdf_data) const {
    Vector3f reflected_direction = reflect(glm::normalize(wi.d), interaction.n);
    bsdf_data.speculuar_ray = Ray(interaction.p, reflected_direction + roughness * random_in_unit_sphere());
    bsdf_data.albedo = this->albedo;
    bsdf_data.is_specular = true;
    bsdf_data.pdf_ptr = 0;
    return true;
}

bool Glass::evaluate(const Ray &wi, Ray &wo, const SurfaceInteraction &interaction, Vector3f &albedo) const {
    albedo = Vector3f(1.0f, 1.0f, 1.0f);
    float etai_over_etat;
    if (interaction.front_facing) {
        etai_over_etat = 1.0f / this->ior;
    } else {
        etai_over_etat = ior;
    }
    Vector3f unit_direction = glm::normalize(wi.d);
    float cos_theta = std::min(glm::dot(-unit_direction, interaction.n), 1.0f);
    float sin_theta = sqrtf(1.0f - cos_theta * cos_theta);  // cos2 + sin2 = 1
    if (etai_over_etat * sin_theta > 1.0f) {
        Vector3f reflected_direction = reflect(unit_direction, interaction.n);
        wo = Ray(interaction.p, reflected_direction);
        return true;
    }
    
    float reflection_probability = fr_schlick(cos_theta, this->ior);
    
    if (random_float() < reflection_probability) {
        Vector3f reflected_direction = reflect(unit_direction, interaction.n);
        wo = Ray(interaction.p, reflected_direction);
        return true;
    }
    
    Vector3f refracted_direction = refract(unit_direction, interaction.n, etai_over_etat);
    wo = Ray(interaction.p, refracted_direction);
    return true;
}

Vector3f Microfacet::sample(const Vector3f& w) const {
    
    ONB uvw;
    uvw.build_from_w(w);
    
    float e1 = random_float();
    float e2 = random_float();
    
    float a2 = this->roughness * this->roughness;
    float theta = acosf(sqrtf((1.0f - e1) / (e1 * (a2 - 1.0f) + 1.0f)));
    float phi = 2.0f * M_PI * e2;
    
    float x = sin(theta) * cos(phi);
    float y = sin(theta) * sin(phi);
    float z = cos(theta);
    
    Vector3f outgoing_direction = Vector3f(x, y, z);
    outgoing_direction = glm::normalize(outgoing_direction);
    return uvw.local(outgoing_direction);
}

bool Microfacet::evaluate(const Ray& wi, const SurfaceInteraction &interaction, BSDFData& bsdf_data) const {
    
    Vector3f h = glm::normalize(-wi.d + interaction.wo);
    
    float G = GGX_PartialGeometryTerm(-wi.d, interaction.n, h, this->roughness);
    float D = GGX_Distribution(interaction.n, h, this->roughness);
    float F = schlick(0.44f, 1.0f, -wi.d, h);

    float spec = (D * F * G) / (4.0f * glm::dot(interaction.wo, interaction.n) * glm::dot(-wi.d, interaction.n));
    bsdf_data.albedo = this->color->value(interaction.uv, interaction.p) * spec;

    return true;
}


float Microfacet::pdf(const Ray& r_in, const SurfaceInteraction& interaction, const Ray& scattered) const {
    
    float e1 = random_float();
    float e2 = random_float();
    
    float a2 = this->roughness * this->roughness;
    float theta = acosf(sqrtf((1.0f - e1) / (e1 * (a2 - 1.0f) + 1.0f)));

    float value = (a2 * cos(theta) * sin(theta)) / (M_PI * powf(((a2 - 1.0f) * (cos(theta) * cos(theta)) + 1.0f), 2.0f));
    
    float cosine = glm::dot(interaction.n, glm::normalize(scattered.d));
    return cosine < 0.0f ? 0.0f : cosine / M_PI;
    
}
