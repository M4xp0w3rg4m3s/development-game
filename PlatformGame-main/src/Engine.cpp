#include "Engine.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <unordered_set>
#include "Log.h"

#include "Window.h"
#include "Input.h"
#include "Render.h"
#include "Textures.h"
#include "Audio.h"
#include "Scene.h"
#include "SceneIntro.h"
#include "SceneTitle.h"
#include "SceneSettings.h"
#include "ScenePause.h"
#include "EntityManager.h"
#include "Map.h"
#include "Physics.h"
#include "Parallax.h"
#include "ParallaxTwoPoints.h"

#include "GuiManager.h"
#include "GameHUD.h"

// Constructor
Engine::Engine() {

	LOG("Constructor Engine::Engine");

    //Measure the amount of ms that takes to execute the App constructor and LOG the result
    Timer timer = Timer();
    startupTime = Timer();
    frameTime = PerfTimer();
    lastSecFrameTime = PerfTimer();
    frames = 0;

    // Modules
    window = std::make_shared<Window>();
    input = std::make_shared<Input>();
    render = std::make_shared<Render>();
    textures = std::make_shared<Textures>();
    audio = std::make_shared<Audio>();
    physics = std::make_shared<Physics>();
    scene = std::make_shared<Scene>();
    sceneIntro = std::make_shared<SceneIntro>();
    sceneTitle = std::make_shared<SceneTitle>();
    sceneSettings = std::make_shared<SceneSettings>();
    scenePause = std::make_shared<ScenePause>();
    map = std::make_shared<Map>();
    parallax = std::make_shared<Parallax>();
    parallaxTwoPoints = std::make_shared<ParallaxTwoPoints>();
    entityManager = std::make_shared<EntityManager>();
    guiManager = std::make_shared<GuiManager>();
    gameHud = std::make_shared<GameHUD>();

    // Ordered for awake / Start / Update
    // Reverse order of CleanUp
    AddModule(std::static_pointer_cast<Module>(window), LoopState::CLEAN_ONCE);
    AddModule(std::static_pointer_cast<Module>(input), LoopState::CLEAN_ONCE);
    AddModule(std::static_pointer_cast<Module>(textures), LoopState::CLEAN_ONCE);
    AddModule(std::static_pointer_cast<Module>(audio), LoopState::CLEAN_ONCE);

    AddModule(std::static_pointer_cast<Module>(window), LoopState::INTRO);
    AddModule(std::static_pointer_cast<Module>(input), LoopState::INTRO);
    AddModule(std::static_pointer_cast<Module>(textures), LoopState::INTRO);
    AddModule(std::static_pointer_cast<Module>(audio), LoopState::INTRO);

    AddModule(std::static_pointer_cast<Module>(window), LoopState::TITLE);
    AddModule(std::static_pointer_cast<Module>(input), LoopState::TITLE);
    AddModule(std::static_pointer_cast<Module>(textures), LoopState::TITLE);
    AddModule(std::static_pointer_cast<Module>(audio), LoopState::TITLE);

    AddModule(std::static_pointer_cast<Module>(window), LoopState::SETTINGS);
    AddModule(std::static_pointer_cast<Module>(input), LoopState::SETTINGS);
    AddModule(std::static_pointer_cast<Module>(textures), LoopState::SETTINGS);
    AddModule(std::static_pointer_cast<Module>(audio), LoopState::SETTINGS);

    AddModule(std::static_pointer_cast<Module>(window), LoopState::PAUSE);
    AddModule(std::static_pointer_cast<Module>(input), LoopState::PAUSE);
    AddModule(std::static_pointer_cast<Module>(textures), LoopState::PAUSE);
    AddModule(std::static_pointer_cast<Module>(audio), LoopState::PAUSE);

    AddModule(std::static_pointer_cast<Module>(window), LoopState::GAME);
    AddModule(std::static_pointer_cast<Module>(input), LoopState::GAME);
    AddModule(std::static_pointer_cast<Module>(textures), LoopState::GAME);
    AddModule(std::static_pointer_cast<Module>(audio), LoopState::GAME);


    // INTRO
    AddModule(std::static_pointer_cast<Module>(sceneIntro), LoopState::INTRO);
    AddModule(std::static_pointer_cast<Module>(parallax), LoopState::INTRO);

    // TITLE
    AddModule(std::static_pointer_cast<Module>(sceneTitle), LoopState::TITLE);
    AddModule(std::static_pointer_cast<Module>(guiManager), LoopState::TITLE);

    // SETTINGS
    AddModule(std::static_pointer_cast<Module>(sceneSettings), LoopState::SETTINGS);
    AddModule(std::static_pointer_cast<Module>(guiManager), LoopState::SETTINGS);

    // PAUSE
    AddModule(std::static_pointer_cast<Module>(scenePause), LoopState::PAUSE);
    AddModule(std::static_pointer_cast<Module>(guiManager), LoopState::PAUSE);

    // GAME
    AddModule(std::static_pointer_cast<Module>(physics), LoopState::GAME);
    AddModule(std::static_pointer_cast<Module>(parallax), LoopState::GAME);
    AddModule(std::static_pointer_cast<Module>(parallaxTwoPoints), LoopState::GAME);
    AddModule(std::static_pointer_cast<Module>(scene), LoopState::GAME);

    // Add the map module
    AddModule(std::static_pointer_cast<Module>(map), LoopState::GAME);

    // Add the entity manager
    AddModule(std::static_pointer_cast<Module>(entityManager),LoopState::GAME);
    AddModule(std::static_pointer_cast<Module>(guiManager),LoopState::GAME);
    AddModule(std::static_pointer_cast<Module>(gameHud), LoopState::GAME);

    // Render last 
    AddModule(std::static_pointer_cast<Module>(render), LoopState::CLEAN_ONCE);
    AddModule(std::static_pointer_cast<Module>(render), LoopState::INTRO);
    AddModule(std::static_pointer_cast<Module>(render), LoopState::TITLE);
    AddModule(std::static_pointer_cast<Module>(render), LoopState::SETTINGS);
    AddModule(std::static_pointer_cast<Module>(render), LoopState::PAUSE);
    AddModule(std::static_pointer_cast<Module>(render), LoopState::GAME);

    LOG("Timer App Constructor: %f", timer.ReadMSec());
}

