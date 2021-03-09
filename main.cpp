//
//  main.cpp
//  Assignment 4
//
//  Created by Vicente Montoya on 3/4/21.
//

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <deque>
#include <stack>
#include "Camera.hpp"
#pragma omp parallel for collapse(2)

using namespace std;

string OUTPUT_FILE = "raytrace.png";

void rightmultiply(const mat4 & M, stack<mat4> &transfstack)
{
  mat4 &T = transfstack.top();
  T = T * M;
}

// Function to read the input data values
// Use is optional, but should be very helpful in parsing.
bool readvals(stringstream &s, const int numvals, float* values)
{
  for (int i = 0; i < numvals; i++) {
    s >> values[i];
    if (s.fail()) {
      cout << "Failed reading value " << i << " will skip\n";
      return false;
    }
  }
  return true;
}

void readfile(const char* filename, Camera& camera, Scene& scene){
  int width = 0;
  int height = 0;
  
  vector<vec3> flatTriangleVerteces;
  
  vector<Vertex> TriNormalVerteces;
  
  Material material = Material();
  
  string str, cmd;
  ifstream in;
  in.open(filename);
  
  if (in.is_open()) {
    // I need to implement a matrix stack to store transforms.
    // This is done using standard STL Templates
    stack <mat4> transfstack;
    transfstack.push(mat4(1.0));  // identity

    getline (in, str);
    while (in) {
      if ((str.find_first_not_of(" \t\r\n") != string::npos) && (str[0] != '#')) {
        // Ruled out comment and blank lines

        stringstream s(str);
        s >> cmd;
        int i;
        float values[10]; // Array to read values
        
        // Up to 10 params for cameras.
        bool validinput;// Validity of input
        
        //Size Command
        if (cmd == "size") {
          validinput = readvals(s,2,values);
          if (validinput) {
            width = (int) values[0];
            height = (int) values[1];
            
            camera.width = width;
            camera.height = height;
          }
        }
        
        else if(cmd == "output") {
          //Ignore for now
        }
        
        //Camera Command
        else if (cmd == "camera") {
          validinput = readvals(s,10,values); // 10 values eye cen up fov
          if (validinput) {

            vec3 eyeinit = vec3(values[0], values[1], values[2]);
            vec3 center = vec3(values[3], values[4], values[5]);
            vec3 upinit = vec3(values[6], values[7], values[8]);
            
            float fovy = values[9];
            camera = Camera(width, height, eyeinit, center, upinit, fovy);
          }
        }
        
        // Lighting Commands
        else if (cmd == "point") {
          validinput = readvals(s,6,values);
          if (validinput) {
            vec3 pos = vec3(values[0], values[1], values[2]);
            Color color = Color(vec3(values[3], values[4], values[5]));
            Light point = Light(pos, color, false);
            scene.addLight(point);
          }
        }
        else if (cmd == "directional") {
          validinput = readvals(s,6,values);
          if (validinput) {
            vec3 pos = vec3(values[0], values[1], values[2]);
            Color color = Color(vec3(values[3], values[4], values[5]));
            Light directional = Light(pos, color, true);
            scene.addLight(directional);
          }
        }
        
        // Material Commands
        // Ambient, diffuse, specular, shininess properties for each object.
        else if (cmd == "ambient") {
          validinput = readvals(s, 3, values); // colors
          if (validinput) {
            Color ambient = Color(vec3(values[0], values[1], values[2]));
            material.ambient = ambient;
          }
        }
        else if (cmd == "diffuse") {
          validinput = readvals(s, 3, values);
          if (validinput) {
            Color diffuse = Color(vec3(values[0], values[1], values[2]));
            material.diffuse = diffuse;
          }
        }
        else if (cmd == "specular") {
          validinput = readvals(s, 3, values);
          if (validinput) {
            Color specular = Color(vec3(values[0], values[1], values[2]));
            material.specular = specular;
          }
        }
        else if (cmd == "emission") {
          validinput = readvals(s, 3, values);
          if (validinput) {
            Color emission = Color(vec3(values[0], values[1], values[2]));
            material.emission = emission;
          }
        }
        else if (cmd == "shininess") {
          validinput = readvals(s, 1, values);
          if (validinput) {
            float shininess = values[0];
            material.shininess = shininess;
          }
        }
          
        else if (cmd == "maxverts") {
          //Ignore
        }
      
        else if (cmd == "vertex") {
          validinput = readvals(s, 3, values);
          if (validinput) {
            vec3 vertex = vec3(values[0], values[1], values[2]);
            flatTriangleVerteces.push_back(vertex);
          }
        }
        
        else if (cmd == "tri") {
          validinput = readvals(s, 3, values);
          if (validinput) {
            int index1 = (int) values[0];
            int index2 = (int) values[1];
            int index3 = (int) values[2];
            
            mat4 transform = transfstack.top();
            
            FlatTriangle* triangle = new FlatTriangle(FlatTriangle(flatTriangleVerteces[index1], flatTriangleVerteces[index2], flatTriangleVerteces[index3], material, transform));
            scene.addPrimitive(triangle);
          }
        }
        
        else if (cmd == "sphere"){
          validinput = readvals(s, 4, values);
          if (validinput) {
            vec3 pos = vec3(values[0], values[1], values[2]);
            float radius = values[3];
            
            mat4 transform = transfstack.top();
            
            Sphere* sphere = new Sphere(pos, radius, material, transform);
            scene.addPrimitive(sphere);
          }
        }

        else if (cmd == "translate") {
          validinput = readvals(s,3,values);
          if (validinput) {
            mat4 translateMatrix = Transform::translate(values[0],values[1], values[2]);
            rightmultiply(translateMatrix, transfstack);
          }
        }
        else if (cmd == "scale") {
          validinput = readvals(s,3,values);
          if (validinput) {
            mat4 scaleMatrix = Transform::scale(values[0],values[1], values[2]);
            rightmultiply(scaleMatrix, transfstack);
          }
        }
        else if (cmd == "rotate") {
          validinput = readvals(s,4,values);
          if (validinput) {
            vec3 rotatingAxis = vec3(values[0],values[1], values[2]);
            mat3 rotateMatrix3 = Transform::rotate(values[3], - rotatingAxis);
            vec4 col1 = vec4(rotateMatrix3[0], 0);
            vec4 col2 = vec4(rotateMatrix3[1], 0);
            vec4 col3 = vec4(rotateMatrix3[2], 0);
            vec4 col4 = vec4(0,0,0,1);
            mat4 rotateMatrix = mat4(col1, col2, col3, col4);
            rightmultiply(rotateMatrix, transfstack);
          }
        }

        // I include the basic push/pop code for matrix stacks
        else if (cmd == "pushTransform") {
          transfstack.push(transfstack.top());
        }
        else if (cmd == "popTransform") {
          if (transfstack.size() <= 1) {
            cerr << "Stack has no elements.  Cannot Pop\n";
          } else {
            transfstack.pop();
          }
        }

        else {
          cerr << "Unknown Command: " << cmd << " Skipping \n";
        }
      }
      getline (in, str);
    }
  
  }
  else {
    cerr << "Unable to Open Input Data File " << filename << "\n";
    throw 2;
  }
}

int main(int argc, char* argv[]){
  string filename = argv[argc - 1];
  
  Camera camera = Camera();
  
  Scene scene = Scene();
  
  readfile(filename.c_str(), camera, scene);

  camera.render(scene);
  
  camera.saveImage(OUTPUT_FILE);
}


