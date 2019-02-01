// Copyright (c) 2002 - 2018, Kit10 Studios LLC
// All Rights Reserved

#include <meedr/SceneViewer.h>
#include <meedr/ResourceBrowser.h>
#include <wab/ui/Window.h>
#include <me/render/Geometry.h>
#include <me/script/IScript.h>

using namespace meedr;
					 
ResourceBrowser::ResourceBrowser( SceneViewer* parent, int nCmdShow, int x, int y, me::game::IGame * gameInstance )
	: Window( parent, L"ResourceBrowserWndClass" )
	, m_sceneViewer{ parent }
	, m_game{ gameInstance }
{
	using namespace create;
	AddContainer( new container::StackPanel( container::Stack::Vertical, 540, 440 ) );
	AddControl( new Static( L"Type:", 50, DefaultHeight() ) );
	AddControl( new Combobox( 230, DefaultHeight() ), "Types" );
	AddControl( (new Listbox( 230, FillHeight() ))->SetHScroll( true ), "Assets" );
	AddControl( new Button( L"Reload Asset", DefaultWidth(), DefaultHeight() ), "ReloadAsset" );
	Create( L"Asset Viewer", x, y, nCmdShow );
}
									
void ResourceBrowser::UpdateResourceTypes()
{
	using namespace ui;			  

	Combobox* types = GetControl< Combobox* >( "Types" );

	// Clear contents...
	types->ResetContent();

	// Fill in resource types...
	for ( size_t i = 0; i < m_game->GetResourceHub().GetTypeCount(); i++ )
	{			  
		std::string name = m_game->GetResourceHub().GetTypeName( i ).c_str();
		types->AddString( name );
	}

	// Select first type:
	types->SetCurSel( 0 );

	UpdateResourceList();
}

void ResourceBrowser::UpdateResourceList()
{
	using namespace ui;			  

	Combobox* types = GetControl< Combobox* >( "Types" );
	Listbox* resources = GetControl< Listbox* >( "Assets" );

	// Clear contents...
	resources->ResetContent();

	size_t typeIndex = types->GetCurSel();

	// Fill in resource types...
	auto manager = m_game->GetResourceHub().GetManagerRaw( typeIndex );

	for ( size_t i = 0; i < manager->Count(); i++ )
	{			  
		std::string text = manager->GetResource( i )->GetName();
		auto resource = manager->GetResource( i );
		text += "  (" + resource->GetSource() + ")";
		resources->AddString( text.c_str() );
	}

	// Select first type:
	resources->SetCurSel( 0 );
}

void ResourceBrowser::OpenResource()
{
	using namespace ui;			  

	Combobox* types = GetControl< Combobox* >( "Types" );
	Listbox* resources = GetControl< Listbox* >( "Assets" );

	size_t typeIndex = types->GetCurSel();
	size_t i = resources->GetCurSel();

	auto manager = m_game->GetResourceHub().GetManagerRaw( typeIndex );
	auto resource = manager->GetResource( i );
	unify::Path source( resource->GetSource() );

	if ( source.Exists() )
	{
		char operation[] = "open";
		source.Combine( m_game->GetOS()->GetOSParameters()->GetRunPath(), source );
		HINSTANCE result = ShellExecuteA( GetHandle(), operation, source.ToString().c_str(), nullptr, nullptr, SW_SHOW );
		switch( (int)result )
		{
		case ERROR_FILE_NOT_FOUND:
		{
			break;
		}
		}
	}
}

void ResourceBrowser::ReloadResource()
{
	using namespace ui;

	Combobox* types = GetControl< Combobox* >( "Types" );
	Listbox* resources = GetControl< Listbox* >( "Assets" );

	size_t typeIndex = types->GetCurSel();
	size_t i = resources->GetCurSel();

	auto manager = m_game->GetResourceHub().GetManagerRaw( typeIndex );
	auto resource = manager->GetResource( i );
	resource->Reload();
}

ui::IResult * ResourceBrowser::OnCreate( ui::message::Params params )
{
	using namespace ui;			  

	return new Result( 0 );
}

ui::IResult * ResourceBrowser::OnDestroy( ui::message::Params params )
{
	using namespace ui;			  

	GetParent()->SendUserMessage( RESOURCEBROWSER_CLOSED, message::Params{ 0, 0 } );
	return new Result( 0 );
}

ui::IResult * ResourceBrowser::OnAfterCreate( ui::message::Params )
{
	using namespace ui;			  

	UpdateResourceTypes();
	return new Result( 0 );
}

ui::IResult * ResourceBrowser::OnControlCommand( ui::message::ControlCommand message )
{
	using namespace ui;			  

	if ( message.IsFor( "Types" ) )
	{
		switch ( message.code )
		{
		case CBN_SELCHANGE:
		{
			UpdateResourceList();
			break;
		}
		}
	}	  
	else if ( message.IsFor( "Assets" ) )
	{
		switch ( message.code )
		{
		case LBN_DBLCLK:
			OpenResource();
			break;
		}
	}
	else if( message.IsFor( "ReloadAsset" ) )
	{
		switch( (Button::Event)message.code )
		{
		case Button::Event::Clicked:
			ReloadResource();
			break;
		}
	}
	return new Result( 0 );
}			
