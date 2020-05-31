//
//  main.cpp
//  ahPathtracer
//
//  Created by Anand Hotwani on 29/03/2020.
//  Copyright © 2020 Anand Hotwani. All rights reserved.
//

#include <iostream>
#include <fstream>
#include "math.hpp"
#include "sphere.hpp"
#include "triangle.hpp"
#include "rect.hpp"
#include "trianglemesh.hpp"
#include "scene.hpp"
#include "camera.hpp"
#include "BSDF.hpp"
#include "bvh.hpp"
#include "pdf.hpp"

//Texture maps
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define TINYEXR_IMPLEMENTATION
#include "tinyexr.h"

// For simple multi-threading
#include <thread>
#include <mutex>

// To calculate UV coords
// Vector3f UV = v0.uv * v0.barycentric + v1.uv * v1.barycentric + v2.uv * v2.barycentric;

struct Options {
    uint32_t width;
    uint32_t height;
    float fov;
    uint32_t spp;
    uint32_t MAX_DEPTH;
    Vector3f look_from;
    Vector3f look_at;
};

// IBL setup
int width, height;
float* rgba;
const char* err;
int ret = LoadEXR(&rgba, &width, &height, "ja_carpark.exr", &err);
auto file = std::make_shared<EXR_Texture>(1.0f, rgba, width, height);
auto light = std::make_shared<Emissive>(file);
auto ibl = std::make_shared<Sphere>(Matrix4x4f(), 99999.9f, light);
// IBL setup

Vector3f cast_ray(const Ray& r, const Scene& scene, uint32_t depth) {
        
        SurfaceInteraction interaction;

        // Max depth check
        if (depth <= 0)
            return Vector3f(0.0f);

        if (scene.intersect(r, MinFloat + ShadowBias, MaxFloat, interaction)) {
            
            // Pick one light randomly for now
            int one_light = random_int(0, scene.lights.size() - 1);
            std::shared_ptr<Shape> light_shape = scene.lights[0]; //(std::make_shared<Rect>(light2world, v00, v10, v11, v01, light_emit));
            LightPDF light_pdf(light_shape, interaction.p);
            
            CosinePDF cosine_pdf(interaction.n);
            MixedPDF pdf(&light_pdf, &cosine_pdf);
                    
            float pdf_val = pdf.value(interaction.n);
            
            BSDFData bsdf_data;
            Vector3f emitted = interaction.BSDF_ptr->emitted(r, interaction, interaction.uv, interaction.p);
            interaction.wo = glm::normalize(pdf.generate());
            Ray outgoing_direction = Ray(interaction.p, interaction.wo);
            
            if (!interaction.BSDF_ptr->evaluate(r, interaction, bsdf_data))
                return emitted;

            // Check if specular
            if (bsdf_data.is_specular) {
                return bsdf_data.albedo * cast_ray(bsdf_data.speculuar_ray, scene, depth - 1);
            }
            
            
            return emitted + bsdf_data.albedo * interaction.BSDF_ptr->pdf(r, interaction, outgoing_direction) * cast_ray(outgoing_direction, scene, depth - 1) / pdf_val;    // Divide by the PDF(x)
        }
    
#if 1
    // Use HDRI for lighting
    ibl->intersect(r, MinFloat + ShadowBias, MaxFloat, interaction);
    return ibl->BSDF_ptr->emitted(r, interaction, interaction.uv, interaction.p);
#else
//    return Vector3f(0.0f);
    float t = 0.5f * (r.d.y + 1.0f);
    Vector3f sky_white = Vector3f(0.18f);
    Vector3f sky_blue = Vector3f(0.09f);
    return lerp(sky_white, sky_blue, t);
#endif

}


inline int intmax(int a, int b) { return a > b ? a : b; }

