//
//  Graphics3D.cpp
//  Graphics2D
//
//  Created by Jeffrey Kesselman on 11/7/14.
//  Copyright (c) 2014 Jeffrey Kesselman. All rights reserved.
//



#include "Graphics3D.h"
#include "Graphics3DPriv.h"

#include <string>

#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include "glfw3.h"
#include "SOIL.h"
#include <cml/cml.h>



namespace Graphics3D {
    /*** Vector3 Implementation ***/
    /**
     * This struct is the internal Implementation of the Vector3 data
     * It is defined as a struct in the CPP file rather then in the
     * header file in order to isolate clients from the immplementation
     */
    struct Vector3::Implementation {
        cml::vector3f vec = cml::vector3f(0,0,0);
    };

    /**
     * This is the default constructor for Vector 3 which creates am
     * Instance of Implementation and sets a shared pr to reference it
     */
    Vector3::Vector3(){
        _pimpl.reset(new Implementation);
    }

    /**
     * This is the value constructor which creates an instance of Implementation
     * and sets its internal cml::vector3f to the passed x, y and z values
     */
    Vector3::Vector3(const float x, const float y, const float z){
        _pimpl.reset(new Implementation);
        _pimpl->vec = cml::vector3f(x,y,z);
    }

    /**
     * This returns the X value from the internal representation
     */
    float Vector3::GetX()const{
        return _pimpl->vec.data()[0];
    }

    /**
     * This returns the Y value from the internal representation
     */
    float Vector3::GetY()const{
        return _pimpl->vec.data()[1];
    }
    
    /**
     * This returns the Z value from the internal representation
     */
    float Vector3::GetZ()const{
        return _pimpl->vec.data()[2];
    }

    /**
     * THis performs a vector scalar multiply and returns the
     * result as a Vector3.  It does not chnage the value of
     * the operands.
     */
    Vector3 Vector3::operator*(const float f)const{
        return Vector3(_pimpl->vec.data()[0]*f,_pimpl->vec.data()[1]*f,_pimpl->vec.data()[2]);
    }

    /**
     * THis performs a vector scalar divide and returns the
     * result as a Vector3.  It does not chnage the value of
     * the operands.
     */
    Vector3 Vector3::operator/(const float f)const{
        return Vector3(_pimpl->vec.data()[0]/f,_pimpl->vec.data()[1]/f,_pimpl->vec.data()[2]);
    }



    /*** Transform implementaton ***/
    
    /**
     * This struct is the internal Implementation of the Transform3D data
     * It is defined as a struct in the CPP file rather then in the
     * header file in order to isolate clients from the immplementation
     */
    struct Transform3D::Implementation {
        cml::matrix44f_c matrix;
    };
    
    /**
     * This is the default constructor for Transform3D which creates am
     * Instance of Implementation and sets a shared pr to reference it
     *
     * A Transform3D starts out as the Identity transform
     */

    Transform3D::Transform3D(){
        _pimpl.reset(new Implementation);
        _pimpl->matrix = cml::identity(_pimpl->matrix);
    }


    /**
     * This modifies the Transform3D, translating it with the 
     * passed in Vector3
     *
     * @param vec the Translation to apply represented as a Vector3
     */
    void Transform3D::Translate(const Vector3 vec){
        cml::matrix44f_c xlateMatrix;
        cml::matrix_translation(xlateMatrix, vec._pimpl->vec);
        
        xlateMatrix = xlateMatrix*_pimpl->matrix;
        _pimpl.reset(new Implementation);
        _pimpl->matrix = xlateMatrix;
    }

    /**
     * This modifies the Transform3D, rotating it with the
     * passed in Vector3.  Angles are applied in the order x,y,z
     *
     * @param vec the Rotation to apply represented as a Vector3 of euler angles
     */
    void Transform3D::Rotate(const Vector3 eulerAngles){
        cml::matrix44f_c rotMatrix;
        cml::matrix_rotation_euler(rotMatrix, eulerAngles.GetX(), eulerAngles.GetY(), eulerAngles.GetZ(), cml::euler_order_xyz);
        rotMatrix = rotMatrix*_pimpl->matrix;
        _pimpl.reset(new Implementation);
        _pimpl->matrix = rotMatrix;
    }

    /**
     * This transforms the passed in vec according to the current setting of the Transform3D
     * It doesnt modify its operands, but instead returns the result as a new Vector3.
     */
    Vector3 Transform3D::TransformVec(const Vector3 sourceVec)const{
        cml::vector3f result =cml::transform_point(_pimpl->matrix, sourceVec._pimpl->vec);
        return Vector3(result.data()[0],result.data()[1],result.data()[3]);
    }

    /*
     * This transforms the passed in vec according to the current setting of the Transofrm3D
     * It returns the new vec as a replacement for the passed in vec.
     */
    void Transform3D::TransformVecInPlace(Vector3 &sourceVec)const{
        sourceVec = TransformVec(sourceVec);
    }

