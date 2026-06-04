#include <Geode/Geode.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>

#include "includes/custom/BarsPopup.hpp"
#include "includes/custom/CustomBarPopup.hpp"
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
        geode::createQuickPopup(
            "Custom Bars",
            "Custom or Database?",
            "Custom", "Database",
            [](auto, bool btn2) {
                if (btn2) {
                    BarsPopup::create()->show();
                } else {
                    CustomBarPopup::create()->show();
                }
            }
        );
        
    }
};