//
//  PathTracing.cpp
//  VI-RT-LPS
//
//  Created by Luis Paulo Santos on 14/03/2023.
//

#include "PathTracingShader.hpp"
#include "BRDF.hpp"
#include "ray.hpp"

#include "Shader_Utils.hpp"

RGB PathTracing::specularReflection (Intersection isect, BRDF *f, int depth) {
    RGB color(0.,0.,0.);

    // generate the specular ray
    // direction R = 2 (N.V) N - V
    Vector Rdir = reflect(isect.wo, isect.sn);
    Ray specular(isect.p, Rdir, SPEC_REFL);
    
    specular.pix_x = isect.pix_x;
    specular.pix_y = isect.pix_y;
    
    specular.FaceID = isect.FaceID;

    specular.adjustOrigin(isect.gn);
    specular.propagating_eta = isect.incident_eta;  // same medium

    // OK, we have the ray : trace and shade it recursively
    bool intersected;
    Intersection s_isect;
    // trace ray
    intersected = scene->trace(specular, &s_isect);

    // shade this intersection
    color = f->Ks * shade (intersected, s_isect, depth+1);

    return color;
}

RGB PathTracing::specularTransmission (Intersection isect, BRDF *f, int depth) {
    RGB color(0., 0., 0.);

    // generate the transmission ray
    // from https://raytracing.github.io/books/RayTracingInOneWeekend.html#dielectrics
    
    // direction T = IOR * V
    // IOR of the new media
    float new_eta;
    // Assumptions:
    //   IF incident_eta==1.0f then
    //      the transmitted ray is entering the refractive object
    if (isect.incident_eta==1.0f) {
        new_eta = isect.f->eta;
    }
    //   IF incident_eta<>1.0f then
    //      the transmitted ray is exiting the refractive object
    else {
        new_eta = 1.0f;
    }

    
    float const IOR = isect.incident_eta / new_eta;
    Vector const V = -1.*isect.wo;
    Vector const N = isect.sn;
    
    auto cos_theta = std::fmin(N.dot(-1.*V), 1.0);
    double sin_theta = std::sqrt(1.0 - cos_theta*cos_theta);
    
    // is there total internal reflection ?
    bool const cannot_refract = (IOR*sin_theta>1.);
    
    Vector const dir = (cannot_refract ? reflect(V,N) : refract (V, N, IOR));

    Ray refraction(isect.p, dir, (cannot_refract ? SPEC_REFL : SPEC_TRANS));
    
    refraction.pix_x = isect.pix_x;
    refraction.pix_y = isect.pix_y;
    
    refraction.FaceID = isect.FaceID;

    refraction.adjustOrigin(-1. * isect.gn);
    
    refraction.propagating_eta = (cannot_refract ? isect.incident_eta : new_eta);

    // OK, we have the ray : trace and shade it recursively
    bool intersected;
    Intersection t_isect;
    // trace ray
    intersected = scene->trace(refraction, &t_isect);

    // shade this intersection
    color = f->Kt * shade (intersected, t_isect, depth+1);
   
    return color;
}

RGB PathTracing::diffuseReflection (Intersection isect, BRDF *f, int depth) {
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

    Ray diffuse(isect.p, dir, DIFF_REFL);
        
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
            
        color = (f->Kd * cos_theta * Rcolor) / pdf ;
    }
    return color;

}

RGB PathTracing::shade(bool intersected, Intersection isect, int depth) {
    RGB color(0.,0.,0.);
    
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
    #define MIN_DEPTH 1
    #define P_CONTINUE 0.2f
    float cont=U_dist(rng);
    if (depth<MIN_DEPTH || cont < P_CONTINUE) {

        float pdf[3], sum, cdf[3];
        
        pdf[0] = f->Ks.Y();
        pdf[1] = f->Kt.Y();
        pdf[2] = f->Kd.Y();
        
        sum = pdf[0] + pdf[1] + pdf[2];
        pdf[0] /= sum;
        pdf[1] /= sum;
        pdf[2] /= sum;

        cdf[0] = pdf[0];
        cdf[1] = cdf[0] + pdf[1];
        cdf[2] = cdf[1] + pdf[2];
        
        float const rnd = U_dist(rng);
        
            // if there is a specular component sample it
        if (!f->Ks.isZero() && rnd < cdf[0]) {
            RGB c_aux;
            c_aux = specularReflection (isect, f, depth);
            c_aux /= pdf[0];
            color += c_aux;
        }
            // if there is a specular component sample it
        else if (!f->Kt.isZero() &&  rnd < cdf[1]) {
            RGB c_aux;
            c_aux = specularTransmission (isect, f, depth);
            c_aux /= pdf[1];
            color += c_aux;
        }
            // if there is a diffuse component sample it
            // do one bounce (do not recurse on indirect diffuse)
        else if (!f->Kd.isZero() && isect.r_type != DIFF_REFL) {
            RGB c_aux;
            c_aux = diffuseReflection (isect, f, depth);
            c_aux /= pdf[2];
            color += c_aux;
        }
        if (depth>=MIN_DEPTH) color /= P_CONTINUE;
    }
    if (!f->Kd.isZero()) {
        color += directLighting(scene, isect, f, rng, U_dist, UNIFORM_ONE);
        //color += directLighting(scene, isect, f, rng, U_dist, ALL_LIGHTS);
    }
    return color;
};
