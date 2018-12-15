// Copyright (c) 2002 - 2018, Evil Quail LLC
// All Rights Reserved

#include <wab/ui/Window.h>
#include <MEEditor.h>
#include <meedr/SceneViewer.h>
#include <meedr/ResourceBrowser.h>
#include <meedr/InputBrowser.h>
#include <meedr/ScriptEditor.h>
#include <meedr/LogViewer.h>
#include <meedr/ComponentViewer.h>
#include <wab/ui/Builder.h>

#include <Richedit.h>
#include <ShObjIdl.h>
#include <vector>

using namespace meedr;

#define USERMESSAGE_UPDATEDATA	0

ui::IWindow::ptr s_parent;

SceneViewer::SceneViewer( ui::IWindow::ptr parent, me::game::IGame * gameInstance )
	: Window( parent.get(), L"SceneViewerWndClass" )
	, m_game{ gameInstance }
	, m_openChildren{ 0 } 	
	, m_sceneManager{ dynamic_cast< me::scene::SceneManager* >( gameInstance->GetComponent( "SceneManager" ).get() ) }
	, m_noScenes{ true }
{
	int nCmdShow = gameInstance->GetOSParameters().nCmdShow;
	RECT rect{};
	::GetWindowRect( GetParentHandle(), &rect );
	int x = rect.right;
	int y = rect.top;
	
	using namespace create;
	AddContainer( new container::StackPanel( container::Stack::Horizontal, 740, 480, 0 ) );
	AddContainer( new container::StackPanel( container::Stack::Vertical, SizeToContentWidth(), FillHeight() ) );
	AddControl( new Button( L"Edit Scene", SizeToContentWidth(), DefaultHeight() ), "EditScene" );
	AddControl( new Static( L"Scene:", SizeToContentWidth(), DefaultHeight() ) );
	AddControl( new Combobox( DefaultWidth(), DefaultHeight() ), "SceneCombobox" );
	AddControl( new Listbox( DefaultWidth(), FillHeight() ), "ObjectList" );
	StepDown();
	AddContainer( new container::StackPanel( container::Stack::Vertical, FillWidth(), FillHeight() ) );
	AddContainer( new container::StackPanel( container::Stack::Horizontal, FillWidth(), SizeToContentHeight() ) );
	AddControl( new Static( L"Name: ", SizeToContentWidth(), DefaultHeight() ) );
	AddControl( (new Edit( FillWidth(), DefaultHeight() )), "Name" );
	StepDown();
	AddContainer( new container::StackPanel( container::Stack::Horizontal, FillWidth(), SizeToContentHeight() ) );
	AddControl( new Static( L"x: ", SizeToContentWidth(), DefaultHeight() ) );
	AddControl( (new Edit( 90, DefaultHeight() ))->SetReadonly( true ), "X" );
	AddControl( new Static( L"y: ", SizeToContentWidth(), DefaultHeight() ) );
	AddControl( (new Edit( 90, DefaultHeight() ))->SetReadonly( true ), "y" );
	AddControl( new Static( L"z: ", SizeToContentWidth(), DefaultHeight() ) );
	AddControl( (new Edit( 90, DefaultHeight() ))->SetReadonly( true ), "z" );
	StepDown();
	AddContainer( new container::StackPanel( container::Stack::Horizontal, FillWidth(), FillHeight() ) );
	AddContainer( new container::StackPanel( container::Stack::Vertical, FillWidth(), FillHeight() ) );
	AddControl( new Static( L"Components: ", SizeToContentWidth(), DefaultHeight() ) );
	AddControl( new Listbox( FillWidth( 2 ), 200 ), "Components" );
	StepDown();
	AddContainer( new container::StackPanel( container::Stack::Vertical, FillWidth(), FillHeight() ) );
	AddControl( new Static( L"Values: ", SizeToContentWidth(), DefaultHeight() ) );
	AddControl( new ListView( FillWidth( 3 ), 200 ), "ValuesList" );

	AddMenu( new Menu(
		{
			{ "File", 
				{
					{ "Restart Scene" },
					{ "Exit Scene Viewer" },
					Seperator(),
					{ "Quit Game" }
				}
			},
			{ "View ",
				{
					{ "Components" },
					{ "Assets" },
					{ "Inputs" },
					{ "Log" }
				}

			}
		}
	) );

	Create( L"Mercury Engine Viwer", x, y, nCmdShow );

	::SetForegroundWindow( GetParentHandle() );
}

