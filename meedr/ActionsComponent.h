// Copyright (c) 2002 - 2018, Evil Quail LLC
// All Rights Reserved

#include <me/game/component/GameComponent.h>

namespace meedr
{
	class ActionsGameComponent : public me::game::component::GameComponent
	{
	public:
		static char* Name();

		ActionsGameComponent();

	public: // IGameComponent...
		me::action::IAction::ptr CreateAction(const qxml::Element * node) override;

	public: // IComponent...
		std::string GetWhat() const override;
	};
}
