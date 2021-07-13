// Copyright (c) 2002 - 2018, Kit10 Studios LLC
// All Rights Reserved

#include <meedr/ActionsComponent.h>
#include <meedr/OpenEditorAction.h>

using namespace meedr;

const char* ActionsGameComponent::Name()
{
	return "ActionsGameComponent";
}


ActionsGameComponent::ActionsGameComponent()
	: GameComponent( Name() )
{
}

me::action::IAction::ptr ActionsGameComponent::CreateAction(const qxml::Element * element)
{
	if ( element->IsTagName( "OpenEditor" ) )
	{
		return me::action::IAction::ptr(new OpenEditorAction());
	}

	return me::action::IAction::ptr();
}

std::string ActionsGameComponent::GetWhat() const
{
	return "MEEditor";
}
