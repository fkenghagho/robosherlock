#include <string>
#include <gtest/gtest.h>

#include <rs/flowcontrol/RSAggregateAnalysisEngine.h>
#include <rs/utils/common.h>
#include <rs/types/all_types.h>
#include <rs/scene_cas.h>
#include <rs/io/CamInterface.h>

#include <rs/utils/array_utils.hpp>

#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/io/pcd_io.h>
#include <ros/ros.h>

class SegmentationTest : public ::testing::Test
{
protected:

  std::string engineFile;
  RSAggregateAnalysisEngine *engine;

  float segment_similarity_threshold;
  float overall_segment_threshold;

  std::vector<std::string> engineList = {"CollectionReader",
                                         "ImagePreprocessor",
                                         "PointCloudFilter",
                                         "NormalEstimator",
                                         "PlaneAnnotator",
                                         "SymmetrySegmentationAnnotator"};

  virtual void SetUp()
  {
    rs::common::getAEPaths("symmetry_segmentation", engineFile);
    CamInterface::resetIdCount();
    engine = rs::createRSAggregateAnalysisEngine(engineFile, false); // do not run parallel for now
  }

  virtual void TearDown()
  {
    //clean up
    CamInterface::resetIdCount();
    engine->destroy();
    delete engine;
  }

  //there is no ground truth for this test, so for simply we just test if there are segments
  inline int test()
  {
    engine->setPipelineOrdering(engineList);

    //main pipeline execution
    engine->resetCas();
    engine->processOnce();

    uima::CAS* tcas = engine->getCas();
    rs::SceneCas cas(*tcas);
    rs::Scene scene = cas.getScene();

    //get segments data
    std::vector<rs::ObjectHypothesis> segments;
    scene.identifiables.filter(segments);

    return segments.size();
  }
};


TEST_F(SegmentationTest, SymmetrySegmentationTest1)
{
  int numSegments = test();
  EXPECT_TRUE(numSegments > 0);
}
