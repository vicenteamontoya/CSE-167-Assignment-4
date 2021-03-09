//
//  Camera.cpp
//  Assignment 4
//
//  Created by Vicente Montoya on 3/4/21.
//

#include "Camera.hpp"

using namespace std;

Scene::Scene(){
}

Scene::~Scene(){
  for(Primitive* primitive : primitives){
    delete primitive;
  }
}

void Scene::addPrimitive(Primitive* p){
  primitives.push_back(p);
}
void Scene::addLight(Light light){
  lights.push_back(light);
}

Intersection Scene::findIntersection(Ray ray){
  float closestT = FLT_MAX;
  Intersection closestIn = Intersection();
  for(Primitive* p : primitives){
    float t_hit = 0;
    Intersection in = Intersection();
    if(p->intersect(ray, t_hit, in)){
      if(in.hitDistance < closestT && in.hitDistance > 0){
        closestT = in.hitDistance;
        //fprintf(stderr, "New Closest T_hit: %f \n", closestT);
        
        closestIn = in;
      }
    }
  }
  
  return closestIn;
}

Image::Image(){
  this->width = 0;
  this->height = 0;
}

Image::Image(int width, int height){
  this->width = width;
  this->height = height;
  
  int size = width * height;
  
  vector<Color> pixels;
  
  for(int i = 0; i < size; i++){
    pixels.push_back(Color(vec3(0, 0, 0)));
  }
  
  this->pixels = pixels;
}


Camera::Camera(){
  
}

Camera::Camera(int width, int height, vec3 eye, vec3 center, vec3 up, float fovy) : image(width,height){
  this->width = width;
  this->height = height;
  this->pos = eye;
  this->center = center;
  this->up = up;
  this->w =  glm::normalize(eye - center);
  this->u = glm::normalize(glm::cross(up, w));
  this->v = glm::cross(w, u);
  this->aspect = 1;
  this->image = Image(width, height);
  this->fovy = glm::radians(fovy);
  this->fovx = getFOVX();
}

float Camera::getFOVX(){
  float tangent = glm::tan(fovy / 2.0);
  float result = tangent * ( (float) width / (float) height);
  return 2 * glm::atan(result);
}

void Camera::saveImage(string filename) {
  int size = width * height;
  BYTE * pixels = new BYTE[3 * size];
  
  for(int i = 0; i < size; i++){
    Color pixel = this->image.pixels[i];
    BYTE r = ((unsigned char) pixel.rgb[0]);
    BYTE g = ((unsigned char) pixel.rgb[1]);
    BYTE b = ((unsigned char) pixel.rgb[2]);
    
    int index = 3 * i;
    pixels[index] = b;
    pixels[index + 1] = g;
    pixels[index + 2] = r;
  }
  
  FreeImage_Initialise();
  
  FIBITMAP *img = FreeImage_ConvertFromRawBits(pixels, width, height, width * 3, 24, 0xFF0000, 0x00FF00, 0x0000FF, false);

  std::cout << "Saving screenshot: " << filename << "\n";

  FreeImage_Save(FIF_PNG, img, filename.c_str(), 0);
  
  FreeImage_DeInitialise();
  
  delete[] pixels;
}

Ray Camera::RayThruPixel(int i, int j){
  float widthTerm = ((j + 0.5) - (width/2.0)) / (width/2.0);
  
  float heightTerm = ((i + 0.5) - (height/2.0)) / (height/2.0);
  
  float alpha = glm::tan(fovx / 2.0) * widthTerm;
  float beta = glm::tan(fovy / 2.0) * heightTerm;
  
  vec3 eye = pos;
  
  vec3 dir = glm::normalize( (alpha * u)  + (beta * v) - w );
  
  return Ray(eye, dir);
}

float max(float a, float b){
  return (a < b) ? b : a;
}

float min(float a, float b){
  return (a > b) ? b : a;
}

vec3 ComputeLight (vec3 direction, vec3 lightcolor, vec3 normal, vec3 halfvec, vec3 diffuse, vec3 specular, float shininess) {
  float nDotL = glm::dot(normal, direction)  ;
  vec3 lambert = diffuse * max(nDotL, 0.0) ;

  float nDotH = glm::dot(normal, halfvec) ;
  vec3 phong = specular * pow(max(nDotH, 0.0), shininess) ;
  
  vec3 color = (lambert + phong) * lightcolor;
  
  return (lambert + phong) * lightcolor ;
}

