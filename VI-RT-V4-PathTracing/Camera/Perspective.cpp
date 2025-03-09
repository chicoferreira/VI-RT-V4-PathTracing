//
//  perspective.cpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 10/02/2023.
//  From https://raytracing.github.io/books/RayTracingInOneWeekend.html#movingcameracodeintoitsownclass
//

#include "Perspective.hpp"

bool Perspective::GenerateRay(const int x, const int y, Ray *r, const float *cam_jitter) {
    Point pc;
    
    if (cam_jitter==NULL) {
        pc.X = (float)x + .5f;
        pc.Y = (float)y + .5f;
    } else {
        // assume that cam_jitter contains 2 floats in (0.0 .. 1.0(
        pc.X = (float)x + cam_jitter[0];
        pc.Y = (float)y + cam_jitter[1];
    }
    
    Point pixel_sample = pixel00_loc + (pc.X * pixel_delta_u) + (pc.Y * pixel_delta_v);
    r->o = Eye;
    if (defocus_angle > 0.f) {
        Point p = random_in_unit_disk();
        r->o = Eye + p.X * defocus_disk_R + p.Y*defocus_disk_Up;
    } else {
        r->o = Eye;
    }
    r->dir = r->o.vec2point(pixel_sample);
    r->dir.normalize();

    r->pix_x = x;
    r->pix_y = y;
    
    r->FaceID = -1;
    r->propagating_eta = 1.f;

    return true;
}
