// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


#include <yarp/os/Semaphore.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/impl/SemaphoreImpl.h>

using namespace yarp::os::impl;
using namespace yarp::os;

Semaphore::Semaphore(int initialCount) {
    implementation = new SemaphoreImpl(initialCount);
    YARP_ASSERT(implementation!=NULL);
}

Semaphore::~Semaphore() {
    if (implementation!=NULL) {
        delete ((SemaphoreImpl*)implementation);
        implementation = NULL;
    }
}

void Semaphore::wait() {
    ((SemaphoreImpl*)implementation)->wait();
}

bool Semaphore::check() {
    return ((SemaphoreImpl*)implementation)->check();
}

void Semaphore::post() {
    ((SemaphoreImpl*)implementation)->post();
}

