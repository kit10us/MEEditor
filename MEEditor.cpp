// Copyright (c) 2002 - 2018, Evil Quail LLC
// All Rights Reserved

#include <MEEditor.h>
#include <meedr/SceneViewer.h>
#include <meedr/ActionsComponent.h>
#include <me/game/Game.h>
#include <Windows.h>

me::game::IGame * s_game;

ui::IWindow::ptr s_rootWindow;
ui::IWindow::ptr s_engineMain;
unify::Owner::ptr s_ownership;

void OpenEditor()
{
	if (!s_engineMain)
	{
		s_engineMain.reset(new meedr::SceneViewer(s_rootWindow, s_game));
	}
}

void DestroyEditor()
{
	s_engineMain.reset();

	HWND activeWindow = GetActiveWindow();
	SetActiveWindow( activeWindow );
}

extern "C" __declspec(dllexport) bool MELoader( me::game::IGame * gameInstance, const qxml::Element * element )
{
	s_game = gameInstance;
	s_ownership = unify::Owner::Create( "MEEditor" );

	s_rootWindow.reset(new ui::Window((HWND)gameInstance->GetOS()->GetOSParameters()->hWnd));

	{
		auto component = new meedr::ActionsGameComponent();
		gameInstance->AddComponent( me::game::component::IGameComponent::ptr( component ) );
	}

	me::input::InputManager * inputManager = gameInstance->GetInputManager();

	if (inputManager)
	{
		auto inputs = element->FindFirstElement("inputs");
		if (inputs)
		{
			size_t failures = inputManager->AddInputActions(s_ownership, inputs, false);
			if (failures)
			{
				return false;
			}
		}
	}

	return true;
}