    /**
     * This transforms an array of Vector3 according to the current Transform3D state
     * It returns the result as a new array of vector3 of length len
     *
     * NOTE: This function allocates memory that needs to be deallocated after use to
     * avoid memory leaks
     *
     * @param sourceVec the array of vectors to transform
     * @param start the index of the first Vector3 to transform
     * @param len the number of Vector3 to transform
     * @return A new array of Vector3
     */
    Vector3* Transform3D::TransformVecs(const Vector3 sourceVec[],const int start, const int len)const{
        Vector3* newVecs = new Vector3[len];
        for(int i=0;i<len;i++){
            newVecs[i] = TransformVec(sourceVec[start+i]);
        }
        return newVecs;
    }

    /**
     * This transforms an array of Vector3 according to the current Transform3D state
     * It returns the result as replacement values in the passed in source array
     * thus it allocates no new memory.
     *
     * @param sourceVec the array of vectors to transform
     * @param start the index of the first Vector3 to transform
     * @param len the number of Vector3 to transform
     */
    void  Transform3D::TransformVecsInPlace(Vector3 sourceVec[],const int start, const int len)const{
        for(int i=0;i<len;i++){
            sourceVec[start+i] = TransformVec(sourceVec[start+i]);
        }
    }

    /**
     * This function post-multiplies the Transform3D's matrix with another Transform3D's
     * matrix, returning the result as a new Transform3D.  It does not
     * modify either operand themselves.
     */
    Transform3D Transform3D::operator*(const Transform3D other)const{
        Transform3D result;
        result._pimpl->matrix=_pimpl->matrix*other._pimpl->matrix;
        return result;
    }
    
    /**
     * This is a utility function used to fetch the matrix data ina form appropriate to
     * give to an openGL matrix call.
     */
    float* Transform3D::GetOGLData()const{
        return _pimpl->matrix.data();
    }

    /*** GraphicsProvider3DPriv Impementation  ***/
    
    /**
     * This callback is registered with GLFW to handle key events in
     * its window. 
     */
    void GLFWKeyCallback(GLFWwindow* window, int key, int scanCode, int action, int mods){
        GraphicsProvider3DPriv* provider = (GraphicsProvider3DPriv*)glfwGetWindowUserPointer(window);
        provider->DoKey(key);
    }
    
    
    /**
     * This the factory method that creates a new GraphicsProvider3D.
     * It opens a new window to draw in according to the passed in values
     * @param windowName  the title of the window
     * @param width the width of the window in screen pixels
     * @param the height of the window in screen pixels.
     */
    GraphicsProvider3D* GraphicsProvider3D::MakeNewProvider(const std::string windowName,const int width, const int height){
        return (GraphicsProvider3D* )new GraphicsProvider3DPriv(windowName,width,height);
    }
   
    /**
     * This is the private constructor that is used by the factory method to actually create the 
     * window
     */
    GraphicsProvider3DPriv::GraphicsProvider3DPriv(const std::string title, const int windowWidth,const int windowHeight){
        
        
        /* Initialize the library */
        if (!glfwInit())
            throw std::runtime_error("Failed to initialize glfw");
        
        /* Create a windowed mode window and its OpenGL context */
        window = glfwCreateWindow(windowWidth, windowHeight, title.c_str(), NULL, NULL);
        if (!window)
        {
            glfwTerminate();
            throw std::runtime_error("Failed to create window");
        }
        
        glfwSetWindowUserPointer(window, this);
        
        /* Make the window's context current */
        glfwMakeContextCurrent(window);
        
        /* set key callback */
        glfwSetKeyCallback(window,GLFWKeyCallback);
    }
    
    /**
     * THis method must be called at the start of a frame, before any models are drawn.
     * It does all the set up of the frame to ready it for drawing.
     */
    void GraphicsProvider3DPriv::BeginFrame()const{
        int win_width;
        int win_height;// framework of choice here
        glfwGetWindowSize(window, &win_width, &win_height); // retrieve window
        float const win_aspect = (float)win_width / (float)win_height;
        // set lighting
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        glEnable(GL_DEPTH_TEST);
        GLfloat lightpos[] = {0, 0.0, 0, 0.};
        glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
        GLfloat lmodel_ambient[] = { 0.2, 0.2, 0.2, 1.0 };
        glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
        glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
        // set up world transform
        glClearColor(0.f, 0.f, 0.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT|GL_ACCUM_BUFFER_BIT);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        
        gluPerspective(45, win_aspect, 1, 10);
        
        glMatrixMode(GL_MODELVIEW);
        
    }
    
    
    /**
     * This method draws the passed model to the output window, transforming all
     * vertices with the passed in Transform3D.
     * @param model a pointer to a G3DModel to draw
     * @param transform the Transform3D to apply to its vertices
     */
    void GraphicsProvider3DPriv::DrawModel(const G3DModel* model, const Transform3D transform)const{
        G3DModelPriv* privModel = (G3DModelPriv *)model;
        glPushMatrix();
        glLoadMatrixf(transform.GetOGLData());
        
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        
        glVertexPointer(3, GL_FLOAT, 0, &privModel->vertices[0]);
        glNormalPointer(GL_FLOAT, 0, &privModel->normals[0]);
        glTexCoordPointer(2, GL_FLOAT, 0, &privModel->texcoords[0]);
        
        glEnable(GL_TEXTURE_2D);
        //glFrontFace(GL_CCW);
        glEnable(GL_CULL_FACE);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, privModel->texname);
        
