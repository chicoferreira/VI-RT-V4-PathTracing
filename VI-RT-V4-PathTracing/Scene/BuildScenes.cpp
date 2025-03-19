//
//  BuildScenes.cpp
//  VI-RT-InitialVersion
//
//  Created by Luis Paulo Santos on 11/02/2025.
//

#include "BuildScenes.hpp"
#include "DiffuseTexture.hpp"

static int AddDiffuseMat (Scene& scene, RGB const color);
static int AddMat (Scene& scene, RGB const Ka, RGB const Kd, RGB const Ks, RGB const Kt, float const eta=1.f);
static int AddTextMat (Scene& scene, std::string filename, RGB const Ka, RGB const Kd, RGB const Ks, RGB const Kt, float const eta=1.f);
static void AddSphere (Scene& scene, Point const C, float const radius,
                            int const mat_ndx);
static void AddTriangle (Scene& scene,
                        Point const v1, Point const v2, Point const v3,
                        int const mat_ndx);


static int AddDiffuseMat (Scene& scene, RGB const color) {
    BRDF *brdf = new BRDF;
    
    brdf->Ka = color;
    brdf->Kd = color;
    brdf->Ks = RGB(0., 0., 0.);
    brdf->Kt = RGB(0., 0., 0.);
    
    return (scene.AddMaterial(brdf));
}

static int AddTextMat (Scene& scene, std::string filename, RGB const Ka, RGB const Kd, RGB const Ks, RGB const Kt, float const eta) {
    DiffuseTexture *brdf = new DiffuseTexture(filename);
    
    brdf->Ka = Ka;
    brdf->Kd = Kd;
    brdf->Ks = Ks;
    brdf->Kt = Kt;
    brdf->eta = eta;
    brdf->textured = true;
    
    return (scene.AddMaterial(brdf));
}


static int AddMat (Scene& scene, RGB const Ka, RGB const Kd, RGB const Ks, RGB const Kt, float const eta) {
    BRDF *brdf = new BRDF;
    
    brdf->Ka = Ka;
    brdf->Kd = Kd;
    brdf->Ks = Ks;
    brdf->Kt = Kt;
    brdf->eta = eta;
    
    return (scene.AddMaterial(brdf));
}

static void AddSphere (Scene& scene, Point const C,
                             float const radius, int const mat_ndx) {
    Sphere *sphere = new Sphere(C, radius);
    Primitive *prim = new Primitive;
    prim->g = sphere;
    prim->material_ndx = mat_ndx;
    scene.AddPrimitive(prim);
}

static void AddTriangle (Scene& scene,
                         Point const v1, Point const v2, Point const v3,
                         int const mat_ndx) {
    
    Triangle *tri = new Triangle(v1, v2, v3);
    Primitive *prim = new Primitive;
    prim->g = tri;
    prim->material_ndx = mat_ndx;
    scene.AddPrimitive(prim);
}

static void AddTriangleUV (Scene& scene,
                         Point const v1, Point const v2, Point const v3,
                           Vec2 const uv1, Vec2 const uv2, Vec2 const uv3,
                         int const mat_ndx) {
    
    Triangle *tri = new Triangle(v1, v2, v3);
    tri->set_uv(uv1, uv2, uv3);
    Primitive *prim = new Primitive;
    prim->g = tri;
    prim->material_ndx = mat_ndx;
    scene.AddPrimitive(prim);
}


// Scene with single triangle
void SingleTriScene (Scene& scene){
    int const mat = AddDiffuseMat(scene, RGB (0.99, 0.99, 0.99));
    AddTriangle(scene, Point(-5., 5., 0.), Point(0., -5., 0.), Point(5., 5., 0.), mat);
    // add an ambient light to the scene
    AmbientLight *ambient = new AmbientLight(RGB(0.1,0.1,0.1));
    //AmbientLight *ambient = new AmbientLight(RGB(0.1,0.1,0.1));
    scene.lights.push_back(ambient);
    scene.numLights++;
    PointLight *p1 = new PointLight(RGB(0.7,0.7,0.7),Point(0,0,-10));
    scene.lights.push_back(p1);
    scene.numLights++;
    return ;
}

