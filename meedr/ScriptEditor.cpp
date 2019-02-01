// Copyright (c) 2002 - 2018, Kit10 Studios LLC
// All Rights Reserved

#include <meedr/SceneViewer.h>
#include <meedr/ScriptEditor.h>
#include <wab/ui/Window.h>

#include <vector>

using namespace meedr;

ScriptEditor::ScriptEditor( SceneViewer* parent, int nCmdShow, int x, int y, me::game::IGame * gameInstance )
	: Window( parent, L"ScriptEditorWndClass" )
	, m_sceneViewer{ parent }
	, m_game{ gameInstance }
{
	using namespace create;
	AddContainer( new container::StackPanel( container::Stack::Vertical, 640, 480 ) );
	AddContainer( new container::StackPanel( container::Stack::Horizontal, SizeToContentWidth(), SizeToContentHeight() ) );
	AddControl( new Button( L"&Save", SizeToContentWidth(), DefaultHeight() ), "Save" );
	AddControl( new Button( L"&Load", SizeToContentWidth(), DefaultHeight() ), "Load" );
	StepDown();
	AddControl( new Richtext( FillWidth(), FillHeight() ), "Text" );
	Window::Create( L"Script Editor", x, y, nCmdShow );
}
				   
void ScriptEditor::LoadFile( unify::Path path )
{
	using namespace ui;			  

	Richtext* text = GetControl< Richtext* >( "Text" );

	if ( path.Empty() )
	{
		text->SetText( "" );
		text->SetEnable( false );
		SetText( "Script Editor" );
		return;
	}
	else
	{
		text->SetEnable( true );
	}

	SetText( std::string( "Script Editor (" ) + path.ToString() + ")" );

	std::vector< std::string > lines;
	
	FILE* fp = 0;
	fopen_s( &fp, path.ToString().c_str(), "r" );
	char str[ 256 ];
	std::string source = "";
	while ( fgets( str, 256, fp ) )
	{
		source += str;
		lines.push_back( str );
	}
	text->SetText( source.c_str() );
	/*
	// Formating...			
	std::map< std::string, CHARFORMAT > formats;
	formats[ "keyword" ] = {};
	formats[ "keyword" ].cbSize = sizeof( CHARFORMAT );
	formats[ "keyword" ].crTextColor = 0xFF0000;
	formats[ "keyword" ].dwMask = CFM_COLOR;

	std::vector< std::string > spaceStack;

	size_t first = 0;
	size_t last = lines[ 0 ].length();
	for ( size_t i = 0; i < lines.size(); i++ )
	{
		// Setup for current line...
		std::string line = lines[ i ];
		last = first + line.length();

		// Format...
		CHARRANGE charRange{ (long)first, (long)last };
		SendMessageA( textHandle, EM_EXSETSEL, 0, (LPARAM)&(charRange) );

		CHARFORMATA format{};
		format.cbSize = sizeof( CHARFORMAT );
		switch ( i % 3 )
		{
		case 0:
			format.crTextColor = 0xFF0000;
			break;
		case 1:
			format.crTextColor = 0x00FF00;
			break;
		case 2:
			format.crTextColor = 0x0000FF;
			break;
		default:
			format.crTextColor = 0xFFFFFF;
		}

		format.dwMask = CFM_COLOR;
		SendMessageA( textHandle, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&format );	  
				
		// Setup for next line...
		first += line.length();
	}	 
	*/

	/*		   
	CHARRANGE charRange{ 0, 10 };
	SendMessageA( textHandle, EM_EXSETSEL, 0, (LPARAM)&(charRange) );

	CHARFORMATA format{};
	format.cbSize = sizeof( CHARFORMAT );
	format.crTextColor = 0xFF0000;
	format.dwMask = CFM_COLOR;
	SendMessageA( textHandle, EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&format );
		*/
}

ui::IResult* ScriptEditor::OnCreate( ui::message::Params params )
{
	using namespace ui;			  
	return new Result( 0 );
}

ui::IResult* ScriptEditor::OnAfterCreate( ui::message::Params params )
{
	using namespace ui;			  

	Button* save = GetControl< Button* >( "save" );
	save->SetEnable( false );
	return new Result( 0 );
}

ui::IResult* ScriptEditor::OnDestroy( ui::message::Params params )
{
	using namespace ui;			  

	GetParent()->SendUserMessage( SCRIPTEDITOR_CLOSED, message::Params{} );
	return new Result(0);
}

ui::IResult* ScriptEditor::OnControlCommand( ui::message::ControlCommand message )
{
	using namespace ui;			  

	if ( message.IsFor( "Save" ) )
	{
		return new Result( 0 );
	}
	else if ( message.IsFor( "Load" ) )
	{
		char filename[ 256 ] = { 0 };
		OPENFILENAMEA ofn{};
		ofn.lStructSize = sizeof( OPENFILENAMEA );
		ofn.hwndOwner = GetHandle();
		//ofn.hInstance = GetInstance();
		ofn.lpstrFilter = "LUA Scripts (*.lua)\0*.lua\0All Files(*.*)\0*.*\0\0";
		ofn.lpstrFile = filename;
		ofn.nMaxFile = 256;
		ofn.lpstrFileTitle = "Open Script...";
		ofn.Flags = OFN_FILEMUSTEXIST;
		if ( GetOpenFileNameA( &ofn ) )
		{
			LoadFile( unify::Path( filename ) );
		}
		else
		{
		}
		return new Result( 0 );
	}
	return new Unhandled();
}
