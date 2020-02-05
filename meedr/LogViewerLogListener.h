// Copyright (c) 2002 - 2018, Kit10 Studios LLC
// All Rights Reserved

#pragma once

#include <me/game/IGame.h>

namespace meedr
{
	class LogViewer;

	/// <summary>
	/// An ILogListener, which binds the ILogListener interface to the LogViewer, 
	/// removing the need for multiple inheritance.
	/// </summary>
	class LogViewerLogListener : public me::ILogListener
	{
	public:
		LogViewerLogListener( LogViewer* logViewer );
		virtual ~LogViewerLogListener();

		void Log( std::string text ) override;

	private:
		LogViewer* m_logViewer;
	};
}
