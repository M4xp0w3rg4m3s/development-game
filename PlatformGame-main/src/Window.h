#pragma once

#include "Module.h"
#include "SDL2/SDL_Video.h"
#include "SDL2/SDL.h"
#include "pugixml.hpp"
using namespace pugi;

class Window : public Module
{
public:

	Window();

	// Destructor
	virtual ~Window();

	// Called before render is available
	bool Awake();

	// Called before quitting
	bool CleanUp();

	// Changae title
	void SetTitle(const char* title);

	// Retrive window size
	void GetWindowSize(int& width, int& height) const;

	// Retrieve window scale
	int GetScale() const;

public:
	// The window we'll be rendering to
	SDL_Window* window;

	std::string title;
	int width = configParameters.child("resolution").attribute("width").as_int();
	int height = configParameters.child("resolution").attribute("height").as_int();
	int scale = configParameters.child("resolution").attribute("scale").as_int();
};