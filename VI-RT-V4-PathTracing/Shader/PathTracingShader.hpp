//
//  IndirectDiffuse.hpp
//  VI-RT-LPS
//
//  Created by Luis Paulo Santos on 14/03/2023.
//

#ifndef PathTracing_hpp
#define PathTracing_hpp

#include "shader.hpp"
#include "BRDF.hpp"
#include "directLighting.hpp"
#include <random>

class PathTracing: public Shader {
    RGB background;
    RGB diffuseReflection (Intersection isect, BRDF *f, int depth);
    RGB specularReflection (Intersection isect, BRDF *f, int depth);
    RGB specularTransmission (Intersection isect, BRDF *f, int depth);
    /****************************************
     
     Our Random Number Generator (rng) */
    std::random_device rdev{};
    std::mt19937 rng{rdev()};
    std::uniform_real_distribution<float>U_dist{0.0,1.0};  // uniform distribution in[0,1[


public:
    PathTracing (Scene *scene, RGB bg): background(bg), Shader(scene) {}
    RGB shade (bool intersected, Intersection isect, int depth);
};

#endif /* PathTracing_hpp */
