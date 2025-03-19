//
//  Shader_Utils.hpp
//  VI-RT-V4-PathTracing
//
//  Created by Luis Paulo Santos on 12/03/2025.
//

#ifndef _ShaderUtils_hpp_
#define _ShaderUtils_hpp_


inline Vector refract(const Vector& V, const Vector& N, double IOR) {
    auto cos_theta = std::fmin(N.dot(-1.*V), 1.0);

    // is there Total Internal Reflection ?
    
    Vector const r_out_perp =  IOR * (V + cos_theta*N);
    Vector const r_out_parallel = -std::sqrt(std::fabs(1.0 - r_out_perp.normSQ())) * N;
    Vector T = r_out_perp + r_out_parallel;
    T.normalize();
    return T;
}

inline Vector reflect(const Vector& V, const Vector& N) {
    float cos = N.dot(V);
    return 2.f * cos * N - V;
}

static float UniformHemiSphereSample (float *rnd, Vector &D) {
    const float cos_theta = D.Z = rnd[1];  // uniform sampling
    const float aux_r1 = powf(rnd[1], 2.);
    D.Y = sinf(2.*M_PI*rnd[0])*sqrtf(1.-aux_r1);
    D.X = cosf(2.*M_PI*rnd[0])*sqrtf(1.-aux_r1);
    float const pdf = 1. / (2. * M_PI );
    return pdf;
}

static float CosineHemiSphereSample (float *rnd, Vector &D) {
    const float cos_theta= D.Z = sqrtf(rnd[1]);  // cosine sampling
    D.Y = sinf(2.*M_PI*rnd[0])*sqrtf(1.-rnd[1]);
    D.X = cosf(2.*M_PI*rnd[0])*sqrtf(1.-rnd[1]);
    float const pdf = cos_theta / ( M_PI );
    return pdf;
}

#endif // _ShaderUtils_hpp_