// Static method to get the instance of the Engine class, following the singletn pattern
Engine& Engine::GetInstance() {
    static Engine instance; // Guaranteed to be destroyed and instantiated on first use
    return instance;
}

void Engine::AddModule(std::shared_ptr<Module> module, LoopState state){
    switch (state)
    {
    case LoopState::INTRO:
        module->Init();
        moduleListIntro.push_back(module);
        break;
    case LoopState::TITLE:
        module->Init();
        moduleListTitle.push_back(module);
        break;
    case LoopState::SETTINGS:
        module->Init();
        moduleListSettings.push_back(module);
        break;
    case LoopState::PAUSE:
        module->Init();
        moduleListPause.push_back(module);
        break;
    case LoopState::GAME:
        module->Init();
        moduleListGame.push_back(module);
        break;
    case LoopState::CLEAN_ONCE:
        module->Init();
        moduleListCleanOnce.push_back(module);
        break;
    default:
        break;
    }
    
}

// Called before render is available
bool Engine::Awake() {

    //Measure the amount of ms that takes to execute the Awake and LOG the result
    Timer timer = Timer();

    LOG("Engine::Awake");

    bool result = LoadConfig();

    if (result == true)
    {
        // Read the title from the config file and set the windows title 
        // replace the inital string from the value of the title in the config file
        // also read maxFrameDuration 
        gameTitle = configFile.child("config").child("app").child("title").child_value();
        window->SetTitle(gameTitle.c_str());
        maxFrameDuration = configFile.child("config").child("app").child("maxFrameDuration").attribute("value").as_int();
        vsync = configFile.child("config").child("render").child("vsync").attribute("value").as_bool();

        //Iterates the module list and calls Awake on each module
        AwakeCurrentLoopState();
    }

    LOG("Timer App Awake(): %f", timer.ReadMSec());

    return result;
}

