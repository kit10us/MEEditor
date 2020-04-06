// Copyright (c) 2002 - 2018, Kit10 Studios LLC
// All Rights Reserved

#include <Windows.h>

#include <meedr/SceneViewer.h>
#include <meedr/ComponentViewer.h>
#include <wab/ui/Window.h>
#include <me/render/Geometry.h>
#include <me/debug/Block.h>

using namespace meedr;

static std::vector< std::string > TypeNames =
{
	"Game",
	"SceneManager",
	"Scene",
	"Object"
};

enum class Types 
{
	Game,
	SceneManager,
	Scene,
	Object
};		  
					 
ComponentViewer::ComponentViewer( SceneViewer* parent, int nCmdShow, int x, int y, me::game::IGame * gameInstance )
	: Window( parent, L"ComponentViewerWndClass" )
	, m_sceneViewer{ parent }
	, m_game{ gameInstance }
{

	struct A {
		virtual int GetValue() { return 1; }
	};
	struct B : public A {
		virtual int GetValue() { return 2; }
	};

	using namespace create;
	AddContainer( new container::StackPanel( container::Stack::Horizontal, 540, 440 ) );
		AddContainer( new container::StackPanel( container::Stack::Vertical, FillWidth(2), FillHeight() ));
			AddControl( new Static( L"Component Type:", SizeToContentWidth(), DefaultHeight() ) );
			AddControl( new Combobox( FillWidth(), DefaultHeight() ), "TypeCombobox" );
			AddControl( new Static( L"Instances:", SizeToContentWidth(), DefaultHeight() ) );
			AddControl( new Listbox( FillWidth(), FillHeight() ), "InstanceList" );
			StepDown();
		AddContainer( new container::StackPanel( container::Stack::Vertical, FillWidth(2), 200 ) );
			AddControl( new Static( L"Components: ", SizeToContentWidth(), DefaultHeight() ) );
			AddControl( new Listbox( FillWidth(), 200 ), "ComponentList" );
			StepDown();
		AddContainer( new container::StackPanel( container::Stack::Vertical, FillWidth(3), FillHeight() ) );
			AddControl( new Static( L"Values: ", SizeToContentWidth(), DefaultHeight() ) );
			AddControl( new ListView( FillWidth(), 200 ), "ValuesList" );
			StepDown();

	Create( L"Component Viewer", x, y, nCmdShow );
}
									
void ComponentViewer::UpdateComponentTypes()
{
	using namespace ui;			  

	Combobox* typeCombobox = GetControl< Combobox* >( "TypeCombobox" );

	typeCombobox->ResetContent();

	for( auto type : TypeNames )
	{
		typeCombobox->AddString( type );
	}				
	typeCombobox->SetCurSel( 0 );

	UpdateTypeInstances();
}		   

void ComponentViewer::UpdateTypeInstances()
{
	using namespace ui;			  
	Combobox* typeCombobox = GetControl< Combobox* >( "TypeCombobox" );
	Listbox* instanceList = GetControl< Listbox* >( "InstanceList" );

	instanceList->ResetContent();

	size_t typeIndex = typeCombobox->GetCurSel();

	switch ( (Types)typeIndex )
	{
	case Types::Game:
	{
		instanceList->SetEnable( false );
		break;
	}
	case Types::SceneManager:
	{
		instanceList->SetEnable(false);
		break;
	}
	case Types::Scene:
	{
		instanceList->SetEnable( false );
		break;
	}
	case Types::Object:
	{
		instanceList->SetEnable( true );

		auto sceneManager = m_game->GetComponentT< me::scene::SceneManager >();
		auto scene = sceneManager->GetCurrentScene();
		for (size_t index = 0; index < scene->GetObjectAllocator()->Count(); index++)
		{
			me::object::Object * object = nullptr;
			if( index >= 0 )
			{
				std::vector< me::object::Object * > objects;
				scene->GetObjectAllocator()->CollectObjects( objects );
				object = objects[index];
			}

			instanceList->AddString(object->GetName());
		}
		instanceList->SetCurSel(0);
		break;
	}
	default:
		break;
	}		

	UpdateComponentList();
}
void ComponentViewer::UpdateComponentList()
{
	using namespace ui;

	Combobox* typeCombobox = GetControl< Combobox* >( "TypeCombobox" );
	Listbox* instanceList = GetControl< Listbox* >( "InstanceList" );
	Listbox* componentList = GetControl< Listbox* >( "ComponentList" );

	// Clear contents...
	componentList->ResetContent();

	size_t typeIndex = typeCombobox->GetCurSel();
	size_t instanceIndex = instanceList->GetCurSel();

	switch ( (Types)typeIndex )
	{
	case Types::Game:
	{
		for ( int index = 0; index < m_game->GetComponentCount(); index++ )
		{
			componentList->AddString( m_game->GetComponent( index )->GetTypeName() );
		}
		break;
	}
	case Types::SceneManager:
	{
		auto sceneManager = m_game->GetComponentT< me::scene::SceneManager >();
		for (int index = 0; index < sceneManager->GetComponentCount(); index++)
		{
			componentList->AddString(sceneManager->GetComponent(index)->GetTypeName());
		}
		break;
	}
	case Types::Scene:
	{
		auto sceneManager = m_game->GetComponentT< me::scene::SceneManager >();
		auto scene = sceneManager->GetCurrentScene();
		for ( int index = 0; index < scene->GetComponentCount(); index++ )
		{
			componentList->AddString( scene->GetComponent( index )->GetTypeName() );
		}
		break;
	}
	case Types::Object:
	{
		auto sceneManager = m_game->GetComponentT< me::scene::SceneManager >();
		auto scene = sceneManager->GetCurrentScene();
		
		me::object::Object * object = nullptr;
		if( instanceIndex >= 0 )
		{
			std::vector< me::object::Object * > objects;
			scene->GetObjectAllocator()->CollectObjects( objects );
			object = objects[instanceIndex];
		}

		for ( int index = 0; index < object->GetComponentCount(); index++ )
		{
			componentList->AddString( object->GetComponent( index )->GetTypeName() );
		}
		break;
	}
	default:
		break;
	}	

	componentList->SetCurSel( 0 );
	UpdateComponentValues();
}

