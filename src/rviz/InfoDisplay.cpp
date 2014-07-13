
#include "InfoDisplay.h"

namespace rtabmap
{

InfoDisplay::InfoDisplay()
  : spinner_(1, &cbqueue_),
    globalCount_(0),
    localCount_(0)
{
	update_nh_.setCallbackQueue( &cbqueue_ );
}

InfoDisplay::~InfoDisplay()
{
	spinner_.stop();
}

void InfoDisplay::onInitialize()
{
	MFDClass::onInitialize();

	this->setStatusStd(rviz::StatusProperty::Ok, "Info", "");
	this->setStatusStd(rviz::StatusProperty::Ok, "Global", "0");
	this->setStatusStd(rviz::StatusProperty::Ok, "Local", "0");

	spinner_.start();
}

void InfoDisplay::processMessage( const rtabmap::InfoConstPtr& msg )
{
	{
		boost::mutex::scoped_lock lock(info_mutex_);
		if(msg->loopClosureId)
		{
			info_ = QString("%1->%2 [Global]").arg(msg->refId).arg(msg->loopClosureId);
			globalCount_ += 1;
		}
		else if(msg->localLoopClosureId)
		{
			info_ = QString("%1->%2 [Local]").arg(msg->refId).arg(msg->localLoopClosureId);
			localCount_ += 1;
		}
		else
		{
			info_ = "";
		}
	}

	this->emitTimeSignal(msg->header.stamp);
}

void InfoDisplay::update( float wall_dt, float ros_dt )
{
	{
		boost::mutex::scoped_lock lock(info_mutex_);
		this->setStatusStd(rviz::StatusProperty::Ok, "Info", tr("%1").arg(info_).toStdString());
		this->setStatusStd(rviz::StatusProperty::Ok, "Global", tr("%1").arg(globalCount_).toStdString());
		this->setStatusStd(rviz::StatusProperty::Ok, "Local", tr("%1").arg(localCount_).toStdString());
	}
}

void InfoDisplay::reset()
{
	MFDClass::reset();
	{
		boost::mutex::scoped_lock lock(info_mutex_);
		info_.clear();
		globalCount_ = 0;
		localCount_ = 0;
	}
}

} // namespace rtabmap

#include <pluginlib/class_list_macros.h>
PLUGINLIB_EXPORT_CLASS( rtabmap::InfoDisplay, rviz::Display )
