/*
 * cthd_cdev_rapl.cpp: thermal cooling class implementation
 *	using RAPL
 * Copyright (C) 2012 Intel Corporation. All rights reserved.
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
 *
 * Author Name <Srinivas.Pandruvada@linux.intel.com>
 *
 */
#include "thd_cdev_rapl.h"
#include "thd_engine.h"

/* This uses Intel RAPL driver to cool the system. RAPL driver show
 * mas thermal spec power in max_state. Each state can compensate
 * rapl_power_dec_percent, from the max state.
 *
 */
void cthd_sysfs_cdev_rapl::set_curr_state(int state, int arg) {

	std::stringstream tc_state_dev;

	std::stringstream state_str;
	int new_state;
	unsigned long min_state_ul = (unsigned long) min_state;
	cdev_sysfs.write("enabled", "1");
	if (state < inc_dec_val) {
		curr_state = (phy_max < min_state_ul) ? 0 : min_state_ul;
		if (!ph_throttled) {
			cdev_sysfs.write("enabled", "0");
		}
		new_state = (phy_max < min_state_ul) ? phy_max : (phy_max - min_state_ul);
	} else {
		if (dynamic_phy_max_enable) {
			if (!calculate_phy_max() && !ph_throttled) {
				curr_state = state;
				return;
			}
		}
		new_state = phy_max - (unsigned long)state;
		curr_state = state;
	}
	state_str << new_state;
	thd_log_info("set cdev state index %d state %d wr:%d\n", index, state,
			new_state);
	tc_state_dev << "constraint_" << constraint_index << "_power_limit_uw";
	if (cdev_sysfs.write(tc_state_dev.str(), state_str.str()) < 0)
		curr_state = (state == 0) ? 0 : max_state;

}

void cthd_sysfs_cdev_rapl::ph_throttle(float percentage, bool onoff) {
	calculate_phy_max();
	cthd_cdev::ph_throttle(percentage, onoff);
}

void cthd_sysfs_cdev_rapl::set_curr_state_raw(int state, int arg) {
	std::stringstream state_str;
	std::stringstream tc_state_dev;
	int new_state;
	unsigned long min_state_ul = (unsigned long) min_state;
	if (state <= min_state)
		new_state = (phy_max <  min_state_ul) ? phy_max : (phy_max - min_state_ul);
	else {
		if (dynamic_phy_max_enable) {
			if (!calculate_phy_max()) {
				curr_state = state;
				return;
			}
		}
		new_state = phy_max - state;
	}
	curr_state = state;
	state_str << new_state;

	tc_state_dev << "constraint_" << constraint_index << "_power_limit_uw";
	if (cdev_sysfs.write(tc_state_dev.str(), state_str.str()) < 0)
		curr_state = (state == 0) ? 0 : max_state;

	thd_log_info("set cdev state raw index %d state %d wr:%d\n", index, state,
			new_state);

}

bool cthd_sysfs_cdev_rapl::calculate_phy_max() {
	if (dynamic_phy_max_enable) {
		unsigned int curr_max_phy;
		curr_max_phy = thd_engine->rapl_power_meter.rapl_action_get_power(domain);
		thd_log_info("curr_phy_max = %u \n", curr_max_phy);
		if (curr_max_phy < rapl_min_default_step)
			return false;
		if (phy_max < curr_max_phy) {
			phy_max = curr_max_phy;
			set_inc_dec_value(phy_max * (float) rapl_power_dec_percent / 100);
			max_state = phy_max;
			max_state -= (float) max_state * rapl_low_limit_percent / 100;
			save_phy_max();
			thd_log_info("PHY_MAX %lu, step %d, max_state %d\n", phy_max,
					inc_dec_val, max_state);
		}
	}

	return true;
}

void cthd_sysfs_cdev_rapl::load_phy_max() {
	std::stringstream filename;

	switch (domain) {
	case PACKAGE:
		filename << TDRUNDIR << "/" << "rapl_phy_max.package.conf";
		break;
	case DRAM:
		filename << TDRUNDIR << "/" << "rapl_phy_max.dram.conf";
		break;
	case CORE:
		filename << TDRUNDIR << "/" << "rapl_phy_max.core.conf";
		break;
	case UNCORE:
		filename << TDRUNDIR << "/" << "rapl_phy_max.uncore.conf";
		break;
	default:
		return;
	}

	std::ifstream ifs(filename.str().c_str(), std::ifstream::in);
	if (ifs.good()) {
		ifs >> phy_max;
		thd_log_info("read phy_max domain:%d as:%u\n", domain, phy_max);
		if (phy_max > 10000000) {
			phy_max = 0;
			ifs.close();
			save_phy_max();
			return;
		}
	}
	ifs.close();
}