// Scene with  spheres
void SpheresScene (Scene& scene, int const N_spheres){
    int const red_mat = AddDiffuseMat(scene, RGB (0.9, 0.1, 0.1));
    AddSphere(scene, Point(0., 0., 3.), 0.8, red_mat);
    // add an ambient light to the scene
    AmbientLight *ambient = new AmbientLight(RGB(0.5,0.5,0.5));
    //AmbientLight *ambient = new AmbientLight(RGB(0.1,0.1,0.1));
    scene.lights.push_back(ambient);
    scene.numLights++;
    PointLight *p1 = new PointLight(RGB(0.7,0.7,0.7),Point(0,2.0,0));
    scene.lights.push_back(p1);
    scene.numLights++;
    return ;
}

// Scene with  sphere and 4 triangles
void SpheresTriScene (Scene& scene) {
    int const red_mat = AddDiffuseMat(scene, RGB (0.9, 0.1, 0.1));
    int const green_mat = AddDiffuseMat(scene, RGB (0.1, 0.9, 0.1));
    AddSphere(scene, Point(0., 0., 3.), 0.8, red_mat);
    AddTriangle(scene, Point(0., 0., 7.), Point(-2., 1.5, 4.), Point(-0.5, 1.5, 5.),green_mat);
    AddTriangle(scene, Point(0., 0., 7.), Point(0.5, 1.5, 5.), Point(2., 1.5, 4.),green_mat);
    AddTriangle(scene, Point(0., 0., 7.), Point(-0.5, -1.5, 5.), Point(-2., -1.5, 4.),green_mat);
    AddTriangle(scene, Point(0., 0., 7.), Point(0.5, -1.5, 5.), Point(2., -1.5, 4.), green_mat);
    // add an ambient light to the scene
    AmbientLight *ambient = new AmbientLight(RGB(0.5,0.5,0.5));
    //AmbientLight *ambient = new AmbientLight(RGB(0.1,0.1,0.1));
    scene.lights.push_back(ambient);
    scene.numLights++;
    PointLight *p1 = new PointLight(RGB(0.7,0.7,0.7),Point(0,2.0,0));
    scene.lights.push_back(p1);
    scene.numLights++;
    return ;
}

