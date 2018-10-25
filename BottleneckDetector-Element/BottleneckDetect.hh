#ifndef __BOTTLENECKDETECT_HH
#define __BOTTLENECKDETECT_HH

#include <click/timer.hh>
#include <click/task.hh>
#include <click/vector.hh>
#include <click/routervisitor.hh>
#include <click/element.hh>

CLICK_DECLS

class BottleneckDetect : public Element {
public:
    BottleneckDetect();
    ~BottleneckDetect();

    //Required
    const char *class_name() const {return "BottleneckDetect";}

    //Init
    int configure(Vector<String> &conf, ErrorHandler *errh);
    int initialize(ErrorHandler *errh);

    //Run on interval (check packets)
    void run_timer(Timer *t);

    //Run Task (build Tree)
    bool run_task(Task *t);

    //RouterVisitor: Required to visit the next elements
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
private:

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

    VisitElement _visitor;
    Element *_baseElement;
    treenode_t *_rootnode;
    Timer _timer;
    Task _task;
    int _interval;
    bool _doPrint;
    bool _treeBuilt;

    void* create_tree(Element *e);
    bool collect_data(treenode_t *node);
    void print_data(treenode_t *node);

};

CLICK_ENDDECLS
#endif