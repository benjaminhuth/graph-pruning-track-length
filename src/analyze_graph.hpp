
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/reverse_graph.hpp>
#include <boost/graph/topological_sort.hpp>

#include <iostream>

#if 0
#define LOG(msg) std::cout << msg << std::endl;
#else
#define LOG(msg) /*nothing*/
#endif

using Graph = boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS>;
using Vi = std::vector<int>;

// helper function that creates a boost graph from two vectors of int
inline auto make_graph(const Vi& from, const Vi& to) {
    Graph g;
    for (size_t i = 0; i < from.size(); ++i) {
        boost::add_edge(from[i], to[i], g);
    }
    return g;
}


inline auto findMaxDistances(const Graph &g, const Vi nodeWeights) {
  Vi distance(boost::num_vertices(g), 1);
  
  // do topological sort with boost
  std::vector<Graph::vertex_descriptor> topoOrder(boost::num_vertices(g));
  boost::topological_sort(g, topoOrder.begin());

  for(auto vit = topoOrder.rbegin(); vit != topoOrder.rend(); ++vit) {
    auto v = *vit;
    LOG("On vtx " << v << " with dist " << distance.at(v));
    auto [it, end] = boost::adjacent_vertices(v, g);
    for (; it != end; ++it) {
      auto weight = nodeWeights.empty() ? 1 : nodeWeights.at(*it);
      auto newDist = std::max(distance.at(*it), distance.at(v) + weight);
      distance.at(*it) = newDist;
      LOG("- set vertex " << *it << "|" << weight << ": " << distance.at(*it) <<
          " -> " << std::max(distance.at(*it), distance.at(v) + 1));
    }
  }

  return distance;
}


inline auto accumulateBackwards(const Graph &g, const Vi &distance, const Vi &nodeWeights) {
  auto rgraph = boost::make_reverse_graph(g);

  Vi accumulated = distance;
  
  // do topological sort with boost
  std::vector<Graph::vertex_descriptor> topoOrder(boost::num_vertices(rgraph));
  boost::topological_sort(rgraph, topoOrder.begin());

  // Go through the graph in reversed topological order
  for(auto vit = topoOrder.rbegin(); vit != topoOrder.rend(); ++vit) {
    auto v = *vit;
    auto weight = nodeWeights.empty() ? 1 : nodeWeights.at(v)
    LOG("On vtx " << n << ", accumulated: " << accumulated.at(v) << ", distance " << distance.at(v));
    auto [it, end] = boost::adjacent_vertices(v, rgraph);
    for (; it != end; ++it) {
      accumulated.at(*it) = std::max(accumulated.at(*it), 
                                     accumulated.at(v) - (distance.at(v) - distance.at(*it) - weight));
      LOG("- set vertex " << *it << ": " << accumulated.at(*it))
    }
  }

  return accumulated;
}

inline std::pair<Vi, Vi> filterEdges(const Vi &src, const Vi &dst,
                 const Vi &nodeWeights, std::size_t trackLengthConstraint) {
    auto g = make_graph(src, dst);
    auto distances = findMaxDistances(g, nodeWeights);
    auto accumulated = accumulateBackwards(g, distances, nodeWeights);

    // filter out edges that are not part of long enough tracks
    for(auto v=0ul; v<accumulated.size(); ++v) {
      LOG("Vtx " << v << " has accumulated " << accumulated.at(v));
      if( accumulated.at(v) < trackLengthConstraint ) {
        LOG("Clear vtx " << v);
        boost::clear_vertex(v, g);
      }
    }

    Vi newSrc, newDst;
    newSrc.reserve(boost::num_edges(g));
    newDst.reserve(boost::num_edges(g));

    for(auto e : boost::make_iterator_range(boost::edges(g))) {
      newSrc.push_back(boost::source(e, g));
      newDst.push_back(boost::target(e, g));
    }

    return {newSrc, newDst};
}
