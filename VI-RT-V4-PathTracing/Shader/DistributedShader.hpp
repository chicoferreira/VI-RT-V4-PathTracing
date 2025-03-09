//
//  AmbientShader.hpp
//  VI-RT-LPS
//
//  Created by Luis Paulo Santos on 14/03/2023.
//

#ifndef DistributedShader_hpp
#define DistributedShader_hpp

#include "shader.hpp"
#include "BRDF.hpp"
#include "directLighting.hpp"
#include <random>

class DistributedShader: public Shader {
    RGB background;
    RGB specularReflection (Intersection isect, BRDF *f, int depth);
    RGB specularTransmission (Intersection isect, BRDF *f, int depth);
    /****************************************
     
     Our Random Number Generator (rng) */
    std::random_device rdev{};
    std::mt19937 rng{rdev()};
    std::uniform_real_distribution<float>U_dist{0.0,1.0};  // uniform distribution in[0,1[


public:
    DistributedShader (Scene *scene, RGB bg): background(bg), Shader(scene) {}
    RGB shade (bool intersected, Intersection isect, int depth);
};

#endif /* AmbientShader_hpp */
