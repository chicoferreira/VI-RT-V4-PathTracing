//
//  ThinLens.hpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 10/02/2023.
//  From https://raytracing.github.io/books/RayTracingInOneWeekend.html#movingcameracodeintoitsownclass
//

#ifndef Perspective_hpp
#define Perspective_hpp

#include "camera.hpp"
#include "ray.hpp"
#include "vector.hpp"
#include <random>

class Perspective: public Camera {
private:
    Vector Up;
    float tan_halfH;
    Vector defocus_disk_Up, defocus_disk_R;
    int W, H;
    float defocus_angle;

    /****************************************
     
     Our Random Number Generator (rng) */
    std::random_device rdev{};
    std::mt19937 rng{rdev()};
    std::uniform_real_distribution<float>U_dist{-1.0,1.0};  // uniform distribution in[0,1[
    
    inline Point random_in_unit_disk() {
        while (true) {
            Point p(U_dist(rng), U_dist(rng), 0.);
            if ((p.X*p.X + p.Y*p.Y) < 1.)
                return p;
        }
    }
    
    Point Eye, At;         // Camera center
    Point pixel00_loc;    // Location of pixel 0, 0
    Vector pixel_delta_u;  // Offset to pixel to the right
    Vector pixel_delta_v;  // Offset to pixel below

public:
    Perspective (const Point _Eye, const Point _At, const Vector _Up, const int _W, const int _H, const float _fovH, float _defocus_angle=0, float _focus_dist=1.): Eye(_Eye), At(_At), W(_W), H(_H), defocus_angle(_defocus_angle) {

        // compute camera 2 world transform
        Vector forward (At.X-Eye.X, At.Y-Eye.Y, At.Z-Eye.Z);
        forward.normalize();
        Vector right = forward.cross(_Up);
        right.normalize();
        // recompute UP exactly as the cross product  right X forward
        Up = right.cross(forward);
        Up.normalize();

        // Determine viewport dimensions.
        // precompute the tangents
        tan_halfH = tanf(_fovH/2.f);
        float viewport_height = 2.0 * tan_halfH * _focus_dist;
        //float viewport_height = 2.0 * focal_length;
        float viewport_width = viewport_height * W/H;

        // Calculate the vectors across the horizontal and down the vertical viewport edges.
        Vector viewport_u = viewport_width * right;
        Vector viewport_v = -viewport_height * Up;

        // Calculate the horizontal and vertical delta vectors from pixel to pixel.
        pixel_delta_u = viewport_u / W;
        pixel_delta_v = viewport_v / H;

        // Calculate the location of the upper left pixel.
        Point viewport_upper_left = Eye + _focus_dist*forward;
        viewport_upper_left = viewport_upper_left - (viewport_u/2 + viewport_v/2);
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
        
        // Calculate the camera defocus disk basis vectors.
        float defocus_radius = _focus_dist * tan(defocus_angle / 2);
        defocus_disk_R = right * defocus_radius;
        defocus_disk_Up = Up * defocus_radius;
    }

    bool GenerateRay(const int x, const int y, Ray *r, const float *cam_jitter=NULL);
    void getResolution (int *_W, int *_H) {*_W=W; *_H=H;}
};

#endif /* Perspective_hpp */
