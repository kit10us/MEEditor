// Copyright (c) 2002 - 2018, Kit10 Studios LLC
// All Rights Reserved

#include <meedr/LogViewerLogListener.h>
#include <meedr/LogViewer.h>

using namespace meedr;

LogViewerLogListener::LogViewerLogListener( LogViewer * logViewer )
	: m_logViewer{ logViewer }
{
}

LogViewerLogListener::~LogViewerLogListener()
{
}

void LogViewerLogListener::Log( std::string text )
{
	if (m_logViewer)
	{
		m_logViewer->Log( text );
	}
}
