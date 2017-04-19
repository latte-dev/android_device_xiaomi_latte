/*
 * android_main.cpp: Thermal Daemon entry point tuned for Android
 *
 * Copyright (C) 2013 Intel Corporation. All rights reserved.
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
 * This is the main entry point for thermal daemon. This has main function
 * which parses command line arguments, setup logs and starts thermal
 * engine.
 */

#include "thd_binder_server.h"
#include "thermald.h"
#include "thd_preference.h"
#include "thd_engine.h"
#include "thd_engine_default.h"
#include "thd_parse.h"
#include "thd_trip_point.h"

// for AID_* constatns
#include <private/android_filesystem_config.h>

// getdtablesize() is removed from bionic/libc in LPDK*/
// use POSIX alternative available. Otherwise fail
#  ifdef _POSIX_OPEN_MAX
#   define   getdtablesize()	(_POSIX_OPEN_MAX)
# endif

// poll mode
int thd_poll_interval = 4; //in seconds
static int pid_file_handle;

// Thermal engine
cthd_engine *thd_engine;

// Default engine mode
engine_mode_t engine_mode = THERMALD;

// Strings from iTUX should be substituted with thermald strings
thermal_api::substitute_string_t substitute_strings[] = { { "RAPL", "intel-rapl" },
		{ "SOC", "soc_dts0" }, { NULL, NULL } };
// Stop daemon
static void daemonShutdown() {
	if (pid_file_handle)
		close(pid_file_handle);
	thd_engine->thd_engine_terminate();
	sleep(1);
	delete thd_engine;
}

// signal handler
static void signal_handler(int sig) {
	switch (sig) {
	case SIGHUP:
		thd_log_warn("Received SIGHUP signal. \n");
		break;
	case SIGINT:
	case SIGTERM:
		thd_log_info("Daemon exiting \n");
		daemonShutdown();
		exit(EXIT_SUCCESS);
		break;
	default:
		thd_log_warn("Unhandled signal %s\n", strsignal(sig));
		break;
	}
}

