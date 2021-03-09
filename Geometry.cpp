//
//  Geometry.cpp
//  Assignment 4
//
//  Created by Vicente Montoya on 3/4/21.
//

#include "Geometry.hpp"

Primitive::Primitive(){
  
}

Vertex::Vertex(vec3 pos, vec3 normal){
  this->pos = pos;
  this->normal = normal;
}

FlatTriangle::FlatTriangle(){
  Primitive();
}

FlatTriangle::FlatTriangle(vec3 v1, vec3 v2, vec3 v3, Material material, mat4 transform){
  Primitive();
  this->vertex.push_back(v1);
  this->vertex.push_back(v2);
  this->vertex.push_back(v3);
  this->normal = glm::normalize(glm::cross(v2 - v1, v3 - v1));
  this->material = material;
  this->transform = transform;
}

bool FlatTriangle::intersect(Ray& ray, float& t_hit, Intersection& in){
  vec4 col1 = transform * vec4(vertex[0], 1);
  vec4 col2 = transform * vec4(vertex[1], 1);
  vec4 col3 = transform * vec4(vertex[2], 1);
  
  vec3 v1 = vec3(col1[0], col1[1], col1[2]);
  vec3 v2 = vec3(col2[0], col2[1], col2[2]);
  vec3 v3 = vec3(col3[0], col3[1], col3[2]);
  
  vec4 col4 = vec4(-ray.dir, 0);
  
  vec4 y = vec4(ray.pos, 1);
  
  mat4 matrix = mat4(col1, col2, col3, col4);
  
  vec4 x = glm::inverse(matrix) * y;
 
  for(int i = 0; i < 4; i++){
    if(x[i] < 0){
      return false;
    }
  }
  
  vec3 pos = (x[0] * v1) + (x[1] * v2) + (x[2] * v3);
  
  vec3 vertecesNormal = glm::normalize(glm::cross(v2 - v1, v3 - v1));
  
  vec3 normal = glm::normalize((x[0] * vertecesNormal) + (x[1] * vertecesNormal) + (x[2] * vertecesNormal));
  
  t_hit = x[3];
  
  in = Intersection(t_hit, pos, normal, ray.dir, this->material, in.modelview);
    
  return true;
}

Sphere::Sphere(){
  
}
Sphere::Sphere(vec3 pos, float radius, Material material, mat4 transform){
  this->pos = pos;
  this->radius = radius;
  this->material = material;
  this->transform = transform;
}

bool Sphere::intersect(Ray& ray, float& t_hit, Intersection& in){
  vec4 rayPosH = glm::inverse(transform) * vec4(ray.pos.x, ray.pos.y, ray.pos.z, 1);
  
  vec3 rayPos = vec3(rayPosH.x, rayPosH.y, rayPosH.z);
  
  mat3 matrix3 = mat3(transform[0][0], transform[0][1], transform[0][2], transform[1][0], transform[1][1], transform[1][2], transform[2][0], transform[2][1], transform[2][2]);
  
  vec3 rayDir = glm::inverse(matrix3) * ray.dir;
  
  float a = glm::dot(rayDir, rayDir);
  float b = 2 * glm::dot((rayPos  - this->pos), rayDir);
  float c = glm::dot((rayPos - this->pos), (rayPos - this->pos)) - (this->radius * this->radius);
  
  float t1, t2, discriminant;
  discriminant = b*b - 4*a*c;
      
  if (discriminant > 0) {
    t1 = (-b + sqrt(discriminant)) / (2*a);
    t2 = (-b - sqrt(discriminant)) / (2*a);
    
    if(t1 > 0 && t2 > 0){
      t_hit = t1 < t2 ? t1 : t2;
    }
    else{
      t_hit = t1 > 0 ? t1 : t2;
    }
    
    vec3 myPos = rayPos + (t_hit * rayDir);
    
    vec3 normal = glm::normalize(myPos - this->pos);
    
    vec4 posH = transform * vec4(myPos.x, myPos.y, myPos.z, 1);
    
    myPos = vec3(posH.x, posH.y, posH.z);
    
    vec3 myNormal = glm::normalize(glm::inverse(glm::transpose(matrix3)) * normal);
    
    float distance = glm::dot(myPos - ray.pos, ray.dir);
    
    in = Intersection(distance, myPos, myNormal, ray.dir, this->material, in.modelview);
    
    return true;
  }
  else if (discriminant == 0) {
    t_hit = -b/(2*a);
    
    vec3 myPos = rayPos + (t_hit * rayDir);
    
    vec3 normal = glm::normalize(myPos - this->pos);
    
    vec4 posH = transform * vec4(myPos.x, myPos.y, myPos.z, 1);
    
    myPos = vec3(posH.x, posH.y, posH.z);
    
    vec3 myNormal = glm::normalize(glm::inverse(glm::transpose(matrix3)) * normal);
    
    float distance = glm::dot(myPos - ray.pos, ray.dir);
    
    in = Intersection(distance, myPos, myNormal, ray.dir, this->material, in.modelview);
    
    return true;
  }
  else {
    
    return false;
  }
    
}
