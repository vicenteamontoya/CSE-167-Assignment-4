//
//  Camera.cpp
//  Assignment 4
//
//  Created by Vicente Montoya on 3/4/21.
//

#include "Camera.hpp"
#pragma omp parallel for collapse(2)

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

Intersection Scene::findIntersection(Ray ray, mat4 modelview){
  float closestT = FLT_MAX;
  Intersection closestIn = Intersection(modelview);
  for(Primitive* p : primitives){
    float t_hit = 0;
    Intersection in = Intersection(modelview);
    if(p->intersect(ray, t_hit, in)){
      if(in.hitDistance < closestT){
        closestT = t_hit;
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
  vec3 lambert = diffuse * lightcolor * max(nDotL, 0.0) ;

  float nDotH = glm::dot(normal, halfvec) ;
  vec3 phong = specular * lightcolor * pow(max(nDotH, 0.0), shininess) ;

  return lambert + phong ;
}

mat4 produceLookAt(vec3 pos, vec3 dir, vec3 normal){
  vec3 center = pos + dir;
  vec3 up = normal;
  return Transform::lookAt(pos, center, up);
}

void findColor(Intersection& in, Scene& scene, int recursionLevel){
  if(recursionLevel == 0){
    return;
  }
  
  Material material = in.material;
  vec3 finalColor = material.ambient.rgb + material.emission.rgb;
  
  vec3 diffuse = material.diffuse.rgb;
  vec3 specular = material.specular.rgb;
  float shininess = material.shininess;
  
  mat4 modelview = in.modelview;
  
  vec4 myvertex = vec4(in.pos, 1);
  vec3 mynormal = in.normal;
  
  const vec3 eyepos = vec3(0,0,0);
  vec4 vertex = modelview * myvertex;
  
  vec3 mypos = vec3(vertex.x, vertex.y, vertex.z) / vertex.w ; // Dehomogenize current location
  vec3 eyedirn = glm::normalize(eyepos - mypos) ;
  
  mat4 modelviewTI = glm::transpose(glm::inverse(modelview));
  vec4 transformedNormal = modelviewTI * (vec4(mynormal, 0));
  vec3 normal = glm::normalize(vec3(transformedNormal.x, transformedNormal.y, transformedNormal.z));
  
  for(Light light : scene.lights){
    vec3 lightcol = light.color.rgb;
    
    if(light.directional){
      float epsilon = 0.01;
      vec3 shadowRayDir = light.vector;
      vec3 posWithEpsilon = in.pos + (epsilon * shadowRayDir);
      Ray shadowRay = Ray(posWithEpsilon, shadowRayDir);
      Intersection hit = scene.findIntersection(shadowRay, mat4(1.0));
      
      if(hit.hitDistance < 0){
        vec4 directionH = modelview * vec4(light.vector, 0);
        vec3 direction = glm::normalize(vec3(directionH[0], directionH[1], directionH[2]));
        vec3 halfvec = normalize(direction + eyedirn);
        vec3 col = ComputeLight(direction, lightcol, normal, halfvec, diffuse, specular, shininess);
        finalColor += col;
      }
    
    }
    else{
      float epsilon = 0.01;
      vec3 shadowRayDir = glm::normalize(light.vector - in.pos);
      vec3 posWithEpsilon = in.pos + (epsilon * shadowRayDir);
      Ray shadowRay = Ray(posWithEpsilon, shadowRayDir);
      
      Intersection hit = scene.findIntersection(shadowRay, mat4(1.0));
      
      float distanceToLight = glm::distance(light.vector, posWithEpsilon);
      
      if(hit.hitDistance > distanceToLight || hit.hitDistance < 0){
        
        float attenuation = 1.0 / (light.attenuation.x + light.attenuation.y * distanceToLight + light.attenuation.z * pow(distanceToLight, 2));

        lightcol = lightcol * attenuation;

        vec4 positionH = modelview * vec4(light.vector, 1);
        vec3 position = vec3(positionH[0], positionH[1], positionH[2]) / positionH[3];

        vec3 direction = normalize (position - mypos); // no attenuation
        vec3 halfvec = normalize (direction + eyedirn);
        vec3 col = ComputeLight(direction, lightcol, normal, halfvec, diffuse, specular, shininess);
        finalColor += col;
      }
    }
  }
  
  float epsilon = 0.01;
  vec3 reflectedRayDir = in.dir - ((2 * dot(in.dir, in.normal)) * in.normal) ;
  vec3 posWithEpsilon = in.pos + (epsilon * reflectedRayDir);
  
  Ray reflectedRay = Ray(posWithEpsilon, reflectedRayDir);
  Intersection hit = scene.findIntersection(reflectedRay, produceLookAt(in.pos, reflectedRayDir, in.normal));
  
  if(hit.hitDistance > 0){
    findColor(hit, scene, recursionLevel - 1);
    for(int i = 0; i < scene.lights.size(); i++){
      finalColor += specular * hit.shade.rgb;
    }
  }
  
  in.shade = Color(finalColor);
}


void Camera::render(Scene &s){
  Image image = Image(width, height);
  
  for (int i=0; i< height; i++){
    for (int j=0; j< width; j++){
      Ray ray = RayThruPixel(i, j);
      Intersection hit = s.findIntersection(ray, this->lookAt());
      
      if(hit.hitDistance > 0){
        findColor(hit, s, 5);
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