static void daemonize(char *rundir, char *pidfile) {
	int pid, sid, i;
	char str[10];
	struct sigaction sig_actions;
	sigset_t sig_set;

	if (getppid() == 1) {
		return;
	}
	sigemptyset(&sig_set);
	sigaddset(&sig_set, SIGCHLD);
	sigaddset(&sig_set, SIGTSTP);
	sigaddset(&sig_set, SIGTTOU);
	sigaddset(&sig_set, SIGTTIN);
	sigprocmask(SIG_BLOCK, &sig_set, NULL);

	sig_actions.sa_handler = signal_handler;
	sigemptyset(&sig_actions.sa_mask);
	sig_actions.sa_flags = 0;

	sigaction(SIGHUP, &sig_actions, NULL);
	sigaction(SIGTERM, &sig_actions, NULL);
	sigaction(SIGINT, &sig_actions, NULL);

	pid = fork();
	if (pid < 0) {
		/* Could not fork */
		exit(EXIT_FAILURE);
	}
	if (pid > 0) {
		thd_log_info("Child process created: %d\n", pid);
		exit(EXIT_SUCCESS);
	}
	umask(027);

	sid = setsid();
	if (sid < 0) {
		exit(EXIT_FAILURE);
	}
	/* close all descriptors */
	for (i = getdtablesize(); i >= 0; --i) {
		close(i);
	}

	i = open("/dev/null", O_RDWR);
	dup(i);
	dup(i);
	chdir(rundir);

	pid_file_handle = open(pidfile, O_RDWR | O_CREAT, 0600);
	if (pid_file_handle == -1) {
		/* Couldn't open lock file */
		thd_log_info("Could not open PID lock file %s, exiting\n", pidfile);
		exit(EXIT_FAILURE);
	}
	/* Try to lock file */
#ifdef LOCKF_SUPPORT
	if (lockf(pid_file_handle, F_TLOCK, 0) == -1) {
#else
		if (flock(pid_file_handle,LOCK_EX|LOCK_NB) < 0) {
#endif
		/* Couldn't get lock on lock file */
		thd_log_info("Couldn't get lock file %d\n", getpid());
		exit(EXIT_FAILURE);
	}
	thd_log_info("Thermal PID %d\n", getpid());
	snprintf(str, sizeof(str), "%d\n", getpid());
	write(pid_file_handle, str, strlen(str));
}

static void print_usage(FILE* stream, int exit_code) {
	fprintf(stream, "Usage:  thermal-daemon options [ ... ]\n");
	fprintf(stream, "  --help Display this usage information.\n"
			"  --version Show version.\n"
			"  --no-daemon No daemon.\n"
			"  --poll-interval poll interval 0 to disable.\n"
			"  --exclusive_control to act as exclusive thermal controller. \n");

	exit(exit_code);
}

void set_engine_mode() {
	char value[PROPERTY_VALUE_MAX];
	int length = property_get("persist.thermal.mode", value, "thermald");
	std::string mode(value);

	if (mode == "itux") {
		::engine_mode = ITUX;
		thd_log_info("thermald is in ITUX mode");
	} else if (mode == "ituxd") {
		::engine_mode = ITUXD;
		thd_log_info("thermald is in ITUXD mode");
	} else if (mode == "none") {
		::engine_mode = NONE;
	}
}

static char* substitute_string(char *name) {
	int i = 0;
	thermal_api::substitute_string_t *list = substitute_strings;
	while (list[i].source) {
		if (!strcmp(name, list[i].source)) {
			thd_log_info("substituted string:%s", list[i].sub_string);
			return list[i].sub_string;
		}
		i++;
	}
	return name;
}

void throttle_cdev(char * cdev_name, int cdev_val) {
	cdev_name = substitute_string(cdev_name);
	thd_engine->throttle_cdev_ituxd(cdev_name, cdev_val);
}

void power_save_cdev(bool on, int percentage) {
	thd_engine->throttle_cdevs(on, percentage);
}

// Profile Change
static void initialize_new_profile() {
	// initialize thd params
	thd_engine->set_engine_pause(true);
	thd_engine->set_control_mode(EXCLUSIVE);
	thd_engine->set_preference(PREF_PERFORMANCE);
	thd_engine->set_poll_interval(thd_poll_interval);

	thd_engine->read_sensors_new_profile();
	thd_engine->read_cdev_new_profile();
	thd_engine->read_zones_new_profile();
	thd_engine->set_engine_pause(false);

	return 0;
}

static void get_zones_from_ituxd(int numZones, std::vector<thermal_api::ThermalZone> thermal_zones) {
	int i, j, k, zone_count, cdev_count, sensor_count, trip_count = 0;

	if (::engine_mode == THERMALD) {
		thd_log_warn("In THERMALD mode but zone mapping info received from ITUX\n");
		return;
	}

	for (i = 0; i < numZones; i++) {
		thermal_zones[i].name = substitute_string(thermal_zones[i].name);
		for (j = 0; j < thermal_zones[i].numCDevs; j++) {
			thermal_zones[i].cdevs[j].name =
				substitute_string(thermal_zones[i].cdevs[j].name);
		}
                for (k = 0; k < thermal_zones[i].numSensors; k++) {
                        thermal_zones[i].sensors[k].name =
                                substitute_string(thermal_zones[i].sensors[k].name);
                }
	}

	// clear data before initiatlization
	thd_engine->sensors_itux.clear();
	thd_engine->cdevs_itux.clear();
	thd_engine->zones_itux.clear();

	sensor_relate_t sensor_rel = SENSOR_INDEPENDENT;
	for (zone_count = 0; zone_count < numZones; zone_count++) {

		cthd_zone *zone = new cthd_sysfs_zone(zone_count, "/sys/class/thermal");
		zone->set_zone_type(thermal_zones[zone_count].name);

		for (sensor_count = 0; sensor_count < thermal_zones[zone_count].numSensors; sensor_count++) {
			cthd_sensor *sensor = new cthd_sensor(zone_count,
					thermal_zones[zone_count].sensors[sensor_count].path,
					thermal_zones[zone_count].sensors[sensor_count].name,
					SENSOR_TYPE_RAW);
			thd_engine->sensors_itux.push_back(sensor);
			// Add to list for this specific zone
			zone->bind_sensor(sensor);
		}
		// For each threshold of zone, from itux only two
		cthd_trip_point trip_pt_psv(trip_count, PASSIVE, thermal_zones[zone_count].psv,
					thermal_api::HYST, zone_count, zone_count, PARALLEL);
		zone->add_trip(trip_pt_psv);
		trip_count++;
		cthd_trip_point trip_pt_max(trip_count, MAX, thermal_zones[i].max,
					thermal_api::HYST, zone_count, zone_count, PARALLEL);
		zone->add_trip(trip_pt_max);
		for (cdev_count = 0; cdev_count < thermal_zones[zone_count].numCDevs; cdev_count++) {
			// if cooling device is RAPL then use RAPL cooling device
			cthd_cdev *cdev =
				new cthd_cdev(zone_count,
						thermal_zones[zone_count].cdevs[cdev_count].path);
			cdev->set_cdev_type(thermal_zones[zone_count].cdevs[cdev_count].name);
			cdev->set_min_state(thermal_zones[zone_count].cdevs[cdev_count].nval);
			cdev->set_max_state(thermal_zones[zone_count].cdevs[cdev_count].critval);
			cdev->set_inc_dec_value(thermal_zones[zone_count].cdevs[cdev_count].step);
			thd_engine->cdevs_itux.push_back(cdev);
			// Map cdev to the zone for all the trip poinats
			int trip_temp_psv = thermal_zones[zone_count].psv;
			int trip_temp_max = thermal_zones[zone_count].max;
			int influence = 100;
			int period = 30;
			// For passive
			zone->bind_cooling_device(PASSIVE, trip_temp_psv, cdev, influence, period);
			// for MAX
			zone->bind_cooling_device(MAX, trip_temp_max, cdev, influence, period);
		}
		thd_engine->zones_itux.push_back(zone);
	}

	// call for profile change once all data is received
	initialize_new_profile();
}

int main(int argc, char *argv[]) {
	int c;
	int option_index = 0;
	bool no_daemon = false;
	bool exclusive_control = false;
	bool test_mode = false;
	bool is_privileged_user = false;

	const char* const short_options = "hvnp:de";
	static struct option long_options[] = {
			{ "help", no_argument, 0, 'h' },
			{ "version", no_argument, 0, 'v' },
			{ "no-daemon", no_argument, 0, 'n' },
			{ "poll-interval", required_argument, 0, 'p' },
			{ "exclusive_control", no_argument, 0, 'e' },
			{ "test-mode", no_argument, 0, 't' },
			{ NULL, 0, NULL, 0 } };

	if (argc > 1) {
		while ((c = getopt_long(argc, argv, short_options, long_options,
				&option_index)) != -1) {
			int this_option_optind = optind ? optind : 1;
			switch (c) {
			case 'h':
				print_usage(stdout, 0);
				break;
			case 'v':
				fprintf(stdout, "1.1\n");
				exit(EXIT_SUCCESS);
				break;
			case 'n':
				no_daemon = true;
				break;
			case 'p':
				thd_poll_interval = atoi(optarg);
				break;
			case 'e':
				exclusive_control = true;
				break;
			case 't':
				test_mode = true;
				break;
			case -1:
			case 0:
				break;
			default:
				break;
			}
		}
	}

	is_privileged_user = (getuid() == 0) || (getuid() == AID_SYSTEM);
	if (!is_privileged_user && !test_mode) {
		thd_log_error("You do not have correct permissions to run thermal dameon!\n");
		exit(1);
	}

	if ((c = mkdir(TDRUNDIR, 0755)) != 0) {
		if (errno != EEXIST) {
			fprintf(stderr, "Cannot create '%s': %s\n", TDRUNDIR,
					strerror(errno));
			exit(EXIT_FAILURE);
		}
	}
	mkdir(TDCONFDIR, 0755); // Don't care return value as directory
	if (!no_daemon) {
		daemonize((char *) "/tmp/", (char *) "/tmp/thermald.pid");
	} else
		signal(SIGINT, signal_handler);

	thd_log_info(
			"Linux Thermal Daemon is starting mode %d : poll_interval %d :ex_control %d\n",
			no_daemon, thd_poll_interval, exclusive_control);

	// Set engine mode
	set_engine_mode();

	thd_engine = new cthd_engine_default();
	if (exclusive_control)
		thd_engine->set_control_mode(EXCLUSIVE);

	thd_engine->set_poll_interval(thd_poll_interval);

	// Initialize thermald objects
	if (thd_engine->thd_engine_start(false) != THD_SUCCESS) {
		thd_log_error("thermald engine start failed:\n");
		exit(EXIT_FAILURE);
	}
#ifdef VALGRIND_TEST
	// lots of STL lib function don't free memory
	// when called with exit().
	// Here just run for some time and gracefully return.
	sleep(10);
	if (pid_file_handle)
		close(pid_file_handle);
	thd_engine->thd_engine_terminate();
	sleep(1);
	delete thd_engine;
#else
	for (;;) {
		if (thd_engine)
			thd_engine->reinspect_max();
		sleep(3600);
	}

	thd_log_info("Linux Thermal Daemon is exiting \n");
#endif
	return 0;
}
