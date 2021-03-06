/*
	This file is part of ReWeb3D.

    ReWeb3D is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License.

    ReWeb3D is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with ReWeb3D. If not, see <http://www.gnu.org/licenses/>.

    Copyright (C) 2011 Vicomtech Foundation, 
	Donostia - San Sebastian, Spain.
    All Rights Reserved.

    Written by Mauricio Aristizabal <maristi7@eafit.edu.co>
*/

#include "DrawGLWidget.h"

#include <Wt/WGLWidget>
#include <Wt/WMatrix4x4>

using namespace Wt;

DrawGLWidget::DrawGLWidget(WContainerWidget *root):
  WGLWidget(root)
{
}


// The initializeGL() captures all JS commands that are to be executed
// before the scene is rendered for the first time. It is executed only
// once. It is re-executed when the WebGL context is restored after it
// was lost.
// In general, it should be used to set up shaders, create VBOs, initialize
// matrices, ...

void DrawGLWidget::initializeGL()
{
    initShaders();
    initBuffers();
    //paintGL();
    angle=0.0;
    viewport(0, 0, 500, 500);
    clearColor(0, 0, 0, 1.0);
    enable(DEPTH_TEST);

}

void DrawGLWidget::animate(){



}
void DrawGLWidget::initShaders()
{

    Shader fragmentShader = createShader(FRAGMENT_SHADER);
    
    shaderSource(fragmentShader, fragmentShader_);
    compileShader(fragmentShader);
    Shader vertexShader = createShader(VERTEX_SHADER);
    shaderSource(vertexShader, vertexShader_);
    compileShader(vertexShader);
    shaderProgram_ = createProgram();
    attachShader(shaderProgram_, vertexShader);
    attachShader(shaderProgram_, fragmentShader);
    linkProgram(shaderProgram_);
    useProgram(shaderProgram_);

    // Extract the references to the attributes from the shader.
    vertexPositionAttribute_ =
      getAttribLocation(shaderProgram_, "aVertexPosition");
    enableVertexAttribArray(vertexPositionAttribute_);
    
    vertexColorAttribute_ =
      getAttribLocation(shaderProgram_, "aVertexColor");
    enableVertexAttribArray(vertexColorAttribute_);

    // Extract the references the uniforms from the shader
    pMatrixUniform_  = getUniformLocation(shaderProgram_, "uPMatrix");
    mvMatrixUniform_ = getUniformLocation(shaderProgram_, "uMVMatrix");
}


