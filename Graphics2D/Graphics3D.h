//
//  Graphics3D.h
//  Graphics3D
//
//  Created by Jeffrey Kesselman on 11/7/14.
//  Copyright (c) 2014 Jeffrey Kesselman. All rights reserved.
//

#ifndef __Graphics3D_h
#define __Graphics3D_h
#include <string>
#include <memory>

#pragma GCC visibility push(default)
namespace Graphics3D {
    
    
    /**
     * This class defines a Vector3 type which is three
     * floating point values, x, y and z.
     *
     * It makes use of the PIMPL idiom to hide private data
     * Assignment of one vector to another makes a shallow copy
     * as this class is an Immutable.
     *
     * This class may be used as an automatic (value type)
     */
    class Vector3 {
        // Transforms need to have access to its values
        // in order to do their work
        friend class Transform3D;
        
    private:
        /***
         * This is a foward declaration of the privately defined
         * structure that holds its actual values
         */
        struct Implementation;
        /**
         * This is a pointer to the value block for the Vector3
         * its is rference counted so that it is automatically cleaned up
         * when its last referer is destroyed.
         * Note: Assigning one vector to another is a shallow copy, creating
         * another owner of the shared pointer.
         */
        
        std::shared_ptr<Implementation> _pimpl;
        
    public:
        /**
         *  This is the empty constructor.
         *  It create a vector with value (0,0)
         */
        Vector3();
        
        /**
         * This is a contructor that takes explicit values for
         * (x,y)
         *
         * @param x The X coordinate value
         * @param y The Y coordinate value
         */
        Vector3(float x, float y, float z);
        
        /**
         *  This returns the X value of the Vector
         *
         * @returns The X value
         */
        float GetX()const;
        /**
         *  This returns the y value of the Vector
         *
         * @returns The Y value
         */
        float GetY()const;
        /**
         *  This returns the z value of the Vector
         *
         * @returns The Z value
         */
        float GetZ()const;
        
        /**
         * The scalar multiplier function
         *
         *  This returns a new vector whose x,y and Z values
         *  are (this->GetX()*f,this->GetY()*f,this->getZ*f)
         *
         * @param f The scalar float to mutliply by
         * @returns A new Vector3 whose value is the called Vector's
         * value multiplied by f
         */
        Vector3 operator*(float f)const;
        /**
         * The scalar division function
         *
         *  This returns a new vector whose x,y and z values
         *  are (this->GetX()/f,this->GetY()/f,this->GetZ())
         *
         * @param f The scalar float to divide by
         * @returns A new Vector3 whose value is the called Vector's
         * value divided by f
         */
        
        Vector3 operator/(float f)const;
        
       
        
    };
    
    /**
     * This class defines a 3D Transform type which can be translated
     * and rotated.
     *
     * It makes use of the PIMPL idiom to hide private data.
     * Assignment of one Tramsfr, to another makes a shallow copy.
     * When this Transform is traslated or rotated however its shared_ptr
     * is reset to a new implemntation of the PIMPL data thereby
     * insulating it and making any instance appear to be individually mutable.
     *
     * This class may be used as an automatic (value type)
     */
    class Transform3D {
    private:
        
        /***
         * This is a foward declaration of the privately defined
         * structure that holds its actual values
         */
        struct Implementation;
        /**
         * This is a pointer to the value block for the Vector3
         * its is rference counted so that it is automatically cleaned up
         * when its last referer is destroyed.
         * Note: Assigning one vector to another is a shallow copy, creating
         * another owner of the shared pointer.
         */
        
        std::shared_ptr<Implementation> _pimpl;
        
    public:
        /**
         * Default constructor that creates an Identity transform.
         */
        Transform3D();
        /**
         * Adds an x and y translation to the Transform
         *
         * This is used to "move" an object that will be drawn using this transform
         *
         * @param vec  a 2D vector containing the X and Y offsets
         *
         */
        void Translate(const Vector3 vec);
        
        /**
         * Rotates the transform about the Z axis
         *
         * This is used to "move" an object that will be drawn using this transform
         * rotated by the passsed vector read as eulerAngles in radians, which takes any previous
         * translations or rotations into account
         *
         * @param radians  the amount to rotate in radians
         */
        void Rotate(const Vector3 eulerAngles);
        
        /**
         * Calculates the result of applying this transform to a set of coords
         *
         * This calls transfroms an existing coordinate pair according to al
         * the translates and rotates applied to this transform.  It returns
         * the result in a new Vector3 object.
         *
         * @param sourceVec  the vector containing the x, y and z coords to transform
         * @returns a vector containing the newly transformed x,y and z coords.
         */
        Vector3 TransformVec(const Vector3 sourceVec)const;
        
        /**
         *Calculates the result of applying this transform to a set of coords
         *
         * This call is just like the pervious one except that it chnages the value of
         * the passed in Vector3 (really replacing it since Vector3s are immutable) rather
         * then returning it as a new vector.
         *
         * @param sourceVec the vector to transform and replace the value of
         */
        void TransformVecInPlace(Vector3 &sourceVec)const;
        
