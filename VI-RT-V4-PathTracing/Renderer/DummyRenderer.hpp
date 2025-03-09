//
//  DummyRenderer.hpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 08/02/2025.
//

#ifndef DummyRenderer_hpp
#define DummyRenderer_hpp

#include "renderer.hpp"

class DummyRenderer: public Renderer {
public:
    DummyRenderer (Camera *cam, Scene * scene, Image * img, Shader *shd): Renderer(cam, scene, img, shd) {}
    void Render ();
};

#endif /* DummyRenderer_hpp */