void DrawGLWidget::paintGL()
{
    clear(COLOR_BUFFER_BIT | DEPTH_BUFFER_BIT);
    // Reset Z-buffer, enable Z-buffering


    // Draw Pyramid.
    bindBuffer(ARRAY_BUFFER, pyramidVertexPositionBuffer);
    // set perspective matrix
    WMatrix4x4 pMatrix;
    pMatrix.perspective(45,1,0.1,100.0);
    uniformMatrix4(pMatrixUniform_, pMatrix);
    //set model-view matrix
    WMatrix4x4 mvMatrix;
    mvMatrix.setToIdentity();

    mvMatrix.translate(-1.5,0.0,-7.0);
    mvMatrix.rotate(angle,0,1,0);
    angle+=1.0;
    //setJavaScriptMatrix4(jsMatrix_, );
    uniformMatrix4(mvMatrixUniform_, mvMatrix);

    vertexAttribPointer(vertexPositionAttribute_,
        3,     // size: Every vertex has an X, Y and Z component
        FLOAT, // type: They are floats
        false, // normalized: Please, do NOT normalize the vertices
        3*4, // stride: The first byte of the next vertex is located this
        //         amount of bytes further. The format of the VBO is
        //         vx, vy, vz, nx, ny, nz and every element is a
        //         Float32, hence 4 bytes large
        0);    // offset: The byte position of the first vertex in the buffer
    
    bindBuffer(ARRAY_BUFFER, pyramidVertexColorBuffer);
    vertexAttribPointer(vertexColorAttribute_,
        4,     // size: Every vertex has an X, Y and Z component
        FLOAT, // type: They are floats
        false, // normalized: Please, do NOT normalize the vertices
        4*4, // stride: The first byte of the next vertex is located this
        //         amount of bytes further. The format of the VBO is
        //         vx, vy, vz, nx, ny, nz and every element is a
        //         Float32, hence 4 bytes large
        0);    // offset: The byte position of the first vertex in the buffer
    
    drawArrays(TRIANGLES, 0, 3*4); // 3*4 = number of vertices


    // Draw Cube
    bindBuffer(ARRAY_BUFFER, cubeVertexPositionBuffer);
    mvMatrix.rotate(-angle,0,1,0);
    // set perspective matrix
    uniformMatrix4(pMatrixUniform_, pMatrix);
    //set model-view matrix
    mvMatrix.translate(3.0,0.0,0.0);
    mvMatrix.rotate(angle,1,1,1);
    uniformMatrix4(mvMatrixUniform_, mvMatrix);

    vertexAttribPointer(vertexPositionAttribute_,
        3,     // size: Every vertex has an X, Y and Z component
        FLOAT, // type: They are floats
        false, // normalized: Please, do NOT normalize the vertices
        3*4, // stride: The first byte of the next vertex is located this
        //         amount of bytes further. The format of the VBO is
        //         vx, vy, vz, nx, ny, nz and every element is a
        //         Float32, hence 4 bytes large
        0);    // offset: The byte position of the first vertex in the buffer

    bindBuffer(ARRAY_BUFFER, cubeVertexColorBuffer);
    vertexAttribPointer(vertexColorAttribute_,
        4,     // size: Every vertex has RGBA components
        FLOAT, // type: They are floats
        false, // normalized: Please, do NOT normalize the vertices
        4*4, // stride: The first byte of the next vertex is located this
        //         amount of bytes further. The format of the VBO is
        //         vx, vy, vz, nx, ny, nz and every element is a
        //         Float32, hence 4 bytes large
        0);    // offset: The byte position of the first vertex in the buffer
    bindBuffer(ELEMENT_ARRAY_BUFFER, cubeVertexIndexBuffer);
    drawElements(TRIANGLES,6*2*3,UNSIGNED_SHORT,0); // What are you going to draw, Offset , Number of elements


}


void DrawGLWidget::setShaders(const std::string &vertexShader,
    const std::string &fragmentShader)
{
  vertexShader_ = vertexShader;
  fragmentShader_ = fragmentShader;

}