void render_loop(int* next_tile_x, int* next_tile_y, std::mutex* tile_info_mutex, int tile_width, int tile_height, Options options, Scene scene, Vector3f* pixel_buffer) {
    
    float inv_samples = 1.0f / float(options.spp);  // Pre-calculating to avoid many divisions
    float aperture = 0.0f;
    float focus_distance = glm::length(options.look_from - options.look_at); // For depth-of-field
    
    // Make Camera
    Camera camera(options.width, options.height, options.fov, options.look_from, options.look_at, aperture, focus_distance);

    
    // Run until all tiles have been processed
    while (true) {
        // Use mutex to access shared information about next tile to render
        tile_info_mutex->lock();
        
        // Get next tile screen position from shared variables
        int tile_x = *next_tile_x;
        int tile_y = *next_tile_y;
        
        // Advance to next tile
        *next_tile_x += tile_width;
        
        // Handle screen borders
        if (*next_tile_x >= options.width) {
            // Advance to next line
            *next_tile_x = 0;
            *next_tile_y += tile_height;
        }
        
        tile_info_mutex->unlock();
        
        // Terminate ray tracing when we go beyond screen borders
        if (tile_y >= options.height) return;
        
        // Clip tile to screen borders
        tile_width = tile_width - intmax(0, tile_x + tile_width - options.width);
        tile_height = tile_height - intmax(0, tile_y + tile_height - options.height);
 
        for (uint32_t j = tile_y; j < tile_y + tile_height; ++j) {
//            std::cout<<"Scanlines remaining : "<<options.height - j<<"\n";
            for (uint32_t i = tile_x; i < tile_x + tile_width; ++i) {
                Vector3f colour = Vector3f(0.0f);
                // Samples per pixel
                for (uint32_t s = 0; s < options.spp; ++s) {
                
                    Ray ray = camera.get_ray(i, j);
                    colour = colour + cast_ray(ray, scene, options.MAX_DEPTH);
                    // NaN check
                    if (isinf(colour.x)) colour.x = 0.0f;
                    if (isinf(colour.y)) colour.y = 0.0f;
                    if (isinf(colour.z)) colour.z = 0.0f;
                    if (isnan(colour.x)) colour.x = 0.0f;
                    if (isnan(colour.y)) colour.y = 0.0f;
                    if (isnan(colour.z)) colour.z = 0.0f;
                    
                }
                // Calculate mean
                colour.x *= inv_samples;
                colour.y *= inv_samples;
                colour.z *= inv_samples;
            
                // Gamma correct
                colour.x = powf(colour.x, 1.0f / 2.2f);
                colour.y = powf(colour.y, 1.0f / 2.2f);
                colour.z = powf(colour.z, 1.0f / 2.2f);
                
                int index = (i + j * options.width);
                pixel_buffer[index] = colour;
            }
        }
    }
    
}


