// Copyright (c) 2002 - 2018, Kit10 Studios LLC
// All Rights Reserved

#pragma once

#include <MEEditor.h>
#include <me/action/IAction.h>

namespace meedr
{
	/// <summary>
	/// Open the editor.
	/// </summary>
	class OpenEditorAction : public me::action::IAction
	{
	public:
		OpenEditorAction();

	public: // IAction...
		bool Perform() override;
	};
}
