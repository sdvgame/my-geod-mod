#include <Geode/Geode.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include "TrailTracker.hpp"

using namespace geode::prelude;

// === 1. Перехват нажатий (прыжков) ===
class $modify(MyPlayerObject, PlayerObject) {
    void pushButton(PlayerButton button) {
        PlayerObject::pushButton(button);

        if (button != PlayerButton::Jump) return;

        auto tracker = TrailTracker::get();
        auto mod = Mod::get();

        bool inEditor = LevelEditorLayer::get() != nullptr;
        bool inPlay = PlayLayer::get() != nullptr;

        if (inEditor) {
            tracker->addJumpPoint(this->getPosition(), !this->m_isSecondPlayer);
            return;
        }

        if (inPlay && mod->getSettingValue<bool>("show-jumps-in-level")) {
            tracker->addJumpPoint(this->getPosition(), !this->m_isSecondPlayer);
        }
    }
};

// === 2. Запись и отрисовка в редакторе (тест) ===
class $modify(EditorHook, LevelEditorLayer) {
    void update(float dt) {
        LevelEditorLayer::update(dt);

        auto tracker = TrailTracker::get();

        if (auto player = this->m_player1) {
            tracker->addTrailPoint(player->getPosition());
        }

        tracker->drawOnLayer(this);
    }

    void onPlaytest() {
        TrailTracker::get()->clearAll();
        LevelEditorLayer::onPlaytest();
    }
};

// === 3. Запись и отрисовка в обычной игре ===
class $modify(PlayHook, PlayLayer) {
    bool init(GJGameLevel* level, bool useReplay, bool dontAddToHistory) {
        if (!PlayLayer::init(level, useReplay, dontAddToHistory)) return false;
        TrailTracker::get()->clearAll();
        return true;
    }

    void update(float dt) {
        PlayLayer::update(dt);

        auto mod = Mod::get();
        auto tracker = TrailTracker::get();

        if (mod->getSettingValue<bool>("show-in-level")) {
            if (this->m_player1) {
                tracker->addTrailPoint(this->m_player1->getPosition());
            }
        }

        tracker->drawOnLayer(this);
    }

    void resetLevel() {
        TrailTracker::get()->clearAll();
        PlayLayer::resetLevel();
    }
};
