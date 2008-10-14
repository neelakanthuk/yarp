// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


#include <yarp/os/impl/Carriers.h>
#include <yarp/os/impl/Logger.h>
#include <yarp/os/impl/TcpFace.h>
#include <yarp/os/impl/FakeFace.h>
#include <yarp/os/impl/TcpCarrier.h>
#include <yarp/os/impl/TextCarrier.h>
#include <yarp/os/impl/UdpCarrier.h>
#include <yarp/os/impl/McastCarrier.h>
#include <yarp/os/impl/ShmemCarrier.h>
#include <yarp/os/impl/LocalCarrier.h>
#include <yarp/os/impl/NameserCarrier.h>
#include <yarp/os/impl/HttpCarrier.h>

using namespace yarp::os::impl;


static Logger carriersLog("Carriers", Logger::get());

Carriers Carriers::instance;

Carriers::Carriers() {
    delegates.push_back(new HttpCarrier());
    delegates.push_back(new NameserCarrier());
    delegates.push_back(new LocalCarrier());
    //delegates.push_back(new ShmemCarrier(1));
    delegates.push_back(new ShmemCarrier(2)); // new Alessandro version
    delegates.push_back(new TcpCarrier());
    delegates.push_back(new TcpCarrier(false));
    delegates.push_back(new McastCarrier());
    delegates.push_back(new UdpCarrier());
    delegates.push_back(new TextCarrier());
    delegates.push_back(new TextCarrier(true));
}

Carriers::~Carriers() {
    ACE_Vector<Carrier *>& lst = delegates;
    for (unsigned int i=0; i<lst.size(); i++) {
        delete lst[i];
    }
    lst.clear();
}

Carrier *Carriers::chooseCarrier(const String *name, const Bytes *header) {
    for (unsigned int i=0; i<delegates.size(); i++) {
        Carrier& c = *delegates[i];
        bool match = false;
        if (name!=NULL) {
            if ((*name) == c.getName()) {
                match = true;
            }
        }
        if (header!=NULL) {
            if (c.checkHeader(*header)) {
                match = true;
            }
        }
        if (match) {
            return c.create();
        }
    }
    ACE_DEBUG((LM_ERROR,"Could not find carrier %s", (name!=NULL)?name->c_str():"[bytes]"));
    //throw IOException("Could not find carrier");
    return NULL;
}


Carrier *Carriers::chooseCarrier(const String& name) {
    return getInstance().chooseCarrier(&name,NULL);
}

Carrier *Carriers::chooseCarrier(const Bytes& bytes) {
    return getInstance().chooseCarrier(NULL,&bytes);
}


Face *Carriers::listen(const Address& address) {
    // for now, only TcpFace exists - otherwise would need to manage 
    // multiple possibilities
    //YARP_DEBUG(carriersLog,"listen called");
    Face *face = NULL;
    if (address.getCarrierName() == String("fake")) {
        face = new FakeFace();
    }
    if (face == NULL) {
        face = new TcpFace();
    }
    bool ok = face->open(address);
    if (!ok) {
        delete face;
        face = NULL;
    }
    return face;
}


OutputProtocol *Carriers::connect(const Address& address) {
    TcpFace tcpFace;
    return tcpFace.write(address);
}


