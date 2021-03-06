// Copyright (c) 2002 - 2018, Kit10 Studios LLC
// All Rights Reserved

#pragma once

#include <me/game/IGame.h>
#include <wab/ui/Window.h>
#include <kit/debug/ILogListener.h>

namespace meedr
{
	class LogViewer : public ui::Window
	{
	private:
		me::game::IGame * m_game;
		class SceneViewer*  m_sceneViewer;
		kit::debug::ILogListener::ptr m_logListener;

	public:
		LogViewer( SceneViewer* parent, int nCmdShow, int x, int y, me::game::IGame * gameInstance );
		~LogViewer();

		// me::ILogListener...
		void Log( std::string text );

		ui::IResult* OnCreate( ui::message::Params params ) override; 
		ui::IResult* OnAfterCreate( ui::message::Params params ) override;
		ui::IResult* OnDestroy( ui::message::Params params ) override;
		ui::IResult* OnControlCommand( ui::message::ControlCommand message ) override;
	};
}
