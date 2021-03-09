//
//  Ray.hpp
//  Assignment 4
//
//  Created by Vicente Montoya on 3/4/21.
//

#ifndef Ray_hpp
#define Ray_hpp

#include <stdio.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Transform.h"

using namespace std;

class Color{
  public:
    Color();
    Color(vec3 rgb);
    vec3 rgb;
};

class Light{
  public:
    Light();
    Light(vec3 vector, Color color, bool directional, vec3 attenuation = vec3(1,0,0));
    bool directional;
    vec3 vector;
    Color color;
    vec3 attenuation;
};

class Material{
  public:
    Material();
    Material(Color diffuse, Color specular, float shininess, Color emission, Color ambient);
    Color diffuse;
    Color specular;
    float shininess;
    Color emission;
    Color ambient;
};

class Ray{
  public:
    Ray(vec3 pos, vec3 dir);
    vec3 pos;
    vec3 dir;
    float t_min, t_max;
};

class Intersection {
  public:
    Intersection(mat4 modelview);
    Intersection(float hitDistance, vec3 pos, vec3 normal, vec3 dir, Material material, mat4 modelview);
  
    float hitDistance;
    vec3 pos;
    vec3 normal;
    vec3 dir;
    Material material;
    mat4 modelview;
  
    Color shade;
};

#endif /* Ray_hpp */
