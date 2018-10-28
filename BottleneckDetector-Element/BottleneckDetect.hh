#ifndef __BOTTLENECKDETECT_HH
#define __BOTTLENECKDETECT_HH

#include <click/timer.hh>
#include <click/task.hh>
#include <click/vector.hh>
#include <click/routervisitor.hh>
#include <click/element.hh>

CLICK_DECLS

//Node Types
typedef struct datanode {
        Element *element;
#if CLICK_STATS >= 1
        Vector<int> npackets_in;
        Vector<int> npackets_out;
#endif
#if CLICK_STATS >= 2
        int cycles;
#endif
} datanode_t;

typedef struct treenode {
    datanode_t *data;
    Vector<struct treenode*> child;
} treenode_t;

//Get Next Element Router Visitor
class VisitElement : public ElementTracker {
public:
    VisitElement(Router *router)
	    : ElementTracker(router)
    {}
    ~VisitElement()
    {}

    bool visit(Element *e, bool isoutput, int port, Element *fe, int from_port, int distance);
    Element *getNext() {return _next;}

private:
    Element *_next;
};

class BottleneckDetect : public Element {
public:
    BottleneckDetect();
    ~BottleneckDetect();

    const char *class_name() const {return "BottleneckDetect";}
    int configure(Vector<String> &conf, ErrorHandler *errh);
    int initialize(ErrorHandler *errh);
    void run_timer(Timer *t);
    bool run_task(Task *t);

private:

    VisitElement _visitor;
    Element *_baseElement;
    treenode_t *_rootnode;
    Vector<datanode_t *> datanodes;
    Timer _timer;
    Task _task;
    int _interval;
    bool _doPrint;
    bool _first;
    bool _treeBuilt;

    treenode_t* create_tree(Element *e);
    bool collect_data(treenode_t *node);
    void print_data(treenode_t *node);
    void verbose(String message);
};

CLICK_ENDDECLS
#endif