SceneViewer::~SceneViewer()
{	
	KillTimer( m_timer );
}

me::game::IGame * SceneViewer::GetGame() const
{
	return m_game;
}

void SceneViewer::OpenResourceBrowser()
{
	RECT rect{};
	GetWindowRect( rect );
	int x = rect.left;
	int y = rect.top;
	if ( !m_resourceBrowser )
	{
		m_openChildren++;
		m_resourceBrowser.reset( new meedr::ResourceBrowser( this, SW_SHOWDEFAULT, x + m_openChildren * 34, y + m_openChildren * 34, m_game ) );
	}
	else
	{
		m_resourceBrowser->ShowWindow( SW_RESTORE );
		m_resourceBrowser->SetForegroundWindow();
	}							  
}	

void SceneViewer::OpenInputBrowser()
{
	RECT rect{};
	GetWindowRect( rect );
	int x = rect.left;
	int y = rect.top;
	if ( !m_inputBrowser )
	{
		m_openChildren++;
		m_inputBrowser.reset( new InputBrowser( this, SW_SHOWDEFAULT, x + m_openChildren * 34, y + m_openChildren * 34, m_game ) );
	}
	else
	{
		m_inputBrowser->ShowWindow( SW_RESTORE );
		m_inputBrowser->SetForegroundWindow();
	}
}

void SceneViewer::OpenScriptEditor( unify::Path source )
{
	RECT rect{};
	GetWindowRect( rect );
	int x = rect.left;
	int y = rect.top;
	if ( ! m_scriptEditor )
	{
		m_openChildren++;
		m_scriptEditor.reset( new ScriptEditor( this, SW_SHOWDEFAULT, x + m_openChildren * 34, y + m_openChildren * 34, m_game ) );
	}
	else
	{
		m_scriptEditor->ShowWindow( SW_RESTORE );
		m_scriptEditor->SetForegroundWindow();
	}

	ScriptEditor* scriptEditor = dynamic_cast<ScriptEditor*>(m_scriptEditor.get());
	scriptEditor->LoadFile( source );
}

void SceneViewer::OpenLogViewer()
{	
	RECT rect{};
	GetWindowRect( rect );
	int x = rect.left;
	int y = rect.top;
	if ( ! m_logViewer )
	{
		m_openChildren++;
		m_logViewer.reset( new LogViewer( this, SW_SHOWDEFAULT, x + m_openChildren * 34, y + m_openChildren * 34, m_game ) );
	}
	else
	{
		m_logViewer->ShowWindow( SW_RESTORE );
		m_logViewer->SetForegroundWindow();
	}
}

void SceneViewer::OpenComponentViewer( std::string type, int index )
{
	RECT rect{};
	GetWindowRect( rect );
	int x = rect.left;
	int y = rect.top;
	if ( ! m_componentViewer )
	{
		m_openChildren++;
		m_componentViewer.reset( new ComponentViewer( this, SW_SHOWDEFAULT, x + m_openChildren * 34, y + m_openChildren * 34, m_game ) );
	}
	else
	{
		m_componentViewer->ShowWindow( SW_RESTORE );
		m_componentViewer->SetForegroundWindow();
	}

	ComponentViewer* componentViewer = dynamic_cast<ComponentViewer*>(m_componentViewer.get());
	componentViewer->GotoComponent( type, index );
}

void SceneViewer::UpdateSceneList()
{	
	using namespace ui;

	Combobox * sceneCombobox = GetControl< Combobox* >( "SceneCombobox" );

	if ( m_sceneManager->GetSceneCount() == 0 )
	{
		m_noScenes = true;
		return;
	}
	else
	{
		m_noScenes = false;
	}

	for ( size_t i = 0; i < m_sceneManager->GetSceneCount(); i++ )
	{
		sceneCombobox->AddString(m_sceneManager->GetSceneName(i));
	}

	sceneCombobox->SetCurSel( 0 );

	UpdateObjectList();
}

