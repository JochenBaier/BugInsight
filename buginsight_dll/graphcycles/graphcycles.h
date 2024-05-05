//Licensed to the Apache Software Foundation (ASF) under one
//or more contributor license agreements.  See the NOTICE file
//distributed with this work for additional information
//regarding copyright ownership.  The ASF licenses this file
//to you under the Apache License, Version 2.0 (the
//"License"); you may not use this file except in compliance
//with the License.  You may obtain a copy of the License at
//
//  http://www.apache.org/licenses/LICENSE-2.0
//
//Unless required by applicable law or agreed to in writing,
//software distributed under the License is distributed on an
//"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
//KIND, either express or implied.  See the License for the
//specific language governing permissions and limitations
//under the License.  

// GraphCycles detects the introduction of a cycle into a directed
// graph that is being built up incrementally.
//
// Nodes are identified by small integers.  It is not possible to
// record multiple edges with the same (source, destination) pair;
// requests to add an edge where one already exists are silently
// ignored.
//
// It is also not possible to introduce a cycle; an attempt to insert
// an edge that would introduce a cycle fails and returns false.
//
// GraphCycles uses no internal locking; calls into it should be
// serialized externally.

// Performance considerations:
//   Works well on sparse graphs, poorly on dense graphs.
//   Extra information is maintained incrementally to detect cycles quickly.
//   InsertEdge() is very fast when the edge already exists, and reasonably fast
//   otherwise.
//   FindPath() is linear in the size of the graph.
// The current implementation uses O(|V|+|E|) space.

#pragma once

#include <cstdint>
#include <thread>
#include <array>
#include <optional>

#include "synch_locks_held.hpp"
#include "attributes.hpp"
#include "leak_types.hpp"

class GraphCycles
{
public:
  GraphCycles(const bool& p_collect_callstacks);
  ~GraphCycles();

  // Return the id to use for ptr, assigning one if necessary.
  // Subsequent calls with the same ptr value will return the same id
  // until Remove().
  void GetId(void* ptr, uint64_t& p_id) const;

  // Remove "ptr" from the graph.  Its corresponding node and all
  // edges to and from it are removed.
  void RemoveNode(void* ptr) const;

  // Return the pointer associated with id, or nullptr if id is not
  // currently in the graph.
  void* Ptr(uint64_t id) const;

  void extra_infos(uint64_t id, std::thread::id& p_thread_id, uint8_t& p_hold_locks_count, std::array<lock_t, MAX_LOCKS_PER_THREAD>& p_hold_locks) const;

  // Attempt to insert an edge from source_node to dest_node.  If the
  // edge would introduce a cycle, return false without making any
  // changes. Otherwise add the edge and return true.
  bool InsertEdge(uint64_t source_node, uint64_t dest_node) const;

  // Remove any edge that exists from source_node to dest_node.
  void RemoveEdge(uint64_t source_node, uint64_t dest_node) const;

  // Return whether node exists in the graph.
  bool HasNode(uint64_t node) const;

  // Return whether there is an edge directly from source_node to dest_node.
  bool HasEdge(uint64_t source_node, uint64_t dest_node) const;

  // Return whether dest_node is reachable from source_node
  // by following edges.
  bool IsReachable(uint64_t source_node, uint64_t dest_node) const;

  TM_ATTRIBUTE_NOINLINE void UpdateStackTrace(uint64_t id, SynchLocksHeld& p_all_locks) const;

  // Find a path from "source" to "dest".  If such a path exists,
  // place the nodes on the path in the array path[], and return
  // the number of nodes on the path.  If the path is longer than
  // max_path_len nodes, only the first max_path_len nodes are placed
  // in path[].  The client should compare the return value with
  // max_path_len" to see when this occurs.  If no path exists, return
  // 0.  Any valid path stored in path[] will start with "source" and
  // end with "dest".  There is no guarantee that the path is the
  // shortest, but no node will appear twice in the path, except the
  // source and destination node if they are identical; therefore, the
  // return value is at most one greater than the number of nodes in
  // the graph.
  int FindPath(uint64_t source, uint64_t dest, int max_path_len,
    uint64_t path[]) const;

  // Set *ptr to the beginning of the array that holds the recorded
  // stack trace for id and return the depth of the stack trace.
  void GetStackTrace(uint64_t id, std::optional<callstack_t>& p_callstack) const;

  // Check internal invariants. Crashes on failure, returns true on success.
  // Expensive: should only be called from graphcycles_test.cc.
  bool CheckInvariants() const;

  // ----------------------------------------------------
  struct Rep;

  GraphCycles(const GraphCycles&)=delete;
  GraphCycles& operator=(const GraphCycles&)=delete;

  const bool m_collect_callstacks;

private:
  Rep* rep_;      // opaque representation
};


