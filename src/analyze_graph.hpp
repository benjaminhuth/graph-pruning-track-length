
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/reverse_graph.hpp>
#include <boost/graph/topological_sort.hpp>

#include <iostream>

#if 0
#define LOG(msg) std::cout << msg << std::endl;
#else
#define LOG(msg) /*nothing*/
#endif


struct NodeProperty {
  int weight{1};
  int distance{weight};
  int accumulated{};
};

struct EdgeProperty {
  int distance{};
  int accumulated{};
};

using Graph = boost::adjacency_list<
  boost::vecS, boost::vecS, boost::bidirectionalS,
  NodeProperty, EdgeProperty>;

using Vi = std::vector<int>;

// helper function that creates a boost graph from two vectors of int
inline auto make_graph(const Vi& from, const Vi& to, const Vi &weights = {}) {
    Graph g;
    for (size_t i = 0; i < from.size(); ++i) {
        boost::add_edge(from[i], to[i], g);
    }
    if(!weights.empty()) {
      for(auto i=0ul; i<boost::num_vertices(g); ++i) {
        g[i].weight = weights.at(i);
      }
    }
    return g;
}


inline auto findMaxDistances(Graph &g, const std::vector<Graph::vertex_descriptor> &topoOrder) {
  for(auto vit = topoOrder.rbegin(); vit != topoOrder.rend(); ++vit) {
    auto src = *vit;
    LOG("On vtx " << v << " with dist " << distance.at(v));
    auto [it, end] = boost::out_edges(src, g);
    for (; it != end; ++it) {
      auto tgt = boost::target(*it, g);
      g[*it].distance = g[src].distance + g[tgt].weight;
      auto newDist = std::max(g[tgt].distance, g[*it].distance);
      LOG("- set vertex " << *it << "|" << g[tgt].weight << ": " << g[tgt].distance <<
          " -> " << newDist);
      g[tgt].distance = newDist;
    }
  }
}


inline void accumulateBackwards(Graph &g, const std::vector<Graph::vertex_descriptor> &topoOrder) {
  for(auto i=0ul; i<boost::num_vertices(g); ++i) {
    g[i].accumulated = g[i].distance;
  }

  auto rg = boost::make_reverse_graph(g);

  // Go through the graph in topological order (reverse order of reversed graph == forward order of graph)
  for(auto vit = topoOrder.begin(); vit != topoOrder.end(); ++vit) {
    auto src = *vit;
    LOG("On vtx " << n << ", accumulated: " << rg[src].accumulated << ", distance " << rg[src].distance);
    auto [it, end] = boost::out_edges(src, rg);
    for (; it != end; ++it) {
      auto tgt = boost::target(*it, rg);
      rg[*it].accumulated = rg[src].accumulated - (rg[src].distance - rg[tgt].distance - rg[src].weight);
      rg[tgt].accumulated = std::max(rg[tgt].accumulated, rg[*it].accumulated);
      LOG("- set vertex " << src << ": " << rg[tgt].accumulated)
    }
  }
}

inline std::pair<Vi, Vi> filterEdges(const Vi &src, const Vi &dst,
                 const Vi &nodeWeights, std::size_t trackLengthConstraint) {
    auto g = make_graph(src, dst, nodeWeights);

    // do topological sort with boost
    std::vector<Graph::vertex_descriptor> topoOrder(boost::num_vertices(g));
    boost::topological_sort(g, topoOrder.begin());

    findMaxDistances(g, topoOrder);
    accumulateBackwards(g, topoOrder);

    // filter out edges that are not part of long enough tracks
  #if 0
    for(auto v=0ul; v<accumulated.size(); ++v) {
      LOG("Vtx " << v << " has accumulated " << accumulated.at(v));
      if( accumulated.at(v) < trackLengthConstraint ) {
        LOG("Clear vtx " << v);
        boost::clear_vertex(v, g);
      }
    }
  #else
    auto [begin, end] = boost::edges(g);
    boost::remove_edge_if([&](auto e){ return g[e].accumulated < trackLengthConstraint; }, g);
  #endif

    Vi newSrc, newDst;
    newSrc.reserve(boost::num_edges(g));
    newDst.reserve(boost::num_edges(g));

    for(auto e : boost::make_iterator_range(boost::edges(g))) {
      newSrc.push_back(boost::source(e, g));
      newDst.push_back(boost::target(e, g));
    }

    return {newSrc, newDst};
}
