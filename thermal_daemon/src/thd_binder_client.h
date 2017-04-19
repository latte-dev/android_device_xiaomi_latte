/*
 * Copyright (C) 2014 Intel Corporation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License version
 * 2 or later as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *
 */

#ifndef __THD_BINDER_CLIENT__
#define __THD_BINDER_CLIENT__

#include "thd_binder_defs.h"
#include <binder/IBinder.h>
#include <binder/IInterface.h>
#include <binder/IPCThreadState.h>
#include <binder/IServiceManager.h>
#include <binder/Parcel.h>
#include <binder/ProcessState.h>

#include <utils/String16.h>

#include <vector>

#define SERVICE_NAME "thermal_daemon"
#define META_INTERFACE_NAME "android.thermal.binder.IThermalAPI"

using namespace android;

namespace thermal_api {

	enum THERMAL_API_ENUM {
		SEND_PROFILE_START=IBinder::FIRST_CALL_TRANSACTION, SEND_SENSOR_MSG,
		SEND_THERMAL_ZONE_MSG, SEND_THROTTLE_MSG, SEND_CDEV_MSG, SEND_POWER_SAVE_MSG
	};

	class IThermalAPI : public IInterface
	{
		public:
		DECLARE_META_INTERFACE(ThermalAPI);
		virtual status_t sendProfileStart(int newProfile) = 0;
		virtual status_t sendSensorMsg(struct ThermalSensorMessage *smsg) = 0;
		virtual status_t sendThermalZoneMsg(struct ThermalZoneMessage *zmsg) = 0;
		virtual status_t sendThrottleMsg(struct ThrottleMessage *tmsg) = 0;
		virtual status_t sendThermalCdevInfoMsg(struct ThermalCdevInfoMessage *cmsg) = 0;
	};

	class BpThermalAPI : public BpInterface<IThermalAPI>
	{
		public:
		BpThermalAPI(const sp<IBinder>& impl );
		virtual status_t sendProfileStart(int newProfile);
		virtual status_t sendSensorMsg(struct ThermalSensorMessage *smsg);
		virtual status_t sendThermalZoneMsg(struct ThermalZoneMessage *zmsg);
		virtual status_t sendThrottleMsg(struct ThrottleMessage *tmsg);
		virtual status_t sendThermalCdevInfoMsg(struct ThermalCdevInfoMessage *cmsg);
	};
}

namespace powerhal_api {

	enum THERMAL_API_ENUM {
		SEND_POWER_SAVE_MSG=IBinder::FIRST_CALL_TRANSACTION
	};

	class IThermalAPI : public IInterface
	{
		public:
		DECLARE_META_INTERFACE(ThermalAPI);
		virtual status_t sendPowerSaveMsg(struct PowerSaveMessage *psmsg) = 0;
	};

	class BpThermalAPI : public BpInterface<IThermalAPI>
	{
		public:
		BpThermalAPI(const sp<IBinder>& impl );
		virtual status_t sendPowerSaveMsg(struct PowerSaveMessage *psmsg);
	};
}

#endif //__THD_BINDER_CLIENT