void SceneViewer::UpdateObjectList()
{
	using namespace ui;

	Combobox* sceneCombobox = GetControl< Combobox* >( "SceneCombobox" );
	Listbox* objectListbox = GetControl<Listbox*>( "ObjectList" );

	objectListbox->ResetContent();
	
	int sceneIndex = sceneCombobox->GetCurSel();
	auto scene = m_sceneManager->GetCurrentScene();
	if ( ! scene )
	{
		return;
	}

	std::vector< me::object::Object * > objects;
	scene->GetObjectAllocator()->CollectObjects( objects );
														
	for ( auto * object : objects )
	{
		objectListbox->AddString( object->GetName() );
	}

	objectListbox->SetCurSel( 0 );

	UpdateObject_All();
}

void SceneViewer::UpdateObject_All()
{
	using namespace ui;
	Combobox* sceneCombobox = GetControl< Combobox* >( "SceneCombobox" );
	Listbox* objectListbox = GetControl< Listbox* >( "ObjectList" );
	Edit* name = GetControl< Edit* >( "Name" );

	int sceneIndex = sceneCombobox->GetCurSel();
	auto scene = m_sceneManager->GetCurrentScene();
	if ( ! scene )
	{
		return;
	}

	int objectIndex = objectListbox->GetCurSel();

	me::object::Object * object = nullptr;
	if( objectIndex >= 0 )
	{
		std::vector< me::object::Object * > objects;
		scene->GetObjectAllocator()->CollectObjects( objects );
		object = objects[ objectIndex ];
	}

	if ( ! object )
	{
		name->SetText( "" );
	}
	else
	{
		if ( name->GetText() != object->GetName() )
		{
			name->SetText( object->GetName() );
		}
	}

	UpdateObject_Position( true );
	UpdateObject_Components();
}

void SceneViewer::UpdateObject_Position( bool force )
{
	using namespace ui;

	Combobox* sceneCombobox = GetControl< Combobox* >( "SceneCombobox" );
	Listbox* objectListbox = GetControl< Listbox* >( "ObjectList" );
	Edit* x = GetControl< Edit* >( "X" );
	Edit* y = GetControl< Edit* >( "Y" );
	Edit* z = GetControl< Edit* >( "Z" );
				  		 
	int sceneIndex = sceneCombobox->GetCurSel();
	auto scene = m_sceneManager->GetCurrentScene();
	int objectIndex = objectListbox->GetCurSel();

	me::object::Object * object = nullptr;
	if( objectIndex >= 0 )
	{
		std::vector< me::object::Object * > objects;
		scene->GetObjectAllocator()->CollectObjects( objects );
		object = objects[ objectIndex ];
	}
							   
	if ( force || ! m_editingLock )
	{ // Shouldn't be editing

		// If object is not selected.
		if ( ! object )
		{
			x->SetText( "" );
			y->SetText( "" );
			z->SetText( "" );
		}
		else
		{
			unify::V3< float > position( object->GetFrame().GetPosition() );
			std::string xs = unify::Cast< std::string >( position.x );
			std::string ys = unify::Cast< std::string >( position.y );
			std::string zs = unify::Cast< std::string >( position.z );

			if ( x->GetText() != xs )
			{
				x->SetText( xs );
			}

			if ( y->GetText() != ys )
			{
				y->SetText( ys );
			}

			if ( z->GetText() != zs )
			{
				z->SetText( zs );
			}
		}
	}
}			  

void SceneViewer::UpdateObject_Components( bool force )
{
	using namespace ui;

	Combobox* sceneCombobox = GetControl< Combobox* >( "SceneCombobox" );
	Listbox* objectListbox = GetControl< Listbox* >( "ObjectList" );
	Listbox* components = GetControl< Listbox* >( "Components" );
				  		 
	int sceneIndex = sceneCombobox->GetCurSel();
	auto scene = m_sceneManager->GetCurrentScene();
	
	int objectIndex = objectListbox->GetCurSel();

	me::object::Object * object = nullptr;
	if( objectIndex >= 0 )
	{
		std::vector< me::object::Object * > objects;
		scene->GetObjectAllocator()->CollectObjects( objects );
		object = objects[objectIndex];
	}

	if ( object == nullptr )
	{
		return;
	}
							   
	components->ResetContent();
	for ( int i = 0; i < object->GetComponentCount(); i++ )
	{
		auto component = object->GetComponent( i );
		std::string type = component->GetTypeName();
		std::string what = component->GetWhat();
		std::string text = type;
		if ( !what.empty() )
		{
			text += " (\"" + what + "\")";
		}
		components->AddString( text );
	}

	// Select the first item in the list.
	components->SetCurSel( 0 );

	UpdateObject_ComponentValues();
}


