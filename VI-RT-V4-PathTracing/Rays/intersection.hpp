//
//  Intersection.hpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 30/01/2023.
//

#ifndef Intersection_hpp
#define Intersection_hpp

#include "vector.hpp"
#include "BRDF.hpp"
#include "ray.hpp"

typedef struct Intersection {
public:
    RayType r_type;
    Point p;
    Vector gn;  // geometric normal
    Vector sn;  // shading normal (the same as gn for the time being)
    Vector wo;
    float depth;
    BRDF *f;
    int pix_x, pix_y;
    int FaceID;  // ID of the intersected face 
    bool isLight;  // for intersections with light sources
    RGB Le;         // for intersections with light sources
    float incident_eta;
    Vec2 TexCoord;    
    
    Intersection() {}
    // from pbrt book, section 2.10, pag 116
    Intersection(const Point &p, const Vector &n, const Vector &wo, const float &depth)
    : p(p), gn(n), sn(n), wo(wo), depth(depth), f(NULL) { }
} Intersection;

#endif /* Intersection_hpp */
