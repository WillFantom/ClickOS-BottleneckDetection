# ClickOS Bottleneck Detection

### Bottleneck Detect Element
This element is designed to find bottlenecks in a click configuration by analysing the configuration and then, on a given interval, check how many packets each element in the pipeline processes. A high delta in pps between two elements can either represent that an element is a bottleneck or that an element is generating packets, dependent on sign.
To use in a configuration, simple add the element anywhere, ensuring to mark the root element of the configuration, in most cases, this is a FromDevice/PollDevice element.

### Simple Monitor
This is intended for use with the MiniOS build of Click as there is no support for the Script element within that build. SimpleMonitor allows for the collection and storage (in main memory) of handler data from elements, including those that require CLICK_STATS > 0. The element can perform an action, such as a simple print, when x number of packets have passed through it. Due to this, SimpleMonitor must be placed mid flow.

### Timed Monitor
Similar to simple monitor, other than an action is pperformed once x number of seconds have passed. This allows the element to not be placed in the flow, however, requires a timer event. The performace impact of this has not been tested.

