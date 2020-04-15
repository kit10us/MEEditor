// Copyright (c) 2002 - 2018, Kit10 Studios LLC
// All Rights Reserved

#pragma once

#include <me/game/IGame.h>
#include <wab/ui/Window.h>

namespace meedr
{	  
	class ComponentViewer : public ui::Window
	{
		me::game::IGame* m_game;
		class SceneViewer* m_sceneViewer;
		kit::debug::IBlock::ptr m_block;

	public:
		ComponentViewer( SceneViewer* parent, int nCmdShow, int x, int y, me::game::IGame * gameInstance );

		void UpdateComponentTypes();
		void UpdateTypeInstances();
		void UpdateComponentList();
		void UpdateComponentValues();
		void GotoComponent( std::string type, int index );

		ui::IResult* OnCreate( ui::message::Params params ) override;
		ui::IResult* OnDestroy( ui::message::Params params ) override;
		ui::IResult* OnAfterCreate( ui::message::Params ) override;
		ui::IResult* OnControlCommand( ui::message::ControlCommand message ) override;
		ui::IResult* OnNotify( ui::message::Notify message ) override;
	};
}