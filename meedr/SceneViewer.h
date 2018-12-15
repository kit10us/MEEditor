// Copyright (c) 2002 - 2018, Evil Quail LLC
// All Rights Reserved

#pragma once

#include <me/game/IGame.h>
#include <me/scene/SceneManager.h>
#include <wab/ui/Window.h>
#include <thread>

#define RESOURCEBROWSER_CLOSED	0
#define INPUTBROWSER_CLOSED		1
#define SCRIPTEDITOR_CLOSED		2
#define SCRIPTEDITOR_OPEN		3
#define LOGVIEWER_CLOSED		4
#define COMPONENTVIEWER_CLOSED	5
#define COMPONENTVIEWER_OPEN	6

namespace meedr
{
	class SceneViewer : public ui::Window
	{
	public:
		SceneViewer::SceneViewer( ui::IWindow::ptr parent, me::game::IGame * gameInstance );
		~SceneViewer();

		me::game::IGame * GetGame() const;

		void OpenResourceBrowser();
		void OpenInputBrowser();
		void OpenScriptEditor( unify::Path source );
		void OpenLogViewer();
		void OpenComponentViewer( std::string type, int index );

		void UpdateSceneList();
		void UpdateObjectList();

		void UpdateObject_All();
		void UpdateObject_Position( bool force = false );
		void UpdateObject_Components( bool force = false);
		void UpdateObject_ComponentValues();
		
		void OpenObjectComponent();

		void EditScene( bool edit );

		bool IsEditing() const;
		
		ui::IResult* OnAfterCreate( ui::message::Params ) override;
		ui::IResult* OnControlCommand( ui::message::ControlCommand message ) override;
		ui::IResult* OnTimer( ui::message::Timer message ) override;
		ui::IResult* OnNotify( ui::message::Notify message ) override;
		ui::IResult* OnMenuCommand( ui::message::MenuCommand message ) override;
		ui::IResult* OnUser( ui::message::User message ) override;
		ui::IResult* OnDestroy(ui::message::Params) override;

	private:
		me::game::IGame * m_game;
		int m_openChildren;
		IWindow::ptr m_resourceBrowser;
		IWindow::ptr m_inputBrowser;
		IWindow::ptr m_scriptEditor;
		IWindow::ptr m_logViewer;
		IWindow::ptr m_componentViewer;
		me::scene::SceneManager * m_sceneManager;
		UINT_PTR m_timer;
		me::UpdateLock::ptr m_editingLock;
		bool m_noScenes;
	};
}