void SceneViewer::UpdateObject_ComponentValues()
{	
	using namespace ui;

	Combobox* sceneCombobox = GetControl< Combobox* >( "SceneCombobox" );
	Listbox* objectListbox = GetControl< Listbox* >( "ObjectList" );
	Listbox* components = GetControl< Listbox* >( "Components" );
	ListView* values = GetControl< ListView* >( "ValuesList" );

	values->DeleteAllItems();

	int sceneIndex = sceneCombobox->GetCurSel();
	auto scene = m_sceneManager->GetCurrentScene();
	
	int objectIndex = objectListbox->GetCurSel();

	me::object::Object * object = nullptr;
	if( objectIndex >= 0 )
	{
		std::vector< me::object::Object * > objects;
		scene->GetObjectAllocator()->CollectObjects( objects );
		object = objects[objectIndex];
	}

	
	int componentIndex = components->GetCurSel();
	if ( componentIndex == -1 )
	{
		return;
	}

	auto component = object->GetComponent( componentIndex );

	for ( int i = 0; i < component->GetValueCount(); i++ )
	{
		values->InsertItem( 0, i, unify::Cast< std::wstring >( component->GetValue( i ) ) );
		values->InsertItem( 1, i, unify::Cast< std::wstring >( component->GetValueName( i ) ) );
	}
}

void SceneViewer::OpenObjectComponent()
{
	using namespace ui;

	Combobox* sceneCombobox = GetControl< Combobox* >( "SceneCombobox" );
	Listbox* objectListbox = GetControl< Listbox* >( "ObjectList" );
	Listbox* components = GetControl< Listbox* >( "Components" );

	int sceneIndex = sceneCombobox->GetCurSel();
	auto scene = m_sceneManager->GetCurrentScene();
	int objectIndex = objectListbox->GetCurSel();
	
	me::object::Object * object = nullptr;
	if( objectIndex >= 0 )
	{
		std::vector< me::object::Object * > objects;
		scene->GetObjectAllocator()->CollectObjects( objects );
		object = objects[objectIndex];
	}

	int componentIndex = components->GetCurSel();
	auto component = object->GetComponent( componentIndex );
	if ( !component )
	{
		return;
	}

	if ( unify::StringIs( component->GetTypeName(), "LUAScript" ) )
	{
		unify::Path path( component->GetValue( "path" ) );
		unify::Path fullPath( m_game->GetOS()->GetRunPath(), path );
		fullPath.Normalize();
		GetParent()->SendUserMessage( SCRIPTEDITOR_OPEN, message::Params{ 0, (LPARAM)fullPath.ToString().c_str() } );
	}
}		

void SceneViewer::EditScene( bool edit )
{
	using namespace ui;

	m_game->Debug()->LogLine( "SceneViewer::EditScene", unify::Cast< std::string >( edit ) );

	if ( edit )
	{
		// Check if we are already editing, if so, do nothing.
		if ( m_editingLock )
		{
			return;
		}
		else
		{
			m_editingLock = m_game->LockUpdate( false );
		}
	}
	else
	{
		// Check if we are already not editing, if so, do nothing.
		if ( !m_editingLock )
		{
			return;
		}
		else
		{
			m_editingLock.reset();
		}
	}

	auto editSceneButton = GetControl< Button* >( "EditScene" );
	auto x = GetControl< Edit* >( "X" );
	auto y = GetControl< Edit* >( "Y" );
	auto z = GetControl< Edit* >( "Z" );
	auto values = GetControl< ListView* >( "ValuesList" );

	editSceneButton->SetText( m_editingLock ? "Resume" : "Edit Scene" );
	x->SetReadonly( m_editingLock ? false : true );
	y->SetReadonly( m_editingLock ? false : true );
	z->SetReadonly( m_editingLock ? false : true );
}

bool SceneViewer::IsEditing() const
{
	return m_editingLock ? true : false;
}

ui::IResult* SceneViewer::OnAfterCreate( ui::message::Params )
{
	using namespace ui;

	ListView* values = GetControl< ListView* >( "ValuesList" );
	values->AddColumn( 0, L"Value", 80 );
	values->AddColumn( 1, L"Name", 120 );
	int a[] = { 1, 0 };
	values->SetColumnOrderArray( 2, a );

	UpdateSceneList();

	m_timer = SetTimer( 0, 10 );

	return new Result(0);
}

