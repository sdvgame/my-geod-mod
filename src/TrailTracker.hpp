#pragma once
#include <Geode/Geode.hpp>
#include <vector>

using namespace geode::prelude;

struct JumpPoint {
    CCPoint worldPos;
    bool isP1;
};

class TrailTracker {
public:
    static TrailTracker* get();

    void addJumpPoint(CCPoint pos, bool p1);
    void addTrailPoint(CCPoint pos);
    void clearAll();
    void drawOnLayer(CCNode* layer);
    void removeNodes();

private:
    std::vector<JumpPoint> m_jumps;
    std::vector<CCPoint> m_trail;
    CCDrawNode* m_drawNode = nullptr;
};