//
//  directLighting.cpp
//  VI-RT-3rdVersion
//
//  Created by Luis Paulo Santos on 05/03/2025.
//

#include "directLighting.hpp"

#include "AmbientLight.hpp"
#include "AreaLight.hpp"
#include "PointLight.hpp"
#include "Shader_Utils.hpp"

static RGB direct_AmbientLight(AmbientLight *l, BRDF *f);
static RGB direct_PointLight(PointLight *l, Scene *scene, Intersection isect, BRDF *f);
static RGB direct_AreaLight(AreaLight *l, Scene *scene, Intersection isect, BRDF *f, float *r);

static RGB sample_light(Scene *scene, Light *light, Intersection isect, BRDF *f, std::mt19937 &rng, std::uniform_real_distribution<float> U_dist) {
    switch (light->type) {
        case AMBIENT_LIGHT: {
            return direct_AmbientLight((AmbientLight *)light, f);
        }
        case POINT_LIGHT: {
            return direct_PointLight((PointLight *)light, scene, isect, f);
        }
        case AREA_LIGHT: {
            float r[2];
            r[0] = U_dist(rng);
            r[1] = U_dist(rng);
            return direct_AreaLight((AreaLight *)light, scene, isect, f, r);
        }
        case NO_LIGHT: {
            return RGB(0., 0., 0.);
        }
    }
}

static float estimateContribution(Scene *scene, Intersection &isect, Light *light, BRDF *f, std::mt19937 &rng, std::uniform_real_distribution<float> U_dist) {
    constexpr float EPS = 1e-6f;

    switch (light->type) {
        case AMBIENT_LIGHT: {
            return 0.f;
        }
        case POINT_LIGHT: {
            auto *pl = static_cast<PointLight *>(light);
            Vector L = isect.p.vec2point(pl->pos);
            float dist2 = L.normSQ();
            if (dist2 < EPS) return 0.f;
            L.normalize();
            float cosS = std::max(0.f, L.dot(isect.sn));
            if (cosS <= 0.f) return 0.f;
            float lum = pl->color.Y();
            return lum * cosS / dist2;
        }
        case AREA_LIGHT: {
            auto *al = static_cast<AreaLight *>(light);
            Point C = (al->gem->v1 + al->gem->v2 + al->gem->v3) * (1.f / 3.f);

            Vector L = isect.p.vec2point(C);
            float dist2 = L.normSQ();
            if (dist2 < EPS) return 0.f;

            L.normalize();
            float cosS = std::max(0.f, L.dot(isect.sn));
            float cosL = std::max(0.f, -L.dot(al->gem->normal));
            if (cosS <= 0.f || cosL <= 0.f) return 0.f;

            float lum = al->intensity.Y();
            float area = al->gem->area();

            return lum * cosS * cosL * area / dist2;
        }
        case NO_LIGHT: {
            return 0.f;
        }
    }
}

static float estimateContributionNoDistance(Scene *scene, Intersection &isect, Light *light, BRDF *f, std::mt19937 &rng, std::uniform_real_distribution<float> U_dist) {
    constexpr float EPS = 1e-6f;

    switch (light->type) {
        case AMBIENT_LIGHT: {
            return 0.f;
        }
        case POINT_LIGHT: {
            auto *pl = static_cast<PointLight *>(light);
            Vector L = isect.p.vec2point(pl->pos);
            L.normalize();
            float cosS = std::max(0.f, L.dot(isect.sn));
            if (cosS <= 0.f) return 0.f;
            float lum = pl->color.Y();
            return lum * cosS;
        }
        case AREA_LIGHT: {
            auto *al = static_cast<AreaLight *>(light);
            Point C = (al->gem->v1 + al->gem->v2 + al->gem->v3) * (1.f / 3.f);

            Vector L = isect.p.vec2point(C);

            L.normalize();
            float cosS = std::max(0.f, L.dot(isect.sn));
            float cosL = std::max(0.f, -L.dot(al->gem->normal));
            if (cosS <= 0.f || cosL <= 0.f) return 0.f;

            float lum = al->intensity.Y();
            float area = al->gem->area();

            return lum * cosS * cosL * area;
        }
        case NO_LIGHT: {
            return 0.f;
        }
    }
}

static float estimateDistance(Scene *scene, Intersection &isect, Light *light, BRDF *f, std::mt19937 &rng, std::uniform_real_distribution<float> U_dist) {
    switch (light->type) {
        case AMBIENT_LIGHT: {
            return 0.f;
        }
        case POINT_LIGHT: {
            auto *pl = static_cast<PointLight *>(light);
            Vector L = isect.p.vec2point(pl->pos);
            return 1.0f / L.norm();
        }
        case AREA_LIGHT: {
            auto *al = static_cast<AreaLight *>(light);
            Point C = (al->gem->v1 + al->gem->v2 + al->gem->v3) * (1.f / 3.f);
            Vector L = isect.p.vec2point(C);
            return 1.0f / L.norm();
        }
        case NO_LIGHT: {
            return 0.f;
        }
    }
}