int main() {
        
    Options options;
    options.width   = 1024;
    options.height  = 429;
    options.fov     = 60.0f;
    options.spp     = 64;    // 64
    options.MAX_DEPTH = 4;
    options.look_from = Vector3f(0.0f, 15.0f, 100.0f);
    options.look_at = Vector3f(0.0f, 15.0f, 0.0f);
////    options.look_from = Vector3f(-240.0f, 123.0f, 333.0f);
    options.look_from = Vector3f(-240.0f, 123.0f, 333.0f);
    options.look_at = Vector3f(0.5f, 73.5f, 9.45f);

    
    int tile_width, tile_height = 16;

    // Make scene
    Scene scene;
    
    // Make o2w matrices
    
    const Matrix4x4f kIdentity = Matrix4x4f(    1.0f, 0.0f, 0.0f, 0.0f,
                                                0.0,  1.0f, 0.0f, 0.0f,
                                                0.0f, 0.0f, 1.0f, 0.0f,
                                                0.0f, 0.0f, 0.0f, 1.0f);   // Identity matrix
    

    auto greyball = std::make_shared<Diffuse>(std::make_shared<Constant_Texture>(Vector3f(0.187f)));
    auto chromeball = std::make_shared<Metal>(Vector3f(0.85f), 0.25f);
    auto backdrop = std::make_shared<Diffuse>(std::make_shared<Constant_Texture>(Vector3f(0.36f)));
    auto lambert = std::make_shared<Diffuse>(std::make_shared<Constant_Texture>(Vector3f(0.45f)));
    auto green = std::make_shared<Diffuse>(std::make_shared<Constant_Texture>(Vector3f(0.0f, 0.65f, 0.0f)));
    auto red = std::make_shared<Diffuse>(std::make_shared<Constant_Texture>(Vector3f(0.65f, 0.0f, 0.0f)));
    auto dino = std::make_shared<Diffuse>(std::make_shared<Constant_Texture>(Vector3f(0.28f, 0.48f, 0.48f)));
    auto light = std::make_shared<Emissive>(std::make_shared<Constant_Texture>(Vector3f(2.0f)));
    
    const Matrix4x4f greyball2world = Matrix4x4f(    1.0f, 0.0f, 0.0f, 0.0f,
                                                    0.0,  1.0f, 0.0f, 0.0f,
                                                    0.0f, 0.0f, 1.0f, 0.0f,
                                                    79.0f, 11.0f, 80.0f, 1.0f);
    
    const Matrix4x4f chromeball2world = Matrix4x4f( 1.0f, 0.0f, 0.0f, 0.0f,
                                                    0.0,  1.0f, 0.0f, 0.0f,
                                                    0.0f, 0.0f, 1.0f, 0.0f,
                                                    98.0f, 11.0f, 95.0f, 1.0f);
    
    
    const Matrix4x4f light2world = Matrix4x4f(    -1.0f, 0.0f, 0.0f, 0.0f,
                                                  0.0,  -1.0f, 0.0f, 0.0f,
                                                0.0f, 0.0f, 1.0f, 0.0f,
                                                0.0f, 105.0f, -5.0f, 1.0f);
    
    // Add calibration balls
    scene.Add(std::make_shared<Sphere>(greyball2world, 11.0f, greyball));
    scene.Add(std::make_shared<Sphere>(chromeball2world, 11.0f, chromeball));
    
    // Add Macbeth chart
    int width, height, nn;
    unsigned char* texture_data = stbi_load("colorchecker_UV.png", &width, &height, &nn, 0);
    auto file = std::make_shared<File_Texture>(texture_data, width, height);
    auto colorchecker = std::make_shared<Diffuse>(file);
    scene.Add(std::make_shared<TriangleMesh>(kIdentity, "macbeth2.obj", colorchecker));
    
    // Vertice for area light
    Vector3f v00 = Vector3f(-5.0f, 0.0f, 5.0f);
    Vector3f v10 = Vector3f(-5.0f, 0.0f, -5.0f);
    Vector3f v11 = Vector3f(5.0f, 0.0f, 5.0f);
    Vector3f v01 = Vector3f(5.0f, 0.0f, -5.0f);
    // Add Area light
    scene.Add(std::make_shared<Rect>(light2world, v00, v10, v11, v01, light));      // Add so it can be hit in scenes
    scene.AddLight(std::make_shared<Rect>(light2world, v00, v10, v11, v01, light)); // Add explicitly to list of lights


    
    std::cout<<"Building scene acceleration structure...\n";
    Scene scene_bvh = Scene(std::make_shared<BVHNode>(scene));
    std::cout<<"Successfully constructed scene acceleration structure! \n\n";
    // Output pixels
    Vector3f* pixel_buffer = new Vector3f[options.width * options.height];
    
    unsigned int threads_num = std::thread::hardware_concurrency();
    std::cout<<"Threads num is "<<threads_num<<"\n";
    std::thread* threads = new std::thread[threads_num];
    
    // Variable info for sharing information about next tile to be rendered between threads
    int next_tile_x = 0;
    int next_tile_y = 0;
    std::mutex tile_info_mutex;
    
    // Time keeping
     auto start = std::chrono::system_clock::now();
    
    std::cout<<"Rendering scene...\n";
    // Spawn threads
    for (unsigned int t = 0; t < threads_num; ++t) {
        threads[t] = std::thread(render_loop, &next_tile_x, &next_tile_y, &tile_info_mutex, tile_width, tile_height, options, scene, pixel_buffer);
    }
    
    // Wait for threads to finish
    for (unsigned int t = 0; t < threads_num; ++t) {
        threads[t].join();
    }
    
    std::cout<<"Render complete! \n\n";
    
    // Time keeping
    auto end = std::chrono::system_clock::now();
    // Time keeping
    std::time_t end_time = std::chrono::system_clock::to_time_t(end);
    std::chrono::duration<double> elapsed_seconds = end-start;
    std::cout << "Render completed on " << std::ctime(&end_time) << "\nTime taken to render : " << elapsed_seconds.count() << "s\n\n";
    //Time keeping
    
    // Write image
    std::cout<<"Writing to PPM image...\n";
    std::ofstream ofs("out.ppm", std::ios::out | std::ios::binary);
    ofs << "P6\n" << options.width << " " << options.height << "\n255\n";
    
    for (int i = 0; i < options.width * options.height; ++i) {
        char r = (char)(255.0f * saturate(pixel_buffer[i].x));
        char g = (char)(255.0f * saturate(pixel_buffer[i].y));
        char b = (char)(255.0f * saturate(pixel_buffer[i].z));
        ofs << r << g << b;
    }
    delete[] pixel_buffer;
    ofs.close();
    std::cout<<"Successfully written out image! Exiting program...\n\n";
    return EXIT_SUCCESS;
    
}
