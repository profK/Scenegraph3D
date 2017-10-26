/*
 *  Scenegraph3D.h
 *  Scenegraph3D
 *
 *  Created by Jeffrey Kesselman on 11/8/14.
 *  Copyright (c) 2014 Jeffrey Kesselman. All rights reserved.
 *
 */

#ifndef Scenegraph3D_
#define Scenegraph3D_
#include "Graphics3D.h"
#include <memory>
#include <string>
#include <list>

using namespace Graphics3D;

/* The classes below are exported */
#pragma GCC visibility push(default)

namespace Scenegraph3D {
    
    /**
     * This class defines a Sprite object
     *
     *  A sprite is a combination of a 2D image with
     *  a currrent rotation and translation
     *
     *  A handle or "pivot point" may be set which will be the
     *  origin for both translation and rotation
     */
    class Sprite3D {
        
        
    private:
        /**
         * The origin for rotation and translation, relative
         * to the bottom left corner of the image.
         */
        Vector3 handle;
        
        /**
         * The transaltion of the origin in world coordinates
         */
        Vector3 position;
        /**
         * The rotation of the image about the handle
         */
        Vector3 rotation;
        /**
         * A transform matrix used to hold and apply the combination
         * of the above 3 values
         */
        Transform3D transform;
        
        std::shared_ptr<G3DModel> modelPtr;
        /**
         * used internally to update the transform when position, rotation
         * or handle change
         */
        void RecalcTransform();
        
    public:
        /**
         * A default constructor that makes a sprite with unset fields.
         *
         * This exists primaruly for the use of the default copy constructor which
         * will set all the new Sprite's fields from the oen being copied.
         */
        Sprite3D();
        
        /**
         * This is the constructor typically used by code that wants to
         * create a sprite.
         *
         * @param image a G2Dimage to get the pixels from
         * @param imageSourceRect a rectangle of pixels to use from a potentially
         * larger G2DImage
         */
        Sprite3D(G3DModel* model);
        
        /**
         * Sets the image handle.
         *
         * The image handle (also called a 'pivot point') is theorigin for
         * rotation and translation. it is specified relative
         * to the bottom left corner of the image.
         */
        void SetHandle(const Vector3 relativePosition);
        /**
         * returns the current handle
         *
         * @see void SetHandle(const Vector3 relativePosition);
         *
         * @returns the current image handle in a new Vector3 object
         */
        Vector3 GetHandle()const;
        /**
         * Sets the image translation
         *
         * This is used to position the image.
         *
         * @param xlation the position of the image handle in the window
         * expressed as a pixel offset from the bottom left window corner
         */
        void SetTranslation(const Vector3 xlation);
        /**
         * returns the current translation
         *
         * @see void SetTranslation(const Vector3 xlation);         *
         * @returns the current image translation in a new Vector3 object
         */
        
        Vector3 GetTranslation()const;
        
        /**
         * Sets the current rotation about the handle
         *
         * @param radians current rotation in radians
         */
        void SetRotationInRadians(Vector3 radians);
        /**
         * returns the current rotation
         *
         * @see  void SetRotationInRadians(float radians);;
         *
         * @returns the current rotation about the handle in radians
         */
        
        Vector3 GetRotationInRadians()const;
        
        /**
         * Returns the current transform
         *
         * This method returns the current transform, which is defined
         * by the handle, translaton and rotation
         *
         * @returns the current transform
         */
        Transform3D GetTransform()const;
        
        /**
         * CURRENTLY UNMPLEMENTED
         * Will throw unimplemented exception if called.
         */
        void SetTransform(const Transform3D t);
        
        /**
         * Gets the size of the image
         *
         * The size of the image are the width andd height of the sourceRectangle
         * in a Vector3 where x==width and y==height.
         *
         * @returns a Vector3 where x==width and y==height.
         */
        Vector3 GetSize()const;
        
        /**
         * Draws an image to the window of the specified grpahics provider
         *
         * This method draws a sprite using its own transfrom as the only
         * transform for determining position and rotation
         *
         * @param provider a pointr to the GrpahicsProvider2D that owns
         * the destination window.
         */
        void Draw(const GraphicsProvider3D* provider)const;
        
        /**
         * Draw image with an overrided transform
         *
         * This is used by the scenegrah which treats a sprite's transform
         * as a local transform and concatenates that with its parent's global
         * transform to get a final transfrom for drawing
         *
         * @param provider The grphics provder whose draw space we are drawing in
         * @param transform he transform to apply to the image when drawn.
         */
        void Draw(const GraphicsProvider3D* provider,const Transform3D transform)const;
    };
    
    /**
     * Forward declaration of a ScenegraphNode
     *
     * Scenegraph nodes are combined to form a tree that describes
     * their relationships to each other.
     * This foward declataion is necessary so the definition of a
     * ScenegraphNode can have pointers to other ScenegraphNodes
     */
    class ScenegraphNode; // foward decl
    
