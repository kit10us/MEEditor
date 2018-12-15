// Copyright (c) 2002 - 2018, Evil Quail LLC
// All Rights Reserved

#include <meedr/ActionsComponent.h>
#include <meedr/OpenEditorAction.h>

using namespace meedr;

char* ActionsGameComponent::Name()
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
