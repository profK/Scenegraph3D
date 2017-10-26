#include "Graphics3D.h"
#include "Scenegraph3D.h"
#include <string>
#include <stdexcept>


using namespace Scenegraph3D;

/*** Sprite Implementation ***/

Sprite3D::Sprite3D(){
    // nop
}

Sprite3D::Sprite3D(G3DModel* model){
    modelPtr.reset(model);
}

void Sprite3D::SetHandle(Vector3 relativePosition){
    handle = relativePosition;
    RecalcTransform();
}

Vector3 Sprite3D::GetHandle()const{
    return handle;
}

void Sprite3D::SetTranslation(Vector3 xlation){
    position=xlation;
    RecalcTransform();
}

Vector3 Sprite3D::GetTranslation()const{
    return position;
}

void Sprite3D::SetRotationInRadians(Vector3 radians){
    rotation=radians;
    RecalcTransform();
}

Vector3 Sprite3D::GetRotationInRadians()const{
    return rotation;
}

Transform3D Sprite3D::GetTransform()const{
    return transform;
}

void Sprite3D::SetTransform(Transform3D t){
    //TOOD: Note, needs matrix decomposition from Graphics2D
    throw std::runtime_error("SetTransform not yet defined");
}

void Sprite3D::RecalcTransform(){
    transform = Transform3D(); // start from identity
    transform.Translate(handle * -1);
    transform.Rotate(rotation);
    transform.Translate(position);
}

void Sprite3D::Draw(const GraphicsProvider3D* provider)const {
    provider->DrawModel(modelPtr.get(),transform);
}

void Sprite3D::Draw(const GraphicsProvider3D* provider,Transform3D transform)const{
    //Note that the local transform overrides the sprite's field
    provider->DrawModel(modelPtr.get(), transform);
}

Vector3 Sprite3D::GetSize()const{
    throw std::runtime_error("Sprite3D::GetSize is currently unimplemented ");
}

/*** Scenegraph Node Implementation ***/

ScenegraphNode::ScenegraphNode(Sprite3D sp){
    sprite = sp;
}

SharedNodePtr ScenegraphNode::Create(Sprite3D sprite){
    return SharedNodePtr(new ScenegraphNode(sprite));
}

Sprite3D& ScenegraphNode::GetSprite(){
    return sprite;
}

void ScenegraphNode::AddChild(SharedNodePtr node){
    if (node->parent!=nullptr){
        node->parent->RemoveChild(node);
    }
    children.push_back(node);
    node->parent = this; // doesnt pin to avoid circular references
}

void ScenegraphNode::Draw(const GraphicsProvider3D* provider, Transform3D parentTransform)const{
    Transform3D worldXform = parentTransform*sprite.GetTransform();
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

static Scenegraph3DKeyCB OnKey = nullptr;

static void GraphicsProvider3DKeyCB(const GraphicsProvider3D* provider, int key){
    if (OnKey!=nullptr){
        OnKey((Scenegraph *)provider->user_data_ptr,key);
    }
}

Scenegraph::Scenegraph(std::string windowName, int windowWidth ,int windowHeight){
    providerPtr.reset(GraphicsProvider3D::MakeNewProvider(windowName,windowWidth,windowHeight));
    providerPtr->user_data_ptr=this;
    providerPtr->SetKeyCallback(GraphicsProvider3DKeyCB);
}

void Scenegraph::SetKeyCallback(Scenegraph3DKeyCB cbFunc){
    OnKey=cbFunc;
}

Sprite3D Scenegraph::MakeTexturedSphere(const float radius, const unsigned int rings,
                                        const unsigned int sectors,const std::string texturePath)const{
    G3DModel* model = providerPtr->MakeTexturedSphere(radius, rings, sectors, texturePath);
    return Sprite3D(model);
}

void Scenegraph::RenderFrame(SharedNodePtr root)const {
    providerPtr->BeginFrame();
    root->Draw(providerPtr.get(), Transform3D());
    providerPtr->EndFrame();
}