	#pragma once

#include <memory>
#include <list>
#include "Module.h"
#include "Timer.h"
#include "PerfTimer.h"
#include "pugixml.hpp"

using namespace pugi;

// Modules
class Window;
class Input;
class Render;
class Textures;
class Audio;
class Scene;
class SceneIntro;
class SceneTitle;
class SceneSettings;
class ScenePause;
class SceneEnd;
class EntityManager;
class Map;
class Physics;
class Parallax;
class ParallaxTwoPoints;
class GuiManager;
class GameHUD;

enum class LoopState {
	CLEAN_ONCE,
	INTRO,
	TITLE,
	SETTINGS,
	GAME,
	PAUSE,
	END
};

class Engine
{
public:

	// Public method to get the instance of the Singleton
	static Engine& GetInstance();

	//	
	void AddModule(std::shared_ptr<Module> module, LoopState state);

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update();

	// Called before quitting
	bool CleanUp();

	// Get the delta time
	float GetDeltaTime() const;

	void ChangeLoopState(LoopState state);
	void ChangeLoopStateWithoutStart(LoopState state);
	void AwakeCurrentLoopState();
	bool StartCurrentLoopState();

	void RestartEngine();

private:

	// Private constructor to prevent instantiation
	// Constructor
	Engine();

	// Delete copy constructor and assignment operator to prevent copying
	Engine(const Engine&) = delete;
	Engine& operator=(const Engine&) = delete;

	// Call modules before each loop iteration
	void PrepareUpdate();

	// Call modules before each loop iteration
	void FinishUpdate();

	// Call modules before each loop iteration
	bool PreUpdate();

	// Call modules on each loop iteration
	bool DoUpdate();

	// Call modules after each loop iteration
	bool PostUpdate();

	// Load config file
	bool LoadConfig();
	bool LoadConfigBackup();

	std::list<std::shared_ptr<Module>> moduleList;
	std::list<std::shared_ptr<Module>> moduleListIntro;
	std::list<std::shared_ptr<Module>> moduleListTitle;
	std::list<std::shared_ptr<Module>> moduleListSettings;
	std::list<std::shared_ptr<Module>> moduleListGame;
	std::list<std::shared_ptr<Module>> moduleListPause;
	std::list<std::shared_ptr<Module>> moduleListEnd;

	std::list<std::shared_ptr<Module>> moduleListCleanOnce;

public:

	enum EngineState
	{
		CREATE = 1,
		AWAKE,
		START,
		LOOP,
		CLEAN,
		FAIL,
		EXIT
	};

	// Modules
	std::shared_ptr<Window> window;
	std::shared_ptr<Input> input;
	std::shared_ptr<Render> render;
	std::shared_ptr<Textures> textures;
	std::shared_ptr<Audio> audio;
	std::shared_ptr<Scene> scene;
	std::shared_ptr<SceneIntro> sceneIntro;
	std::shared_ptr<SceneTitle> sceneTitle;
	std::shared_ptr<SceneSettings> sceneSettings;
	std::shared_ptr<ScenePause> scenePause;
	std::shared_ptr<SceneEnd> sceneEnd;
	std::shared_ptr<EntityManager> entityManager;
	std::shared_ptr<Map> map;
	std::shared_ptr<Physics> physics;
	std::shared_ptr<Parallax> parallax;
	std::shared_ptr<ParallaxTwoPoints> parallaxTwoPoints;
	std::shared_ptr<GuiManager> guiManager;
	std::shared_ptr<GameHUD> gameHud;

	LoopState currentLoopState = LoopState::INTRO;
	LoopState previousLoopState = LoopState::INTRO;
	LoopState beforePreviousLoopState = LoopState::INTRO;

private: 
	bool introStarted = false;
	bool titleStarted = false;
	bool settingsStarted = false;
	bool gameStarted = false;
	bool pauseStarted = false;
	bool endStarted = false;

	// Delta time
	float dt; 
	//Frames since startup
	int frames;

	bool vsync;

	// Calculate timing measures
	// required variables are provided:
	Timer startupTime;
	PerfTimer frameTime;
	PerfTimer lastSecFrameTime;

	int frameCount = 0;
	int framesPerSecond = 0;
	int lastSecFrameCount = 0;

	float averageFps = 0.0f;
	int secondsSinceStartup = 0;

	//Maximun frame duration in miliseconds.
	int maxFrameDuration = 16;

	std::string gameTitle = "Platformer Game";

	// Variable to load and store the XML file in memory
	xml_document configFile;
};