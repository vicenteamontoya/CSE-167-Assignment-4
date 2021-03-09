//
//  Geometry.hpp
//  Assignment 4
//
//  Created by Vicente Montoya on 3/4/21.
//

#ifndef Geometry_hpp
#define Geometry_hpp
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Ray.hpp"
#include <stdio.h>
#include "Transform.h"
#include <vector>

using namespace std;


class Vertex {
  public:
    Vertex(vec3 pos, vec3 normal);
    vec3 pos;
    vec3 normal;
};

class Primitive{
  public:
    Primitive();
    virtual bool intersect(Ray& ray, float& t_hit, Intersection& in){
      return false;
    }
};

class FlatTriangle : public Primitive{
  public:
    FlatTriangle();
    FlatTriangle(vec3 v1, vec3 v2, vec3 v3, Material material, mat4 transform);
    
    bool intersect(Ray& ray, float& t_hit, Intersection& in) override;
  
    vector<vec3> vertex;
    vec3 normal;
    Material material;
    mat4 transform;
};

class TriNormal : public Primitive{
  public:
    TriNormal();
    TriNormal(Vertex v1, Vertex v2, Vertex v3, Material material, mat4 transform);
    vec3 getNormal(vec3 pos);
  
    bool intersect(Ray& ray, float& t_hit, Intersection& in) override;
    
    vector<Vertex> vertex;
    Material material;
  mat4 transform;
};

class Sphere: public Primitive{
  public:
    Sphere();
    Sphere(vec3 pos, float radius, Material material, mat4 transform);

    bool intersect(Ray& ray, float& t_hit, Intersection& in) override;
  
    vec3 pos;
    float radius;
    Material material;
    mat4 transform;
};


#endif /* Geometry_hpp */
