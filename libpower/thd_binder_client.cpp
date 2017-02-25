/*
 * Copyright (C) 2014 Intel Corporation. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#include <thd_binder_client.h>

using namespace powerhal_api;

IMPLEMENT_META_INTERFACE(ThermalAPI, META_INTERFACE_NAME);


BpThermalAPI::BpThermalAPI(const sp<IBinder>& impl)
    : BpInterface<IThermalAPI>(impl) {
}

status_t BpThermalAPI::sendPowerSaveMsg(struct PowerSaveMessage *tmsg) {
    Parcel data, reply;
    data.writeInterfaceToken(IThermalAPI::getInterfaceDescriptor());
    data.write((const void*)tmsg, sizeof(*tmsg));
    remote()->transact(SEND_POWER_SAVE_MSG, data, &reply);
    return reply.readInt32();
}
