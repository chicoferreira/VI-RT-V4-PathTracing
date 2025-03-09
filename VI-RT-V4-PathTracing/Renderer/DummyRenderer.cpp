//
//  DummyRenderer.cpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 08/02/2025.
//

#include "DummyRenderer.hpp"

void DummyRenderer::Render () {
    int W=0,H=0;  // resolution
    int x,y;

    // get resolution from the camera
    cam->getResolution(&W, &H);
    
    // main rendering loop: get primary rays from the camera until done
    for (y=0 ; y< H ; y++) {  // loop over rows
        printf ("%c\r",(y&1 ? '/' : '\\'));
        for (x=0 ; x< W ; x++) { // loop over columns
            Ray primary;
            Intersection isect;
            RGB color;
          
            // Generate Ray (camera)
            cam->GenerateRay(x, y, &primary);
            
            // trace ray (scene)
            // we have no scene
            // let's just forward (x,y) to isect
            isect.pix_x=x;
            isect.pix_y=y;
            
            
            // shade this pixel (shader)
            color = shd->shade(true, isect, 0);
            
            // write the result into the image frame buffer (image)
            img->set(x,y,color);
            
        } // loop over columns
    }   // loop over rows
}
