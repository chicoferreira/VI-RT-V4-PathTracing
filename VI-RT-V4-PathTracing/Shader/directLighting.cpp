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

static RGB direct_AmbientLight (AmbientLight * l, BRDF  *  f);
static RGB direct_PointLight (PointLight  *  l, Scene *scene, Intersection isect, BRDF  *  f);
static RGB direct_AreaLight (AreaLight * l, Scene *scene, Intersection isect, BRDF* f, float *r);

RGB directLighting (Scene *scene, Intersection isect, BRDF *f, std::mt19937& rng, std::uniform_real_distribution<float>U_dist, DIRECT_SAMPLE_MODE mode) {
    RGB color (0.,0.,0.);
    
#define XX 725
#define YY 540
    // Loop over scene's light sources
    for (Light* l : scene->lights) {

        if (mode==UNIFORM_ONE) {
            int l_ndx = U_dist(rng)*scene->numLights;
            if (isect.pix_x==XX && isect.pix_y==YY) {
                fprintf (stderr, "numLights=%d, l_ndx=%d, ", scene->numLights, l_ndx);
            }
            if (l_ndx >= scene->numLights) l_ndx=scene->numLights-1;
            if (isect.pix_x==XX && isect.pix_y==YY) {
                fprintf (stderr, "l_ndx_corrected=%d \n", l_ndx);
            }
            l = scene->lights[l_ndx];
        }
        
        if (l->type == AMBIENT_LIGHT) {  // is it an ambient light ?
            color += direct_AmbientLight ((AmbientLight *)l, f);
            continue;
        }
        if (l->type == POINT_LIGHT) {  // is it a point light ?
            color += direct_PointLight ((PointLight *)l, scene, isect, f);
            continue;
        } // is POINT_LIGHT
        if (l->type == AREA_LIGHT) {  // is it a area light ?
            float r[2];
            RGB color_temp(0.,0.,0.);
            r[0] = U_dist(rng);
            r[1] = U_dist(rng);
            color_temp = direct_AreaLight ((AreaLight *)l, scene, isect, f, r);
            color += color_temp;
            if (isect.pix_x==XX && isect.pix_y==YY) {
                fprintf (stderr, "ARea light contributes with (%f,%f,%f) \n", color.R, color.G, color.B);
            }
        } // is AREA_LIGHT
        
        if (mode==UNIFORM_ONE) {
            color = color * scene->numLights;
            break;
        }
    }  // loop over all light sources

    if (isect.pix_x==XX && isect.pix_y==YY) {
        fprintf (stderr, "Direct contributes with (%f,%f,%f) \n", color.R, color.G, color.B);
    }

    return color;
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
            
            Ray shadow = Ray(isect.p, Ldir, SHADOW);
            shadow.pix_x = isect.pix_x;
            shadow.pix_y = isect.pix_y;
            
            shadow.adjustOrigin(isect.gn);
            
            if (scene->visibility(shadow, Ldistance-EPSILON)) {
                color += L * Kd * cosL;
                if (Ldistance>0.f) color /= (Ldistance*Ldistance);
            }
        }
    } // Kd is zero

    
    return (color);
}

static RGB direct_AreaLight (AreaLight* l, Scene *scene, Intersection isect, BRDF* f, float *r) {
    RGB color (0., 0., 0.);
    RGB Kd;
    float pdf;
    
    if (f->textured) {
        DiffuseTexture * df = (DiffuseTexture *)f;
        Kd = df->GetKd(isect.TexCoord);
    }
    else {
        Kd = f->Kd;
    }

    pdf = 0.;
    if (!Kd.isZero()) {
        Point Lpos;
        
        RGB L = l->Sample_L(r, &Lpos, pdf);
        // the pdf computed above is just 1/Area
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
                color = L * Kd * cosL;
                if (pdf >0.) color /= pdf;
                if (Ldistance>0.f) color /= (Ldistance*Ldistance);
                float cosLN_l = abs(Ldir.dot(l->gem->normal));
                color *= cosLN_l;
            }
        }
    } // Kd is zero
    
    return (color);
}