ui::IResult* SceneViewer::OnControlCommand( ui::message::ControlCommand message )
{
	using namespace ui;

	if ( message.IsFor( "SceneCombobox" ) )
	{
		switch ( (Combobox::Event)message.code )
		{
		case Combobox::Event::SelEndOk:
			UpdateObjectList();
			break;
		}
	}
	else if ( message.IsFor( "ObjectList" ) )
	{
		switch ( (Listbox::Event)message.code )
		{
		case Listbox::Event::SelChange:
			UpdateObject_All();
			return new Result(0);
		}
	}
	else if ( message.IsFor( "Components" ) )
	{
		switch ( (Listbox::Event)message.code )
		{
		case Listbox::Event::SelChange:
			UpdateObject_ComponentValues();
			return new Result( 0 );
		case Listbox::Event::DblClick:
			OpenObjectComponent();
			return new Result( 0 );
		}	  
	}
	else if ( message.IsFor( "EditScene" ) )
	{
		switch ( (Button::Event)message.code )
		{
		case Button::Event::Clicked:
			EditScene( !m_editingLock );
			return new Result( 0 );
		}
	}


	// Editing messages...
	if ( m_editingLock )
	{
		if ( message.IsFor( "x" ) )
		{
			switch ( (Edit::Event)message.code )
			{
			case Edit::Event::Change:
			{
				float x( unify::Cast< float >( message.control->GetText() ) );

				Combobox* sceneCombobox = GetControl< Combobox* >( "SceneCombobox" );
				Listbox* objectListbox = GetControl< Listbox* >( "ObjectList" );
				  		 
				int sceneIndex = sceneCombobox->GetCurSel();
				auto scene = m_sceneManager->GetCurrentScene();
				int objectIndex = objectListbox->GetCurSel();

				me::object::Object * object = nullptr;
				if( objectIndex >= 0 )
				{
					std::vector< me::object::Object * > objects;
					scene->GetObjectAllocator()->CollectObjects( objects );
					object = objects[objectIndex];
				}

				auto position( object->GetFrame().GetPosition() );
				position.x = x;
				object->GetFrame().SetPosition( position );
				break;
			}
			}
		}
		else if ( message.IsFor( "y" ) )
		{
			switch ( (Edit::Event)message.code )
			{
			case Edit::Event::Change:
			{
				float y( unify::Cast< float >( message.control->GetText() ) );

				Combobox* sceneCombobox = GetControl< Combobox* >( "SceneCombobox" );
				Listbox* objectListbox = GetControl< Listbox* >( "ObjectList" );
				  		 
				int sceneIndex = sceneCombobox->GetCurSel();
				auto scene = m_sceneManager->GetCurrentScene();
				int objectIndex = objectListbox->GetCurSel();
				
				me::object::Object * object = nullptr;
				if( objectIndex >= 0 )
				{
					std::vector< me::object::Object * > objects;
					scene->GetObjectAllocator()->CollectObjects( objects );
					object = objects[objectIndex];
				}

				auto position( object->GetFrame().GetPosition() );
				position.y = y;
				object->GetFrame().SetPosition( position );
				break;
			}
			}
		}
		else if ( message.IsFor( "z" ) )
		{
			switch ( (Edit::Event)message.code )
			{
			case Edit::Event::Change:
			{
				float z( unify::Cast< float >( message.control->GetText() ) );

				Combobox* sceneCombobox = GetControl< Combobox* >( "SceneCombobox" );
				Listbox* objectListbox = GetControl< Listbox* >( "ObjectList" );
				  		 
				int sceneIndex = sceneCombobox->GetCurSel();
				auto scene = m_sceneManager->GetCurrentScene();
				int objectIndex = objectListbox->GetCurSel();
				
				me::object::Object * object = nullptr;
				if( objectIndex >= 0 )
				{
					std::vector< me::object::Object * > objects;
					scene->GetObjectAllocator()->CollectObjects( objects );
					object = objects[objectIndex];
				}

				auto position( object->GetFrame().GetPosition() );
				position.z = z;
				object->GetFrame().SetPosition( position );
				break;
			}
			}
		}
	}
	return new Unhandled();
}

