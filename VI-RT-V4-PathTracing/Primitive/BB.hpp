//
//  AABB.hpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 30/01/2023.
//

#ifndef BB_hpp
#define BB_hpp

#include "vector.hpp"
#include "ray.hpp"
#include <limits>

// pbrt 3rd edition, pag 214 (pbrt.org)
static constexpr float MachineEpsilon =
    std::numeric_limits<float>::epsilon() * 0.5;

// pbrt 3rd edition, pag 217 (pbrt.org)
inline constexpr float gamma(int n) {
    return (n * MachineEpsilon) / (1 - n * MachineEpsilon);
}
    
typedef struct BB {
public:
    Point min, max;
    void update (Point p) {
        if (p.X < min.X) min.X = p.X;
        else if (p.X > max.X) max.X = p.X;
        if (p.Y < min.Y) min.Y = p.Y;
        else if (p.Y > max.Y) max.Y = p.Y;
        if (p.Z < min.Z) min.Z = p.Z;
        else if (p.Z > max.Z) max.Z = p.Z;
    }
    /*
     * I suggest you implement:
     *  bool intersect (Ray r) { }
     *
     * based on PBRT's 3rd ed. book , sec 3.1.2, pags 125..128 + 214,217,221
     *
     * or https://doi.org/10.1007/978-1-4842-7185-8_2
     *
     */
    // based on PBRT's 3rd ed. book , sec 3.1.2, pag 125.. 12 (pbrt.org)
#define BB_TEST
#ifdef BB_TEST
    bool intersect (Ray r) {
        float t0 = 0.f, t1 = MAXFLOAT;
        float invRayDir, tNear, tFar;
        // XX slabs
        invRayDir = (r.dir.X!=0.f ? 1.f / r.dir.X : 1.e5);
        //invRayDir = r.invDir.X;
        tNear = (min.X - r.o.X) * invRayDir;
        tFar = (max.X - r.o.X) * invRayDir;
        if (tNear > tFar) {
            float aux = tNear;
            tNear = tFar;
            tFar = aux;
        }
        // pbrt 3rd edition, pag 221 (pbrt.org)
        tFar *= 1 + 2 * gamma(3);
        t0 = tNear > t0 ? tNear : t0;
        t1 = tFar < t1 ? tFar : t1;
        if (t0 > t1) return false;
        // YY slabs
        //invRayDir = 1 / r.dir.Y;
        invRayDir = (r.dir.Y!=0.f ? 1.f / r.dir.Y : 1.e5);
        //invRayDir = r.invDir.Y;
        tNear = (min.Y - r.o.Y) * invRayDir;
        tFar = (max.Y - r.o.Y) * invRayDir;
        if (tNear > tFar) {
            float aux = tNear;
            tNear = tFar;
            tFar = aux;
        }
        // pbrt 3rd edition, pag 221 (pbrt.org)
        tFar *= 1 + 2 * gamma(3);
        t0 = tNear > t0 ? tNear : t0;
        t1 = tFar < t1 ? tFar : t1;
        if (t0 > t1) return false;
        // ZZ slabs
        //invRayDir = 1 / r.dir.Z;
        invRayDir = (r.dir.Z!=0.f ? 1.f / r.dir.Z : 1.e5);
        //invRayDir = r.invDir.Z;
        tNear = (min.Z - r.o.Z) * invRayDir;
        tFar = (max.Z - r.o.Z) * invRayDir;
        if (tNear > tFar) {
            float aux = tNear;
            tNear = tFar;
            tFar = aux;
        }
        // pbrt 3rd edition, pag 221 (pbrt.org)
        tFar *= 1 + 2 * gamma(3);
        t0 = tNear > t0 ? tNear : t0;
        t1 = tFar < t1 ? tFar : t1;
        if (t0 > t1) return false;

        return true;
    }
#else
    bool intersect (Ray r) {
        return true;
    }
#endif
} BB;

#endif /* AABB_hpp */
