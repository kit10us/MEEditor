// Copyright (c) 2002 - 2018, Kit10 Studios LLC
// All Rights Reserved

#pragma once

#include <me/game/IGame.h>
#include <kit/ILogListener.h>

namespace meedr
{
	class LogViewer;

	/// <summary>
	/// An ILogListener, which binds the ILogListener interface to the LogViewer, 
	/// removing the need for multiple inheritance.
	/// </summary>
	class LogViewerLogListener : public kit::ILogListener
	{
		LogViewer* m_logViewer;
	public:
		LogViewerLogListener( LogViewer* logViewer );
		~LogViewerLogListener() override;

		void LogEvent( const kit::LogEvent* event ) override;
	};
}
