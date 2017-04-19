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

#include "thd_binder_server.h"
#include "thermald.h"
#include <signal.h>
#include <vector>
#include <binder/IPCThreadState.h>
#include <binder/Parcel.h>


namespace thermal_api {
	int zone_count = -1;
	int cdev_count = -1;
        int sensor_count = -1;
	std::vector<ThermalZone> thermal_zones;

	IMPLEMENT_META_INTERFACE(ThermalAPI, META_INTERFACE_NAME);

	status_t ThermalAPI::sendProfileStart(int newProfile) {
		if (newProfile) {
			thd_log_info("thermald shifting to new profile");
			get_zones_from_ituxd(++zone_count, thermal_zones);
		} else {
			thd_log_info("thermald profile shift halted");
		}
		while(zone_count != -1 && zone_count--)
			thermal_zones.pop_back();
		return 0;
	};

	status_t ThermalAPI::sendSensorMsg(ThermalSensorMessage *smsg) {
		thd_log_info("thermald sensormsg: name_len=%d, name=%s, path_len=%d, path=%s",
				smsg->name_len,	smsg->name, smsg->path_len, smsg->path);
		if (zone_count == -1)
			return -1;
                sensor_count++;
                thermal_zones[zone_count].numSensors = sensor_count + 1;
                thermal_zones[zone_count].sensors[sensor_count].name = (char *)malloc(smsg->name_len + 1);
		if (thermal_zones[zone_count].sensors[sensor_count].name == NULL) {
			thd_log_warn("Unable to allocate memory, malloc failed");
			sensor_count--;
			return -1;
		}
                strncpy(thermal_zones[zone_count].sensors[sensor_count].name, smsg->name, smsg->name_len);
		thermal_zones[zone_count].sensors[sensor_count].path = (char *)malloc(smsg->path_len + 1);
		if (thermal_zones[zone_count].sensors[sensor_count].path == NULL) {
			thd_log_warn("Unable to allocate memory, malloc failed");
			sensor_count--;
			return -1;
		}
		strncpy(thermal_zones[zone_count].sensors[sensor_count].path, smsg->path, smsg->path_len);
		return 0;
	}

	status_t ThermalAPI::sendThermalZoneMsg(ThermalZoneMessage *zmsg) {
		thd_log_info("thermald zonemsg: len=%d, name=%s, psv=%d, max=%d",
				zmsg->len, zmsg->name, zmsg->psv, zmsg->max);
		zone_count++;
		cdev_count = -1;
		sensor_count = -1;
		thermal_zones.push_back(ThermalZone());
		thermal_zones[zone_count].name = (char *)malloc(zmsg->len + 1);
		if (thermal_zones[zone_count].name == NULL) {
			thd_log_warn("Unable to allocate memory, malloc failed");
			zone_count--;
			return -1;
		}
		strncpy(thermal_zones[zone_count].name, zmsg->name, zmsg->len);
		thermal_zones[zone_count].psv = zmsg->psv;
		thermal_zones[zone_count].max = zmsg->max;
		return 0;
	}

	status_t ThermalAPI::sendThrottleMsg(ThrottleMessage *tmsg) {
		thd_log_info("thermald throttlemsg: len=%d, name=%s, val=%d",
				tmsg->len, tmsg->name, tmsg->val);
		char * cdev_name = (char *)malloc(tmsg->len + 1);
		if (cdev_name == NULL) {
			thd_log_warn("Unable to allocate memory, malloc failed");
			return THD_ERROR;
		}
		strncpy(cdev_name, tmsg->name, tmsg->len);
		throttle_cdev(cdev_name, tmsg->val);
		return THD_SUCCESS;
	}

	status_t ThermalAPI::sendThermalCdevInfoMsg(ThermalCdevInfoMessage *cmsg) {
		thd_log_info("thermald cdevinfomsg: len=%d, name=%s, normal val=%d,"
				"crit val=%d, step size=%d, path_len=%d, path=%s", cmsg->name_len,
				cmsg->name, cmsg->nval, cmsg->critval, cmsg->step, cmsg->path_len, cmsg->path);
		cdev_count++;
		if (zone_count == -1)
			return -1;
		thermal_zones[zone_count].cdevs[cdev_count].name = (char *)malloc(cmsg->name_len + 1);
		if (thermal_zones[zone_count].cdevs[cdev_count].name == NULL) {
			thd_log_warn("Unable to allocate memory, malloc failed");
			cdev_count--;
			return -1;
		}
		strncpy(thermal_zones[zone_count].cdevs[cdev_count].name, cmsg->name, cmsg->name_len);
		thermal_zones[zone_count].cdevs[cdev_count].path = (char *)malloc(cmsg->path_len + 1);
		if (thermal_zones[zone_count].cdevs[cdev_count].path == NULL) {
			thd_log_warn("Unable to allocate memory, malloc failed");
			cdev_count--;
			return -1;
		}
		strncpy(thermal_zones[zone_count].cdevs[cdev_count].path, cmsg->path, cmsg->path_len);
		thermal_zones[zone_count].cdevs[cdev_count].nval = cmsg->nval;
		thermal_zones[zone_count].cdevs[cdev_count].nval = cmsg->nval;
		thermal_zones[zone_count].cdevs[cdev_count].critval = cmsg->critval;
		thermal_zones[zone_count].cdevs[cdev_count].step = cmsg->step;
		thermal_zones[zone_count].numCDevs = cdev_count + 1;
		return 0;
	}

