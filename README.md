![Builds](https://github.com/ArnisLielturks/Urho3D-Project-Template/workflows/Builds/badge.svg)

# Urho3D-Project-Template
App template with the following features:
* Continuous integration with artifact publishing to itch.io (Android, OSX, Windows, Linux, WEB)
* Level management
* UI Window management
* LUA/AS mods with hot-reload
* Sound management
* Input mapping
* Splitscreen support
* Achievement logic
* Console command management
* INI configuration file loading/saving
* Settings window to configure all aspects of the engine
* UI Joystick controls

## How to build
Make sure that the URHO3D_HOME environment variable is set and points to the right directory. Build this the same way how you would build Urho3D engine itself

### Ubuntu:
```
git clone https://github.com/ArnisLielturks/Urho3D-Project-Template.git
cd Urho3D-Project-Template
./script/cmake_generic.sh build
cd  build
make
```


If everything worked, build/bin directory should contain `ProjectTemplate` executable.


### Few screenshots
![MainMenu](https://github.com/ArnisLielturks/Urho3D-Project-Template/blob/master/Screenshots/MainMenu.png)
![Settings](https://github.com/ArnisLielturks/Urho3D-Project-Template/blob/master/Screenshots/Settings.png)
![Achievements](https://github.com/ArnisLielturks/Urho3D-Project-Template/blob/master/Screenshots/Achievements.png)
![Credits](https://github.com/ArnisLielturks/Urho3D-Project-Template/blob/master/Screenshots/Credits.png)
![NewGame](https://github.com/ArnisLielturks/Urho3D-Project-Template/blob/master/Screenshots/NewGame.png)
![Loading](https://github.com/ArnisLielturks/Urho3D-Project-Template/blob/master/Screenshots/Loading.png)
![Level](https://github.com/ArnisLielturks/Urho3D-Project-Template/blob/master/Screenshots/Level.png)
![VoxelMap](https://github.com/ArnisLielturks/Urho3D-Project-Template/blob/master/Screenshots/VoxelMap.png)


# How to extend the functionality?
Read the wiki: https://github.com/ArnisLielturks/Urho3D-Project-Template/wiki

