#include <Urho3D/Scene/ObjectAnimation.h>
#include <Urho3D/Scene/ValueAnimation.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/UI/UI.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/UI/Font.h>
#include <Urho3D/IO/Log.h>
#include "Notifications.h"
#include "../Global.h"

static const int NOTIFICATION_OVERLAP_TIME = 500;

Notifications::Notifications(Context* context) :
    Object(context)
{
    Init();
}

Notifications::~Notifications()
{
}

void Notifications::Init()
{
    SubscribeToEvents();

    // Create light animation
    notificationAnimation = new ObjectAnimation(context_);

    // Create light position animation
    positionAnimation = new ValueAnimation(context_);
    // Use spline interpolation method
    positionAnimation->SetInterpolationMethod(IM_SPLINE);
    // Set spline tension
    positionAnimation->SetSplineTension(0.7f);
    positionAnimation->SetKeyFrame(0.0f, IntVector2(-10, -300));
    positionAnimation->SetKeyFrame(7.0f, IntVector2(-10, -500));
    notificationAnimation->AddAttributeAnimation("Position", positionAnimation);

    opacityAnimation = new ValueAnimation(context_);
    opacityAnimation->SetKeyFrame(0.0f, 0.0f);
    opacityAnimation->SetKeyFrame(1.0f, 1.0f);
    opacityAnimation->SetKeyFrame(4.0f, 1.0f);
    opacityAnimation->SetKeyFrame(5.0f, 0.0f);
    opacityAnimation->SetKeyFrame(10.0f, 0.0f);
    notificationAnimation->AddAttributeAnimation("Opacity", opacityAnimation);
}

void Notifications::SubscribeToEvents()
{
    SubscribeToEvent("ShowNotification", URHO3D_HANDLER(Notifications, HandleNewNotification));
    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(Notifications, HandleUpdate));
}

void Notifications::HandleNewNotification(StringHash eventType, VariantMap& eventData)
{
    String message = eventData["Message"].GetString();
    String status = eventData["Status"].GetString();
    Color color(0.4f, 1.0f, 0.4f);

    if (status == "Error") {
        color = Color(1.0f, 0.4f, 0.4f);
    } else if (status == "Warning") {
        color = Color(1.0f, 1.0f, 0.4f);
    }
    NotificationData data;
    data.message = message;
    data.color = color;

    if (_timer.GetMSec(false) < NOTIFICATION_OVERLAP_TIME) {
        _messageQueue.Push(data);
        URHO3D_LOGINFO("Too many notification request, pushing notification on queue");
        return;
    }
    CreateNewNotification(data);
}

void Notifications::CreateNewNotification(NotificationData data)
{
    float fontSize = 16.0f;
    auto* cache = GetSubsystem<ResourceCache>();

    SharedPtr<BorderImage> messageContainer(GetSubsystem<UI>()->GetRoot()->CreateChild<BorderImage>());
    // Notification must appear on top of everything
    messageContainer->SetPriority(99999);
    messageContainer->SetTexture(cache->GetResource<Texture2D>("Textures/Gray.png"));
    messageContainer->SetObjectAnimation(notificationAnimation);
    messageContainer->SetLayout(LM_HORIZONTAL, 0, IntRect(8, 8, 8, 8));
    // Align Text center-screen
    messageContainer->SetHorizontalAlignment(HA_RIGHT);
    messageContainer->SetVerticalAlignment(VA_BOTTOM);
    messageContainer->SetVar("Lifetime", 6.0f);

    // Construct new Text object
    SharedPtr<Text> messageElement(messageContainer->CreateChild<Text>());
    // Set String to display
    messageElement->SetText(data.message);
    messageElement->SetTextEffect(TextEffect::TE_STROKE);
    messageElement->SetStyleAuto();
    messageElement->SetUseDerivedOpacity(true);

    auto *font = cache->GetResource<Font>(APPLICATION_FONT);
    messageElement->SetColor(data.color);
    messageElement->SetFont(font, fontSize);

    _messages.Push(messageContainer);

    _timer.Reset();
}

void Notifications::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
    using namespace Update;

    float timeStep = eventData[P_TIMESTEP].GetFloat();
    for (auto it = _messages.Begin(); it != _messages.End(); ++it) {
        if (!(*it)) {
            _messages.Erase(it);
            return;
        }
        float lifetime = (*it)->GetVar("Lifetime").GetFloat();
        if (lifetime <= 0) {
            (*it)->Remove();
            _messages.Erase(it);
            return; 
        }
        lifetime -= timeStep;
        (*it)->SetVar("Lifetime", lifetime);
    }

    if (_timer.GetMSec(false) > 1000 && !_messageQueue.Empty()) {
        CreateNewNotification(_messageQueue.Front());
        _messageQueue.PopFront();
    }
}

void Notifications::HandleGameEnd(StringHash eventType, VariantMap& eventData)
{
    _messages.Clear();
}
