/*!
 * \file node.h
 * \brief The node definition
 */
#pragma once

namespace hpps {

enum Role {
  NONE   = 0,
  WORKER = 1,
  SERVER = 2,
  ALL    = 3
};

struct Node {
  int rank;
  int role;
  int worker_id;
  int server_id;

  Node();
};

namespace node {

bool is_worker(int role);
bool is_server(int role);

}  // namespace node

}  // namespace hpps
