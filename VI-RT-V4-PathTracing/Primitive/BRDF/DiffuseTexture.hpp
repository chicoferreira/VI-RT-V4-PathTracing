//
//  DiffuseTexture.hpp
//  VI-RT-V4-PathTracing
//
//  Created by Luis Paulo Santos on 19/03/2025.
//

#ifndef DiffuseTexture_hpp
#define DiffuseTexture_hpp

#include "BRDF.hpp"
#include "ImagePPM.hpp"
#include "Triangle.hpp"

class DiffuseTexture: public BRDF {
private:
    ImagePPM texture;
    float tex_W, tex_H;
public:
    DiffuseTexture(std::string filename) {
        texture.Load(filename);
        textured=true;
        tex_W = float (texture.W);
        tex_H = float (texture.H);
    }
    RGB GetKd (Vec2 TexCoord) {
        int x = (int)floor(TexCoord.u * tex_W);
        int y = (int)floor(TexCoord.v * tex_H);

        RGB color = Kd*texture.get(x, y);
        return color;
    }
};


#endif /* DiffuseTexture_hpp */
