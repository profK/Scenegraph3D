//
//  main.cpp
//  Scenegraph2D Test
//
//  Created by Jeffrey Kesselman on 11/5/14.
//  Copyright (c) 2014 Jeffrey Kesselman. All rights reserved.
//

#include <iostream>
#include "Graphics2D.h"
#include "Scenegraph2D.h"


using namespace Scenegraph2D;


bool done = false;
void GotKey(Scenegraph2D::Scenegraph* scenegraph,int key){
    done=true;
}



int main(int argc, const char * argv[]) {
    Scenegraph scenegraph = Scenegraph("Scenegraph Test",800,600);
    Sprite mandrillSprite = scenegraph.LoadSprite("mandrill.png");
    Sprite teapot = scenegraph.LoadSprite("teapot_256x192.png");
    
    mandrillSprite.SetHandle(mandrillSprite.GetSize()/2);
    mandrillSprite.SetTranslation(Vector2(400,300));
    
    SharedNodePtr root = ScenegraphNode::Create(mandrillSprite);
    
    
    teapot.SetHandle(teapot.GetSize()/2);
    teapot.SetTranslation(mandrillSprite.GetSize());

    SharedNodePtr teapotNode = ScenegraphNode::Create(teapot);
    root->AddChild(teapotNode);

    scenegraph.SetKeyCallback(GotKey);
    float rot=0;
    while(!done){
        rot += 0.1;
        root->GetSprite().SetRotationInRadians(rot);
        teapotNode->GetSprite().SetRotationInRadians(-rot);
        scenegraph.RenderFrame(root);
    }
}