ui::IResult* SceneViewer::OnTimer( ui::message::Timer message )
{
	using namespace ui;
	if ( m_noScenes )
	{
		UpdateSceneList();
	}

	UpdateObject_Position();
	return new Result( 0 );
}

ui::IResult* SceneViewer::OnNotify( ui::message::Notify message )
{
	using namespace ui;
	if ( message.IsFor( "ValuesList" ) )
	{
		switch( (ListView::Notify)message.code )
		{
		case ListView::Notify::EndLabelEditW:
		{
			NMLVDISPINFO info = *(NMLVDISPINFO*)message.lParam;

			Combobox* sceneCombobox = GetControl< Combobox* >( "SceneCombobox" );
			Listbox* objectListbox = GetControl< Listbox* >( "ObjectList" );
			Listbox* components = GetControl< Listbox* >( "Components" );
			ListView* values = GetControl< ListView* >( "ValuesLidy" );

			int sceneIndex = sceneCombobox->GetCurSel();
			auto scene = m_sceneManager->GetCurrentScene();
			int objectIndex = objectListbox->GetCurSel();
			
			me::object::Object * object = nullptr;
			if( objectIndex >= 0 )
			{
				std::vector< me::object::Object * > objects;
				scene->GetObjectAllocator()->CollectObjects( objects );
				object = objects[objectIndex];
			}

			int componentIndex = components->GetCurSel();
			auto component = object->GetComponent( componentIndex );

			int valueIndex = info.item.iItem;
			component->SetValue( valueIndex, unify::Cast< std::string >( std::wstring( info.item.pszText ) ) );

			return new Result( 1 );
		}
		case ListView::Notify::BeginLabelEditW:
			return new Result( IsEditing() ? 0 : 1 );
		}
	}
	return new Unhandled();
}					   

ui::IResult* SceneViewer::OnMenuCommand( ui::message::MenuCommand message )
{
	using namespace ui;

	if ( message.IsFor( "Quit Game" ) )
	{
		m_game->Quit();
		return new Result( 0 );
	}
	else if (message.IsFor("Restart Scene"))
	{
		m_sceneManager->RestartScene();
		UpdateSceneList();
		GetParent()->SetForegroundWindow();
	}
	else if ( message.IsFor( "Exit Scene Viewer" ) )
	{
		Destroy();
		return new Result( 0 );
	}
	else if ( message.IsFor( "Components" ) )
	{
		OpenComponentViewer( std::string(), -1 );
		return new Result( 0 );
	}
	else if ( message.IsFor( "Assets" ) )
	{
		OpenResourceBrowser();
		return new Result( 0 );
	}
	else if ( message.IsFor(  "Inputs" ) )
	{
		OpenInputBrowser();
		return new Result( 0 );
	}
	else if ( message.IsFor( "ScriptEditor" ) )
	{
		OpenScriptEditor( unify::Path() );
		return new Result( 0 );
	}
	else if ( message.IsFor( "Log" ) )
	{
		OpenLogViewer();
		return new Result( 0 );
	}

	return new Unhandled();
}

ui::IResult* SceneViewer::OnUser( ui::message::User message )
{
	using namespace ui;

	switch ( message.message )
	{
	case RESOURCEBROWSER_CLOSED:
		m_resourceBrowser.reset();
		m_openChildren--;
		return new Result( 0 );
	case INPUTBROWSER_CLOSED:
		m_inputBrowser.reset();
		m_openChildren--;
		return new Result( 0 );
	case SCRIPTEDITOR_CLOSED:
		m_scriptEditor.reset();
		m_openChildren--;
		return new Result( 0 );
	case LOGVIEWER_CLOSED:
		m_logViewer.reset();
		m_openChildren--;
		return new Result( 0 );
	case SCRIPTEDITOR_OPEN:
		OpenScriptEditor( message.params.lParam ? unify::Path( (char*)message.params.lParam ) : unify::Path() );
		return new Result( 0 );
	case COMPONENTVIEWER_CLOSED:
		m_componentViewer.reset();
		m_openChildren--;
		return new Result( 0 );
	}
	return new Unhandled();
}

ui::IResult* SceneViewer::OnDestroy(ui::message::Params)
{
	using namespace ui;

	DestroyEditor();
	return new Unhandled();
}
