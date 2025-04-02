//
//  Ray.hpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 30/01/2023.
//

#ifndef Ray_hpp
#define Ray_hpp

#include "vector.hpp"
#include "RGB.hpp"

typedef enum {
    PRIMARY,
    SHADOW,
    SPEC_REFL,
    SPEC_TRANS,
    DIFF_REFL
} RayType;

class Ray {
public:
    RayType rtype;
    Point o; // ray origin
    Vector dir; // ray direction
    int FaceID;  // ID of the face where the origin lays in
    Vector invDir;  // ray direction reciprocal for intersections
    RGB throughput;
    int pix_x, pix_y;
    float propagating_eta;
    Ray () {}
    Ray (Point o, Vector d, RayType t, RGB _throughput): o(o),dir(d), rtype(t), throughput(_throughput) {
        //invertDir();
    }
    Ray (Point o, Vector d, RayType t): o(o),dir(d), rtype(t) {
        Ray (o, d, t, RGB(1.0, 1.0, 1.0));
    }
    ~Ray() {}

    void invertDir (void) {
        invDir.X = (dir.X!=0.f ? 1.f / dir.X : 1.e5);
        invDir.Y = (dir.Y!=0.f ? 1.f / dir.Y : 1.e5);
        invDir.Z = (dir.Z!=0.f ? 1.f / dir.Z : 1.e5);
    }

    void adjustOrigin (Vector normal) {
        Vector offset = EPSILON * normal;
        if (dir.dot(normal) < 0)
            offset = -1.f * offset;
        o.X = o.X + offset.X;
        o.Y = o.Y + offset.Y;
        o.Z = o.Z + offset.Z;
    }
};

#endif /* Ray_hpp */
