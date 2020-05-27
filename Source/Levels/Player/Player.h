#pragma once

#include <Urho3D/Core/Object.h>
#include <Urho3D/Scene/Scene.h>
#include <Urho3D/Physics/RigidBody.h>
#include <Urho3D/Input/Controls.h>
#include <Urho3D/Graphics/Terrain.h>
#include <Urho3D/Network/Connection.h>

using namespace Urho3D;

class Player : public Object
{
    URHO3D_OBJECT(Player, Object);

public:
    explicit Player(Context* context);
    ~Player();

    static void RegisterObject(Context* context);

    /**
     * Create player controlled node
     */
    void CreateNode(Scene* scene, int controllerId, Terrain* terrain);

    void FindNode(Scene* scene, int id);

    /**
     * Set controller ID to know which controller is controlling this player
     */
    void SetControllerId(unsigned int id);

    int GetControllerId() { return _controllerId; }

    void SetName(const String& name);

    /**
     * Get created player node
     */
    Node* GetNode();

    /**
     * Set if this instance can be controleld with the user input
     */
    void SetControllable(bool value);

    void SetClientConnection(Connection* connection);

    void SetCameraTarget(Node* target);
    Node* GetCameraTarget();

    void SetCameraDistance(float distance);
    float GetCameraDistance();

private:

    bool IsCameraTargetSet();
    void HandlePhysicsPrestep(StringHash eventType, VariantMap& eventData);
    void RegisterConsoleCommands();

    /**
     * Detect when player is on the ground or not
     */
    void HandleNodeCollision(StringHash eventType, VariantMap& eventData);

    void HandlePredictPlayerPosition(StringHash eventType, VariantMap& eventData);

    RigidBody* _rigidBody;
    SharedPtr<Node> _node;

    WeakPtr<Node> _cameraTarget;
    float _cameraDistance{1.5f};

    void ResetPosition();
    
    /**
     * Controller ID
     */
    int _controllerId;

    /**
     * Player controlers
     */
    Controls _controls;

    /**
     * Is the player on the ground
     */
    bool _onGround;

    Terrain* _terrain{nullptr};

    Connection* _connection{nullptr};

    bool _isControlled{false};
};
