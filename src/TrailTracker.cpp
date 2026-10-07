#include "TrailTracker.hpp"

TrailTracker* TrailTracker::get() {
    static TrailTracker* instance = new TrailTracker();
    return instance;
}

void TrailTracker::addJumpPoint(CCPoint pos, bool p1) {
    m_jumps.push_back({pos, p1});
}

void TrailTracker::addTrailPoint(CCPoint pos) {
    m_trail.push_back(pos);
    if (m_trail.size() > 2000) {
        m_trail.erase(m_trail.begin());
    }
}

void TrailTracker::clearAll() {
    m_jumps.clear();
    m_trail.clear();
    removeNodes();
}

void TrailTracker::removeNodes() {
    if (m_drawNode) {
        m_drawNode->removeFromParent();
        m_drawNode = nullptr;
    }
}

void TrailTracker::drawOnLayer(CCNode* layer) {
    auto mod = Mod::get();

    // Создаём ноду для рисования, если её нет
    if (!m_drawNode) {
        m_drawNode = CCDrawNode::create();
        layer->addChild(m_drawNode, 9999);
    }

    m_drawNode->clear();

    // --- Линия следа ---
    auto trailColor = mod->getSettingValue<ccColor3B>("trail-color");
    auto trailThickness = mod->getSettingValue<double>("trail-thickness");
    ccColor4F trailCol4 = {
        trailColor.r / 255.f,
        trailColor.g / 255.f,
        trailColor.b / 255.f,
        1.f
    };

    if (m_trail.size() > 1) {
        for (size_t i = 1; i < m_trail.size(); i++) {
            m_drawNode->drawSegment(m_trail[i - 1], m_trail[i], trailThickness, trailCol4);
        }
    }

    // --- Кружки прыжков ---
    auto circleColor = mod->getSettingValue<ccColor3B>("circle-color");
    auto circleSize = mod->getSettingValue<double>("circle-size");
    bool outlineEnabled = mod->getSettingValue<bool>("outline-enabled");
    auto outlineColor = mod->getSettingValue<ccColor3B>("outline-color");
    float yOffset = mod->getSettingValue<double>("y-offset");

    ccColor4F circCol4 = {
        circleColor.r / 255.f,
        circleColor.g / 255.f,
        circleColor.b / 255.f,
        1.f
    };
    ccColor4F outCol4 = {
        outlineColor.r / 255.f,
        outlineColor.g / 255.f,
        outlineColor.b / 255.f,
        1.f
    };

    for (auto& jump : m_jumps) {
        CCPoint drawPos = jump.worldPos;
        drawPos.y += yOffset;

        if (outlineEnabled) {
            m_drawNode->drawDot(drawPos, circleSize + 1.5f, outCol4);
        }
        m_drawNode->drawDot(drawPos, circleSize, circCol4);
    }
}