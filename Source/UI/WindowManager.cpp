#include <Urho3D/IO/Log.h>
#include <Urho3D/Core/CoreEvents.h>
#include <Urho3D/Input/Input.h>
#include "WindowManager.h"
#include "../MyEvents.h"

#include "Settings/SettingsWindow.h"
#include "Scoreboard/ScoreboardWindow.h"
#include "Pause/PauseWindow.h"
#include "QuitConfirmation/QuitConfirmationWindow.h"
#include "NewGameSettings/NewGameSettingsWindow.h"
#include "Achievements/AchievementsWindow.h"
#include "PopupMessage/PopupMessageWindow.h"
#include "Settings/UIOption.h"

WindowManager::WindowManager(Context* context) :
    Object(context)
{
    SubscribeToEvents();
    RegisterAllFactories();
}

WindowManager::~WindowManager()
{
    _windowList.Clear();
}

void WindowManager::RegisterAllFactories()
{
    // Register all available windows
    context_->RegisterFactory<BaseWindow>();
    context_->RegisterFactory<ScoreboardWindow>();
    context_->RegisterFactory<AchievementsWindow>();
    context_->RegisterFactory<QuitConfirmationWindow>();
    context_->RegisterFactory<NewGameSettingsWindow>();
    context_->RegisterFactory<AchievementsWindow>();
    context_->RegisterFactory<PauseWindow>();
    context_->RegisterFactory<PopupMessageWindow>();
    context_->RegisterFactory<PopupMessageWindow>();

    // Custom UI elements
    UIOption::RegisterObject(context_);
    UIMultiOption::RegisterObject(context_);
    UIBoolOption::RegisterObject(context_);
    UISliderOption::RegisterObject(context_);
    context_->RegisterFactory<SettingsWindow>();
}

void WindowManager::SubscribeToEvents()
{
    SubscribeToEvent(MyEvents::E_OPEN_WINDOW, URHO3D_HANDLER(WindowManager, HandleOpenWindow));
    SubscribeToEvent(MyEvents::E_CLOSE_WINDOW, URHO3D_HANDLER(WindowManager, HandleCloseWindow));
    SubscribeToEvent(MyEvents::E_CLOSE_ALL_WINDOWS, URHO3D_HANDLER(WindowManager, HandleCloseAllWindows));
    SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(WindowManager, HandleUpdate));
}

void WindowManager::HandleOpenWindow(StringHash eventType, VariantMap& eventData)
{
    using namespace MyEvents::OpenWindow;
    String windowName = eventData["Name"].GetString();
    for (auto it = _windowList.Begin(); it != _windowList.End(); ++it) {
        if ((*it)->GetType() == StringHash(windowName)) {
            if (!(*it).Refs()) {
                _windowList.Erase(it);
            } else {
                URHO3D_LOGWARNING("Window '" + windowName + "' already opened!");
                BaseWindow* window = (*it)->Cast<BaseWindow>();
                //TODO bring this window to the front

                if (eventData.Contains(P_CLOSE_PREVIOUS)) {
                    if (eventData[P_CLOSE_PREVIOUS].GetBool()) {
                        CloseWindow(windowName);
                    } else {
                        return;
                    }
                } else {
                    return;
                }
            }
        }
    }

    URHO3D_LOGINFO("Opening window: " + windowName);
    SharedPtr<Object> newWindow;
    newWindow = context_->CreateObject(StringHash(windowName));
    if (newWindow) {
        BaseWindow *window = newWindow->Cast<BaseWindow>();
        window->SetData(eventData);
        window->Init();
        _windowList.Push(newWindow);

        _openedWindows.Push(windowName);
    } else {
        URHO3D_LOGERROR("Failed to open window: " + windowName);
    }
}

void WindowManager::HandleCloseWindow(StringHash eventType, VariantMap& eventData)
{
    String windowName = eventData["Name"].GetString();
    _closeQueue.Push(windowName);
}

void WindowManager::HandleCloseAllWindows(StringHash eventType, VariantMap& eventData)
{
    URHO3D_LOGINFO("Closing all windows");
    for (auto it = _openedWindows.Begin(); it != _openedWindows.End(); ++it) {
        _closeQueue.Push((*it));
    }

    _openedWindows.Clear();
}

bool WindowManager::IsWindowOpen(String windowName)
{
    for (auto it = _windowList.Begin(); it != _windowList.End(); ++it) {
        if ((*it)->GetType() == StringHash(windowName)) {
            BaseWindow* window = (*it)->Cast<BaseWindow>();
            if ((*it).Refs()) {
                return true;
            }
        }
    }

    return false;
}

void WindowManager::CloseWindow(String windowName)
{
    using namespace MyEvents::CloseWindow;
    URHO3D_LOGINFO("Closing window: " + windowName);
    for (auto it = _windowList.Begin(); it != _windowList.End(); ++it) {
        if ((*it)->GetType() == StringHash(windowName)) {

            _windowList.Erase(it);
            VariantMap data;
            data[P_NAME] = windowName;
            SendEvent(MyEvents::E_WINDOW_CLOSED, data);
            return;
        }
    }
}

void WindowManager::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
    if (!_closeQueue.Empty()) {
        for (auto it = _closeQueue.Begin(); it != _closeQueue.End(); ++it) {
            CloseWindow((*it));
        }
        _closeQueue.Clear();
    }
}
