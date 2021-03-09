//
//  Camera.hpp
//  Assignment 4
//
//  Created by Vicente Montoya on 3/4/21.
//

#ifndef Camera_hpp
#define Camera_hpp

#include <stdio.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <FreeImage.h>
#include "Geometry.hpp"
#include "Ray.hpp"
#include "Transform.h"
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

class Image{
  public:
    Image();
    Image(int width, int height);
    int width, height;
    vector<Color> pixels;
};

class Scene{
  public:
    Scene();
    ~Scene();
    void addPrimitive(Primitive* p);
    void addLight(Light light);
    
    Intersection findIntersection(Ray ray, mat4 modelview);
  
    vector<Primitive*> primitives;
    vector<Light> lights;
  
};

class Camera{
  public:
    Camera();
    Camera(int width, int height, vec3 eye, vec3 center, vec3 up, float fovy);
  
    mat4 lookAt();
  
    void render(Scene &s);
    void saveImage(string filename);
    
    float getFOVX();
    Ray RayThruPixel(int i, int j);
  
  
    int width, height;
    vec3 pos, u, v, w;
    vec3 center, up;
    float fovy;
    float fovx;
    float aspect;
    Image image;
};

#endif /* Camera_hpp */
