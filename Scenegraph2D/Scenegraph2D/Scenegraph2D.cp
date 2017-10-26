#include "Graphics2D.h"
#include "Scenegraph2D.h"
#include <string>
#include <stdexcept>


using namespace Graphics2D;
using namespace Scenegraph2D;

/*** Sprite Implementation ***/

Sprite::Sprite(){
    // nop
}

Sprite::Sprite(G2DImage* image,Rectangle imageSourceRect){
    imagePtr.reset(image);
    sourceRect = imageSourceRect;
}

void Sprite::SetHandle(Vector2 relativePosition){
    handle = relativePosition;
    RecalcTransform();
}

Vector2 Sprite::GetHandle()const{
    return handle;
}

void Sprite::SetTranslation(Vector2 xlation){
    position=xlation;
    RecalcTransform();
}

Vector2 Sprite::GetTranslation()const{
    return position;
}

void Sprite::SetRotationInRadians(float radians){
    rotation=radians;
    RecalcTransform();
}

float Sprite::GetRotationInRadians()const{
    return rotation;
}

Transform2D Sprite::GetTransform()const{
    return transform;
}

void Sprite::SetTransform(Transform2D t){
    //TOOD: Note, needs matrix decomposition from Graphics2D
    throw std::runtime_error("SetTransform not yet defined");
}

void Sprite::RecalcTransform(){
    transform = Transform2D(); // start from identity
    transform.Translate(handle * -1);
    transform.Rotate(rotation);
    transform.Translate(position);
}

void Sprite::Draw(const GraphicsProvider2D* provider)const {
    provider->DrawImage(imagePtr.get(),sourceRect, transform);
}

void Sprite::Draw(const GraphicsProvider2D* provider,Transform2D transform)const{
    //Note that the local transform overrides the sprite's field
    provider->DrawImage(imagePtr.get(),sourceRect,transform);
}

Vector2 Sprite::GetSize()const{
    return Vector2(sourceRect.width,sourceRect.height);
}

/*** Scenegraph Node Implementation ***/

ScenegraphNode::ScenegraphNode(Sprite sp){
    sprite = sp;
}

SharedNodePtr ScenegraphNode::Create(Sprite sprite){
    return SharedNodePtr(new ScenegraphNode(sprite));
}

Sprite& ScenegraphNode::GetSprite(){
    return sprite;
}

void ScenegraphNode::AddChild(SharedNodePtr node){
    if (node->parent!=nullptr){
        node->parent->RemoveChild(node);
    }
    children.push_back(node);
    node->parent = this; // doesnt pin to avoid circular references
}

void ScenegraphNode::Draw(const GraphicsProvider2D* provider, Transform2D parentTransform)const{
    Transform2D worldXform = parentTransform*sprite.GetTransform();
    sprite.Draw(provider, worldXform);
    for(auto i : children){
        i->Draw(provider, worldXform);
    }
}

void ScenegraphNode::RemoveChild(const SharedNodePtr childNode){
    children.remove(childNode);
    childNode->parent = nullptr;
}

//*** Scenegraph Implementation

static Scenegraph2DKeyCB OnKey = nullptr;

static void GraphicsProvider2DKeyCB(const GraphicsProvider2D* provider, int key){
    if (OnKey!=nullptr){
        OnKey((Scenegraph *)provider->user_data_ptr,key);
    }
}

Scenegraph::Scenegraph(std::string windowName, int windowWidth ,int windowHeight){
    providerPtr.reset(GraphicsProvider2D::MakeNewProvider(windowName,windowWidth,windowHeight));
    providerPtr->user_data_ptr=this;
    providerPtr->SetKeyCallback(GraphicsProvider2DKeyCB);
}

void Scenegraph::SetKeyCallback(Scenegraph2DKeyCB cbFunc){
    OnKey=cbFunc;
}

Sprite Scenegraph::LoadSprite(std::string sprite)const{
    G2DImage* image = providerPtr->LoadImage(sprite);
    return Sprite(image,Rectangle(0,0,image->GetWidth(),image->GetHeight()));
}

void Scenegraph::RenderFrame(SharedNodePtr root)const {
    providerPtr->BeginFrame();
    root->Draw(providerPtr.get(), Transform2D());
    providerPtr->EndFrame();
}