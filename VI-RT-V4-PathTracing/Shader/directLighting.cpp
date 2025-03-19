//
//  directLighting.cpp
//  VI-RT-3rdVersion
//
//  Created by Luis Paulo Santos on 05/03/2025.
//

#include "directLighting.hpp"
#include "AmbientLight.hpp"
#include "PointLight.hpp"
#include "AreaLight.hpp"

static RGB directLighting_ALL (Scene *scene, Intersection isect, BRDF *f, std::mt19937& rng, std::uniform_real_distribution<float>U_dist);
static RGB directLighting_ONE (Scene *scene, Intersection isect, BRDF *f, std::mt19937& rng, std::uniform_real_distribution<float>U_dist);

static RGB direct_AmbientLight (AmbientLight * l, BRDF  *  f);
static RGB direct_PointLight (PointLight  *  l, Scene *scene, Intersection isect, BRDF  *  f);
static RGB direct_AreaLight (AreaLight * l, Scene *scene, Intersection isect, BRDF* f, float &pdf, float *r);

RGB directLighting (Scene *scene, Intersection isect, BRDF *f, std::mt19937& rng, std::uniform_real_distribution<float>U_dist, DIRECT_SAMPLE_MODE mode) {
    RGB color (0.,0.,0.);
    
    switch (mode) {
        case ALL_LIGHTS:
            color = directLighting_ALL(scene, isect, f, rng, U_dist);
            break;
        case UNIFORM_ONE:
            color = directLighting_ONE(scene, isect, f, rng, U_dist);
            break;
        default:
            break;
    }
    return color;
}


RGB directLighting_ALL (Scene *scene, Intersection isect, BRDF *f, std::mt19937& rng, std::uniform_real_distribution<float>U_dist) {
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
        if (l->type == AREA_LIGHT) {  // is it a area light ?
            float pdf, r[2];
            RGB color_temp(0.,0.,0.);
            r[0] = U_dist(rng);
            r[1] = U_dist(rng);
            color_temp = direct_AreaLight ((AreaLight *)l, scene, isect, f, pdf, r);
            color += (color_temp/pdf);
        } // is AREA_LIGHT
    }  // loop over all light sources
    return color;
}

RGB directLighting_ONE (Scene *scene, Intersection isect, BRDF *f, std::mt19937& rng, std::uniform_real_distribution<float>U_dist) {
    RGB color (0.,0.,0.);
    
    
    int l_ndx = U_dist(rng)*scene->numLights;
    if (l_ndx >= scene->numLights) l_ndx=scene->numLights-1;
    Light *l = scene->lights[l_ndx];

    if (l->type == AMBIENT_LIGHT) {  // is it an ambient light ?
            color += direct_AmbientLight ((AmbientLight *)l, f);
    }
    else if (l->type == POINT_LIGHT) {  // is it a point light ?
        color += direct_PointLight ((PointLight *)l, scene, isect, f);
    } // is POINT_LIGHT
    else if (l->type == AREA_LIGHT) {  // is it a area light ?
        float pdf, r[2];
        r[0] = U_dist(rng);
        r[1] = U_dist(rng);
        color += direct_AreaLight ((AreaLight *)l, scene, isect, f, pdf, r);
        color /= pdf;
    } // is AREA_LIGHT
    return color * scene->numLights;
}

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
    RGB Kd;
    
    if (f->textured) {
        DiffuseTexture * df = (DiffuseTexture *)f;
        Kd = df->GetKd(isect.TexCoord);
    }
    else {
        Kd = f->Kd;
    }

    if (!Kd.isZero()) {
        Point Lpos;
        RGB L = l->Sample_L(NULL, &Lpos);
        Vector Ldir=isect.p.vec2point(Lpos);
        float Ldistance = Ldir.norm();
        Ldir.normalize();
        float cosL = Ldir.dot(isect.sn);
        if (cosL>0) {
            
            Ray shadow = Ray(isect.p, Ldir);
            shadow.pix_x = isect.pix_x;
            shadow.pix_y = isect.pix_y;
            
            shadow.adjustOrigin(isect.gn);
            
            if (scene->visibility(shadow, Ldistance-EPSILON)) {
                color += L * Kd * cosL;
            }
        }
    } // Kd is zero

    
    return (color);
}

static RGB direct_AreaLight (AreaLight* l, Scene *scene, Intersection isect, BRDF* f, float& pdf, float *r) {
    RGB color (0., 0., 0.);
    RGB Kd;
    
    if (f->textured) {
        DiffuseTexture * df = (DiffuseTexture *)f;
        Kd = df->GetKd(isect.TexCoord);
    }
    else {
        Kd = f->Kd;
    }

    if (!Kd.isZero()) {
        Point Lpos;
        
        RGB L = l->Sample_L(r, &Lpos, pdf);
        Vector Ldir=isect.p.vec2point(Lpos);
        float Ldistance = Ldir.norm();
        Ldir.normalize();
        float cosL = Ldir.dot(isect.sn);
        if (cosL>0) {
            
            Ray shadow = Ray(isect.p, Ldir);
            shadow.pix_x = isect.pix_x;
            shadow.pix_y = isect.pix_y;
            
            shadow.adjustOrigin(isect.gn);
            
            if (scene->visibility(shadow, Ldistance-EPSILON)) {
                color = L * Kd * cosL;
            }
        }
    } // Kd is zero
    
    return (color);
}
