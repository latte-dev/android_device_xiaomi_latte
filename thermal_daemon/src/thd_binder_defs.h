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

#ifndef __THD_BINDER_DEFS__
#define __THD_BINDER_DEFS__

namespace thermal_api {

	const int MAX_CDEVS = 15;
	const int MAX_SENSORS = 10;
	const int HYST = 0;
	struct ThermalSensorMessage {
	int name_len;
	char *name;
	int path_len;
	char *path;
	};

	struct ThermalZoneMessage {
		int len;
		char *name;
		int psv;
		int max;
	};

	struct ThrottleMessage {
		int len;
		char *name;
		int val;
	};

	struct ThermalCdevInfoMessage {
		int name_len;
		char *name;
		int path_len;
		char *path;
		int nval;
		int critval;
		int step;
	};

	struct CoolingDevice {
		char *name;
		char *path;
		int nval;
		int critval;
		int step;
	};

	struct ThermalSensor {
		char *name;
		char *path;
	};

	struct ThermalZone {
		char *name;
		int psv;
		int max;
		int numSensors;
		int numCDevs;
		struct ThermalSensor sensors[MAX_SENSORS];
		struct CoolingDevice cdevs[MAX_CDEVS];
	};

	// Communication with ituxd
	typedef struct {
		const char *source;
		const char *sub_string;
	} substitute_string_t;
	extern substitute_string_t substitute_strings[];
}

namespace powerhal_api {
	struct PowerSaveMessage {
		int on;
		int percentage;
	};
}
#endif //__THD_BINDER_DEFS__

