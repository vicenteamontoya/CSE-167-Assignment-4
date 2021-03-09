// Transform.cpp: implementation of the Transform class.

// Note: when you construct a matrix using mat4() or mat3(), it will be COLUMN-MAJOR
// Keep this in mind in readfile.cpp and display.cpp
// See FAQ for more details or if you're having problems.

#include "Transform.h"

// Helper rotation function.  Please implement this.  
mat3 Transform::rotate(const float degrees, const vec3& axis) 
{
    vec3 normalAxis = normalize(axis);
    float cos = glm::cos(glm::radians(degrees));
    float sin = glm::sin(glm::radians(degrees));
    mat3 axisX = mat3(0, -normalAxis.z, normalAxis.y,
                      normalAxis.z, 0, -normalAxis.x,
                      -normalAxis.y, normalAxis.x, 0);
        
    return (cos * mat3(1)) + ((1-cos)* glm::outerProduct(normalAxis, normalAxis)) + (sin* axisX);
}

void Transform::left(float degrees, vec3& eye, vec3& up) 
{
    mat3 transformation = rotate(degrees, normalize(-up)) ;
        
    eye = transformation * eye;
}

void Transform::up(float degrees, vec3& eye, vec3& up) 
{
    vec3 left = cross(normalize(up), normalize(eye));
        
    mat3 transformation = rotate(degrees, left) ;
        
    eye = transformation * eye;
        
    up = normalize(glm::inverse(glm::transpose(transformation)) * up);
}

mat4 Transform::lookAt(const vec3 &eye, const vec3 &center, const vec3 &up) 
{
    vec3 c = normalize(eye - center);
    vec3 b = normalize(up);
    vec3 a = glm::cross(b,c);

    return mat4(a.x, b.x, c.x, 0,
                a.y, b.y, c.y, 0,
                a.z, b.z, c.z, 0,
                glm::dot((-a),(eye)),  glm::dot((-b),(eye)), glm::dot((-c),(eye)), 1 );
}

mat4 Transform::perspective(float fovy, float aspect, float zNear, float zFar)
{
    float angle = glm::radians(fovy)/2.0;
    float tan = glm::tan(angle);
    
    vec4 col1 = vec4((1.0/(aspect * tan)), 0, 0, 0);
    vec4 col2 = vec4(0,(1.0/(tan)), 0, 0);
    vec4 col3 = vec4(0, 0, (-1.0)*(zFar+zNear)/(zFar-zNear), -1);
    vec4 col4 = vec4(0, 0, (-2.0)*(zFar*zNear)/(zFar-zNear), 0);
    
    
    return mat4(col1, col2, col3, col4);
}

mat4 Transform::scale(const float &sx, const float &sy, const float &sz) 
{
    return mat4(sx,0,0,0,
                0,sy,0,0,
                0,0,sz,0,
                0,0,0, 1);
}

mat4 Transform::translate(const float &tx, const float &ty, const float &tz) 
{
  return mat4(1,0,0,0,
              0,1,0,0,
              0,0,1,0,
              tx, ty, tz, 1);
}

// To normalize the up direction and construct a coordinate frame.  
// As discussed in the lecture.  May be relevant to create a properly 
// orthogonal and normalized up. 
// This function is provided as a helper, in case you want to use it. 
// Using this function (in readfile.cpp or display.cpp) is optional.  

vec3 Transform::upvector(const vec3 &up, const vec3 & zvec) 
{
  vec3 x = glm::cross(up,zvec); 
  vec3 y = glm::cross(zvec,x); 
  vec3 ret = glm::normalize(y); 
  return ret; 
}


Transform::Transform()
{

}

Transform::~Transform()
{

}