void DrawGLWidget::initBuffers()
{

    // Define the geometry of the Pyramid
    pyramidVertexPositionBuffer = createBuffer();
    bindBuffer(ARRAY_BUFFER,pyramidVertexPositionBuffer);

    double pyramidVertices[] = {
        // Front face
        0.0,  1.0,  0.0,
       -1.0, -1.0,  1.0,
        1.0, -1.0,  1.0,
       // Right face
        0.0,  1.0,  0.0,
        1.0, -1.0,  1.0,
        1.0, -1.0, -1.0,
       // Back face
        0.0,  1.0,  0.0,
        1.0, -1.0, -1.0,
       -1.0, -1.0, -1.0,
       // Left face
        0.0,  1.0,  0.0,
       -1.0, -1.0, -1.0,
       -1.0, -1.0,  1.0 };
    bufferDatafv(ARRAY_BUFFER,pyramidVertices,pyramidVertices+3*3*4, STATIC_DRAW);

    pyramidVertexColorBuffer = createBuffer();
    bindBuffer(ARRAY_BUFFER,pyramidVertexColorBuffer);
    double pyramidColor[] = {
        // Front face
        1.0, 0.0, 0.0, 1.0,
        0.0, 1.0, 0.0, 1.0,
        0.0, 0.0, 1.0, 1.0,
        // Right face
        1.0, 0.0, 0.0, 1.0,
        0.0, 0.0, 1.0, 1.0,
        0.0, 1.0, 0.0, 1.0,
        // Back face
        1.0, 0.0, 0.0, 1.0,
        0.0, 1.0, 0.0, 1.0,
        0.0, 0.0, 1.0, 1.0,
        // Left face
        1.0, 0.0, 0.0, 1.0,
        0.0, 0.0, 1.0, 1.0,
        0.0, 1.0, 0.0, 1.0};
    bufferDatafv(ARRAY_BUFFER,pyramidColor,pyramidColor+4*3*4, STATIC_DRAW);


    // Define the geometry of the Cube
    cubeVertexPositionBuffer = createBuffer();
    bindBuffer(ARRAY_BUFFER,cubeVertexPositionBuffer);


    double cubeVertices[] = {
        // Front face
        -1.0, -1.0,  1.0,
         1.0, -1.0,  1.0,
         1.0,  1.0,  1.0,
        -1.0,  1.0,  1.0,
        // Back face
        -1.0, -1.0, -1.0,
        -1.0,  1.0, -1.0,
         1.0,  1.0, -1.0,
         1.0, -1.0, -1.0,
        // Top face
        -1.0,  1.0, -1.0,
        -1.0,  1.0,  1.0,
         1.0,  1.0,  1.0,
         1.0,  1.0, -1.0,
        // Bottom face
        -1.0, -1.0, -1.0,
         1.0, -1.0, -1.0,
         1.0, -1.0,  1.0,
        -1.0, -1.0,  1.0,
        // Right face
         1.0, -1.0, -1.0,
         1.0,  1.0, -1.0,
         1.0,  1.0,  1.0,
         1.0, -1.0,  1.0,
        // Left face
        -1.0, -1.0, -1.0,
        -1.0, -1.0,  1.0,
        -1.0,  1.0,  1.0,
        -1.0,  1.0, -1.0};
    bufferDatafv(ARRAY_BUFFER,cubeVertices,cubeVertices+3*4*6, STATIC_DRAW);

    cubeVertexColorBuffer = createBuffer();
    bindBuffer(ARRAY_BUFFER,cubeVertexColorBuffer);
    double cubeColor[] = {
        // Front Face
        1.0, 0.0, 0.0, 1.0,
        1.0, 0.0, 0.0, 1.0,
        1.0, 0.0, 0.0, 1.0,
        1.0, 0.0, 0.0, 1.0,
        // Back Face
        1.0, 1.0, 0.0, 1.0,
        1.0, 1.0, 0.0, 1.0,
        1.0, 1.0, 0.0, 1.0,
        1.0, 1.0, 0.0, 1.0,
        // Top Face
        0.0, 1.0, 0.0, 1.0,
        0.0, 1.0, 0.0, 1.0,
        0.0, 1.0, 0.0, 1.0,
        0.0, 1.0, 0.0, 1.0,
        // Bottom Face
        1.0, 0.5, 0.5, 1.0,
        1.0, 0.5, 0.5, 1.0,
        1.0, 0.5, 0.5, 1.0,
        1.0, 0.5, 0.5, 1.0,
        // Right Face
        1.0, 0.0, 1.0, 1.0,
        1.0, 0.0, 1.0, 1.0,
        1.0, 0.0, 1.0, 1.0,
        1.0, 0.0, 1.0, 1.0,
        // Left Face
        0.0, 0.0, 1.0, 1.0,
        0.0, 0.0, 1.0, 1.0,
        0.0, 0.0, 1.0, 1.0,
        0.0, 0.0, 1.0, 1.0};
    bufferDatafv(ARRAY_BUFFER,cubeColor,cubeColor+4*4*6, STATIC_DRAW);

    cubeVertexIndexBuffer = createBuffer();
    bindBuffer(ELEMENT_ARRAY_BUFFER,cubeVertexIndexBuffer);
    unsigned short int cubeIndices[] = {
                0, 1, 2,      0, 2, 3,    // Front face
                4, 5, 6,      4, 6, 7,    // Back face
                8, 9, 10,     8, 10, 11,  // Top face
                12, 13, 14,   12, 14, 15, // Bottom face
                16, 17, 18,   16, 18, 19, // Right face
                20, 21, 22,   20, 22, 23  // Left face
            };


    bufferDataiv(ELEMENT_ARRAY_BUFFER,cubeIndices,cubeIndices+6*2*3,STATIC_DRAW,UNSIGNED_SHORT);
}


















