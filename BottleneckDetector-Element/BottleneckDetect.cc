#include <click/config.h>
#include "BottleneckDetect.hh"
#include <click/args.hh>
#include <click/error.hh>
#include <click/glue.hh>
#include <click/timestamp.hh>
#include <click/router.hh>
#include <click/handlercall.hh>
#include <click/standard/scheduleinfo.hh>

CLICK_DECLS

BottleneckDetect::BottleneckDetect()
    : _timer(this), _task(this), _interval(1), _doPrint(false), 
    _rootnode(NULL), _treeBuilt(false), _first(true)
{
}

BottleneckDetect::~BottleneckDetect() 
{    
}

int 
BottleneckDetect::configure(Vector<String> &conf, ErrorHandler *errh) 
{
    String elementName = "";
    if(Args(conf, this, errh)
        .read_mp("ELEMENT", elementName)
        .read("INTERVAL", _interval)
        .read("PRINT", _doPrint)
        .complete() < 0 )
    return -1;

    if(_interval < 1) 
        return -1;

    if(this->router()->find(elementName) == NULL)
        return -1;
    else
        _baseElement = this->router()->find(elementName, errh);

    verbose("Configuration Complete");
    return 0;
}

int 
BottleneckDetect::initialize(ErrorHandler *errh) 
{
    //Click Stats > 0 needed to  get packets per port
    if(CLICK_STATS < 1) {
        verbose("For there to be any real point to this element, build clickos with stats >= 1");
        return -1;
    }

    ScheduleInfo::initialize_task(this, &_task, errh);

    Timestamp ts;
    ts.assign(_interval, 0);
    _timer.initialize(this);
    _timer.schedule_after(ts);

    return 0;
}

bool 
BottleneckDetect::run_task(Task *t) 
{
    RouterVisitor visitor = VisitElement(this->router());
    _rootnode = (treenode_t *)create_tree(_baseElement, visitor);
    _treeBuilt = true;

    verbose("Tree Build Complete");
    return true;
}

treenode_t* 
BottleneckDetect::create_tree(Element *e, VisitElement *visitor) 
{
    treenode_t *node = new treenode_t();

    if(e != _baseElement) {
        for(int d=0 ; d<datanodes.size() ; d++) {
            if (datanodes[d]->element == e) {
                node->data = datanodes[d];
                break;
            }
        }
    }

    if(node->data == NULL) {
        datanode_t *data = new datanode_t();
        node->data = data;
        data->element = e;
#if CLICK_STATS >= 1
        for(int p=0 ; p<e->ninputs() ; p++)
            data->npackets_in.push_back(p);
        for(int p=0 ; p<e->noutputs() ; p++)
            data->npackets_out.push_back(p);
#endif
        datanodes.push_back(node->data);
    }

    for(int p=0 ; p<e->noutputs() ; p++) {
        this->router()->visit_downstream(node->data->element , p, &visitor);
        node->child.push_back((treenode_t *)create_tree( visitor.getNext(), visitor ));
    }

    return node;
}

void 
BottleneckDetect::run_timer(Timer *t) 
{
    Timestamp ts;
    if(_treeBuilt) {
        for(int n=0 ; n<datanodes.size() ; n++)
            collect_data()
        if(_doPrint)
            print_data(_rootnode);
        if(_first)
            _first = false;
        ts.assign(_interval, 0);
        _timer.reschedule_after(ts);
    }
    ts.assign(1, 0);
    _timer.reschedule_after(ts);
}

void 
BottleneckDetect::collect_data(datanode_t *data) 
{
#if CLICK_STATS >= 1
    for(int p=0 ; p<node->data->element->ninputs() ; p++)
        data->npackets_in[p] = (data->element->input(p).npackets() - (_first)?data->element->input(p).npackets():data->npackets_in[p]);
    for(int p=0 ; p<node->data->element->noutputs() ; p++)
        data->npackets_out[p] = (data->element->output(p).npackets() - (_first)?data->element->output(p).npackets():data->npackets_out[p]);
#endif
#if CLICK_STATS >= 2
    //Fix me: Cycles does not work on MiniOS
#endif
}

void 
BottleneckDetect::print_data(treenode_t *node) 
{
    click_chatter("Element: %s", node->data->element->name().c_str());
#if CLICK_STATS >= 1
    for(int p=0 ; p<(node->data->npackets_in.size()) ; p++)
        click_chatter("  Input Port %d ::: %d packets", p, node->data->npackets_in[p]);
    for(int p=0 ; p<(node->data->npackets_out.size()) ; p++)
        click_chatter("  Output Port %d ::: %d packets", p, node->data->npackets_out[p]);
#endif
#if CLICK_STATS >= 2
    click_chater("  Cycles ::: %d", node->data->cycles);
#endif

    for(int c=0 ; c<node->child.size() ; c++)
        print_data(node->child[c]);
}

bool 
VisitElement::visit(Element *e, bool isoutput, int port, Element *fe, 
    int from_port, int distance) 
{
    _next = e;
    return false;
}

void
BottleneckDetect::verbose(String message)
{
    if(_doPrint)
        click_chatter("#!# BottleneckDetect Message: %s", message.c_str());
}

CLICK_ENDDECLS
EXPORT_ELEMENT(BottleneckDetect)