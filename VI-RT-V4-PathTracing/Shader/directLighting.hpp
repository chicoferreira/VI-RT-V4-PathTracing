//
//  directLighting.hpp
//  VI-RT-3rdVersion
//
//  Created by Luis Paulo Santos on 05/03/2025.
//

#ifndef directLighting_hpp
#define directLighting_hpp

#include <random>

#include "DiffuseTexture.hpp"
#include "RGB.hpp"
#include "intersection.hpp"
#include "scene.hpp"
#include "shader.hpp"

typedef enum {
    ALL_LIGHTS,
    UNIFORM_ONE,
    IMPORTANCE_ONE,
    IMPORTANCE_ONE_NO_DISTANCE,
    DISTANCE_ONE,
    DISTANCE_SQUARED_ONE,
} DIRECT_SAMPLE_MODE;

RGB directLighting(Scene *scene, Intersection isect, BRDF *f, std::mt19937 &rng, std::uniform_real_distribution<float> U_dist, DIRECT_SAMPLE_MODE mode = ALL_LIGHTS);

#endif /* directLighting_hpp */
