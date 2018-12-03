/*!
 * \file node.cc
 * \brief The node defintion
 */
#include "hpps/frame/node.h"

namespace hpps {

Node::Node() : rank(-1), role(-1), worker_id(-1), server_id(-1) {}

namespace node {

bool is_worker(int role) { return (role & Role::WORKER) != 0; }
bool is_server(int role) { return (role & Role::SERVER) != 0; }

}  // namespace node

}  // namespace hpps
