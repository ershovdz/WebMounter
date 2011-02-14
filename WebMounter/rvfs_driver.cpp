//#include "StdAfx.h"
#include "rvfs_driver.h"
#include <QtCore>
#include <boost/bind.hpp>

using namespace RemoteDriver;
using namespace Common;

RESULT RVFSDriver::createFile(const QString& path, const QString& title,  const QString& id, const QString& parentId)
{
	return eNO_ERROR;
}
/***********************************************************************
***************************** SLOTS ************************************							  
***********************************************************************/
void RVFSDriver::startPlugin(PluginSettings& pluginSettings)
{
	QtConcurrent::run(
		boost::bind(&RemoteDriver::RVFSDriver::connectHandler, _1, _2)
		, this
		, pluginSettings
		);
}
void RVFSDriver::stopPlugin()
{
	QtConcurrent::run(boost::bind(&RemoteDriver::RVFSDriver::disconnectHandler, _1), this);
}
void RVFSDriver::startSync()
{
	QtConcurrent::run(boost::bind(&RemoteDriver::RVFSDriver::syncHandler, _1), this);
}
void RVFSDriver::stopSync()
{
	QtConcurrent::run(boost::bind(&RemoteDriver::RVFSDriver::stopSyncHandler, _1), this);
}

void RVFSDriver::updateState(int progress, DriverState newState)
{
	_state = newState;
	emit updateView(progress, (int)newState);
}

RESULT RVFSDriver::checkKey(const PluginSettings&)
{
	return eNO_ERROR;
}
/***********************************************************************
*********************** Handlers for slots *****************************							  
***********************************************************************/

void RVFSDriver::connectHandler(PluginSettings& pluginSettings)
{
	updateState(100, eAuthInProgress);

	updateState(100, eAuthorized);

	updateState(100, eSync);

	updateState(100, eConnected);
}

void RVFSDriver::disconnectHandler()
{
	updateState(100, eNotConnected);
}

void RVFSDriver::syncHandler()
{
	updateState(100, eSync);

	updateState(100, eConnected);
}

void RVFSDriver::stopSyncHandler()
{
	updateState(100, eSyncStopping);

	updateState(100, eConnected);
}

bool RVFSDriver::areFileAttributesValid(const QString&, unsigned long)
{
	return true;
}
