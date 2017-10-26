#include "Graphics2D.h"
#include "Graphics2DPriv.h"

#include <string>

#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include "glfw3.h"
#include "SOIL.h"
#include <cml/cml.h>

namespace Graphics2D{
    /*** Vector2 Implementation ***/
    /**
     * This struct is the internal Implementation of the Vector2 data
     * It is defined as a struct in the CPP file rather then in the
     * header file in order to isolate clients from the immplementation
     */
    struct Vector2::Implementation {
        cml::vector2f vec = cml::vector2f(0,0);
    };
    
    /**
     * This is the default constructor for Vector 3 which creates am
     * Instance of Implementation of (0,0) and sets a shared pr to reference it
     */
    Vector2::Vector2(){
        _pimpl.reset(new Implementation);
    }
    
    /**
     * This is the value constructor which creates an instance of Implementation
     * and sets its internal cml::vector3f to the passed x, and y values
     */
    Vector2::Vector2(const float x, const float y){
        _pimpl.reset(new Implementation);
        _pimpl->vec = cml::vector2f(x,y);
    }

    /**
     * This returns the X value from the internal representation
     */
    float Vector2::GetX()const{
        return _pimpl->vec.data()[0];
    }
    
    /**
     * This returns the Y value from the internal representation
     */
    float Vector2::GetY()const{
        return _pimpl->vec.data()[1];
    }
    
    /**
     * THis performs a vector scalar multiply and returns the
     * result as a Vector2.  It does not chnage the value of
     * the operands.
     */
    Vector2 Vector2::operator*(const float f)const{
        return Vector2(_pimpl->vec.data()[0]*f,_pimpl->vec.data()[1]*f);
    }
    
    /**
     * THis performs a vector scalar divide and returns the
     * result as a Vector2.  It does not chnage the value of
     * the operands.
     */
    Vector2 Vector2::operator/(const float f)const{
        return Vector2(_pimpl->vec.data()[0]/f,_pimpl->vec.data()[1]/f);
    }
    
    
        
    /*** Transform implementaton ***/
    /**
     * This struct is the internal Implementation of the Transform2D data
     * It is defined as a struct in the CPP file rather then in the
     * header file in order to isolate clients from the immplementation
     */
    
    struct Transform2D::Implementation {
        cml::matrix33f matrix;
    };
    
    /**
     * This is the default constructor for Transform2D which creates am
     * Instance of Implementation and sets a shared pr to reference it
     *
     * A Transform2D starts out as the Identity transform
     */
    
    
    Transform2D::Transform2D(){
        _pimpl.reset(new Implementation);
        _pimpl->matrix = cml::identity(_pimpl->matrix);
    }

   
    /**
     * This modifies the Transform3D, translating it with the
     * passed in Vector3
     *
     * @param vec the Translation to apply represented as a Vector3
     */

    void Transform2D::Translate(const Vector2 vec){
        cml::matrix33f xlateMatrix;
        cml::matrix_translation_2D(xlateMatrix, vec._pimpl->vec);
       
        xlateMatrix = xlateMatrix * _pimpl->matrix;
         _pimpl.reset(new Implementation);
        _pimpl->matrix = xlateMatrix;
    }
    
    /**
    * This modifies the Transform3D, rotating it with the
    * passed in Vector3.  Angles are applied in the order x,y,z
    *
    * @param vec the Rotation to apply represented as a Vector3 of euler angles
    */
    void Transform2D::Rotate(const float radians){
       cml::matrix33f rotMatrix;
       cml::matrix_rotation_2D(rotMatrix, radians);
       rotMatrix = rotMatrix * _pimpl->matrix;
        _pimpl.reset(new Implementation);
        _pimpl->matrix = rotMatrix;
   }

    /**
     * This transforms the passed in vec according to the current setting of the Transform3D
     * It doesnt modify its operands, but instead returns the result as a new Vector3.
     */
    Vector2 Transform2D::TransformVec(const Vector2 sourceVec)const{
        cml::vector2f result =cml::transform_point_2D(_pimpl->matrix, sourceVec._pimpl->vec);
       return Vector2(result.data()[0],result.data()[1]);
   }
    
    
    /*
     * This transforms the passed in vec according to the current setting of the Transofrm3D
     * It returns the new vec as a replacement for the passed in vec.
     */
    void Transform2D::TransformVecInPlace(Vector2 &sourceVec)const{
       sourceVec = TransformVec(sourceVec);
   }
   
    /**
     * This transforms an array of Vector2 according to the current Transform3D state
     * It returns the result as a new array of vector2 of length len
     *
     * NOTE: This function allocates memory that needs to be deallocated after use to
     * avoid memory leaks
     *
     * @param sourceVec the array of vectors to transform
     * @param start the index of the first Vector2 to transform
     * @param len the number of Vector2 to transform
     * @return A new array of Vector2
     */
   
    Vector2* Transform2D::TransformVecs(const Vector2 sourceVec[],const int start, const int len)const{
       Vector2* newVecs = new Vector2[len];
       for(int i=0;i<len;i++){
           newVecs[i] = TransformVec(sourceVec[start+i]);
       }
       return newVecs;
   }
   
    /**
     * This transforms an array of Vector2 according to the current Transform3D state
     * It returns the result as replacement values in the passed in source array
     * thus it allocates no new memory.
     *
     * @param sourceVec the array of vectors to transform
     * @param start the index of the first Vector2 to transform
     * @param len the number of Vector2 to transform
     */
    void  Transform2D::TransformVecsInPlace(Vector2 sourceVec[],const int start, const int len)const{
       for(int i=0;i<len;i++){
           sourceVec[start+i] = TransformVec(sourceVec[start+i]);
       }
   }
    
