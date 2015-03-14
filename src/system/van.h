#pragma once
#include "util/common.h"
#include "system/proto/node.pb.h"
#include "system/message.h"
namespace PS {

// Van sends (receives) packages to (from) a node
// The current implementation uses ZeroMQ
class Van {
 public:
  Van() { }
  ~Van();

  void Init(char*);

  void Disconnect(const Node&  node);
  bool Connect(const Node&  node);

  bool Send(const MessagePtr& msg, size_t* send_bytes);
  bool Recv(const MessagePtr& msg, size_t* recv_bytes);

  Node& my_node() { return my_node_; }
  Node& scheduler() { return scheduler_; };
 private:
  // bind to my port
  void Bind();

  Node ParseNode(const string& node_str);

  Node AssembleMyNode();
  bool IsScheduler() { return my_node_.role() == Node::SCHEDULER; }
  bool GetMonitorEvent(void *monitor, int *event, int *value);
  // scheduler: monitor the liveness of all other nodes
  // other nodes: monitor the liveness of the scheduler
  void Monitor();

  void *context_ = nullptr;
  void *receiver_ = nullptr;
  Node my_node_;
  Node scheduler_;
  std::unordered_map<NodeID, void *> senders_;

  DISALLOW_COPY_AND_ASSIGN(Van);

  // TODO move to postoffice::perf_monitor_
  // print statistic info
  void Statistic();
  std::unordered_map<NodeID, string> hostnames_;
  size_t sent_to_local_ = 0;
  size_t sent_to_others_ = 0;
  size_t received_from_local_ = 0;
  size_t received_from_others_ = 0;

  // for monitor
  std::unordered_map<int, NodeID> fd_to_nodeid_;
  std::mutex fd_to_nodeid_mu_;
  std::thread* monitor_thread_;
};

} // namespace PS
