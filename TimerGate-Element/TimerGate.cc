#include <click/config.h>
#include "TimerGate.hh"
#include <click/args.hh>
#include <click/error.hh>
#include <click/glue.hh>
#include <click/timestamp.hh>
#include <click/router.hh>
#include <click/vector.hh>

CLICK_DECLS

TimerGate::TimerGate()
    : _id(0), _isStart(true), _paintChance(100), _doPrint(false),
        _interval(60), _average(0), _count(0), _timer(this)
{

}

TimerGate::~TimerGate()
{

}

int 
TimerGate::configure(Vector<String> &conf, ErrorHandler *errh) 
{
    if(Args(conf, this, errh)
        .read_mp("ID", _id)
        .read_mp("START", _isStart)
        .read_mp("CHANCE", _paintChance)
        .read("PRINT", _doPrint)
        .read("INTERVAL", _interval)
        .complete() < 0 )
    return -1;

    if(_paintChance < 1 || _interval < 1 || _id < 1)
        return -1;

    return 0;
}

int 
TimerGate::initialize(ErrorHandler *errh) 
{
    for(int e=0 ; this->router()->elements().size() ; e++) {
        if(this->router()->elements()[e].class_name() == "TimerGate" ) {
            if(this->router()->elements()[e].get_id() == _id 
                && this->router()->elements()[e].get_type() != _isStart)
        }
    }
}

void 
TimerGate::run_timer(Timer *t) 
{

}

Packet*
TimerGate::simple_action(Packet *p)
{

}

CLICK_ENDDECLS
EXPORT_ELEMENT(TimerGate)