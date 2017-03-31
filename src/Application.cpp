#include "Application.h"

#include "states/State_Playing.h"

#include <iostream>

Application::Application(std::string&& name, const WindowSettings& settings)
:   m_title(std::move(name))
,   m_windowSettings(settings)
{
    auto style  = settings.isFullscreen ? sf::Style::Fullscreen : sf::Style::Default;
    auto width  = settings.width;
    auto height = settings.height;

    m_window.create({width, height}, m_title, style);
	m_window.setVerticalSyncEnabled(settings.isVsyncEnabled);

	pushState(std::make_unique<State::SPlaying>(this));
}

void Application::start()
{
    constexpr static auto UP_TICK = 1000.0f / 60.0f;

	sf::Clock clock;

	float timer = 0.0f;
	float upTimer = float(clock.getElapsedTime().asMilliseconds());

	uint frames = 0;
	uint updates = 0;

	while (m_isRunning)
	{
		m_window.clear();
		sf::Event e;
		while (m_window.pollEvent(e))
        {
			onEvent(e);
        }
        if (!m_window.isOpen())
        {
            m_isRunning = false;
            break;
        }

		//Runs 60 times a second
		float now = float(clock.getElapsedTime().asMilliseconds());
		if (now - upTimer > UP_TICK)
		{
			updates++;
			upTimer += UP_TICK;
			onUpdate();
		}

		//Runs as fast as possible
		{
			frames++;
			sf::Clock frametime;
			onRender();
			m_frameTime = float(frametime.getElapsedTime().asMilliseconds());
		}

		// Runs each second
		if (clock.getElapsedTime().asSeconds() - timer > 1.0f)
		{
			timer += 1.0f;
			m_framesPerSecond   = frames;
			m_updatesPerSecond  = updates;
			frames  = 0;
			updates = 0;
			onTick();
		}

		m_window.display();

		m_isRunning = m_window.isOpen();
	}
}

void Application::onUpdate()
{
	m_states.back()->update();
}

void Application::onRender()
{
	m_states.back()->render();
}

void Application::onTick()
{
	m_states.back()->tick();

	printf("%d fps, %d ups\n", m_framesPerSecond, m_updatesPerSecond);
}

void Application::onEvent(sf::Event& event)
{
    switch(event.type)
    {
        case sf::Event::Closed:
            m_window.close();
            break;

        case sf::Event::KeyReleased:
            switch (event.key.code)
            {
                ///@TODO Change this, we may later need to use the escape key for pausing the game.
                case sf::Keyboard::Escape:
                    m_window.close();
                    break;

                default:
                    break;
            }
            break;

        default:
            break;
    }

	m_states.back()->event(event);
}

void Application::pushState(std::unique_ptr<State::SBase> state)
{
	m_states.push_back(std::move(state));
}

void Application::popState()
{
	m_states.pop_back();
}

void Application::setVSync(bool enabled)
{
	m_windowSettings.isVsyncEnabled = enabled;
	m_window.setVerticalSyncEnabled(enabled);
}

void Application::setWindowTitle(std::string&& title)
{
	m_title = std::move(title);
	m_window.setTitle(m_title);
}
