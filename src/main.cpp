﻿#include "app/Application.h"

#include <iostream>

#include "util/Log.h"

#ifdef __WIN32
#include "windows.h"
#endif // __WIN32

#include "maths/Random.h"
#include "util/Exceptions.h"

namespace
{
	void errorMessage(const std::string& message)
	{
#ifdef __WIN32
		MessageBox(nullptr, message.c_str(), "Error", MB_OK);
#elif __linux
		const std::string command = "zenity --error --text \"" + message + "\"";
		system(command.c_str());
#elif __APPLE__
		const std::string command = "osascript -e 'tell app \"System Events\" to display dialog \"" + message + "\" buttons {\"OK\"} default button 1 with icon caution with title \"Error\"'";
		system(command.c_str());
#else
		LOG_ERROR(message);
		std::cin.ignore();
#endif
	}
}

int main() try
{
	Random::init();

	Application app("Project Comonidy", { 1280, 720, false, VSYNC_DISABLED });
	app.start();
	return EXIT_SUCCESS;
}
catch (std::out_of_range& e)
{
	std::string msg = e.what();
	errorMessage(msg);
	std::cin.ignore();
}
catch (std::runtime_error& e)
{
	std::string msg = e.what();
	errorMessage(msg);
	std::cin.ignore();
}
catch (Exceptions::CannotGetResource& e)
{
	std::string msg = e.what();
	errorMessage(msg);
	std::cin.ignore();
}