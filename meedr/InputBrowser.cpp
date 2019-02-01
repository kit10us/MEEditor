// Copyright (c) 2002 - 2018, Kit10 Studios LLC
// All Rights Reserved

#include <meedr/SceneViewer.h>
#include <meedr/InputBrowser.h>

using namespace meedr;

#define USERMESSAGE_UPDATEDATA	 0

InputBrowser::InputBrowser( SceneViewer* parent, int nCmdShow, int x, int y, me::game::IGame * gameInstance )
	: Window( parent, L"InputBrowserWndClass" )
	, m_sceneViewer{ parent }
	, m_game{ gameInstance }
	, m_closing{ false }
{
	using namespace create;
	AddContainer( new container::StackPanel( container::Stack::Horizontal, 540, 440, 0 ) );
	AddContainer( new container::StackPanel( container::Stack::Vertical, FillWidth(), FillHeight() ) );
	AddControl( new Static( L"Type:", SizeToContentWidth(), DefaultHeight() ) );
	AddControl( new Combobox( FillWidth(), DefaultHeight() ), "InputDevice" );
	AddContainer( new container::StackPanel( container::Stack::Horizontal, FillWidth(), FillHeight() ) );
	AddControl( (new Listbox( 260, FillHeight() ) )->SetSorted( false ), "InputNames" );
	AddContainer( new container::StackPanel( container::Stack::Vertical, FillWidth(), FillHeight() ) );
	AddControl( new Static( L"Data:", SizeToContentWidth(), DefaultHeight() ) );
	AddControl( new Static( L"PLACE FOR DATA", FillWidth(), FillHeight() ), "InputData" );
	Create( L"Input Browser", x, y, nCmdShow );	
}

InputBrowser::~InputBrowser()
{
	m_closing = true;
	using namespace std::chrono_literals;
	std::this_thread::sleep_for( 1s );
	m_updateData.detach();
}

void InputBrowser::UpdateInputData()
{
	using namespace ui;

	if ( m_game->GetInputManager()->GetSourceCount() == 0 )
	{
		return;
	}

	Combobox* inputSource = GetControl< Combobox* >( "InputDevice" );
	Listbox* inputNames = GetControl< Listbox* >( "InputNames" );
	Static* inputData = GetControl< Static* >( "InputData" );
 
	size_t sourceIndex = inputSource->GetCurSel();
	size_t inputIndex = inputNames->GetCurSel();
	auto source = m_game->GetInputManager()->GetSource( sourceIndex );
	size_t subSource = 0;  	
	me::input::IData::ptr dataRaw = source->GetInputData( subSource, inputIndex );
	me::input::InputType type = dataRaw ? dataRaw->type : me::input::InputType::Invalid;

	std::string dataString( "" );
	switch ( type )
	{
	default:
		dataString += "<unknown>";
		break;
	case me::input::InputType::Button:
	{
		me::input::ButtonData * data( reinterpret_cast<me::input::ButtonData*>(dataRaw.get()) );
		dataString += std::string( "down:        " ) + (data->down ? "true" : "false");
		dataString += std::string( "\npressed:     " )+ (data->pressed ? "true" : "false");
		break;
	}
	case me::input::InputType::Toggle:
	{
		me::input::ToggleData * data( reinterpret_cast<me::input::ToggleData*>(dataRaw.get()) );
		dataString += std::string( "on:          " ) + (data->on ? "true" : "false");
		break;
	}
	case me::input::InputType::Tracker:
	{
		me::input::TrackerData * data( reinterpret_cast<me::input::TrackerData*>(dataRaw.get()) );
		dataString += std::string( "position: " ) + data->position.ToString();
		dataString += std::string( "\nchange:   " ) + data->change.ToString();
		break;
	}
	case me::input::InputType::Trigger:
	{
		me::input::TriggerData * data( reinterpret_cast<me::input::TriggerData*>(dataRaw.get()) );
		dataString += std::string( "value:    " ) + unify::Cast< std::string >( data->value );
		break;
	}
	case me::input::InputType::Stick:
	{
		me::input::StickData * data( reinterpret_cast<me::input::StickData*>(dataRaw.get()) );
		dataString += std::string( "axis:     " ) + data->axis.ToString();
		break;
	}
	}

	inputData->SetText( dataString );
}

void InputBrowser::UpdateInputManagerList()
{
	using namespace ui;

	Combobox* inputSource = GetControl< Combobox* >( "InputDevice" );

	// Clear contents...
	inputSource->ResetContent();

	if ( m_game->GetInputManager()->GetSourceCount() == 0 )
	{
		return;
	}

	// Fill in resource types...
	for ( size_t i = 0; i < m_game->GetInputManager()->GetSourceCount(); i++ )
	{			  
		auto source = m_game->GetInputManager()->GetSource( i );
		std::string name = source->GetName();
		inputSource->AddString( name );
	}

	// Select first type:
	inputSource->SetCurSel( 0 );
}

void InputBrowser::UpdateInputDeviceInputList()
{
	using namespace ui;

	Combobox* inputSource = GetControl< Combobox* >( "InputDevice" );
	Listbox* inputNames = GetControl< Listbox* >( "InputNames" );

	// Clear contents...
	inputNames->ResetContent();

	if ( m_game->GetInputManager()->GetSourceCount() == 0 )
	{
		return;
	}

	
	size_t sourceIndex = inputSource->GetCurSel();
	auto source = m_game->GetInputManager()->GetSource( sourceIndex );
	size_t subSource = 0;

	for ( size_t i = 0; i < source->GetInputCount( subSource ); i++ )
	{
		std::string name = source->GetInputName( subSource, i ) + " - " + me::input::InputTypeToString( source->GetInputType( subSource, i ) );
		inputNames->AddString( name );
	}

	// Select first type:
	inputNames->SetCurSel( 0 );

	UpdateInputData();
}

void InputBrowser::Timer_UpdateInputData()
{
	using namespace ui;

	while ( ! m_closing )
	{
		using namespace std::chrono_literals;
		std::this_thread::sleep_for( 100ms );			
		SendUserMessage( USERMESSAGE_UPDATEDATA, message::Params{} );
	}
}

ui::IResult* InputBrowser::OnAfterCreate( ui::message::Params params )
{
	using namespace ui;

	UpdateInputManagerList();
	UpdateInputDeviceInputList();
	m_updateData = std::thread( &InputBrowser::Timer_UpdateInputData, this );
	return new Result( 0 );
}

ui::IResult * InputBrowser::OnDestroy( ui::message::Params params )
{	
	using namespace ui;

	GetParent()->SendUserMessage( INPUTBROWSER_CLOSED, message::Params{} );
	return new Result( 0 );
}

ui::IResult* InputBrowser::OnControlCommand( ui::message::ControlCommand message )
{
	using namespace ui;

	if ( message.IsFor( "InputDevice" ) )
	{
		switch ( (Combobox::Event)message.code )
		{
		case Combobox::Event::SelEndOk:
		{
			UpdateInputDeviceInputList();
			break;
		}
		}
	}

	return new Result( 0 );
}

ui::IResult* InputBrowser::OnUser( ui::message::User message )
{
	using namespace ui;

	switch ( message.message )
	{
	case USERMESSAGE_UPDATEDATA:
	{
		UpdateInputData();
		break;
	}		
	}
	return new Result( 0 );
}

