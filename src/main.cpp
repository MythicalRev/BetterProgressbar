#include <Geode/Geode.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>

#include "includes/custom/BarsPopup.hpp"
#include "includes/modifies/BPBPlayLayer.cpp"

using namespace geode::prelude;

#include <Geode/modify/MenuLayer.hpp>
class $modify(BPBMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        auto openBars = CCMenuItemSpriteExtra::create(
            CircleButtonSprite::createWithSpriteFrameName("best.png"_spr),
            this,
            menu_selector(BPBMenuLayer::onMyButton)
        );

        auto menu = this->getChildByID("bottom-menu");
        menu->addChild(openBars);
        openBars->setID("openBars"_spr);
        menu->updateLayout();

        return true;
    }

    void onMyButton(CCObject*) {
        BarsPopup::create()->show();
    }
};