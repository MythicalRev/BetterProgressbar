#pragma once

#include <Geode/Geode.hpp>
#include "BarsPopup.hpp"

using namespace geode::prelude;

class CustomBarPopup : public Popup {
protected:
    TextInput* customBarURL;
    TaskHolder<web::WebResponse> m_listener;

    bool init() {
        if (!Popup::init(150, 100))
            return false;
        
        this->setTitle("Custom Bar");

        auto layout = ColumnLayout::create();
        layout->setGap(5);
        layout->setAxisReverse(true);

        auto database = ButtonSprite::create("Database");
        database->setScale(0.7f);

        auto databaseBtn = CCMenuItemSpriteExtra::create(database, this, menu_selector(CustomBarPopup::onDatabase));
        databaseBtn->setAnchorPoint(ccp(0.5,0.5));
        databaseBtn->ignoreAnchorPointForPosition(false);

        auto custom = ButtonSprite::create("Custom");
        custom->setScale(0.7f);

        auto customBtn = CCMenuItemSpriteExtra::create(custom, this, menu_selector(CustomBarPopup::onCustom));
        customBtn->setAnchorPoint(ccp(0.5,0.5));
        customBtn->ignoreAnchorPointForPosition(false);


        auto menu = CCMenu::create();
        menu->setAnchorPoint(ccp(0.5,0.5));
        menu->setContentHeight(100);
        menu->ignoreAnchorPointForPosition(false);

        this->m_mainLayer->addChildAtPosition(menu, Anchor::Center, ccp(0, -10));

        menu->addChildAtPosition(databaseBtn, Anchor::Center);
        menu->addChildAtPosition(customBtn, Anchor::Center);
        

        menu->setLayout(layout);
        menu->updateLayout();

        return true;
    }

    void onDatabase(CCObject* sender) {
        BarsPopup::create()->show();
        this->onClose(nullptr);
    }

    void onCustom(CCObject* sender) {
        FLAlertLayer::create(
            "Custom Bar",
            "Upload Image in Mod Settings.",
            "OK"
        )->show();

        this->onClose(nullptr);
    }
public:
    static CustomBarPopup* create() {
        auto pRet = new CustomBarPopup();
        if (pRet->init()) {
            pRet->autorelease();
            return pRet;
        }

        CC_SAFE_DELETE(pRet);
        return nullptr;
    }
};