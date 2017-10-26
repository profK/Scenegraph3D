//
//  Graphics3DPriv.h
//  Graphics2D
//
//  Created by Jeffrey Kesselman on 11/7/14.
//  Copyright (c) 2014 Jeffrey Kesselman. All rights reserved.
//

#ifndef Graphics3DPriv_h
#define Graphics3DPriv_h

#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include "Graphics3D.h"
#include <glfw3.h>
#include <vector>

namespace Graphics3D{
    
    /**
     * This defines a private sub-class that implements the pure abstract class
     * G3DModel. See Graphics3D.h for its public interface.
     */
    class G3DModelPriv:G3DModel{
        /**
         * The private implementation class for GrpahicsProvider3D needs access to
         * the internals of this class so that it can render it
         */
        friend class GraphicsProvider3DPriv;
        
    private:
        /* This is all the data that defines a G3DModel's internal
         * representation
         */
        std::vector<GLfloat> vertices;
        std::vector<GLfloat> normals;
        std::vector<GLfloat> texcoords;
        std::vector<GLushort> indices;
        GLuint texname;
        bool _textured=false;
        
    public:
        /**
         * This constructor is used by factory methods on GrpahicsProvider3DPriv to
         * create concrete instances of G3DModel on request
         */
        G3DModelPriv(std::vector<GLfloat> vertices,
                     std::vector<GLfloat> normals,
                     std::vector<GLfloat> texcoords,
                     std::vector<GLushort> indices,GLuint texname){
            this->vertices = vertices;
            this->normals = normals;
            this->texcoords = texcoords;
            this->indices=indices;
            this->texname = texname;
        }
        
        
    };

    
    /**
     * This defines a private sub-class that implements the pure abstract class
     * GraphicsProvider3D. See Graphics3D.h for its public interface.
     */
    class GraphicsProvider3DPriv: GraphicsProvider3D
    {
    private:
        /**
         * A pointer to an underlying GLFW window.
         */
        GLFWwindow* window;
        /**
         * A registered key event callback to which to pass key events
         */
        KeyCallback keyCB=nullptr;
        
        
    public:
      
        /**
         * This is the private constructor that is used by the factory method to actually create the
         * window.  It is used by the public factory method GraphicsProvider3D:MakeGraphicsProvider(...)
         */
        GraphicsProvider3DPriv(const std::string title, const int windowWidth,const int windowHeight);
       
        /**
         * THis method must be called at the start of a frame, before any models are drawn.
         * It does all the set up of the frame to ready it for drawing.
         */
        void BeginFrame()const;
        
        /**
         * This method draws the passed model to the output window, transforming all
         * vertices with the passed in Transform3D.
         * @param model a pointer to a G3DModel to draw
         * @param transform the Transform3D to apply to its vertices
         */
        void DrawModel(const G3DModel* model, const Transform3D transform)const;
        
        /**
         * THis method must be called at the end of a frame, after all models are drawn.
         * It finalizes the frame and puts it to the screen.
         */
        void EndFrame()const;
        
        
        /**
         * This is used to register a handler for all key events that occur in the
         * GraphicsProvider3D's associated window.
         *
         * @param keyCallback a function pointer of type KeyCallback, or nullptr
         * to deactivate callbacks.
         */
        void SetKeyCallback(const KeyCallback keyCallback){
            keyCB = keyCallback;
        }
        
        /**
         *This method is used to proxy callbacks from GLFW for key events to
         * a user defined handler
         */
        void DoKey(const int key)const{
            if (keyCB!=nullptr){
                keyCB(this,key);
            }
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

        G3DModel* MakeTexturedSphere(const float radius, const unsigned int rings, const unsigned int sectors,
                                     const std::string path)const;
        
        /**
         *Destructor to allow for cleanup
         */
        ~GraphicsProvider3DPriv();
    };

}
#endif
