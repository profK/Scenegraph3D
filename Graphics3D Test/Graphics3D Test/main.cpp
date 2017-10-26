//
//  main.cpp
//  Graphics3D Test
//
//  Created by Jeffrey Kesselman on 11/7/14.
//  Copyright (c) 2014 Jeffrey Kesselman. All rights reserved.
//

#include <iostream>
#include "Graphics3D.h"

using  namespace Graphics3D;

static bool done=false;

void GotKey(const GraphicsProvider3D* provider, const int key){
    done=true;
}

int main(int argc, const char * argv[]) {
    GraphicsProvider3D* provider = GraphicsProvider3D::MakeNewProvider("3D window",800,600);
    G3DModel* sphere = provider->MakeTexturedSphere(1,12,24,"mandrill.png");
    Transform3D t;
    t.Translate(Vector3(0,0,-6));
    provider->SetKeyCallback(GotKey);
    while(!done){
        provider->BeginFrame();
        provider->DrawModel(sphere,t);
        t.Translate(Vector3(0,0,6));
        t.Rotate(Vector3(0,0.01,0));
        t.Translate(Vector3(0,0,-6));
        provider->EndFrame();
    }
}