// Called before the first frame
bool Engine::Start() {

    //Measure the amount of ms that takes to execute the App Start() and LOG the result
    Timer timer = Timer();

    LOG("Engine::Start");

    //Iterates the module list and calls Start on each module

    bool result = true;

    result = StartCurrentLoopState();

    LOG("Timer App Start(): %f", timer.ReadMSec());

    return result;
}

// Called each loop iteration
bool Engine::Update() {

    bool ret = true;
    PrepareUpdate();

    if (input->GetWindowEvent(WE_QUIT) == true)
        ret = false;

    if (ret == true)
        ret = PreUpdate();

    if (input.get()->GetKey(SDL_SCANCODE_F11) == KEY_DOWN) {
        if (maxFrameDuration == 16) maxFrameDuration = 32;
        else maxFrameDuration = 16;
    }

    if (input.get()->GetKey(SDL_SCANCODE_F) == KEY_DOWN) {
        Uint32 flags = SDL_GetWindowFlags(window->window);

        if (flags & SDL_WINDOW_FULLSCREEN_DESKTOP) {
            SDL_SetWindowFullscreen(window->window, 0);
        }
        else {
            SDL_SetWindowFullscreen(window->window, SDL_WINDOW_FULLSCREEN_DESKTOP);
        }
    }

    if (ret == true) {
        ret = DoUpdate();
    }

    if (ret == true)
        ret = PostUpdate();

    FinishUpdate();
    return ret;
}

// Called before quitting
bool Engine::CleanUp() {

    //Measure the amount of ms that takes to execute the App CleanUp() and LOG the result
    Timer timer = Timer();

    LOG("Engine::CleanUp");

    //Iterates the module list and calls CleanUp on each module
    bool result = true;
    std::unordered_set<Module*> cleanedModules;

    auto cleanModuleList = [&cleanedModules, &result](std::list<std::shared_ptr<Module>>& moduleList) {
        for (const auto& module : moduleList) {
            if (cleanedModules.find(module.get()) == cleanedModules.end()) {
                result = module->CleanUp();
                if (!result) {
                    break;
                }
                cleanedModules.insert(module.get());
            }
        }
    };

    cleanModuleList(moduleListCleanOnce);
    if (IntroStarted) if (result) cleanModuleList(moduleListIntro);
    if (TitleStarted) if (result) cleanModuleList(moduleListTitle);
    if (SettingsStarted) if (result) cleanModuleList(moduleListSettings);
    if (PauseStarted) if (result) cleanModuleList(moduleListPause);
    if (GameStarted) if (result) cleanModuleList(moduleListGame);
    LOG("Timer App CleanUp(): %f", timer.ReadMSec());

    return result;
}

// ---------------------------------------------
void Engine::PrepareUpdate()
{
    frameTime.Start();
}

// ---------------------------------------------
void Engine::FinishUpdate()
{
    //Cap the framerate of the gameloop
    double currentDt = frameTime.ReadMs();
    if (maxFrameDuration > 0 && currentDt < maxFrameDuration) {
        int delay = (int)(maxFrameDuration - currentDt);

        PerfTimer delayTimer = PerfTimer();
        SDL_Delay(delay);
        //Measure accurately the amount of time SDL_Delay() actually waits compared to what was expected
        //LOG("We waited for %I32u ms and got back in %f ms",delay,delayTimer.ReadMs());
    }

    // Amount of frames since startup
    frameCount++;

    // Amount of time since game start (use a low resolution timer)
    secondsSinceStartup = startupTime.ReadSec();

    // Amount of ms took the last update (dt)
    dt = (float)frameTime.ReadMs();

    // Amount of frames during the last second
    lastSecFrameCount++;

    // Average FPS for the whole game life
    if (lastSecFrameTime.ReadMs() > 1000) {
        lastSecFrameTime.Start();
        averageFps = (averageFps + lastSecFrameCount) / 2;
        framesPerSecond = lastSecFrameCount;
        lastSecFrameCount = 0;
    }

    // Shows the time measurements in the window title
    // check sprintf formats here https://cplusplus.com/reference/cstdio/printf/
    std::stringstream ss;
    ss << gameTitle << ": Av.FPS: " << std::fixed << std::setprecision(2) << averageFps
        << " Last sec frames: " << framesPerSecond
        << " Last dt: " << std::fixed << std::setprecision(3) << dt
        << " Time since startup: " << secondsSinceStartup
        << " Frame Count: " << frameCount
        << " Vsync: " << vsync;

    std::string titleStr = ss.str();

    window.get()->SetTitle(titleStr.c_str());
}

