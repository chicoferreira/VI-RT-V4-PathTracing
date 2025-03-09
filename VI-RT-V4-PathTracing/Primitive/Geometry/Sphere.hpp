//
//  Sphere.hpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 08/02/2025.
//

#ifndef Sphere_hpp
#define Sphere_hpp

#include "geometry.hpp"
#include "vector.hpp"
#include <math.h>

class Sphere: public Geometry {
public:
    Point C;
    float radius;
    float radiusSq;
    BB bb;      // sphere bounding box
                // this is min={0.,0.,0.} , max={0.,0.,0.} due to the Point constructor
    bool intersect (Ray r, Intersection *isect);
    
    Sphere(Point _C, float _r): C(_C), radius(_r) {
        radiusSq = radius * radius;
        bb.min.set(C.X-radius, C.Y-radius, C.Z-radius);
        bb.max.set(C.X+radius, C.Y+radius, C.Z+radius);
    }
};


#endif /* Sphere_hpp */