// Cornell Box
void CornellBox (Scene& scene) {
    int const white_mat = AddMat(scene, RGB (0.9, 0.9, 0.9), RGB (0.4, 0.4, 0.4), RGB (0., 0., 0.), RGB (0., 0., 0.));
    int const red_mat = AddMat(scene, RGB (0.9, 0., 0.), RGB (0.4, 0., 0.), RGB (0., 0., 0.), RGB (0., 0., 0.));
    int const green_mat = AddMat(scene, RGB (0., 0.9, 0.), RGB (0., 0.2, 0.), RGB (0., 0., 0.), RGB (0., 0., 0.));
    int const blue_mat = AddMat(scene, RGB (0., 0., 0.9), RGB (0., 0., 0.4), RGB (0., 0., 0.), RGB (0., 0., 0.));
    int const orange_mat = AddMat(scene, RGB (0.99, 0.65, 0.), RGB (0.37, 0.24, 0.), RGB (0., 0., 0.), RGB (0., 0., 0.));
    int const mirror_mat = AddMat(scene, RGB (0., 0., 0.), RGB (0., 0., 0.), RGB (0.9, 0.9, 0.9), RGB (0., 0., 0.));
    int const glass_mat = AddMat(scene, RGB (0., 0., 0.), RGB (0., 0., 0.), RGB (0.2, 0.2, 0.2), RGB (0.9, 0.9, 0.9), 1.2);
    //int const glass_mat = AddPhongMat(scene, RGB (0., 0., 0.), RGB (0., 0., 0.), RGB (0.2, 0.2, 0.2), RGB (0.9, 0.9, 0.9), 1, 0.9);
    // Floor
    AddTriangle(scene, Point(552.8, 0.0, 0.0), Point(0.0, 0.0, 0.0), Point(0.0, 0.0, 559.2), white_mat);
    AddTriangle(scene, Point(549.6, 0.0, 559.2), Point(552.8, 0.0, 0.0), Point(0.0, 0.0, 559.2), white_mat);
    // Ceiling
    AddTriangle(scene, Point(556.0, 548.8, 0.0), Point(0.0, 548.8, 0.0), Point(0.0, 548.8, 559.2), white_mat);
    AddTriangle(scene, Point(556.0, 548.8, 559.2), Point(556.0, 548.8, 0.0), Point(0., 548.8, 559.2), white_mat);
    // Back wall
    AddTriangle(scene, Point(0.0, 0.0, 559.2), Point(549.6, 0.0, 559.2), Point(556.0, 548.8, 559.2), white_mat);
    AddTriangle(scene, Point(0.0, 0.0, 559.2), Point(0.0, 548.8, 559.2), Point(556.0, 548.8, 559.2), white_mat);
    // Left Wall
    AddTriangle(scene, Point(0.0, 0.0, 0.), Point(0., 0., 559.2), Point(0., 548.8, 559.2), green_mat);
    AddTriangle(scene, Point(0.0, 0.0, 0.), Point(0., 548.8, 0.), Point(0., 548.8, 559.2), green_mat);
    // Right Wall
    AddTriangle(scene, Point(552.8, 0.0, 0.), Point(549.6, 0., 559.2), Point(549.6, 548.8, 559.2), red_mat);
    AddTriangle(scene, Point(552.8, 0.0, 0.), Point(552.8, 548.8, 0.), Point(549.6, 548.8, 559.2), red_mat);
    // Right Wall Mirror
    AddTriangle(scene, Point(552, 50.0, 50.), Point(549, 50., 509.2), Point(549, 488.8, 509.2), mirror_mat);
    AddTriangle(scene, Point(552, 50.0, 50.), Point(552, 488.8, 50.), Point(549, 488.8, 509.2), mirror_mat);
    // short block
    // top
    AddTriangle(scene, Point(130.0, 165.0,  65.0), Point( 82.0, 165.0, 225.0), Point(240.0, 165.0, 272.0), orange_mat);
    AddTriangle(scene, Point(130.0, 165.0,  65.0), Point( 290.0, 165.0, 114.0), Point(240.0, 165.0, 272.0), orange_mat);
    // bottom
    AddTriangle(scene, Point(130.0, 0.01,  65.0), Point( 82.0, 0.01, 225.0), Point(240.0, 0.01, 272.0), orange_mat);
    AddTriangle(scene, Point(130.0, 0.01,  65.0), Point( 290.0, 0.01, 114.0), Point(240.0, 0.01, 272.0), orange_mat);
    // left
    AddTriangle(scene, Point(290.0,   0.0, 114.0), Point(  290.0, 165.0, 114.0), Point(240.0, 165.0, 272.0), orange_mat);
    AddTriangle(scene, Point(290.0,   0.0, 114.0), Point( 240.0,  0.0, 272.0), Point(240.0, 165.0, 272.0), orange_mat);
    // back
    AddTriangle(scene, Point(240.0, 0.0, 272.0), Point(240.0, 165.0, 272.0), Point(82.0, 165., 225.0), orange_mat);
    AddTriangle(scene, Point(240.0, 0.0, 272.0), Point(82.0, 0.0, 225.0), Point(82.0, 165.0, 225.0), orange_mat);
    // right
    AddTriangle(scene, Point(82.0, 0.0, 225.0), Point(82.0, 165.0, 225.0), Point(130.0, 165.0, 65.0), orange_mat);
    AddTriangle(scene, Point(82.0, 0.0, 225.0), Point(130.0, 0.0, 65.0), Point(130.0, 165.0, 65.0), orange_mat);
    // front
    AddTriangle(scene, Point( 130.0,   0.0,  65.0), Point(130.0, 165.0, 65.0), Point(290.0, 165.0, 114.0), orange_mat);
    AddTriangle(scene, Point( 130.0,   0.0,  65.0), Point(290.0, 0.0, 114.0), Point(290.0, 165.0, 114.0), orange_mat);

    // tall block
    // top
    AddTriangle(scene, Point(423.0, 330.0, 247.0), Point(265.0, 330.0, 296.0), Point(314.0, 330.0, 456.0), blue_mat);
    AddTriangle(scene, Point(423.0, 330.0, 247.0), Point(472.0, 330.0, 406.0), Point(314.0, 330.0, 456.0), blue_mat);
    // bottom
    AddTriangle(scene, Point(423.0, 0.1, 247.0), Point(265.0, 0.1, 296.0), Point(314.0, 0.1, 456.0), blue_mat);
    AddTriangle(scene, Point(423.0, 0.1, 247.0), Point(472.0, 0.1, 406.0), Point(314.0, 0.1, 456.0), blue_mat);
    // left
    AddTriangle(scene, Point(423.0, 0.0, 247.0), Point(423.0, 330.0, 247.0), Point(472.0, 330.0, 406.0), blue_mat);
    AddTriangle(scene, Point(423.0, 0.0, 247.0), Point(472.0, 0.0, 406.0), Point(472.0, 330.0, 406.0), blue_mat);
    // back
    AddTriangle(scene, Point(472.0, 0.0, 406.0), Point(472.0, 330.0, 406.0), Point(314.0, 330.0, 456.0), blue_mat);
    AddTriangle(scene, Point(472.0, 0.0, 406.0), Point(314.0, 0.0, 406.0), Point(314.0, 330.0, 456.0), blue_mat);
    // right
    AddTriangle(scene, Point(314.0, 0.0, 456.0), Point(314.0, 330.0, 456.0), Point(265.0, 330.0, 296.0), blue_mat);
    AddTriangle(scene, Point(314.0, 0.0, 456.0), Point(265.0, 0.0, 296.0), Point(265.0, 330.0, 296.0), blue_mat);
    // front
    AddTriangle(scene, Point(265.0, 0.0, 296.0), Point(265.0, 330.0, 296.0), Point(423.0, 330.0, 247.0), blue_mat);
    AddTriangle(scene, Point(265.0, 0.0, 296.0), Point(423.0, 0.0, 247.0), Point(423.0, 330.0, 247.0), blue_mat);
    
    // transparent sphere
    AddSphere(scene, Point(160., 320., 225.), 110., glass_mat);
  
    // add an ambient light to the scene
    //AmbientLight *ambient = new AmbientLight(RGB(0.15,0.15,0.15));
    AmbientLight *ambient = new AmbientLight(RGB(0.07,0.07,0.07));
    scene.lights.push_back(ambient);
    scene.numLights++;
#define AREA
#ifndef AREA
    for (int x=-1 ; x<2 ; x++) {
        for (int z=-1 ; z<2 ; z++) {
            PointLight *p = new PointLight(RGB(0.16,0.16,0.16),Point(278.+x*150.,545.,280.+z*150));
            scene.lights.push_back(p);
            scene.numLights++;
        }
    }
#else
    for (int lll=-1 ; lll<2 ; lll++) {
        AreaLight *a1 = new AreaLight(RGB(.3,.3,.3), Point(250.+lll*150, 545., 250.+lll*150), Point(300.+lll*150, 545., 250.+lll*150), Point(300.+lll*150, 545., 300.+lll*150), Vector (0.,-1.,0.));
            scene.lights.push_back(a1);
            scene.numLights++;
        AreaLight *a2 = new AreaLight(RGB(.3,.3,.3), Point(250.+lll*150, 545., 250.+lll*150), Point(250.+lll*150, 545., 300.+lll*150), Point(300.+lll*150, 545., 300.+lll*150), Vector (0.,-1.,0.));
            scene.lights.push_back(a2);
            scene.numLights++;
    }
#endif
    return ;
}

