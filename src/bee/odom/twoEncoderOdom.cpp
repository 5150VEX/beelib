#include "bee/odom/twoEncoderOdom.hpp"

#include <cmath>

namespace bee {
TwoEncoderOdom::TwoEncoderOdom(std::shared_ptr<Tracker> leftTracker, std::shared_ptr<Tracker> horzTracker,
                               std::shared_ptr<HeadingTracker> headingTracker)
    : m_leftTracker(leftTracker),
      m_horzTracker(horzTracker),
      m_headingTracker(headingTracker) {}

void TwoEncoderOdom::calibrate() { m_headingTracker->calibrate(); }

void TwoEncoderOdom::update() {
    const float deltaRotation = m_headingTracker->getDeltaRotation();
    const float avgHeading = m_pose.theta + deltaRotation / 2;

    const float rawDx = m_horzTracker->getDeltaPosition();
    const float rawDy = m_leftTracker->getDeltaPosition();

    float localDx = rawDx;
    float localDy = rawDy;

    if (deltaRotation != 0) {
        const float arcToLine = 2 * std::sin(deltaRotation / 2);
        localDx = arcToLine * (rawDx / deltaRotation + m_horzTracker->getOffset());
        localDy = arcToLine * (rawDy / deltaRotation + m_leftTracker->getOffset());
    }

    m_pose = m_pose + Pose(localDx, localDy).rotateBy(avgHeading);
    m_pose.theta += deltaRotation;
}
} // namespace bee