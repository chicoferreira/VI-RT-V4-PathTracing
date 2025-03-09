//
//  DummyShader.cpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 08/02/2025.
//

#include "DummyShader.hpp"

RGB DummyShader::shade(bool intersected, Intersection isect, int depth) {
    /*if (isect.pix_x==320 && isect.pix_y==240) {
        fprintf (stderr, "DUMMY SHADER. intersected = %s !\n", (intersected?"TRUE":"FALSE"));
        fflush(stderr);
    }*/

    RGB color(((float) isect.pix_x) / W, ((float) isect.pix_y) / H, 0.);
    
    return color;
};