        glDrawElements(GL_QUADS, privModel->indices.size(), GL_UNSIGNED_SHORT, &privModel->indices[0]);
        glPopMatrix();
        glDisable(GL_TEXTURE_2D);
       
    }
    
    /**
     * THis method must be called at the end of a frame, after all models are drawn.
     * It finalizes the frame and puts it to the screen.
     */
    void GraphicsProvider3DPriv::EndFrame()const{
        /* Swap front and back buffers */
        glDisable(GL_LIGHTING);
        glDisable(GL_LIGHT0);
        glDisable(GL_CULL_FACE);
        glfwSwapBuffers(window);
        
        /* Poll for and process events */
        glfwPollEvents();
    }
    
    /**
     * This is an internal utiltiy used to load model textures
     */
    static GLuint LoadImage(const std::string path){
        
        /*load an image file directly as a new OpenGL texture */
        GLuint texName = SOIL_load_OGL_texture(path.c_str(),SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,
                                               SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
                                               );
        
        /* check for an error during the load process */
        if( 0 == texName )
        {
            std::string message( "SOIL loading error: ");
            message = message + SOIL_last_result();
            throw std::runtime_error(message.c_str());
        }
        
        return texName;
    }

    /**
     * This factory method creates a textured sphere and returns it as a pointer to a G3DModel
     * It is actually a polygonal approximation of a sphere with appropriate vertex normals to make it
     * appear continuosly curved
     *
     * @param radius the radius of the model in world coordinates
     * @param the humber of horizontal ring divisions in the sphere approximation
     * @param the number of verticle slice divisions in the sphere approximation
     * @param path a relative or absolute file path to an image file to use as the sphere's texture
     */
    G3DModel* GraphicsProvider3DPriv::MakeTexturedSphere(const float radius, const unsigned int rings,
                                                     const unsigned int sectors,
                                                     const std::string path)const{
        
        std::vector<GLfloat> vertices;
        std::vector<GLfloat> normals;
        std::vector<GLfloat> texcoords;
        std::vector<GLushort> indices;
        
        float const R = 1./(float)(rings-1);
        float const S = 1./(float)(sectors-1);
        int r, s;
        
        vertices.resize(rings * sectors * 3);
        normals.resize(rings * sectors * 3);
        texcoords.resize(rings * sectors * 2);
        std::vector<GLfloat>::iterator v = vertices.begin();
        std::vector<GLfloat>::iterator n = normals.begin();
        std::vector<GLfloat>::iterator t = texcoords.begin();
        for(r = 0; r < rings; r++) for(s = 0; s < sectors; s++) {
            float const y = sin( -M_PI_2 + M_PI * r * R );
            float const x = cos(2*M_PI * s * S) * sin( M_PI * r * R );
            float const z = sin(2*M_PI * s * S) * sin( M_PI * r * R );
            
            *t++ = s*S;
            *t++ = r*R;
            
            *v++ = x * radius;
            *v++ = y * radius;
            *v++ = z * radius;
            
            *n++ = -x;
            *n++ = -y;
            *n++ = -z;
        }
        
        indices.resize(rings * sectors * 4);
        std::vector<GLushort>::iterator i = indices.begin();
        for(r = 0; r < rings-1; r++)
            for(s = 0; s < sectors-1; s++) {
               /* 
                *i++ = r * sectors + s;
                *i++ = r * sectors + (s+1);
                *i++ = (r+1) * sectors + (s+1);
                *i++ = (r+1) * sectors + s;
                */
                 *i++ = (r+1) * sectors + s;
                 *i++ = (r+1) * sectors + (s+1);
                *i++ = r * sectors + (s+1);
                 *i++ = r * sectors + s;

        }
        
        return (G3DModel *)new G3DModelPriv(vertices,normals,texcoords,indices,LoadImage(path));
    }
    
    /**
     * This destructor cleans up the glfw window
     */

    GraphicsProvider3DPriv::~GraphicsProvider3DPriv(){
        glfwTerminate();
    }

}