void cthd_sysfs_cdev_rapl::save_phy_max() {
	std::stringstream filename;
	std::stringstream temp_str;

	switch (domain) {
	case PACKAGE:
		filename << TDRUNDIR << "/" << "rapl_phy_max.package.conf";
		break;
	case DRAM:
		filename << TDRUNDIR << "/" << "rapl_phy_max.dram.conf";
		break;
	case CORE:
		filename << TDRUNDIR << "/" << "rapl_phy_max.core.conf";
		break;
	case UNCORE:
		filename << TDRUNDIR << "/" << "rapl_phy_max.uncore.conf";
		break;
	default:
		return;
	}

	std::ofstream fout(filename.str().c_str());
	if (!fout.good()) {
		return;
	}
	temp_str << phy_max;
	fout << temp_str.str();
	fout.close();
}

int cthd_sysfs_cdev_rapl::get_curr_state() {
	return curr_state;
}

int cthd_sysfs_cdev_rapl::get_max_state() {

	return max_state;
}

int cthd_sysfs_cdev_rapl::update() {
	int i;
	std::stringstream temp_str;
	int _index = -1;
	unsigned long constraint_phy_max;

	for (i = 0; i < rapl_no_time_windows; ++i) {
		temp_str << "constraint_" << i << "_name";
		if (cdev_sysfs.exists(temp_str.str())) {
			std::string type_str;
			cdev_sysfs.read(temp_str.str(), type_str);
			if (type_str == "long_term") {
				_index = i;
				break;
			}
		}
	}
	if (_index < 0) {
		thd_log_info("powercap RAPL no long term time window\n");
		return THD_ERROR;
	}

	temp_str.str(std::string());
	temp_str << "constraint_" << _index << "_max_power_uw";
	if (!cdev_sysfs.exists(temp_str.str())) {
		thd_log_info("powercap RAPL no max power limit range %s \n",
				temp_str.str().c_str());
		return THD_ERROR;
	}
	if (cdev_sysfs.read(temp_str.str(), &phy_max) < 0) {
		thd_log_info("powercap RAPL invalid max power limit range \n");
		thd_log_info("Calculate dynamically phy_max \n");
		phy_max = 0;
		thd_engine->rapl_power_meter.rapl_start_measure_power();
		max_state = rapl_min_default_step;
		set_inc_dec_value(rapl_min_default_step);
		dynamic_phy_max_enable = true;
		load_phy_max();
		if (phy_max > rapl_min_default_step) {
			max_state = phy_max;
			max_state -= (float) max_state * rapl_low_limit_percent / 100;
		}
		return THD_SUCCESS;
	}

	std::stringstream temp_power_str;
	temp_power_str.str(std::string());
	temp_power_str << "constraint_" << _index << "_power_limit_uw";
	if (!cdev_sysfs.exists(temp_power_str.str())) {
		thd_log_info("powercap RAPL no  power limit uw %s \n",
				temp_str.str().c_str());
		return THD_ERROR;
	}
	if (cdev_sysfs.read(temp_power_str.str(), &constraint_phy_max) <= 0) {
		thd_log_info("powercap RAPL invalid max power limit range \n");
		constraint_phy_max = 0;
	}
	if (constraint_phy_max > phy_max) {
		thd_log_info(
				"Default constraint power limit is more than max power %lu:%lu\n",
				constraint_phy_max, phy_max);
		phy_max = constraint_phy_max;
	}
	thd_log_info("powercap RAPL max power limit range %lu \n", phy_max);

	set_inc_dec_value(phy_max * (float) rapl_power_dec_percent / 100);
	max_state = phy_max;
	max_state -= (float) max_state * rapl_low_limit_percent / 100;

	std::stringstream time_window;
	temp_str.str(std::string());
	temp_str << "constraint_" << _index << "_time_window_us";
	if (!cdev_sysfs.exists(temp_str.str())) {
		thd_log_info("powercap RAPL no time_window_us %s \n",
				temp_str.str().c_str());
		return THD_ERROR;
	}
	time_window << def_rapl_time_window;
	cdev_sysfs.write(temp_str.str(), time_window.str());

	std::stringstream enable;
	temp_str.str(std::string());
	temp_str << "enabled";
	if (!cdev_sysfs.exists(temp_str.str())) {
		thd_log_info("powercap RAPL no enabled %s \n", temp_str.str().c_str());
		return THD_ERROR;
	}
	cdev_sysfs.write(temp_str.str(), "0");

	thd_log_debug("RAPL max limit %d increment: %d\n", max_state, inc_dec_val);
	constraint_index = _index;
	set_pid_param(1000, 100, 10);
	//enable_pid();

	return THD_SUCCESS;
}
