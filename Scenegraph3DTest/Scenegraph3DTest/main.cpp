//
//  main.cpp
//  Scenegraph3DTest
//
//  Created by Jeffrey Kesselman on 11/8/14.
//  Copyright (c) 2014 Jeffrey Kesselman. All rights reserved.
//

#include <iostream>
#include "Graphics3D.h"
#include "Scenegraph3D.h"
#include <string>

using namespace Scenegraph3D;

static bool done=false;

static void GotKey(Scenegraph* scenegraph,int key){
    done=true;
}

int main(int argc, const char * argv[]) {
    Scenegraph* scenegraph = new Scenegraph("3D Scenegraph",800,600);
    scenegraph->SetKeyCallback(GotKey);
    Sprite3D mandrillSprite = scenegraph->MakeTexturedSphere(1,12,24,"mandrill.png");
    mandrillSprite.SetTranslation(Vector3(0,0,-6));
    SharedNodePtr mandrillNode = ScenegraphNode::Create(mandrillSprite);
    Sprite3D teapotSprite = scenegraph->MakeTexturedSphere(0.25, 12, 24, "teapot_256x192.png");
    teapotSprite.SetTranslation(Vector3(2,0,0));
    SharedNodePtr teapotNode = ScenegraphNode::Create(teapotSprite);
    mandrillNode->AddChild(teapotNode);
    float rot;
    while(!done){
        rot+=0.01;
        mandrillNode->GetSprite().SetRotationInRadians(Vector3(0,rot,0));
        teapotNode->GetSprite().SetRotationInRadians(Vector3(0,-rot*2,0));
        scenegraph->RenderFrame(mandrillNode);
    }
}
