#include <Geode/Geode.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include "TrailTracker.hpp"

using namespace geode::prelude;

// === 1. Перехват нажатий (прыжков) ===
class $modify(MyPlayerObject, PlayerObject) {
    void pushButton(PlayerButton button) {
        PlayerObject::pushButton(button);

        if (button != PlayerButton::Jump) return;

        log::info("Jump button pressed!");

        auto tracker = TrailTracker::get();
        auto mod = Mod::get();

        bool inEditor = LevelEditorLayer::get() != nullptr;
        bool inPlay = PlayLayer::get() != nullptr;

        log::info("inEditor: {}, inPlay: {}", inEditor, inPlay);

        if (inEditor) {
            tracker->addJumpPoint(this->getPosition(), !this->m_isSecondPlayer);
            log::info("Jump point added in editor");
            return;
        }

        if (inPlay && mod->getSettingValue<bool>("show-jumps-in-level")) {
            tracker->addJumpPoint(this->getPosition(), !this->m_isSecondPlayer);
            log::info("Jump point added in play");
        }
    }
};

// === 2. Запись и отрисовка в редакторе (тест) ===
class $modify(EditorHook, LevelEditorLayer) {
    void update(float dt) {
        LevelEditorLayer::update(dt);

        static int counter = 0;
        if (counter++ % 60 == 0) {
            log::info("EditorHook::update called! Player: {}", this->m_player1 != nullptr);
        }

        auto tracker = TrailTracker::get();

        if (auto player = this->m_player1) {
            tracker->addTrailPoint(player->getPosition());
        }

        tracker->drawOnLayer(this);
    }

    void onPlaytest() {
        log::info("onPlaytest called");
        TrailTracker::get()->clearAll();
        LevelEditorLayer::onPlaytest();
    }
};

// === 3. Запись и отрисовка в обычной игре ===
class $modify(PlayHook, PlayLayer) {
    bool init(GJGameLevel* level, bool useReplay, bool dontAddToHistory) {
        if (!PlayLayer::init(level, useReplay, dontAddToHistory)) return false;
        log::info("PlayLayer::init called");
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

// === 4. Кнопка в главном меню ===
class $modify(MenuHook, MenuLayer) {
    bool init() {
        if (!MenuLayer::init())
            return false;

        log::info("MenuLayer::init called");

        auto bottomMenu = this->getChildByID("bottom-menu");
        if (!bottomMenu) {
            log::warn("bottom-menu not found!");
            return true;
        }

        auto sprite = CCSprite::create("tracker-icon.png"_spr);
        if (!sprite) {
            log::warn("tracker-icon.png not found, using fallback");
            sprite = CCSprite::createWithSpriteFrameName("GJ_optionsBtn_001.png");
        }

        auto button = CCMenuItemSpriteExtra::create(
            sprite,
            this,
            menu_selector(MenuHook::onTrailTrackerButton)
        );

        button->setID("trail-tracker-button"_spr);
        bottomMenu->addChild(button);

        if (auto menu = typeinfo_cast<CCMenu*>(bottomMenu)) {
            menu->updateLayout();
        }

        log::info("Trail Tracker button added to main menu");

        return true;
    }

    void onTrailTrackerButton(CCObject* sender) {
        Notification::create("Trail Tracker: Settings coming soon!", NotificationIcon::Info)->show();
    }
};