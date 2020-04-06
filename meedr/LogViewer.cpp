// Copyright (c) 2002 - 2018, Kit10 Studios LLC
// All Rights Reserved

#include <meedr/LogViewerLogListener.h>
#include <meedr/SceneViewer.h>
#include <meedr/LogViewer.h>
#include <wab/ui/Window.h>

#include <Richedit.h>
#include <ShObjIdl.h>
#include <vector>

using namespace meedr;

LogViewer::LogViewer( SceneViewer* parent, int nCmdShow, int x, int y, me::game::IGame* gameInstance )
	: Window( parent, L"LogViewerWndClass" )
	, m_sceneViewer{ parent }
	, m_game{ gameInstance }
{
	using namespace create;
	AddContainer( new container::StackPanel( container::Stack::Horizontal, 640, 480 ) );
	AddControl( (new Richtext( FillWidth(), FillHeight() ))->/*SetMultiline( true )->*/SetReadonly( true )->SetAutoHScroll( true )->SetVScroll( true ), "LogText" );
	Window::Create( L"Log Viewer", x, y, nCmdShow );
}

LogViewer::~LogViewer()
{
	m_game->Debug()->GetLogger()->DetachListener( m_logListener );
}

void LogViewer::Log( std::string text )
{
	using namespace ui;			  

	Richtext* logText = GetControl< Richtext* >( "LogText" );
	logText->AppendText( text  );
}

ui::IResult* LogViewer::OnCreate( ui::message::Params params )
{
	using namespace ui;			  

	return new Result( 0 );
}

ui::IResult* LogViewer::OnAfterCreate( ui::message::Params params )
{
	using namespace ui;			  

	Richtext* logText = GetControl< Richtext* >( "LogText" );
	logText->SetText( "" );
	m_logListener.reset( new LogViewerLogListener( this ) );
	m_game->Debug()->GetLogger()->AttachListener( m_logListener );
	return new Result( 0 );
}

ui::IResult* LogViewer::OnDestroy( ui::message::Params params )
{
	using namespace ui;

	if ( m_game->Debug()->GetLogger() != nullptr )
	{
		m_game->Debug()->GetLogger()->DetachListener( m_logListener );
	}

	GetParent()->SendUserMessage( LOGVIEWER_CLOSED, message::Params{}  );
	return new Result( 0 );
}

ui::IResult* LogViewer::OnControlCommand( ui::message::ControlCommand message )
{
	using namespace ui;			  

	/*
	if ( message.IsFor( "Save" ) )
	{
		return new Result( 0 );
	}
	else if ( message.IsFor( "Load" ) )
	{
		return new Result( 0 );
	}
	*/
	return new Unhandled();
}
