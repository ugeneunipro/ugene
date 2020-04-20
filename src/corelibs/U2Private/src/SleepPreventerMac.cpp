/**
 * UGENE - Integrated Bioinformatics Tools.
 * Copyright (C) 2008-2020 UniPro <ugene@unipro.ru>
 * http://ugene.net
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#import "SleepPreventerMac.h"
#import <IOKit/pwr_mgt/IOPMLib.h>

namespace U2 {

#if MAC_OS_X_VERSION_MAX_ALLOWED >= 1060

class SleepPreventerMac::Private {
public:
    IOReturn doCapture() {
        CFStringRef reasonForActivity = CFSTR("New UGENE task is started");
        return IOPMAssertionCreateWithName(kIOPMAssertionTypeNoIdleSleep,
                             kIOPMAssertionLevelOn, reasonForActivity, &assertionID);
    }

    IOReturn doRelease() {
        return IOPMAssertionRelease(assertionID);
    }

private:
    IOPMAssertionID assertionID;
};

SleepPreventerMac::SleepPreventerMac() {
    refCount = 0;
    d = new Private;
}

SleepPreventerMac::~SleepPreventerMac() {
    if (isActive) {
        d->doRelease();
    }
    delete d;
}

void SleepPreventerMac::capture() {
    refCount++;
    if (!isActive) {
        IOReturn res = d->doCapture();
        if (res == kIOReturnSuccess) {
            isActive = true;
        }
    }
}

void SleepPreventerMac::release() {
    refCount--;
    if (refCount == 0 && isActive) {
        d->doRelease();
        isActive = false;
    }
}

#endif

}   // namespace U2
