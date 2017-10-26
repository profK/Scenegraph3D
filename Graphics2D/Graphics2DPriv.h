/*
 *  Graphics2DPriv.h
 *  Graphics2D
 *
 *  Created by Jeffrey Kesselman on 11/1/14.
 *  Copyright (c) 2014 Jeffrey Kesselman. All rights reserved.
 *
 */

#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>
#include "Graphics2D.h"
#include <glfw3.h>

/* The classes below are not exported */
#pragma GCC visibility push(hidden)



namespace Graphics2D{

    class GraphicsProvider2DPriv: GraphicsProvider2D
    {
        private:
            GLFWwindow* window;
            KeyCallback keyCB=nullptr;
            
            
        public:
            void* user_data_ptr;
            
        GraphicsProvider2DPriv(const std::string title, const int windowWidth,const int windowHeight);
        G2DImage* LoadImage(const std::string path)const;
        void BeginFrame()const;
        void DrawImage(const G2DImage* sprite,const Rectangle source, const Transform2D transform)const;
        void EndFrame()const;
        ~GraphicsProvider2DPriv();
        void SetKeyCallback(const KeyCallback keyCallback){
            keyCB = keyCallback;
        }
        void DoKey(const int key)const{
            if (keyCB!=nullptr){
                keyCB(this,key);
            }
        }
    };

    class G2DImagePriv: G2DImage {
        public:
            std::string _path;
            GLuint _texname;
            int _width,_height;
        
            G2DImagePriv(const std::string path,const GLuint texname){
                _path=path;
                _texname=texname;
                glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &_width);
                glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &_height);
            }
        
        int GetWidth()const{
            return _width;
        }
        
        int GetHeight()const{
            return _height;
        }
        
    };
    

    
}

#pragma GCC visibility pop
