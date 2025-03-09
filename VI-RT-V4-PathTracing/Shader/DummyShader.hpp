//
//  DummyShader.hpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 08/02/2025.
//

#ifndef DummyShader_hpp
#define DummyShader_hpp

#include "shader.hpp"

class DummyShader: public Shader {
    float W, H;
public:
    DummyShader (Scene *scene, const int _W, const int _H): Shader(scene) {
        W = (float)_W;
        H = (float)_H;
    }
    RGB shade (bool intersected, Intersection isect, int depth);
};

#endif /* DummyShader_hpp */
