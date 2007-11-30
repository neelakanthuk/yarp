// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2007 Paul Fitzpatrick
 * CopyPolicy: Released under the terms of the GNU GPL v2.0.
 *
 */


/*
 *
 * Confusingly, this is NOT the source of the program currently 
 * called "yarpview"!  That is in src/yarp-imageviewer.
 *
 */


#include <yarp/os/all.h>
#include <yarp/sig/all.h>
#include <yarp/dev/all.h>

using namespace yarp::os;
using namespace yarp::sig;
using namespace yarp::dev;


class Monitor : public Thread {
public:
    IService *service;
    PolyDriver *closer;

    virtual void run() {
        bool ok = true;
        while (ok) {
            ok = service->updateService();
            if (!ok) {
                printf("Monitor alerted of shutdown\n");
                closer->close();
            } 
        }
    }
};


int main(int argc, char *argv[]) {
    Network yarp;

    PolyDriver source, sink;


    Property input;
    input.fromCommand(argc,argv);

    ConstString portName = input.check("port",
                                       Value("/yarpview"),
                                       "name of port").asString();
    int width = input.check("w",
                            Value(128),
                            "width of viewer").asInt();
    int height = input.check("h",
                             Value(128),
                             "height of viewer").asInt();

    // common shortcut
    if (argc==2) {
        portName = argv[1];
    }

    // actual useful configuration - port to wxsdl
    Property pSource;
    pSource.put("device","remote_grabber");
    pSource.put("local",portName.c_str());
    source.open(pSource);

    Property pSink;
    pSink.put("device","wxsdl");
    pSink.put("w",width);
    pSink.put("h",height);
    sink.open(pSink);

    IFrameGrabberImage *iSource;
    source.view(iSource);
    IFrameWriterImage *iSink;
    sink.view(iSink);
    IService *iThread;
    sink.view(iThread);

    bool done = false;

    if (iSource==NULL||iSink==NULL) {
        if (iSource!=NULL) {
            printf("Cannot find image source\n");
        }
        if (iSink!=NULL) {
            printf("Cannot find image sink\n");
        }
        done = true;
    }

    //Time::delay(1);
    //Network::connect("/grabber","/foo");

    Monitor monitor;
    monitor.service = iThread;
    monitor.closer = &source;
    if (iThread!=NULL) {
        printf("Starting monitor\n");
        monitor.start();
    }

    ImageOf<PixelRgb> image;
    while (!done) {
        bool ok = iSource->getImage(image);
        if (ok) {
            ok = iSink->putImage(image);
        } 
        if (!ok) {
            done = true;
        }
    }

    printf("Stopping monitor\n");
    monitor.stop();

    sink.close();
    source.close();

    return 0;
}

