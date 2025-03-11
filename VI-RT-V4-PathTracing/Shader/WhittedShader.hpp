//
//  AmbientShader.hpp
//  VI-RT-LPS
//
//  Created by Luis Paulo Santos on 14/03/2023.
//

#ifndef WhittedShader_hpp
#define WhittedShader_hpp

#include "shader.hpp"
#include "BRDF.hpp"
#include <random>

class WhittedShader: public Shader {
    RGB background;
    RGB specularReflection (Intersection isect, BRDF *f, int depth);
    RGB specularTransmission (Intersection isect, BRDF *f, int depth);
public:
    WhittedShader (Scene *scene, RGB bg): background(bg), Shader(scene) {}
    RGB shade (bool intersected, Intersection isect, int depth);
};

#endif /* AmbientShader_hpp */
