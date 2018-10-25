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
    : _timer(this), _task(this), _interval(1), _doPrint(false), _rootnode(NULL), _treeBuilt(false), _visitor(this->router())
{}

BottleneckDetect::~BottleneckDetect()
{}

int BottleneckDetect::configure(Vector<String> &conf, ErrorHandler *errh) {

    //Parse Arguments
    String elementName = "";
    if(Args(conf, this, errh)
        .read_mp("ELEMENT", elementName)
        .read("INTERVAL", _interval)
        .read("PRINT", _doPrint)
        .complete() < 0 )
    return -1;

    //Valid Interval?
    if(_interval < 1) 
        return -1;

    //Check Starter Element is in the router config
    if(this->router()->find(elementName) == NULL)
        return -1;
    else
        _baseElement = this->router()->find(elementName, errh);

    return 0;
}

int BottleneckDetect::initialize(ErrorHandler *errh)
{
    if(_doPrint)
        click_chatter("## Init ##");

    //Click Stats > 0 needed to get packets per port
#if CLICK_STATS < 1
    click_chatter("Bottleneck Detect will be pointless without stats >= 1, currently stats < 1");
#endif
    
    //Router Visitor
    _visitor = VisitElement(this->router());

    //Run build tree task
    ScheduleInfo::initialize_task(this, &_task, errh);

    //Schedule Data Collection
    Timestamp ts;
    ts.assign(_interval, 0);
    _timer.initialize(this);
    _timer.schedule_after(ts);

    return 0;
}

bool BottleneckDetect::run_task(Task *t)
{
    //Create tree
    _rootnode = (treenode_t *)create_tree(_baseElement);
    _treeBuilt = true;

    if(_doPrint)
        click_chatter("## Tree Built ##");
    return true;
}

void* BottleneckDetect::create_tree(Element *e)
{
    if(_doPrint)
        click_chatter("## Building Tree ##");

    treenode_t *node = malloc(sizeof(treenode_t));
    datanode_t *data = malloc(sizeof(datanode_t));
    node->data = data;
    data->element = e;

    //Init Vectors
#if CLICK_STATS >= 1
    for (int p=0 ; p<e->ninputs() ; p++)
        data->npackets_in.push_back(p);
    for (int p=0 ; p<e->noutputs() ; p++)
        data->npackets_out.push_back(p);
#endif 

    //Check Children (Recursive, sorry)
    for (int p=0 ; p<e->noutputs() ; p++) {
        this->router()->visit_downstream(data->element , p, &_visitor);
        node->child.push_back((treenode_t *)create_tree( _visitor.getNext() ));
    }

    return node;
}

void BottleneckDetect::run_timer(Timer *t)
{
    Timestamp ts;
    if(_treeBuilt) {
        if(collect_data(_rootnode)) {
            if (_doPrint)
                print_data(_rootnode);
        } else {
            click_chatter("## Monitor: Failed to build element tree");
        }
        ts.assign(_interval, 0);
        _timer.reschedule_after(ts);
    }
    ts.assign(1, 0);
    _timer.reschedule_after(ts);
}

bool BottleneckDetect::collect_data(treenode_t *node)
{
    if(_doPrint)
        click_chatter("## Collecting Data ##");

#if CLICK_STATS >= 1
    for (int p=0 ; p<node->data->element->ninputs() ; p++)
        node->data->npackets_in[p] = (node->data->element->input(p).npackets() - node->data->npackets_in[p]);
    for (int p=0 ; p<node->data->element->noutputs() ; p++)
        node->data->npackets_out[p] = (node->data->element->output(p).npackets() - node->data->npackets_out[p]);
#endif
#if CLICK_STATS >= 2
    //Fix me: Cycles does not work on MiniOS
#endif

    //Go Recursive
    for(int c=0 ; c<node->child.size() ; c++)
        collect_data(node->child[c]);
}

void BottleneckDetect::print_data(treenode_t *node) 
{

}

bool BottleneckDetect::VisitElement::visit(Element *e, bool isoutput, int port, Element *fe, int from_port, int distance)
{
    _next = e;
    return false;
}

CLICK_ENDDECLS
EXPORT_ELEMENT(BottleneckDetect)