    /**
     * This is for convenience and readbaility
     *
     * It defines a type called SharedNodePtr that is a handle
     * to a ScenegraphNode based on std::shared_ptr
     *
     * This is what the scenegrah uses internally and how
     * a client program should refer to ScenegraphNodes so
     * that refernces to them are refernce counted and
     * memory autoatically cleaned up when a node is no longer
     * referenced.
     */
    typedef std::shared_ptr<ScenegraphNode> SharedNodePtr;
    
    
    /***
     * The workhorse of the Scenegraph
     *
     * This class represents one node in the Scenegraph.  It handles
     * the relationship between it and other nodes and has the recursive draw
     * logic to concatenate transforms going down the draw tree.
     */
    class ScenegraphNode {
        
        
    private:
        /**
         * This contains a reference to the sprite to draw
         * It uses the Sprite's internal transform to define
         * the local trasfrom and concatenates it with the parent node's
         * world transform to define this node's world transform.
         */
        Sprite3D sprite;
        /**
         * This is a list of child nodes.  They are reference through
         * their handles so that, if this node is removed from the tree
         * all sub nodes with no other outstanding handle instacnes will be removed
         * as well.  (This is based on the fact that a std::list, when destroyed,
         * destroys all its current members.)
         */
        std::list<SharedNodePtr> children;
        /**
         * This is a back pointer back up the tree to the node's parent.
         * it is primarily used from removing a node from the tree.
         * Note that is is a C pointer and *not* a handle.  This is
         * to prevent a circular reference from pinning removed node
         * with children in memory.
         */
        ScenegraphNode* parent=nullptr;//does not pin parent
        
        /**
         * This is the constructor the static Scenegraphnode::Create
         * method uses to make nodes
         *
         * @param sprite  the sprite object that the created node will wrap
         *
         * NOTE: The sprite's transfrom gets deep copied at the time that
         * this method is invoked.  Chnages to the sprite's transform after
         * a node has been created will *not* chnage the local transform of
         * the node's sprite.
         */
        ScenegraphNode(Sprite3D sprite);
        
    public:
        /**
         * The factory method to create ScenegraphNodes
         *
         * This method takes a Sprite object to wrap the scenegraph around
         * and returns a reference counting handle to the ScneegraphNode.
         *
         * NOTE: The sprite's transfrom gets deep copied at the time that
         * this method is invoked.  Chnages to the sprite's transform after
         * a node has been created will *not* chnage the local transform of
         * the node's sprite.
         *
         * @param sprite The sprite 'template' to use to define this
         * scenegraph node.
         * @returns a handle that points to the created node
         */
        static SharedNodePtr Create(Sprite3D sprite);
        /**
         * Returns a reference to the sprite within this scenegraph node
         *
         * Note that, since a Sprite refernce is returned, call mthods on
         * the returned sprint *will* effect the scenegraph node.
         */
        Sprite3D& GetSprite();
        /***
         * This method adds a scenegraph node as a child node of this one
         *
         * Children get drawn in the transformed world coordinate system of their parent
         * A child may only have one parent.  Adding a child to a node that is already
         * a child of another node automatically removes it from the children of its old
         * parent and re-writes its parent pointer.
         *
         * @params node  the node to make a child of this one.
         */
        void AddChild(SharedNodePtr node);
        /**
         * Draws the node and all its chilsren
         *
         * This is a recursive draw call that draws this node in the transformed world space
         * passed in as parentTransform by concatenating that with its own local transform
         * to create this node's transformed world space.
         *
         * Its own transformed world space is then passed recursively to its children
         * as their parentTransform
         *
         * @param provider  the graohcis provider that owns the window to draw within
         * @param parentTransfrom the transformed world space to use as the context
         * for creating this node's transfromed world space.
         */
        void Draw(const GraphicsProvider3D* provider, Transform3D parentTransform)const;
        
        /**
         * Removs a child node from this node's children list
         *
         * @param childNode the handle of the child to remove from our children
         */
        void RemoveChild(const SharedNodePtr childNode);
    };
    
    /**
     * This is a forward declation which is needed by the type definition of
     * Scenegraph2DKeyCB
     */
    class Scenegraph; // forward declaration
    
    /**
     * This is a convenience typedef that defines the type of function pointer
     * used to receive key click callbacks.
     *
     * Any function pointers passed to Scenegraph::SetKeyCallback must match this
     * definition
     */
    typedef void (*Scenegraph3DKeyCB)(Scenegraph* scenegraphPointer, int key);
    
    /**
     * This is the base class of the Scenegraph2D system.  Create an instacne of this
     * class in order to use the Scenegraph to render to the screen.
     */
    class Scenegraph
    {
    private:
        /**
         * This contains a pointer to the underlying GraphicsProvider2D object that
         * actually owns the render window and does the drawing.
         * It is defined as a shared_ptr so that it will be automatically cleaned up
         * when all copies of the Scenegraph object that created it are destroyed.
         */
        std::shared_ptr<GraphicsProvider3D> providerPtr;
        
        
    public:
        /**
         * This is the constructor client programs use to make a
         * Scenegraph.
         *
         * @param name name of the window to create to draw into
         * @param windowWidth width of the window to create in pixels
         * @param windowHeight height of the window to create in pixels
         */
        Scenegraph(std::string name, int windowWidth,int windowHeight);
        
        /**
         * TODO
         **/
        Sprite3D MakeTexturedSphere(const float radius, const unsigned int rings,
                                    const unsigned int sectors,const std::string texturePath)const;
        /**
         * Sets the function to call in order to proccess key
         * events in the Scenegra[h's window.
         *
         * @param cbFunch is a function pointer to a callback function as defined
         * by Scenegraph3DKeyCb above or nullptr to disable key event handling
         */
        void SetKeyCallback(Scenegraph3DKeyCB cbFunc);
        /**
         *  Draws the current state of a ScengraphNode graph.
         *
         *  This function takes a root node of a tree of ScenegraphNodes
         *  and recursively descends it, drawing the current state
         *  of the tree to the current offscreen video buffer.
         *  It then swaps the buffer onto the screen.
         *
         * @param root  the root of the scenegraph node tree to draw
         */
        void RenderFrame(const SharedNodePtr root)const;
    };
}


#pragma GCC visibility pop
#endif
