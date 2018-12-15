// Copyright (c) 2002 - 2018, Evil Quail LLC
// All Rights Reserved

#pragma once

#include <me/game/IGame.h>
#include <wab/ui/Window.h>

namespace meedr
{	  
	class ResourceBrowser : public ui::Window
	{
		me::game::IGame* m_game;
		class SceneViewer * m_sceneViewer;

	public:
		ResourceBrowser( SceneViewer* parent, int nCmdShow, int x, int y, me::game::IGame * gameInstance );

		void UpdateResourceTypes();
		void UpdateResourceList();
		void OpenResource();
		void ReloadResource();

		ui::IResult * OnCreate( ui::message::Params params ) override;
		ui::IResult * OnDestroy( ui::message::Params params ) override;
		ui::IResult * OnAfterCreate( ui::message::Params ) override;
		ui::IResult * OnControlCommand( ui::message::ControlCommand message ) override;
	};
}