#include <Geode/Geode.hpp>
#include <Geode/loader/SettingV3.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>

#include "includes/custom/CustomBarPopup.hpp"
#include "includes/modifies/BPBPlayLayer.cpp"

using namespace geode::prelude;

$on_mod(Loaded) {
    listenForSettingChanges<std::filesystem::path>("customBar", [](std::filesystem::path value) {
        if (Mod::get()->getSettingValue<std::filesystem::path>("customBar") != "Please pick an image file.") {
            Mod::get()->setSavedValue<bool>("custom_bar", true);
        }
    });
}

#include <Geode/modify/MenuLayer.hpp>
class $modify(BPBMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        auto openBars = CCMenuItemSpriteExtra::create(
            CircleButtonSprite::createWithSpriteFrameName("best.png"_spr),
            this,
            menu_selector(BPBMenuLayer::onButton)
        );

        auto menu = this->getChildByID("bottom-menu");
        menu->addChild(openBars);
        openBars->setID("openBars"_spr);
        menu->updateLayout();

        return true;
    }

    void onButton(CCObject*) {
        CustomBarPopup::create()->show();
    }
};
