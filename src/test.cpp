#include <boost/test/unit_test.hpp>

#include "analyze_graph.hpp"

auto test_graph(Graph &g, const std::vector<int> &distances, 
                const std::vector<int> &accumulated) {
    // do topological sort with boost
    std::vector<Graph::vertex_descriptor> topoOrder(boost::num_vertices(g));
    boost::topological_sort(g, topoOrder.begin());
    
    findMaxDistances(g, topoOrder);
    Vi res;
    for(auto i=0ul; i<boost::num_vertices(g); ++i) {
      res.push_back(g[i].distance);
    }
    BOOST_CHECK_EQUAL_COLLECTIONS(res.begin(), res.end(), distances.begin(), distances.end());

    accumulateBackwards(g, topoOrder);
    Vi res2;
    for(auto i=0ul; i<boost::num_vertices(g); ++i) {
      res2.push_back(g[i].accumulated);
    }
    BOOST_CHECK_EQUAL_COLLECTIONS(res2.begin(), res2.end(), accumulated.begin(), accumulated.end());
}

BOOST_AUTO_TEST_CASE(test_graph_one_root_1) {
    auto g = make_graph(
      {0, 1, 2, 3, 4, 0, 6},  
      {1, 2, 3, 4, 5, 6, 3}
    );
    std::vector<int> distances = {1, 2, 3, 4, 5, 6, 2};
    std::vector<int> accumulated = {6, 6, 6, 6, 6, 6, 5};
    test_graph(g, distances, accumulated);
}

// TODO this needs topological sort to work, but could be also simpler than BFS
BOOST_AUTO_TEST_CASE(test_graph_one_root_2) {
    const auto r = 0; // root
    const auto f = 5; // final
    const auto j = 3; // junction
    auto g = make_graph(
      {r, 1, 2, j, 4, r, 6, j, 7, 8},  
      {1, 2, j, 4, f, 6, j, 7, 8, f}
    );
    std::vector<int> distances = {1, 2, 3, 4, 5, 7, 2, 5, 6};
    std::vector<int> accumulated = {7, 7, 7, 7, 6, 7, 6, 7, 7};
    test_graph(g, distances, accumulated);
}

BOOST_AUTO_TEST_CASE(test_graph_y_shape) {
  auto g = make_graph(
    {0, 1, 2, 3, 4, 5, 2, 7},
    {1, 2, 3, 4, 5, 6, 7, 8}
  );
  std::vector<int> distances = {1, 2, 3, 4, 5, 6, 7, 4, 5};
  std::vector<int> accumulated = {7, 7, 7, 7, 7, 7, 7, 5, 5};
  test_graph(g, distances, accumulated);
}

BOOST_AUTO_TEST_CASE(test_graph_x_shape) {
  auto g = make_graph(
    {0, 1, 2, 4, 5, 3, 6, 7, 3},
    {1, 2, 3, 5, 3, 6, 7, 8, 9}
  );

  std::vector<int> distances =   {1, 2, 3, 4, 1, 2, 5, 6, 7, 5};
  std::vector<int> accumulated = {7, 7, 7, 7, 6, 6, 7, 7, 7, 5};
  test_graph(g, distances, accumulated);
}

BOOST_AUTO_TEST_CASE(test_simple_weighted) {
  std::vector<int> weights = {1, 1, 1, 2, 2, 2};
  auto g = make_graph(
    {0, 1, 2, 3, 4},  
    {1, 2, 3, 4, 5},
    weights  
  );
  std::vector<int> distances = {1, 2, 3, 5, 7, 9};
  std::vector<int> accumulated = {9, 9, 9, 9, 9, 9};
  test_graph(g, distances, accumulated);
}

BOOST_AUTO_TEST_CASE(test_h_shape_weighted) {
  std::vector<int> weights = {1, 1, 1, 2, 2, 1, 2, 1, 1};
  auto g = make_graph(
    {0, 1, 2, 3, 2, 5, 8, 7},
    {1, 2, 3, 4, 5, 6, 7, 5},
    weights
  );
  std::vector<int> distances = {1, 2, 3, 5, 7, 4, 6, 2, 1};
  std::vector<int> accumulated = {7, 7, 7, 7, 7, 6, 6, 5, 5};
  test_graph(g, distances, accumulated);
}

BOOST_AUTO_TEST_CASE(test_filter_edges) {
  std::vector<int> src = {0, 1, 2, 3, 4, 0, 6};
  std::vector<int> dst = {1, 2, 3, 4, 5, 6, 5};
  std::vector<int> nodeWeights = {1, 1, 1, 1, 1, 1, 1};
  std::size_t trackLengthConstraint = 4;
  auto [res1, res2] = filterEdges(src, dst, nodeWeights, trackLengthConstraint);

  std::vector<int> src2 = {0, 1, 2, 3, 4};
  std::vector<int> dst2 = {1, 2, 3, 4, 5};
  BOOST_CHECK_EQUAL_COLLECTIONS(res1.begin(), res1.end(), src2.begin(), src2.end());
  BOOST_CHECK_EQUAL_COLLECTIONS(res2.begin(), res2.end(), dst2.begin(), dst2.end());
}

BOOST_AUTO_TEST_CASE(test_filter_edges_shortcut) {
  std::vector<int> src = {0, 1, 2, 3, 4, 5, 0};
  std::vector<int> dst = {1, 2, 3, 4, 5, 6, 6};
  std::vector<int> nodeWeights = {1, 1, 1, 1, 1, 1, 1};
  std::size_t trackLengthConstraint = 4;
  auto [res1, res2] = filterEdges(src, dst, nodeWeights, trackLengthConstraint);

  std::vector<int> src2 = {0, 1, 2, 3, 4, 5};
  std::vector<int> dst2 = {1, 2, 3, 4, 5, 6};
  BOOST_CHECK_EQUAL_COLLECTIONS(res1.begin(), res1.end(), src2.begin(), src2.end());
  BOOST_CHECK_EQUAL_COLLECTIONS(res2.begin(), res2.end(), dst2.begin(), dst2.end());
}
