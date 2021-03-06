
#include "cartographer/transform/transform_interpolation_buffer.h"

#include "Eigen/Core"
#include "Eigen/Geometry"
#include "cartographer/transform/rigid_transform.h"
#include "cartographer/transform/rigid_transform_test_helpers.h"
#include "gtest/gtest.h"

namespace cartographer {
namespace transform {
namespace {

TEST(TransformInterpolationBufferTest, testHas) {
  TransformInterpolationBuffer buffer;
  EXPECT_FALSE(buffer.Has(common::FromUniversal(50)));//50us
  buffer.Push(common::FromUniversal(50), transform::Rigid3d::Identity());
  EXPECT_FALSE(buffer.Has(common::FromUniversal(25)));
  EXPECT_TRUE(buffer.Has(common::FromUniversal(50)));
  EXPECT_FALSE(buffer.Has(common::FromUniversal(75)));
  buffer.Push(common::FromUniversal(100), transform::Rigid3d::Identity());
  EXPECT_FALSE(buffer.Has(common::FromUniversal(25)));
  EXPECT_TRUE(buffer.Has(common::FromUniversal(50)));
  EXPECT_TRUE(buffer.Has(common::FromUniversal(75)));
  EXPECT_TRUE(buffer.Has(common::FromUniversal(100)));
  EXPECT_FALSE(buffer.Has(common::FromUniversal(125)));
  EXPECT_EQ(common::FromUniversal(50), buffer.earliest_time());
  EXPECT_EQ(common::FromUniversal(100), buffer.latest_time());
}

TEST(TransformInterpolationBufferTest, testLookup) {
  TransformInterpolationBuffer buffer;
  buffer.Push(common::FromUniversal(50), transform::Rigid3d::Identity());
  // The rotation needs to be relatively small in order for the interpolation to
  // remain a z-axis rotation.
  buffer.Push(common::FromUniversal(100),
              transform::Rigid3d::Translation(Eigen::Vector3d(10., 10., 10.)) *
                  transform::Rigid3d::Rotation(
                      Eigen::AngleAxisd(2., Eigen::Vector3d::UnitZ())));
  const common::Time time = common::FromUniversal(75);
  const transform::Rigid3d interpolated = buffer.Lookup(time);
  EXPECT_THAT(
      interpolated,
      IsNearly(transform::Rigid3d::Translation(Eigen::Vector3d(5., 5., 5.)) *
                   transform::Rigid3d::Rotation(
                       Eigen::AngleAxisd(1., Eigen::Vector3d::UnitZ())),
               1e-6));
}

}  // namespace
}  // namespace transform
}  // namespace cartographer
