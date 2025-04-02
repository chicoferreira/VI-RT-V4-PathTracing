//
//  WhittedShader.cpp
//  VI-RT-LPS
//
//  Created by Luis Paulo Santos on 14/03/2023.
//

#include "WhittedShader.hpp"
#include "BRDF.hpp"
#include "ray.hpp"
#include "AmbientLight.hpp"
#include "PointLight.hpp"

#include "Shader_Utils.hpp"

static RGB direct_AmbientLight (AmbientLight * l, BRDF * f) {
    RGB color (0., 0., 0.);
    if (!f->Ka.isZero()) {
        RGB Ka = f->Ka;
        color += Ka * l->L();
    }
    return (color);
}

static RGB direct_PointLight (PointLight* l, Scene *scene, Intersection isect, BRDF * f) {
    RGB color (0., 0., 0.);

    if (!f->Kd.isZero()) {
        Point Lpos;
        RGB L = l->Sample_L(NULL, &Lpos);
        Vector Ldir=isect.p.vec2point(Lpos);
        float Ldistance = Ldir.norm();
        Ldir.normalize();
        float cosL = Ldir.dot(isect.sn);
        if (cosL>0) {
            
            Ray shadow = Ray(isect.p, Ldir, SHADOW);
            shadow.pix_x = isect.pix_x;
            shadow.pix_y = isect.pix_y;
            
            shadow.adjustOrigin(isect.gn);
            
            if (scene->visibility(shadow, Ldistance-EPSILON)) {
                color = L * f->Kd * cosL;
                if (Ldistance>0.f) color/= (Ldistance*Ldistance);
            }
        }
    } // Kd is zero
    return (color);
}


static RGB directLighting (Scene *scene, Intersection isect, BRDF *f) {
    RGB color (0.,0.,0.);
    
    // Loop over scene's light sources
    for (auto l : scene->lights) {

        if (l->type == AMBIENT_LIGHT) {  // is it an ambient light ?
            color += direct_AmbientLight ((AmbientLight *)l, f);
            continue;
        }
        if (l->type == POINT_LIGHT) {  // is it a point light ?
            color += direct_PointLight ((PointLight *)l, scene, isect, f);
            continue;
        } // is POINT_LIGHT
    }  // loop over all light sources
    return color;
}

RGB WhittedShader::specularReflection (Intersection isect, BRDF *f, int depth) {
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

RGB WhittedShader::specularTransmission (Intersection isect, BRDF *f, int depth) {
    RGB color(0., 0., 0.);

    // generate the transmission ray
    // from https://raytracing.github.io/books/RayTracingInOneWeekend.html#dielectrics
    
    // direction T = IOR * V
    float const IOR = isect.incident_eta / isect.f->eta;
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
    
    refraction.propagating_eta = (cannot_refract ? isect.incident_eta : isect.f->eta);

    // OK, we have the ray : trace and shade it recursively
    bool intersected;
    Intersection t_isect;
    // trace ray
    intersected = scene->trace(refraction, &t_isect);

    // shade this intersection
    color = f->Kt * shade (intersected, t_isect, depth+1);
   
    return color;
}

RGB WhittedShader::shade(bool intersected, Intersection isect, int depth) {
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
    
    #define MAX_DEPTH 3
    // if there is a specular component sample it
    if (!f->Ks.isZero() && depth<MAX_DEPTH) {
        RGB scolor;
        scolor = specularReflection (isect, f, depth);
        color += scolor;
    }
    // if there is a specular component sample it
    if (!f->Kt.isZero() && depth<MAX_DEPTH) {
        RGB tcolor;
        tcolor = specularTransmission (isect, f, depth);
        color += tcolor;
    }
    
    RGB dcolor;
    dcolor = directLighting(scene, isect, f);
    color += dcolor;

    return color;
};