// Diffuse Cornell Box
void DiffuseCornellBox (Scene& scene) {
    int const text_backwall = AddTextMat(scene, "Dog.ppm", RGB (0.3, 0.3, 0.3), RGB (0.6, 0.6, 0.6), RGB (0., 0., 0.), RGB (0., 0., 0.));
    int const uminho_text = AddTextMat(scene, "UMinho.ppm", RGB (0.3, 0.3, 0.3), RGB (0.6, 0.6, 0.6), RGB (0., 0., 0.), RGB (0., 0., 0.));
    int const white_mat = AddMat(scene, RGB (0.1, 0.1, 0.1), RGB (0.6, 0.6, 0.6), RGB (0., 0., 0.), RGB (0., 0., 0.));
    int const red_mat = AddMat(scene, RGB (0.1, 0., 0.), RGB (0.6, 0., 0.), RGB (0., 0., 0.), RGB (0., 0., 0.));
    int const green_mat = AddMat(scene, RGB (0., 0.1, 0.), RGB (0., 0.6, 0.), RGB (0., 0., 0.), RGB (0., 0., 0.));
    int const blue_mat = AddMat(scene, RGB (0., 0., 0.1), RGB (0., 0., 0.6), RGB (0., 0., 0.), RGB (0., 0., 0.));
    int const orange_mat = AddMat(scene, RGB (0.37, 0.24, 0.), RGB (0.66, 0.44, 0.), RGB (0., 0., 0.), RGB (0., 0., 0.));
    // Floor
    AddTriangle(scene, Point(552.8, 0.0, 0.0), Point(0.0, 0.0, 0.0), Point(0.0, 0.0, 559.2), white_mat);
    AddTriangle(scene, Point(549.6, 0.0, 559.2), Point(552.8, 0.0, 0.0), Point(0.0, 0.0, 559.2), white_mat);
    // Ceiling
    AddTriangle(scene, Point(556.0, 548.8, 0.0), Point(0.0, 548.8, 0.0), Point(0.0, 548.8, 559.2), white_mat);
    AddTriangle(scene, Point(556.0, 548.8, 559.2), Point(556.0, 548.8, 0.0), Point(0., 548.8, 559.2), white_mat);
    // Back wall
    //AddTriangleUV (scene, Point(0.0, 0.0, 559.2), Point(549.6, 0.0, 559.2), Point(556.0, 548.8, 559.2), Vec2(1.,1.), Vec2(0.,1.), Vec2(0.,0.), text_backwall);
    //AddTriangleUV(scene, Point(0.0, 0.0, 559.2), Point(0.0, 548.8, 559.2), Point(556.0, 548.8, 559.2), Vec2(1.,1.), Vec2(1.,0.), Vec2(0.,0.), text_backwall);
    AddTriangle (scene, Point(0.0, 0.0, 559.2), Point(549.6, 0.0, 559.2), Point(556.0, 548.8, 559.2), white_mat);
    AddTriangle(scene, Point(0.0, 0.0, 559.2), Point(0.0, 548.8, 559.2), Point(556.0, 548.8, 559.2), white_mat);
    // Left Wall
    AddTriangle(scene, Point(0.0, 0.0, 0.), Point(0., 0., 559.2), Point(0., 548.8, 559.2), green_mat);
    AddTriangle(scene, Point(0.0, 0.0, 0.), Point(0., 548.8, 0.), Point(0., 548.8, 559.2), green_mat);
    // Right Wall
    AddTriangle(scene, Point(552.8, 0.0, 0.), Point(549.6, 0., 559.2), Point(549.6, 548.8, 559.2), red_mat);
    AddTriangle(scene, Point(552.8, 0.0, 0.), Point(552.8, 548.8, 0.), Point(549.6, 548.8, 559.2), red_mat);

    // short block
    // top
    //AddTriangleUV(scene, Point(130.0, 165.0,  65.0), Point( 82.0, 165.0, 225.0), Point(240.0, 165.0, 272.0), Vec2(0.,0.), Vec2(0.,1.), Vec2(1.,1.), uminho_text);
    //AddTriangleUV(scene, Point(130.0, 165.0,  65.0), Point( 290.0, 165.0, 114.0), Point(240.0, 165.0, 272.0), Vec2(0.,0.), Vec2(1.,0.), Vec2(1.,1.), uminho_text);
    AddTriangle(scene, Point(130.0, 165.0,  65.0), Point( 82.0, 165.0, 225.0), Point(240.0, 165.0, 272.0), orange_mat);
    AddTriangle(scene, Point(130.0, 165.0,  65.0), Point( 290.0, 165.0, 114.0), Point(240.0, 165.0, 272.0), orange_mat);
    // bottom
    AddTriangle(scene, Point(130.0, 0.01,  65.0), Point( 82.0, 0.01, 225.0), Point(240.0, 0.01, 272.0), orange_mat);
    AddTriangle(scene, Point(130.0, 0.01,  65.0), Point( 290.0, 0.01, 114.0), Point(240.0, 0.01, 272.0), orange_mat);
    // left
    AddTriangle(scene, Point(290.0,   0.0, 114.0), Point(  290.0, 165.0, 114.0), Point(240.0, 165.0, 272.0), orange_mat);
    AddTriangle(scene, Point(290.0,   0.0, 114.0), Point( 240.0,  0.0, 272.0), Point(240.0, 165.0, 272.0), orange_mat);
    // back
    AddTriangle(scene, Point(240.0, 0.0, 272.0), Point(240.0, 165.0, 272.0), Point(82.0, 165., 225.0), orange_mat);
    AddTriangle(scene, Point(240.0, 0.0, 272.0), Point(82.0, 0.0, 225.0), Point(82.0, 165.0, 225.0), orange_mat);
    // right
    AddTriangle(scene, Point(82.0, 0.0, 225.0), Point(82.0, 165.0, 225.0), Point(130.0, 165.0, 65.0), orange_mat);
    AddTriangle(scene, Point(82.0, 0.0, 225.0), Point(130.0, 0.0, 65.0), Point(130.0, 165.0, 65.0), orange_mat);
    // front
    AddTriangle(scene, Point( 130.0,   0.0,  65.0), Point(130.0, 165.0, 65.0), Point(290.0, 165.0, 114.0), orange_mat);
    AddTriangle(scene, Point( 130.0,   0.0,  65.0), Point(290.0, 0.0, 114.0), Point(290.0, 165.0, 114.0), orange_mat);

    // tall block
    // top
    AddTriangle(scene, Point(423.0, 330.0, 247.0), Point(265.0, 330.0, 296.0), Point(314.0, 330.0, 456.0), blue_mat);
    AddTriangle(scene, Point(423.0, 330.0, 247.0), Point(472.0, 330.0, 406.0), Point(314.0, 330.0, 456.0), blue_mat);
    // bottom
    AddTriangle(scene, Point(423.0, 0.1, 247.0), Point(265.0, 0.1, 296.0), Point(314.0, 0.1, 456.0), blue_mat);
    AddTriangle(scene, Point(423.0, 0.1, 247.0), Point(472.0, 0.1, 406.0), Point(314.0, 0.1, 456.0), blue_mat);
    // left
    AddTriangle(scene, Point(423.0, 0.0, 247.0), Point(423.0, 330.0, 247.0), Point(472.0, 330.0, 406.0), blue_mat);
    AddTriangle(scene, Point(423.0, 0.0, 247.0), Point(472.0, 0.0, 406.0), Point(472.0, 330.0, 406.0), blue_mat);
    // back
    AddTriangle(scene, Point(472.0, 0.0, 406.0), Point(472.0, 330.0, 406.0), Point(314.0, 330.0, 456.0), blue_mat);
    AddTriangle(scene, Point(472.0, 0.0, 406.0), Point(314.0, 0.0, 406.0), Point(314.0, 330.0, 456.0), blue_mat);
    // right
    AddTriangle(scene, Point(314.0, 0.0, 456.0), Point(314.0, 330.0, 456.0), Point(265.0, 330.0, 296.0), blue_mat);
    AddTriangle(scene, Point(314.0, 0.0, 456.0), Point(265.0, 0.0, 296.0), Point(265.0, 330.0, 296.0), blue_mat);
    // front
    AddTriangle(scene, Point(265.0, 0.0, 296.0), Point(265.0, 330.0, 296.0), Point(423.0, 330.0, 247.0), blue_mat);
    AddTriangle(scene, Point(265.0, 0.0, 296.0), Point(423.0, 0.0, 247.0), Point(423.0, 330.0, 247.0), blue_mat);
    
  
    // add an ambient light to the scene
    //AmbientLight *ambient = new AmbientLight(RGB(0.15,0.15,0.15));
    AmbientLight *ambient = new AmbientLight(RGB(0.07,0.07,0.07));
    scene.lights.push_back(ambient);
    scene.numLights++;
#define AREA
#ifndef AREA
    for (int x=-1 ; x<2 ; x++) {
        for (int z=-1 ; z<2 ; z++) {
            PointLight *p = new PointLight(RGB(0.16,0.16,0.16),Point(278.+x*150.,545.,280.+z*150));
            scene.lights.push_back(p);
            scene.numLights++;
        }
    }
#else
    for (int lll=-1 ; lll<2 ; lll++) {
        AreaLight *a1 = new AreaLight(RGB(.2,.2,.2), Point(250.+lll*150, 545., 250.+lll*150), Point(300.+lll*150, 545., 250.+lll*150), Point(300.+lll*150, 545., 300.+lll*150), Vector (0.,-1.,0.));
            scene.lights.push_back(a1);
            scene.numLights++;
        AreaLight *a2 = new AreaLight(RGB(.2,.2,.2), Point(250.+lll*150, 545., 250.+lll*150), Point(250.+lll*150, 545., 300.+lll*150), Point(300.+lll*150, 545., 300.+lll*150), Vector (0.,-1.,0.));
            scene.lights.push_back(a2);
            scene.numLights++;
    }
#endif
    return ;
}