template <typename WeightFunc>
static RGB sampleLightDiscrete(Scene *scene, WeightFunc weight_func, Intersection &isect, BRDF *f, std::mt19937 &rng, std::uniform_real_distribution<float> U_dist) {
    RGB color(0., 0., 0.);

    // Compute the contribution of each light source
    float *contributions = new float[scene->numLights];
    float total_contribution = 0.f;
    for (int i = 0; i < scene->numLights; ++i) {
        contributions[i] = weight_func(scene, isect, scene->lights[i], f, rng, U_dist);
        total_contribution += contributions[i];
    }

    if (total_contribution <= 0.f) {
        delete[] contributions;
        return color;  // No contribution from any light source
    }

    // Build the CDF
    float *cdf = new float[scene->numLights];
    for (int i = 0; i < scene->numLights; ++i) {
        float last_cdf = (i == 0) ? 0.f : cdf[i - 1];
        cdf[i] = last_cdf + contributions[i] / total_contribution;
    }

    cdf[scene->numLights - 1] = 1.f;  // Ensure the last CDF value is 1 (to avoid rounding errors)

    // Sample a random number and find the corresponding light source
    float rnd = U_dist(rng);
    int chosen = 0;
    for (int i = 0; i < scene->numLights; ++i) {
        if (rnd < cdf[i]) {
            chosen = i;
            break;
        }
    }

    Light *l = scene->lights[chosen];
    float contribution = contributions[chosen] / total_contribution;
    color = sample_light(scene, l, isect, f, rng, U_dist) / contribution;

    delete[] contributions;
    delete[] cdf;

    return color;
}

RGB directLighting(Scene *scene, Intersection isect, BRDF *f, std::mt19937 &rng, std::uniform_real_distribution<float> U_dist, DIRECT_SAMPLE_MODE mode) {
    RGB color(0., 0., 0.);

    if (scene->numLights == 0) return color;

    switch (mode) {
        case ALL_LIGHTS: {
            for (Light *light : scene->lights) {
                color += sample_light(scene, light, isect, f, rng, U_dist);
            }
            break;
        }
        case UNIFORM_ONE: {
            int l_ndx = U_dist(rng) * scene->numLights;
            if (l_ndx >= scene->numLights) l_ndx = scene->numLights - 1;
            Light *l = scene->lights[l_ndx];

            color = sample_light(scene, l, isect, f, rng, U_dist);
            color = color * scene->numLights;
            break;
        }
        case IMPORTANCE_ONE: {
            color = sampleLightDiscrete(scene, estimateContribution, isect, f, rng, U_dist);
            break;
        }
        case IMPORTANCE_ONE_NO_DISTANCE: {
            color = sampleLightDiscrete(scene, estimateContributionNoDistance, isect, f, rng, U_dist);
            break;
        }
        case DISTANCE_ONE: {
            color = sampleLightDiscrete(scene, estimateDistance, isect, f, rng, U_dist);
            break;
        }
        case DISTANCE_SQUARED_ONE: {
            auto sampler = [](Scene *scene, Intersection &isect, Light *light, BRDF *f, std::mt19937 &rng, std::uniform_real_distribution<float> U_dist) {
                auto dist = estimateDistance(scene, isect, light, f, rng, U_dist);
                return dist * dist;
            };
            color = sampleLightDiscrete(scene, sampler, isect, f, rng, U_dist);
            break;
        }
    }

    return color;
}

static RGB direct_AmbientLight(AmbientLight *l, BRDF *f) {
    RGB color(0., 0., 0.);
    if (!f->Ka.isZero()) {
        RGB Ka = f->Ka;
        color += Ka * l->L();
    }
    return (color);
}

static RGB direct_PointLight(PointLight *l, Scene *scene, Intersection isect, BRDF *f) {
    RGB color(0., 0., 0.);
    RGB Kd;

    if (f->textured) {
        DiffuseTexture *df = (DiffuseTexture *)f;
        Kd = df->GetKd(isect.TexCoord);
    } else {
        Kd = f->Kd;
    }

    if (!Kd.isZero()) {
        Point Lpos;
        RGB L = l->Sample_L(NULL, &Lpos);
        Vector Ldir = isect.p.vec2point(Lpos);
        float Ldistance = Ldir.norm();
        Ldir.normalize();
        float cosL = Ldir.dot(isect.sn);
        if (cosL > 0) {
            Ray shadow = Ray(isect.p, Ldir, SHADOW);
            shadow.pix_x = isect.pix_x;
            shadow.pix_y = isect.pix_y;

            shadow.adjustOrigin(isect.gn);

            if (scene->visibility(shadow, Ldistance - EPSILON)) {
                color += L * Kd * cosL;
                if (Ldistance > 0.f) color /= (Ldistance * Ldistance);
            }
        }
    }  // Kd is zero

    return (color);
}

static RGB direct_AreaLight(AreaLight *l, Scene *scene, Intersection isect, BRDF *f, float *r) {
    RGB color(0., 0., 0.);
    RGB Kd;
    float pdf, cosL, cosLN_l, Ldistance;
    RGB L;
    Point Lpos;

    if (f->textured) {
        DiffuseTexture *df = (DiffuseTexture *)f;
        Kd = df->GetKd(isect.TexCoord);
    } else {
        Kd = f->Kd;
    }

    pdf = 0.;
    if (!Kd.isZero()) {
        L = l->Sample_L(r, &Lpos, pdf);
        // the pdf computed above is just 1/Area
        Vector Ldir = isect.p.vec2point(Lpos);
        Ldistance = Ldir.norm();
        Ldir.normalize();
        cosL = Ldir.dot(isect.sn);
        // Ldir points into the light: * -1 to get the correct sign
        cosLN_l = -1.f * Ldir.dot(l->gem->normal);
        // The light source will only contribute if the above cosine is positive
        if (cosL > 1.e-4 && cosLN_l > 1.e-4) {
            Ray shadow = Ray(isect.p, Ldir, SHADOW);
            shadow.pix_x = isect.pix_x;
            shadow.pix_y = isect.pix_y;

            shadow.adjustOrigin(isect.gn);

            if (scene->visibility(shadow, Ldistance - EPSILON)) {
                color = L * Kd * cosL;
                if (pdf > 0.) color /= pdf;
                if (Ldistance > 0.f) color /= (Ldistance * Ldistance);
                color *= cosLN_l;
            }
        }
    }  // Kd is zero

    return (color);
}