	status_t BnThermalAPI::onTransact(uint32_t code, const Parcel& data,
					Parcel* reply, uint32_t flags) {
		IPCThreadState* self = IPCThreadState::self();
		thd_log_info("thermald onTransact: Calling MSG: PID=%d, UID=%d, code=%d",
				self->getCallingPid(), self->getCallingUid(), code);
		int temp;
		char * name;
		switch(code) {
			case SEND_PROFILE_START:
				CHECK_INTERFACE(IThermalAPI, data, reply);
				int profile;
				data.readInt32(&profile);
				reply->writeInt32(sendProfileStart(profile));
				return OK;
				break;
			case SEND_SENSOR_MSG:
				CHECK_INTERFACE(IThermalAPI, data, reply);
				struct ThermalSensorMessage smsg;
				data.readInt32(&temp);
				data.readInt32(&smsg.name_len);
				name = String8(data.readString16()).string();
				name[smsg.name_len] = '\0';
				smsg.name = (char *)malloc(smsg.name_len + 1);
				strncpy(smsg.name, name, smsg.name_len + 1);

				data.readInt32(&smsg.path_len);
				name = String8(data.readString16()).string();
				name[smsg.path_len] = '\0';
				smsg.path = (char *)malloc(smsg.path_len + 1);
				strncpy(smsg.path, name, smsg.path_len + 1);
				reply->writeInt32(sendSensorMsg(&smsg));
				return OK;
				break;
			case SEND_THERMAL_ZONE_MSG:
				CHECK_INTERFACE(IThermalAPI, data, reply);
				struct ThermalZoneMessage zmsg;
				data.readInt32(&temp);
				data.readInt32(&zmsg.len);
				name = String8(data.readString16()).string();
				name[zmsg.len] = '\0';
				zmsg.name = (char *)malloc(zmsg.len + 1);
				strncpy(zmsg.name, name, zmsg.len + 1);
				data.readInt32(&zmsg.psv);
				data.readInt32(&zmsg.max);
				reply->writeInt32(sendThermalZoneMsg(&zmsg));
				return OK;
				break;
			case SEND_THROTTLE_MSG:
				CHECK_INTERFACE(IThermalAPI, data, reply);
				struct ThrottleMessage tmsg;
				data.readInt32(&temp);
				data.readInt32(&tmsg.len);
				name = String8(data.readString16()).string();
				name[tmsg.len] = '\0';
				tmsg.name = (char *)malloc(tmsg.len + 1);
				strncpy(tmsg.name, name, tmsg.len + 1);
				data.readInt32(&tmsg.val);
				reply->writeInt32(sendThrottleMsg(&tmsg));
				return OK;
				break;
			case SEND_CDEV_MSG:
				CHECK_INTERFACE(IThermalAPI, data, reply);
				struct ThermalCdevInfoMessage cmsg;
				data.readInt32(&temp);
				data.readInt32(&cmsg.name_len);
				name = String8(data.readString16()).string();
				name[cmsg.name_len] = '\0';
				cmsg.name = (char *)malloc(cmsg.name_len + 1);
				strncpy(cmsg.name, name, cmsg.name_len + 1);

				data.readInt32(&cmsg.path_len);
				name = String8(data.readString16()).string();
				name[cmsg.path_len] = '\0';
				cmsg.path = (char *)malloc(cmsg.path_len + 1);
				strncpy(cmsg.path, name, cmsg.path_len + 1);

				data.readInt32(&cmsg.nval);
				data.readInt32(&cmsg.critval);
				data.readInt32(&cmsg.step);
				reply->writeInt32(sendThermalCdevInfoMsg(&cmsg));
				default:break;
		}
		return NO_ERROR;
	}
}

namespace powerhal_api {

	IMPLEMENT_META_INTERFACE(ThermalAPI, META_INTERFACE_NAME);

	status_t ThermalAPI::sendPowerSaveMsg(PowerSaveMessage *psmsg) {
		bool on = psmsg->on != 0;
		thd_log_info("thermald powersavemsg: on=%d, percent=%d",
				psmsg->on, psmsg->percentage);
		power_save_cdev(on, psmsg->percentage);
		return THD_SUCCESS;
	}


	status_t BnThermalAPI::onTransact(uint32_t code, const Parcel& data,
					Parcel* reply, uint32_t flags) {
		IPCThreadState* self = IPCThreadState::self();
		thd_log_info("thermald onTransact: Calling MSG: PID=%d, UID=%d, code=%d",
				self->getCallingPid(), self->getCallingUid(), code);
		int temp;
		char * name;
		switch(code) {
		case SEND_POWER_SAVE_MSG:
			CHECK_INTERFACE(IThermalAPI, data, reply);
			struct PowerSaveMessage psmsg;
			data.readInt32(&psmsg.on);
			data.readInt32(&psmsg.percentage);
			reply->writeInt32(sendPowerSaveMsg(&psmsg));
			return OK;
			break;
		default:
			break;
		}
		return NO_ERROR;
	}

}