    /**
     * This function post-multiplies the Transform3D's matrix with another Transform3D's
     * matrix, returning the result as a new Transform3D.  It does not
     * modify either operand themselves.
     */
    Transform2D Transform2D::operator*(const Transform2D other)const{
        Transform2D result;
        result._pimpl->matrix=_pimpl->matrix*other._pimpl->matrix;
        return result;
    }

    /*** GraphicsProvider2DPriv Impementation  ***/
    /**
     * This callback is registered with GLFW to handle key events in
     * its window.
     */
    
    void GLFWKeyCallback(GLFWwindow* window, int key, int scanCode, int action, int mods){
        GraphicsProvider2DPriv* provider = (GraphicsProvider2DPriv*)glfwGetWindowUserPointer(window);
        provider->DoKey(key);
    }
    
    /**
     * This the factory method that creates a new GraphicsProvider2D.
     * It opens a new window to draw in according to the passed in values
     * @param windowName  the title of the window
     * @param width the width of the window in screen pixels
     * @param the height of the window in screen pixels.
     */
    GraphicsProvider2D* GraphicsProvider2D::MakeNewProvider(const std::string windowName,const int width, const int height){
        return (GraphicsProvider2D* )new GraphicsProvider2DPriv(windowName,width,height);
    }
    
    
    /**
     * This is the private constructor that is used by the factory method to actually create the
     * window
     */
    GraphicsProvider2DPriv::GraphicsProvider2DPriv(const std::string title, const int windowWidth,const int windowHeight){
        
        
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
        
        /* set drawing dimensions to screen dimensions */
          gluOrtho2D(0, windowWidth, 0, windowHeight);
                             
    }
    
    /**
     * This method loads an image bitmap at the passed relative or absolute path and returns 
     * a pointer to a G2DImage object that represents it.
     */
    G2DImage* GraphicsProvider2DPriv::LoadImage(const std::string path)const{
        /* load an image file directly as a new OpenGL texture */
        GLuint texName = SOIL_load_OGL_texture(path.c_str(),SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,
         SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
         );
        
        /* check for an error during the load process */
        if( 0 == texName )
        {
            printf( "SOIL loading error: '%s'\n", SOIL_last_result() );
            return nullptr;
        }
        
        return (G2DImage *)new G2DImagePriv(path,texName);
    }
    
    /**
     * THis method must be called at the start of a frame, before any images are drawn.
     * It does all the set up of the frame to ready it for drawing.
     */
    void GraphicsProvider2DPriv::BeginFrame()const{
        // completely clear the drawing buffer
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT|GL_ACCUM_BUFFER_BIT);
      
    }
    
    /**
     * THis method must be called at the end of a frame, after all images are drawn.
     * It finalizes the frame and puts it to the screen.
     */
    void GraphicsProvider2DPriv::EndFrame()const{
        /* Swap front and back buffers */
        glfwSwapBuffers(window);
        
        /* Poll for and process events */
        glfwPollEvents();
    }

    /**
     * This method draws a rectangle of bits from the passed in image to the output window, transforming all
     * vertices with the passed in Transform2D
     * It supports a sub-rectange to make it easy to use animated sprites where the image contains
     * multiple frames.
     *
     * @param image a pointer to a G3DImage to get bits from
     * @param source the sub-rectangle of bits to extract from the image
     * @param transform the Transform3D to apply to its vertices
     */

    void GraphicsProvider2DPriv::DrawImage(const G2DImage *image, const Rectangle source, const Transform2D transform)const{
        G2DImagePriv* img = (G2DImagePriv *)image;
        
        // figure dest coords
        
        Vector2 destVerts[4]= {
            Vector2(0,0),
            Vector2(source.width,0),
            Vector2(source.width,source.height),
            Vector2(0,source.height)
        };
        
        
        Vector2* xformedVerts = transform.TransformVecs(destVerts,0,4);
        // figure texture coords
        int textureWidth = image->GetWidth();
        int textureHeight = image->GetHeight();
        float v_tlx=source.x/textureWidth;
        float v_tly=source.y/textureHeight;
        float v_brx=(source.x+source.width)/textureWidth;
        float v_bry=(source.y+source.height)/textureHeight;
        
        Vector2 texCoords[4] = {
            Vector2(v_tlx,v_tly),
            Vector2(v_brx,v_tly),
            Vector2(v_brx,v_bry),
            Vector2(v_tlx,v_bry)
        };

        
        //start render
        glBindTexture(GL_TEXTURE_2D, img->_texname);
        glEnable(GL_TEXTURE_2D);
        glBegin(GL_QUADS);
        
       
        //transform->TransformVecs(destVerts,0,4);
        // draw
        for(int i=0;i<4;i++){
            glTexCoord2d(texCoords[i].GetX(),texCoords[i].GetY());
           // glVertex2d(destVerts[i].GetX(),destVerts[i].GetY());
            glVertex2d(xformedVerts[i].GetX(),xformedVerts[i].GetY());

            //printf("draw vert at %f,%f\n",destVerts[i].GetX(),destVerts[i].GetY());
        }
        glEnd();
        delete [] xformedVerts;
    }
    
    /**
     * This destructor cleans up the glfw window
     */
    GraphicsProvider2DPriv::~GraphicsProvider2DPriv(){
        glfwTerminate();
    }
    
   
    
    
    
}