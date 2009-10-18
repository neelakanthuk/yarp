// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */

#include <yarp/os/Value.h>

//#include "TestList.h"
#include <yarp/os/impl/UnitTest.h>

using namespace yarp::os::impl;
using namespace yarp::os;

class ValueTest : public UnitTest {
public:
    virtual String getName() { return "ValueTest"; }

    void checkCopy() {
        report(0,"check value copying");
        Value v;
        v.fromString("10");
        checkTrue(v.isInt(),"integer created");
        v.fromString("10.0");
        checkTrue(v.isDouble(),"floating point number created");
        v.fromString("(1 2 3)");
        checkTrue(v.isList(),"list created");

        Value v2(10);
        Value v3 = v2;
        checkEqual(v3.asInt(),10,"copy integer");
        Value v4 = v;
        checkTrue(v4.isList(),"list copied");
        checkEqual(v4.asList()->get(1).asInt(),2,"right integer present");

        Bottle b("(x 10) (y 42)");
        checkEqual(b.check("x",Value(5)).asInt(),10,"default not used");
        checkEqual(b.check("xx",Value(5)).asInt(),5,"default used");
    }

    virtual void runTests() {
        checkCopy();
    }
};

static ValueTest theValueTest;

UnitTest& getValueTest() {
    return theValueTest;
}