// Call modules before each loop iteration
bool Engine::PreUpdate()
{
    //Iterates the module list and calls PreUpdate on each module
    bool result = true;
    if (currentLoopState == LoopState::INTRO)
    {
        for (const auto& module : moduleListIntro) {
            result = module.get()->PreUpdate();
            if (!result) {
                break;
            }
        }
    }
    if (currentLoopState == LoopState::TITLE)
    {
        for (const auto& module : moduleListTitle) {
            result = module.get()->PreUpdate();
            if (!result) {
                break;
            }
        }
    }

    if (currentLoopState == LoopState::SETTINGS)
    {
        for (const auto& module : moduleListSettings) {
            result = module.get()->PreUpdate();
            if (!result) {
                break;
            }
        }
    }

    if (currentLoopState == LoopState::PAUSE)
    {
        for (const auto& module : moduleListPause) {
            result = module.get()->PreUpdate();
            if (!result) {
                break;
            }
        }
    }

    if (currentLoopState == LoopState::GAME)
    {
        for (const auto& module : moduleListGame) {
            result = module.get()->PreUpdate();
            if (!result) {
                break;
            }
        }
    }

    return result;
}

// Call modules on each loop iteration
bool Engine::DoUpdate()
{
    //Iterates the module list and calls Update on each module
    bool result = true;

    if (currentLoopState == LoopState::INTRO)
    {
        for (const auto& module : moduleListIntro) {
            result = module.get()->Update(dt);
            if (!result) {
                break;
            }
        }
    }
    if (currentLoopState == LoopState::TITLE)
    {
        for (const auto& module : moduleListTitle) {
            result = module.get()->Update(dt); 
            if (!result) {
                break;
            }
        }
    }

    if (currentLoopState == LoopState::SETTINGS)
    {
        for (const auto& module : moduleListSettings) {
            result = module.get()->Update(dt);
            if (!result) {
                break;
            }
        }
    }

    if (currentLoopState == LoopState::PAUSE)
    {
        for (const auto& module : moduleListPause) {
            result = module.get()->Update(dt);
            if (!result) {
                break;
            }
        }
    }

    if (currentLoopState == LoopState::GAME)
    {
        for (const auto& module : moduleListGame) {
            result = module.get()->Update(dt);
            if (!result) {
                break;
            }
        }
    }

    return result;
}

// Call modules after each loop iteration
bool Engine::PostUpdate()
{
    //Iterates the module list and calls PostUpdate on each module
    bool result = true;
    if (currentLoopState == LoopState::INTRO)
    {
        for (const auto& module : moduleListIntro) {
            result = module.get()->PostUpdate();
            if (!result) {
                break;
            }
        }
    }
    if (currentLoopState == LoopState::TITLE)
    {
        for (const auto& module : moduleListTitle) {
            result = module.get()->PostUpdate();
            if (!result) {
                break;
            }
        }
    }

    if (currentLoopState == LoopState::SETTINGS)
    {
        for (const auto& module : moduleListSettings) {
            result = module.get()->PostUpdate();
            if (!result) {
                break;
            }
        }
    }

    if (currentLoopState == LoopState::PAUSE)
    {
        for (const auto& module : moduleListPause) {
            result = module.get()->PostUpdate();
            if (!result) {
                break;
            }
        }
    }

    if (currentLoopState == LoopState::GAME)
    {
        for (const auto& module : moduleListGame) {
            result = module.get()->PostUpdate();
            if (!result) {
                break;
            }
        }
    }

    return result;
}

bool Engine::LoadConfig()
{
    bool ret = true;

    //Load config.xml file using load_file() method from the xml_document class
    // If the result is ok get the main node of the XML
    // else, log the error
    // check https://pugixml.org/docs/quickstart.html#loading

    pugi::xml_parse_result result = configFile.load_file("config.xml");
    if (result)
    {
        LOG("config.xml parsed without errors");
    }
    else
    {
        LOG("Error loading config.xml: %s", result.description());
    }

    return ret;
}

