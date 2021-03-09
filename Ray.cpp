//
//  Ray.cpp
//  Assignment 4
//
//  Created by Vicente Montoya on 3/4/21.
//

#include "Ray.hpp"

Material::Material(){
  this->diffuse = Color(vec3(0,0,0));
  this->specular = Color(vec3(0,0,0));
  this->shininess = 0;
  this->emission = Color(vec3(0,0,0));
  this->ambient = Color(vec3(0,0,0));
}

Material::Material(Color diffuse, Color specular, float shininess, Color emission, Color ambient){
  this->diffuse = diffuse;
  this->specular = specular;
  this->shininess = shininess;
  this->emission = emission;
  this->ambient = ambient;
}

Color::Color(){
  this->rgb = vec3(0,0,0);
}

Color::Color(vec3 rgb){
  this->rgb = rgb;
}

Light::Light(vec3 vector, Color color, bool directional, vec3 attenuation){
  this->vector = vector;
  this->directional = directional;
  this->color = color;
  this->attenuation = attenuation;
}

Ray::Ray(vec3 pos, vec3 dir){
  this->pos = pos;
  this->dir = dir;
  this->t_min = FLT_MIN;
  this->t_max = FLT_MAX;
}

Intersection::Intersection(mat4 modelview){
  this->hitDistance = -1.0;
  this->modelview = modelview;
}

Intersection::Intersection(float hitDistance, vec3 pos, vec3 normal, vec3 dir, Material material, mat4 modelview){
  this->hitDistance = hitDistance;
  this->pos = pos;
  this->normal = normal;
  this->dir = dir;
  this->material = material;
  this->modelview = modelview;
}
