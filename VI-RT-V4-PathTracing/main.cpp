//
//  main.cpp
//  VI-RT
//
//  Created by Luis Paulo Santos on 30/01/2023.
//

#include <iostream>
#include "scene.hpp"
#include "Perspective.hpp"
#include "DummyRenderer.hpp"
#include "StandardRenderer.hpp"
#include "ImagePPM.hpp"
#include "AmbientShader.hpp"
#include "WhittedShader.hpp"
#include "DistributedShader.hpp"
#include "IndirectDiffuseShader.hpp"
#include "AmbientLight.hpp"
#include "Sphere.hpp"
#include "BuildScenes.hpp"
#include <time.h>


int main(int argc, const char * argv[]) {
    Scene scene;
    ImagePPM *img;    // Image
    Shader *shd;      // Shader
    clock_t start, end;
    double cpu_time_used;
    
    // Image resolution
    const int W= 640;
    const int H= 640;

    img = new ImagePPM(W,H);
    
    /* Scenes*/
    
    /* Single Sphere */
    //SpheresScene(scene, 1);
    /* Single Sphere and Triangles */
    //SpheresTriScene(scene);
    // Camera parameters for the simple scenes
    //const Point Eye ={0,0,0}, At={0,0,1};
    /* Cornell Box */
    //CornellBox(scene);
    DiffuseCornellBox(scene);
    // Camera parameters for the Cornell Box
    const Point Eye ={280,265,-500}, At={280,260,0};
    const float deFocusRad = 0*3.14f/180.f;    // to radians
    const float FocusDist = 1.;
    //const float deFocusRad = 2.5*3.14f/180.f;    // to radians
    //const float FocusDist = 800.;
    //const Point Eye ={0,325,0}, At={560,345,350};

    //SingleTriScene(scene);
    /* Camera parameters for the Cornell Box*/
    /*const Point Eye ={0,0,-5}, At={0,0,0};
    const float deFocusRad = 0.*3.14f/180.f;    // to radians
    const float FocusDist = 1.;*/

    /*DeFocusTriScene (scene);
    float const Z=5.f;
    const Point Eye ={0.,1.0,Z}, At={0.1,1.0,Z+1.f};
    const float deFocusRad = 5.*3.14f/180.f;    // to radians
    const float FocusDist = 5.;*/

    const Vector Up={0,1,0};
    const float fovH = 60.f;
    const float fovHrad = fovH*3.14f/180.f;    // to radians
    //Perspective *cam = new Perspective(Eye, At, Up, W, H, fovHrad);
    Perspective *cam = new Perspective(Eye, At, Up, W, H, fovHrad, deFocusRad, FocusDist);

    /*   Dummy */
    // create the shader
    //shd = new DummyShader(&scene, W, H);
    // declare the renderer
    //DummyRenderer myRender (cam, &scene, img, shd);

    /*   Standard */
    // create the shader
    //shd = new AmbientShader(&scene, RGB(0.1,0.1,0.8));
    //shd = new WhittedShader(&scene, RGB(0.1,0.1,0.8));
    //shd = new DistributedShader(&scene, RGB(0.1,0.1,0.8));
    shd = new IndirectDiffuse(&scene, RGB(0.,0.,0.2));
    // declare the renderer
    int const spp=1;
    
    bool const jitter=true;
    StandardRenderer myRender (cam, &scene, img, shd, spp, jitter);
    // render
    start = clock();
    
    myRender.Render();
    
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;

    // save the image
    img->Save("MyImage.ppm");
    
    fprintf (stdout, "Rendering time = %.3lf secs\n\n", cpu_time_used);
    
    std::cout << "That's all, folks!" << std::endl;
    return 0;
}