void ComponentViewer::UpdateComponentValues()
{
	using namespace ui;

	Combobox* typeCombobox = GetControl< Combobox* >( "TypeCombobox" );
	Listbox* instanceList = GetControl< Listbox* >( "InstanceList" );
	Listbox* componentList = GetControl< Listbox* >( "ComponentList" );
	ListView* valuesList = GetControl< ListView* >( "ValuesList" );

	// Clear contents...
	valuesList->DeleteAllItems();

	size_t typeIndex = typeCombobox->GetCurSel();
	size_t instanceIndex = instanceList->GetCurSel();
	size_t componentIndex = componentList->GetCurSel();

	switch ( (Types)typeIndex )
	{
	case Types::Game:
	{
		auto component = m_game->GetComponent( componentIndex );
		if ( !component )
		{
			break;
		}

		int i = 0;
		auto begin = component->GetLookup()->begin();
		auto end = component->GetLookup()->end();
		for ( auto itr = begin; itr != end; itr++ )
		{
			valuesList->InsertItem( 0, i, unify::Cast< std::wstring >( itr->value->ToString() ) );
			valuesList->InsertItem( 1, i, unify::Cast< std::wstring >( itr->key ) );
		}
		break;
	}
	case Types::SceneManager:
	{
		auto sceneManager = m_game->GetComponentT< me::scene::SceneManager >();
		auto component = sceneManager->GetComponent(componentIndex);
		if (!component)
		{
			break;
		}
		int i = 0;
		auto begin = component->GetLookup()->begin();
		auto end = component->GetLookup()->end();
		for (auto itr = begin; itr != end; itr++)
		{
			valuesList->InsertItem( 0, i, unify::Cast< std::wstring >( itr->value->ToString() ) );
			valuesList->InsertItem( 1, i, unify::Cast< std::wstring >( itr->key ) );
		}
		break;
	}
	case Types::Scene:
	{
		auto sceneManager = m_game->GetComponentT< me::scene::SceneManager >();
		auto scene = sceneManager->GetCurrentScene();
		auto component = scene->GetComponent( componentIndex );
		if ( !component )
		{
			break;
		}
		int i = 0;
		auto begin = component->GetLookup()->begin();
		auto end = component->GetLookup()->end();
		for (auto itr = begin; itr != end; itr++)
		{
			valuesList->InsertItem( 0, i, unify::Cast< std::wstring >( itr->value->ToString() ) );
			valuesList->InsertItem( 1, i, unify::Cast< std::wstring >( itr->key ) );
		}
		break;
	}
	case Types::Object:
	{
		auto sceneManager = m_game->GetComponentT< me::scene::SceneManager >();
		auto scene = sceneManager->GetCurrentScene();
		
		me::object::Object * object = nullptr;
		if( instanceIndex >= 0 )
		{
			std::vector< me::object::Object * > objects;
			scene->GetObjectAllocator()->CollectObjects( objects );
			object = objects[instanceIndex];
		}

		auto component = object->GetComponent( componentIndex );
		int i = 0;
		auto begin = component->GetLookup()->begin();
		auto end = component->GetLookup()->end();
		for (auto itr = begin; itr != end; itr++)
		{
			valuesList->InsertItem( 0, i, unify::Cast< std::wstring >( itr->value->ToString() ) );
			valuesList->InsertItem( 1, i, unify::Cast< std::wstring >( itr->key ) );
		}
		break;
	}
	default:
		break;
	}	
}

