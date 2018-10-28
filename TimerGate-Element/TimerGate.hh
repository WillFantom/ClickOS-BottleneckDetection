#ifndef __TIMERGATE_HH
#define __TIMERGATE_HH

#include <click/timer.hh>
#include <click/routervisitor.hh>
#include <click/element.hh>

CLICK_DECLS

class TimerGate : public Element {
public:
    TimerGate();
    ~TimerGate();
    const char *class_name() const {return "TimerGate";}
    const char *port_count() const {return PORTS_1_1;}
    const char *processing() const {return AGNOSTIC;}
    int configure(Vector<String> &conf, ErrorHandler *errh);
    int initialize(ErrorHandler *errh);
    Packet *simple_action(Packet *);
    void run_timer(Timer *t);
    int get_id() {return _id;}

private:
    Timer _timer;
    bool _isStart;
    bool _doPrint;
    int _id;
    int _paintChance;
    int _interval;
    int _average;
    int _count;

};

CLICK_ENDDECLS
#endif