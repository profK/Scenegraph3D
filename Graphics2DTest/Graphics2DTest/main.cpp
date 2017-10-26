//
//  main.cpp
//  Graphics2DTest
//
//  Created by Jeffrey Kesselman on 11/2/14.
//  Copyright (c) 2014 Jeffrey Kesselman. All rights reserved.
//

#include <iostream>
#include "Graphics2D.h"

static bool done=false;

static void GotKey(const Graphics2D::GraphicsProvider2D* provider, int key){
    done=true;
}

int main(int argc, const char * argv[]) {
    Graphics2D::GraphicsProvider2D* provider = Graphics2D::GraphicsProvider2D::MakeNewProvider("My Window",800,600);
    Graphics2D::G2DImage* image = provider->LoadImage("mandrill.png");
    Graphics2D::Transform2D transform = Graphics2D::Transform2D();
    
    provider->SetKeyCallback(GotKey);
    transform.Translate(Graphics2D::Vector2(-image->GetWidth()/2,-image->GetHeight()/2));
    transform.Translate(Graphics2D::Vector2(400,300));
    
    while(!done){
        transform.Translate(Graphics2D::Vector2(-400,-300));
        transform.Rotate(0.1);
        transform.Translate(Graphics2D::Vector2(400,300));
        provider->BeginFrame();
        provider->DrawImage(image,
                            Graphics2D::Rectangle(0,0,image->GetWidth(),image->GetHeight()),
                            transform);
        provider->EndFrame();
    }
    
}