        //NOTE: This function allocates memory that needs to be deallocated after use to
        // avoid memory leaks
        /**
         * Transforms an array of vectors
         *
         *  This function applies the transform to an array of vectors and returns an array
         *  containing the new values.
         *  IMPORTANT: This array is allocated on the heap and must be disposed of using
         *  delete[] when you are finished with it to avoid memory leaks.
         *
         *  @param sourceVec  an array of Vector3 objects to transform
         *  @param start the index of the first entry to transform
         *  @param len the number of squential entries, starting at start, to transform
         *  @returns a new array of size len containing the transformed values.
         */
        Vector3* TransformVecs(const Vector3 sourceVec[],const int start, const int len)const;
        
        /**
         * Transforms an array of vectors
         *
         *  This call is just like the previous call except, instead of allocating a new
         *  array to put the results in, it replaces the values in sourceVec with the results.
         *  Thus, it allocates no heap storage that you need to manage.
         *
         *  @param sourceVec  an array of Vector3 objects to transform, and to return the results in
         *  @param start the index of the first entry to transform
         *  @param len the number of squential entries, starting at start, to transform
         */
        void  TransformVecsInPlace(Vector3 sourceVec[],const int start, const int len)const;
        
        /**
         * Perform a matrix multiply with another Transform
         *
         * Multiplying two transform matrix concatenates their lists of translations and rotation.
         * it is a basic operation used in any scene graph or jointed object.
         *
         * @param other The transform to append to append its rotations and translations to this one
         * @param returns The result of appending the second parameter of a * expression to the first
         */
        Transform3D operator*(const Transform3D other)const;
        
        float* GetOGLData()const;
        
    };
    

    class G3DModel{
        public:
      
    };
    

    //foward decalre the GraphicsProvider3D class
    class GraphicsProvider3D;
    
    /**
     * This typdef defines a KeyCallback function pointer type that must be matched by the parameter
     * to a Setkeycallback call
     */
    typedef void (*KeyCallback)(const GraphicsProvider3D* cbContext,const int key);
    
    /**
     * This is the main class of the Graphics3D system.
     *
     * To draw, you use the static Graphics3DProvider::MakeNewProvider function call.
     * You then use the returned instance to create and displaye G2DImage objects
     *
     */
    class GraphicsProvider3D
    {
        
    public:
        /**
         * A pointer to user defined data
         *
         * When passing around structures it is often useful to higher level code to allow an arbitrary
         * block or struct of data to be associated with it.
         *
         * This is used by Scenegraph3D to associate a back-pointer to the scenegraph that created and
         * owns a pasticular GraphcisProvider3D
         */
        void* user_data_ptr;
        
        /**
         * This is the factory to produce a Graphics3D provider
         *
         *  what it actually returns is a full implemented sub-class.
         *  NOTE:  the returned object is a heap object and must be destroyed with delete when
         *  you are done with it to prevent memory leaks,
         *
         * @param windowName name of the window which this provider will draw in
         * @param width the width of the drawing space (window) in pixels
         * @param height the height of the drawing space (window) in pixels
         * @return An instance of a sub class of GraphicsProvider3D
         */
        static GraphicsProvider3D* MakeNewProvider(const std::string windowName,const int width, const int height);
        
                /**
         * This method must be called to start drawing a new video frame
         */
        virtual void BeginFrame()const=0;
        /**
         * Draws an image to the window
         *
         * This call draws an image to the provider window.  That image can be an intre G2Dimage as loaded or
         * a subsection of the image, as defined by the source rectangle.  The image position and rotation
         * are set by the passed Transform
         *
         * @param image the G2DImage object containing the image to be drawn
         * @param source the pixels to draw from the image where x and y are >=o and, width <= image->GetHeight()
         * and height <= image->GetHeight
         * @param transform  a tranformation matrix to apply to the image in order to position and rotate it.
         */
        virtual void DrawModel(const G3DModel* model, const Transform3D transform)const=0;
        /**
         * This method must be called after all images for a frame have been drawn in order to complete the
         * frame and swap it to the screen.
         *
         */
        virtual void EndFrame()const=0;
        
        /**
         * This is a virtual destructor that gets overriden by the private implementation to allwo it to
         * clean up its rosurces when destroyed
         */
        virtual ~GraphicsProvider3D(){
            //nop
        }
        
        /**
         * This function sets the callback method for this provider's window.  Any key events in this
         * provider's window will trigger this callback.  It may be left aloen or set to nullptr if you
         * do not wish to receieve events.
         *
         * @param keyCallback A function that macthes the KeyCallback type def and that will receieve
         * your key events.
         */
        virtual void SetKeyCallback(const KeyCallback keyCallback)=0;
        
        /**
         * This method actually handles key events.  You cna call it yourself to simulate a key event
         *
         * @param key the value of the key pressed in the current platform's representation (usually
         * either ascii or Unicode, which is a super-set of ASCII.)
         */
        virtual void DoKey(const int key)const=0;
        
        virtual G3DModel* MakeTexturedSphere(const float radius, const unsigned int rings,
                                             const unsigned int sectors,const std::string texturePath)const=0;

        
    };

    
}
#pragma GCC visibility pop

#endif /* defined(__Graphics3D__Graphics3D__) */