void findColor(Ray ray, Intersection& in, Scene& scene, int recursionLevel){
  //fprintf(stderr, "Recursion Level: %d \n", recursionLevel);
  
  if(recursionLevel == 0){
    return;
  }
  
  Material material = in.material;
  
  vec3 finalColor = material.ambient.rgb + material.emission.rgb;
  
  vec3 diffuse = material.diffuse.rgb;
  
  vec3 specular = material.specular.rgb;
  
  float shininess = material.shininess;
  
//  fprintf(stderr, "Ambient: (%f, %f, %f) \n", material.ambient.rgb[0], material.ambient.rgb[1], material.ambient.rgb[2]);
//  fprintf(stderr, "Emission: (%f, %f, %f) \n", material.emission.rgb[0], material.emission.rgb[1], material.emission.rgb[2]);
//  fprintf(stderr, "Diffuse: (%f, %f, %f) \n", diffuse[0], diffuse[1], diffuse[2]);
//  fprintf(stderr, "Specular: (%f, %f, %f) \n", specular[0], specular[1], specular[2]);
//  fprintf(stderr, "Shininess: %f \n", shininess);
  
  vec3 eyepos = ray.pos;
  
  vec3 eyedirn = -ray.dir;
  
  vec3 normal = in.normal;
  
//  fprintf(stderr, "Hit Position: (%f, %f, %f) \n", in.pos[0], in.pos[1], in.pos[2]);
//
//  fprintf(stderr, "Hit Normal: (%f, %f, %f) \n", normal[0], normal[1], normal[2]);

  
  for(Light light : scene.lights){
    vec3 lightcol = light.color.rgb;
    
    float epsilon = 0.05;
    
    
    if(light.directional){
      vec3 shadowRayDir = light.vector;
      
      vec3 posWithEpsilon = in.pos + (epsilon * shadowRayDir);
      
      Ray shadowRay = Ray(posWithEpsilon, shadowRayDir);
      
      Intersection hit = scene.findIntersection(shadowRay);
      
      if(hit.hitDistance < 0){
        
        vec3 direction = light.vector;
        
        vec3 halfvec = normalize(direction + eyedirn);
        
        vec3 col = ComputeLight(direction, lightcol, normal, halfvec, diffuse, specular, shininess);
        
        finalColor += col;
      }
    
    }
    else{
      vec3 shadowRayDir = glm::normalize(light.vector - in.pos);
      
      vec3 posWithEpsilon = in.pos + (epsilon * shadowRayDir);
//
//      fprintf(stderr, "Ray from intersection to light, origin: (%f, %f, %f) \n", posWithEpsilon[0], posWithEpsilon[1], posWithEpsilon[2]);
//
//      fprintf(stderr, "Ray from intersection to light, direction: (%f, %f, %f) \n", shadowRayDir[0], shadowRayDir[1], shadowRayDir[2]);
      
      Ray shadowRay = Ray(posWithEpsilon, shadowRayDir);
      
      Intersection hit = scene.findIntersection(shadowRay);
      
      float distanceToLight = glm::distance(light.vector, posWithEpsilon);
      
      //fprintf(stderr, "Distance to Light: %f \n", distanceToLight);
      
      if(hit.hitDistance > distanceToLight || hit.hitDistance < 0){
        
        float attenuation = 1.0 / ( (light.attenuation.x) + (light.attenuation.y * distanceToLight) + (light.attenuation.z * pow(distanceToLight, 2)));

        lightcol = lightcol * attenuation;

        vec3 direction = normalize(light.vector - in.pos); // no attenuation
        
        //fprintf(stderr, "Eye Direction: (%f, %f, %f) \n", eyedirn[0], eyedirn[1], eyedirn[2]);
        
        vec3 halfvec = normalize(direction + eyedirn);
        
//        fprintf(stderr, "Half Vector: (%f, %f, %f) \n", halfvec[0], halfvec[1], halfvec[2]);
//
//        fprintf(stderr, "Attenuation: %f \n", (light.attenuation.x + light.attenuation.y * distanceToLight + light.attenuation.z * pow(distanceToLight, 2)));
        
        vec3 col = ComputeLight(direction, lightcol, normal, halfvec, diffuse, specular, shininess);
        
        finalColor += col;
        
        //fprintf(stderr, "Color: (%f, %f, %f) \n", finalColor[0], finalColor[1], finalColor[2]);
      }
    }
  }
  
  
  float epsilon = 0.05;
  
  vec3 reflectedRayDir = ((2 * dot(eyedirn, normal)) * normal) - eyedirn;
  
  vec3 posWithEpsilon = in.pos + (epsilon * reflectedRayDir);
  
//  fprintf(stderr, "Reflected Ray Origin: (%f, %f, %f) \n", posWithEpsilon[0], posWithEpsilon[1], posWithEpsilon[2]);
//
//  fprintf(stderr, "Reflected Ray Direction: (%f, %f, %f) \n", reflectedRayDir[0], reflectedRayDir[1], reflectedRayDir[2]);
  
  Ray reflectedRay = Ray(posWithEpsilon, reflectedRayDir);
  
  Intersection hit = scene.findIntersection(reflectedRay);
  
//  fprintf(stderr, "Hit Position: (%f, %f, %f) \n", hit.pos[0], hit.pos[1], hit.pos[2]);
//
//  fprintf(stderr, "Hit Normal: (%f, %f, %f) \n", hit.normal[0], hit.normal[1], hit.normal[2]);
//
//  fprintf(stderr, "Hit Distance: %f \n", hit.hitDistance);
  
  if(hit.hitDistance > 0){
    
    findColor(reflectedRay, hit, scene, recursionLevel - 1);
    
    finalColor += specular * hit.shade.rgb;
  
  }
  
  in.shade = Color(finalColor);
}


void Camera::render(Scene &s, int maxdepth){
  Image image = Image(width, height);
  
  #pragma omp parallel for collapse(2)
  for (int i=0; i< height; i++){
    for (int j=0; j< width; j++){
      Ray ray = RayThruPixel(i, j);
      Intersection hit = s.findIntersection(ray);

      if(hit.hitDistance > 0){
        findColor(ray, hit, s, maxdepth);
        int r = (int) (min(hit.shade.rgb[0]*255, 255));
        int g = (int) (min(hit.shade.rgb[1]*255, 255));
        int b = (int) (min(hit.shade.rgb[2]*255, 255));
        image.pixels[(i * width) + j] = vec3(r,g,b);
      }
      else{
        image.pixels[(i * width) + j] = vec3(0, 0, 0);
      }
      fprintf(stderr, "\rCompleted %f%%", (((float) (i * width) + j ) / ((float) (width * height)) * 100));
    }
  }
  
  fprintf(stderr, "\rCompleted %f%%\n", 100.0);
  this->image = image;
}

mat4 Camera::lookAt(){
  mat4 modelview = Transform::lookAt(pos, center, up);
    
  return modelview;
}