float Engine::GetDeltaTime() const
{
    return dt;
}

void Engine::ChangeLoopState(LoopState state)
{
    previousLoopState = currentLoopState;
    currentLoopState = state;
    AwakeCurrentLoopState();
    StartCurrentLoopState();
}

void Engine::ChangeLoopStateWithoutStart(LoopState state)
{
    previousLoopState = currentLoopState;
    currentLoopState = state;
}

void Engine::AwakeCurrentLoopState()
{
    auto isInCleanOnce = [&](const std::shared_ptr<Module>& module) {
        return std::find(moduleListCleanOnce.begin(), moduleListCleanOnce.end(), module) != moduleListCleanOnce.end();
        };

    if (currentLoopState == LoopState::INTRO)
    {
        bool result = true;
        for (const auto& module : moduleListIntro) {
            result = module.get()->LoadParameters(configFile.child("config").child(module.get()->name.c_str()));
            if (result) result = module.get()->Awake();
            if (!result) {
                break;
            }
        }
    }
    if (currentLoopState == LoopState::TITLE)
    {
        bool result = true;
        for (const auto& module : moduleListTitle) {
            if (!isInCleanOnce(module)) {
                result = module.get()->LoadParameters(configFile.child("config").child(module.get()->name.c_str()));
                if (result) result = module.get()->Awake();
                if (!result) {
                    break;
                }
            }
        }
    }

    if (currentLoopState == LoopState::SETTINGS)
    {
        bool result = true;
        for (const auto& module : moduleListSettings) {
            if (!isInCleanOnce(module)) {
                result = module.get()->LoadParameters(configFile.child("config").child(module.get()->name.c_str()));
                if (result) result = module.get()->Awake();
                if (!result) {
                    break;
                }
            }
        }
    }

    if (currentLoopState == LoopState::PAUSE)
    {
        bool result = true;
        for (const auto& module : moduleListPause) {
            if (!isInCleanOnce(module)) {
                result = module.get()->LoadParameters(configFile.child("config").child(module.get()->name.c_str()));
                if (result) result = module.get()->Awake();
                if (!result) {
                    break;
                }
            }
        }
    }

    if (currentLoopState == LoopState::GAME)
    {
        bool result = true;
        for (const auto& module : moduleListGame) {
            if (!isInCleanOnce(module)) {
                result = module.get()->LoadParameters(configFile.child("config").child(module.get()->name.c_str()));
                if (result) result = module.get()->Awake();
                if (!result) {
                    break;
                }
            }
        }
    }

}

bool Engine::StartCurrentLoopState()
{
    bool result = true;

    auto isInCleanOnce = [&](const std::shared_ptr<Module>& module) {
        return std::find(moduleListCleanOnce.begin(), moduleListCleanOnce.end(), module) != moduleListCleanOnce.end();
        };

    if (currentLoopState == LoopState::INTRO)
    {
        for (const auto& module : moduleListIntro) {
            result = module.get()->Start();
            if (!result) {
                break;
            }
        }
        IntroStarted = true;
    }
    if (currentLoopState == LoopState::TITLE)
    {
        for (const auto& module : moduleListTitle) {
            if (!isInCleanOnce(module)) {
                result = module.get()->Start();
                if (!result) {
                    break;
                }
            }
        }
        TitleStarted = true;
    }
    if (currentLoopState == LoopState::SETTINGS)
    {
        for (const auto& module : moduleListSettings) {
            if (!isInCleanOnce(module)) {
                result = module.get()->Start();
                if (!result) {
                    break;
                }
            }
        }
        SettingsStarted = true;
    }
    if (currentLoopState == LoopState::PAUSE)
    {
        for (const auto& module : moduleListPause) {
            if (!isInCleanOnce(module)) {
                result = module.get()->Start();
                if (!result) {
                    break;
                }
            }
        }
        PauseStarted = true;
    }
    if (currentLoopState == LoopState::GAME)
    {
        for (const auto& module : moduleListGame) {
            if (!isInCleanOnce(module)) {
                result = module.get()->Start();
                if (!result) {
                    break;
                }
            }
        }
        GameStarted = true;
    }
    return result;
}
