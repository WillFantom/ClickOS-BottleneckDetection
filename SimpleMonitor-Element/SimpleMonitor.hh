#ifndef __SIMPLEMONTOR_HH
#define __SIMPLEMONTOR_HH

#include <click/element.hh>

CLICK_DECLS

class SimpleMonitor : public Element {

    public:
        SimpleMonitor();
        ~SimpleMonitor();

        const char *class_name() const { return "SimpleMonitor"; }
        const char *port_count() const { return PORTS_1_1; }
        int configure(Vector<String> &conf, ErrorHandler *errh);
        int initialize(ErrorHandler *errh);

        Packet *simple_action(Packet *p);

    private:
        bool _doPrint;
        int _count;
        int _limit;
        int _data;
        String _elementName;
        String _handlerName;

};

CLICK_ENDDECLS
#endif