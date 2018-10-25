#include <click/config.h>
#include <click/glue.hh>
#include <click/args.hh>
#include <click/router.hh>
#include <click/handlercall.hh>
#include "SimpleMonitor.hh"

CLICK_DECLS

SimpleMonitor::SimpleMonitor() 
    : _doPrint(false), _count(0), _limit(1),
    _data(0), _elementName(""), _handlerName("") 
{
}

SimpleMonitor::~SimpleMonitor() 
{
}

int SimpleMonitor::configure(Vector<String> &conf, ErrorHandler *errh) 
{
    //Read Arguments
    if(Args(conf, this, errh)
        .read_mp("ELEMENT", _elementName)
        .read_mp("HANDLER", _handlerName)
        .read("FREQ", _limit)
        .read("PRINT", _doPrint)
        .complete() < 0)
    return -1;

    //Does the Element exist?

    //Does the Handler Exist?

    //Is Freq >= 1
    if(_limit < 1)
        return -1;
}

int SimpleMonitor::initialize() 
{

}

Packet *SimpleMonitor::simple_action(Packet *p) 
{

}

CLICK_ENDDECLS
EXPORT_ELEMENT(SimpleMonitor)