void ComponentViewer::GotoComponent( std::string type, int index )
{
}

ui::IResult * ComponentViewer::OnCreate( ui::message::Params params )
{
	using namespace ui;			  

	return new Result( 0 );
}

ui::IResult * ComponentViewer::OnDestroy( ui::message::Params params )
{
	using namespace ui;			  

	GetParent()->SendUserMessage( COMPONENTVIEWER_CLOSED, message::Params{ 0, 0 } );
	return new Result( 0 );
}

ui::IResult * ComponentViewer::OnAfterCreate( ui::message::Params )
{	
	using namespace ui;			  

	ListView* valuesList = GetControl< ListView* >( "ValuesList" );
	valuesList->AddColumn( 0, L"Value", 80 );
	valuesList->AddColumn( 1, L"Name", 120 );
	int a[] = { 1, 0 };
	valuesList->SetColumnOrderArray( 2, a );

	UpdateComponentTypes();
	return new Result( 0 );
}

ui::IResult * ComponentViewer::OnControlCommand( ui::message::ControlCommand message )
{
	using namespace ui;
	auto debug = m_game->Debug();
	auto parent( debug->MakeBlock( "ComponentViewer::OnControlCommand" ) );

	if ( message.control )
	{
		OutputDebugStringA( message.control->GetName().c_str() );
	}

	if ( message.IsFor( "TypeCombobox" ) )
	{
		switch ( (Combobox::Event)message.code )
		{
		case Combobox::Event::SelEndOk:
		{
			UpdateTypeInstances();
			break;
		}
		}
	}	  
	else if ( message.IsFor( "InstanceList" ) )
	{
		switch ( (Listbox::Event)message.code )
		{
		case Listbox::Event::SelChange:
		{
			UpdateComponentList();
			break;
		}
		}
	}
	else if ( message.IsFor( "ComponentList" ) )
	{
		switch ( (Listbox::Event)message.code )
		{
		case Listbox::Event::SelChange:
			UpdateComponentValues();
			break;
		}
	}
	return new Result( 0 );
}			

ui::IResult* ComponentViewer::OnNotify( ui::message::Notify message )
{
	using namespace ui;
	if ( message.IsFor( "ValuesList" ) )
	{
		switch( (ListView::Notify)message.code )
		{
		case ListView::Notify::EndLabelEditW:
		{
			NMLVDISPINFO info = *(NMLVDISPINFO*)message.lParam;

			Combobox* typeCombobox = GetControl< Combobox* >( "TypeCombobox" );
			Listbox* instanceList = GetControl< Listbox* >( "InstanceList" );
			Listbox* componentList = GetControl< Listbox* >( "ComponentList" );
			ListView* valuesList = GetControl< ListView* >( "ValuesList" );

			size_t typeIndex = typeCombobox->GetCurSel();
			size_t instanceIndex = instanceList->GetCurSel();
			size_t componentIndex = componentList->GetCurSel();

			me::IComponent* component = nullptr;

			switch ( (Types)typeIndex )
			{
			case Types::Game:
			{
				component = m_game->GetComponent( componentIndex ).get();
				break;
			}
			case Types::SceneManager:
			{
				auto sceneManager = m_game->GetComponentT< me::scene::SceneManager >();
				component = sceneManager->GetComponent(componentIndex);
				break;
			}
			case Types::Scene:
			{
				auto sceneManager = m_game->GetComponentT< me::scene::SceneManager >();
				auto scene = sceneManager->GetCurrentScene();
				component = scene->GetComponent( componentIndex );
				break;
			}
			case Types::Object:
			{
				auto sceneManager = m_game->GetComponentT< me::scene::SceneManager >();
				auto scene = sceneManager->GetCurrentScene();

				me::object::Object * object = nullptr;
				if( instanceIndex >= 0 )
				{
					std::vector< me::object::Object * > objects;
					scene->GetObjectAllocator()->CollectObjects( objects );
					object = objects[instanceIndex];
				}

				component = object->GetComponent( componentIndex ).get();
			}
			default:
				break;
			}	

			int valueIndex = info.item.iItem;
			//component->GetLookup()->SetValue( valueIndex, unify::Cast< std::string >( std::wstring( info.item.pszText ) ) );

			return new Result( 1 );
		}
		case ListView::Notify::BeginLabelEditW:
			return new Result( m_sceneViewer->IsEditing() ? 0 : 1 );
		}
	}
	return new Unhandled();
}