#include <Urho3D/Urho3DAll.h>
#include "Level.h"
#include "../MyEvents.h"

using namespace Levels;

    /// Construct.
Level::Level(Context* context) :
    BaseLevel(context),
    shouldReturn(false),
    _showScoreboard(false)
{
}

Level::~Level()
{
}

void Level::Init()
{
    Renderer* renderer = GetSubsystem<Renderer>();
    renderer->SetNumViewports(1);

    Network* network = GetSubsystem<Network>();
    network->RegisterRemoteEvent("SendPlayerNodeID");

    URHO3D_LOGRAW("Starting level: Level");
    BaseLevel::Init();

    // Create the scene content
    CreateScene();

    // Create the UI content
    CreateUI();

    // Subscribe to global events for camera movement
    SubscribeToEvents();

    Input* input = GetSubsystem<Input>();
    input->SetMouseVisible(false);
}



void Level::CreateScene()
{
    scene_ = new Scene(context_);
    scene_->CreateComponent<Octree>(LOCAL);
    scene_->CreateComponent<PhysicsWorld>(LOCAL);
    scene_->CreateComponent<DebugRenderer>(LOCAL);
    File loadFile(context_, GetSubsystem<FileSystem>()->GetProgramDir() + "Data/Scenes/Scene.xml", FILE_READ);
    scene_->LoadXML(loadFile);

    CreateCamera();
}

void Level::CreateUI()
{
    UI* ui = GetSubsystem<UI>();
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    Text* text = ui->GetRoot()->CreateChild<Text>();
    text->SetHorizontalAlignment(HA_CENTER);
    text->SetVerticalAlignment(VA_CENTER);
    text->SetStyleAuto();
    text->SetText("This is ingame text!\nPress ESC to exit game\nPress TAB to show ScoreboardWindow\nPress F1 to show/hide console");
    text->SetTextEffect(TextEffect::TE_STROKE);
    text->SetFontSize(16);
    text->SetColor(Color(0.8f, 0.8f, 0.2f));
}

void Level::SubscribeToEvents()
{
    SubscribeToEvent(E_POSTUPDATE, URHO3D_HANDLER(Level, HandlePostUpdate));
    SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(Level, HandleKeyDown));
    SubscribeToEvent(E_KEYUP, URHO3D_HANDLER(Level, HandleKeyUp));
}

void Level::HandlePostUpdate(StringHash eventType, VariantMap& eventData)
{
    float timeStep = eventData["TimeStep"].GetFloat();
    cameraNode_->Yaw(timeStep * 50);
    Input* input = GetSubsystem<Input>();
    if (input->IsMouseVisible()) {
        input->SetMouseVisible(false);
    }
    if (_controlledNode) {
        Vector3 position = _controlledNode->GetWorldPosition();
        position.y_ += 3.5;
        cameraNode_->SetPosition(position);
    }
    if (input->GetKeyDown(KEY_ESCAPE)) {
        VariantMap eventData;
        eventData["Name"] = "ExitGame";
        eventData["Message"] = "";
        SendEvent(MyEvents::E_SET_LEVEL, eventData);
        UnsubscribeFromEvent(E_POSTUPDATE);
    }
}

void Level::OnLoaded()
{
    if (shouldReturn) {
        VariantMap eventData;
        eventData["Name"] = "MainMenu";
        eventData["Message"] = returnMessage;
        SendEvent(MyEvents::E_SET_LEVEL, eventData);
    }
}

void Level::HandleKeyDown(StringHash eventType, VariantMap& eventData)
{
    int key = eventData["Key"].GetInt();

    // Toggle console by pressing F1
    if (key == KEY_TAB && !_showScoreboard) {
        VariantMap data;
        data["Name"] = "ScoreboardWindow";
        SendEvent(MyEvents::E_OPEN_WINDOW, data);
        _showScoreboard = true;
    }
}

void Level::HandleKeyUp(StringHash eventType, VariantMap& eventData)
{
    int key = eventData["Key"].GetInt();

    // Toggle console by pressing F1
    if (key == KEY_TAB && _showScoreboard) {
        VariantMap data;
        data["Name"] = "ScoreboardWindow";
        SendEvent(MyEvents::E_CLOSE_WINDOW, data);
        _showScoreboard = false;
    }
}