//
//  PathTracing.cpp
//  VI-RT-LPS
//
//  Created by Luis Paulo Santos on 14/03/2023.
//

#include "IndirectDiffuseShader.hpp"
#include "BRDF.hpp"
#include "ray.hpp"

#include "Shader_Utils.hpp"

RGB IndirectDiffuse::diffuseReflection (Intersection isect, BRDF *f, int depth) {
    RGB color(0.,0.,0.);
    Vector dir;
    float pdf;
    
    // generate the specular ray
    
    // actual direction distributed around N
    // get 2 random number in [0,1[
    float rnd[2];
    rnd[0] = U_dist(rng);
    rnd[1] = U_dist(rng);
        
    Vector D_around_Z;
    
    // Sample the HemiSphere
    // Uniform
    //pdf = UniformHemiSphereSample (rnd, D_around_Z);
    // Cosine Sampled
    pdf = CosineHemiSphereSample (rnd, D_around_Z);

    // independently of the sampling function
    // the cosine of theta is always equal do D_around_Z.Z
    
    float const cos_theta = D_around_Z.Z;
    // generate a coordinate system from N
    Vector Rx, Ry;
    isect.gn.CoordinateSystem(&Rx, &Ry);

    // rotate sampling direction to world space
    dir = D_around_Z.Rotate  (Rx, Ry, isect.sn);

    Ray diffuse(isect.p, dir);
        
    diffuse.pix_x = isect.pix_x;
    diffuse.pix_y = isect.pix_y;
        
    diffuse.FaceID = isect.FaceID;
    
    diffuse.adjustOrigin(isect.sn);
    diffuse.propagating_eta = isect.incident_eta;  // same medium

    // OK, we have the ray : trace and shade it recursively
    bool intersected;
    Intersection d_isect;
    // trace ray
    intersected = scene->trace(diffuse, &d_isect);

    if (!d_isect.isLight) {  // if light source return 0 ; handled by direct
        // shade this intersection
        RGB Rcolor = shade (intersected, d_isect, depth+1);
            
        color = (f->Kd  * cos_theta * Rcolor) /pdf ;
    }
    return color;

}

RGB IndirectDiffuse::shade(bool intersected, Intersection isect, int depth) {
    RGB color(0.,0.,0.);
    
    /*if ((isect.pix_x==320) && (isect.pix_y==320)) {
        fprintf (stderr, "(%d,%d) \n ", isect.pix_x, isect.pix_y);
        fflush(stderr);
    }*/
    // if no intersection, return background
    if (!intersected) {
        return (background);
    }
    if (isect.isLight) { // intersection with a light source
        return isect.Le;
    }
    // get the BRDF
    BRDF *f = isect.f;
    
    
    // Russian Roullette
    /*#define MIN_DEPTH 1
    #define CONTINUE_PROB 0.2f
    //float continueRecurseProb =  U_dist(rng);
    //if (depth<MIN_DEPTH || continueRecurseProb<= CONTINUE_PROB) {
    if (depth<MIN_DEPTH) {
        // if there is a diffuse component sample it
        if (!f->Kd.isZero()) {
            color += diffuseReflection (isect, f, depth+1);
        }
        if (depth>= MIN_DEPTH) color /= CONTINUE_PROB;
    }*/
    if (!f->Kd.isZero()) {
        //color += directLighting(scene, isect, f, rng, U_dist, UNIFORM_ONE);
        color += directLighting(scene, isect, f, rng, U_dist, ALL_LIGHTS);
    }
    return color;
};
