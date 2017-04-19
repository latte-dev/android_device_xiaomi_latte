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

#ifndef __THERMAL_API_SERVER__
#define __THERMAL_API_SERVER__ 1


#include "thd_binder_client.h"

namespace thermal_api {

	class BnThermalAPI: public BnInterface<IThermalAPI> {
		public:
		virtual status_t onTransact(uint32_t code, const Parcel& data, Parcel* reply,
				uint32_t flags = 0);
	};
	class ThermalAPI : public BnThermalAPI {
		public:
		virtual status_t sendProfileStart(int newProfile);
		virtual status_t sendSensorMsg(struct ThermalSensorMessage *msg);
		virtual status_t sendThermalZoneMsg(struct ThermalZoneMessage *zmsg);
		virtual status_t sendThrottleMsg(struct ThrottleMessage *tmsg);
		virtual status_t sendThermalCdevInfoMsg(struct ThermalCdevInfoMessage *cmsg);
	};
}

namespace powerhal_api {

	class BnThermalAPI: public BnInterface<IThermalAPI> {
		public:
		virtual status_t onTransact(uint32_t code, const Parcel& data, Parcel* reply,
				uint32_t flags = 0);
	};
	class ThermalAPI : public BnThermalAPI {
		public:
		virtual status_t sendPowerSaveMsg(struct PowerSaveMessage *psmsg);
	};

}

#endif