// Scene for testing defocus blur 5 triangles
void DeFocusTriScene (Scene& scene) {
    int const red_mat = AddDiffuseMat(scene, RGB (0.9, 0.1, 0.1));
    int const green_mat = AddDiffuseMat(scene, RGB (0.1, 0.9, 0.1));
    int const brown_mat = AddDiffuseMat(scene, RGB (210./256.,105./256.,30./256.));
    // floor
    AddTriangle(scene, Point(-20., -0.1, -20.), Point(-20., -0.1, 20.), Point(20., -0.1, 20.),brown_mat);
    AddTriangle(scene, Point(-20., -0.1, -20.), Point(20., -0.1, -20.), Point(20., -0.1, 20.),brown_mat);

    float const Xbase=0.;
    float const Zbase=10.;
    // central triangle
    AddTriangle(scene, Point(Xbase-0.5, 1., Zbase), Point(Xbase+0.5, 1., Zbase), Point(Xbase, 0.1, Zbase),green_mat);
    //AddTriangle(scene, Point(-0.5, 1., 10.), Point(0.5, 1., 10.), Point(0., 0.1, 10.),green_mat);
    AddTriangle(scene, Point(Xbase+0.5, 1., Zbase+1.), Point(Xbase+1.5, 1., Zbase+1.), Point(Xbase+1., 0.1, Zbase+1.),red_mat);
    AddTriangle(scene, Point(Xbase+1.5, 1., Zbase+2.), Point(Xbase+2.5, 1., Zbase+2.), Point(Xbase+2., 0.1, Zbase+2.),green_mat);
    AddTriangle(scene, Point(Xbase-1.0, 1., Zbase-1.), Point(Xbase, 1., Zbase-1.), Point(Xbase-0.5, 0.1, Zbase-1.),red_mat);
    AddTriangle(scene, Point(Xbase-1.5, 1., Zbase-2.), Point(Xbase-0.5, 1., Zbase-2.), Point(Xbase-1., 0.1, Zbase-2.),green_mat);

    // add an ambient light to the scene
    AmbientLight *ambient = new AmbientLight(RGB(0.5,0.5,0.5));
    //AmbientLight *ambient = new AmbientLight(RGB(0.1,0.1,0.1));
    scene.lights.push_back(ambient);
    scene.numLights++;
    return ;
}
