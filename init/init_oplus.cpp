/*
 * Copyright (C) 2022-2023 The LineageOS Project
 * SPDX-License-Identifier: Apache-2.0
 */

#include <android-base/logging.h>
#include <android-base/properties.h>
#include <cstdlib>
#include <string.h>

#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/_system_properties.h>

using android::base::GetProperty;
using std::string;

/*
 * SetProperty does not allow updating read only properties and as a result
 * does not work for our use case. Write "OverrideProperty" to do practically
 * the same thing as "SetProperty" without this restriction.
 */
void OverrideProperty(const char* name, const char* value) {
    size_t valuelen = strlen(value);

    prop_info* pi = (prop_info*)__system_property_find(name);
    if (pi != nullptr) {
        __system_property_update(pi, value, valuelen);
    } else {
        __system_property_add(name, strlen(name), value, valuelen);
    }
}

// list of partitions to override props
static const string source_partitions[] = {
    "", "bootimage.", "odm.", "product.",
    "system.", "system_ext.", "vendor.", "vendor_dlkm.", "system_dlkm."
};

void setProductRO(const string &prop, const string &value) {
    string prop_name;
    for (const string &source : source_partitions) {
        prop_name = "ro.product." + source + prop;
        OverrideProperty(prop_name.c_str(), value.c_str());
    }
}

/*
 * Only for read-only properties. Properties that can be wrote to more
 * than once should be set in a typical init script (e.g. init.oplus.hw.rc)
 * after the original property has been set.
 */
void vendor_load_properties() {
    auto hw_region_id = std::stoi(GetProperty("ro.boot.hw_region_id", "0"));
    auto prjname = std::stoi(GetProperty("ro.boot.prjname", "0"));

    switch (hw_region_id) {
        case 0: // aston IN/EU
         setProductRO("model", "CPH2585");
         setProductRO("name", "12R");
            break;
        case 21:
            if (prjname == 22811) { 
                // salami CN
                setProductRO("device", "0P591BL1");
                setProductRO("model", "PHB110");
                setProductRO("name", "11");
            } else if (prjname == 23801) { 
                // aston CN
            setProductRO("device", "0P5CF9L1");
            setProductRO("model", "PJE110");
            setProductRO("name", "Ace 3");
            } else if (prjname == 22861) { 
                // salami IN
                setProductRO("model", "CPH2447");
                setProductRO("name", "11");
            }
            break;
        case 22: 
        // salami EU
        setProductRO("model", "CPH2449");
        setProductRO("name", "11");
            break;
        case 23:
            if (prjname == 23861) { 
                // aston NA
                setProductRO("model", "CPH2611");
                setProductRO("name", "12R");
            } else {   
                // salami NA
                setProductRO("model", "CPH2451");
                setProductRO("name", "11");
            }
            break;
        default:
            LOG(ERROR) << "Unexpected region ID: " << hw_region_id;
    }
}
