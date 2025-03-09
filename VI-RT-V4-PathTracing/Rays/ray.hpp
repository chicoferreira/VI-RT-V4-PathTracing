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

const float EPSILON=1e-3;

class Ray {
public:
    Point o; // ray origin
    Vector dir; // ray direction
    int FaceID;  // ID of the face where the origin lays in
    Vector invDir;  // ray direction reciprocal for intersections
    RGB throughput;
    int pix_x, pix_y;
    float propagating_eta;
    Ray () {}
    Ray (Point o, Vector d, RGB _throughput): o(o),dir(d), throughput(_throughput) {}
    Ray (Point o, Vector d): o(o),dir(d) {
        Ray (o, d, RGB(1.0, 1.0, 1.0));
    }
    ~Ray() {}

    void invertDir (void) {
        invDir.X = 1.f / dir.X;
        invDir.Y = 1.f / dir.Y;
        invDir.Z = 1.f / dir.Z;
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
