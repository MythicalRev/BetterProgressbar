#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

class CustomBarPopup : public Popup {
protected:
    TextInput* customBarURL;
    TaskHolder<web::WebResponse> m_listener;

    bool init() {
        if (!Popup::init(350, 100))
            return false;
        
        this->m_closeBtn->setVisible(false);
        
        this->setTitle("Custom Bar Image");

        auto layout = ColumnLayout::create();
        layout->setGap(5);
        layout->setAxisReverse(true);

        customBarURL = TextInput::create(330, "Direct Image URL (823x32)", "bigFont.fnt");
        customBarURL->setTextAlign(TextInputAlign::Center);
        customBarURL->setAnchorPoint(ccp(0.5,0.5));
        customBarURL->ignoreAnchorPointForPosition(false);
        customBarURL->setFilter("0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ:/._-=&%?");

        auto confirmBtnSpr = ButtonSprite::create("Confirm");
        confirmBtnSpr->setScale(0.7f);

        auto confirmBtn = CCMenuItemSpriteExtra::create(confirmBtnSpr, this, menu_selector(CustomBarPopup::onConfirm));
        confirmBtn->setAnchorPoint(ccp(0.5,0.5));
        confirmBtn->ignoreAnchorPointForPosition(false);

        auto menu = CCMenu::create();
        menu->setAnchorPoint(ccp(0.5,1));
        menu->setContentHeight(confirmBtn->getContentHeight());
        menu->ignoreAnchorPointForPosition(false);

        this->m_mainLayer->addChildAtPosition(menu, Anchor::Bottom, ccp(0,30));

        this->m_mainLayer->addChildAtPosition(customBarURL, Anchor::Center);
        menu->addChildAtPosition(confirmBtn, Anchor::Bottom);

        menu->setLayout(layout);
        menu->updateLayout();

        return true;
    }

    void fetchImage(const std::string& url) {
        auto req = web::WebRequest();

        m_listener.spawn(
            req.get(url), 
            [this, url](web::WebResponse res) {
                if (!res.ok()) return;

                auto data = res.data();
                
                Loader::get()->queueInMainThread([this, data, url]() {
                    auto img = new CCImage();
                    if (img->initWithImageData(const_cast<unsigned char*>(data.data()), data.size())) {
                        auto tex = new CCTexture2D();
                        if (tex->initWithImage(img)) {
                            if (img->getWidth() == 823 && img->getHeight() == 32) {
                                Mod::get()->setSavedValue<std::string>("selected_bar", url);
                                Mod::get()->setSavedValue<bool>("custom_bar", true);
                                this->onClose(nullptr);
                            } else {
                                auto newAlert = TextAlertPopup::create("Bad Image Size! Must be 823x32", 2, .5f, 0, "bigFont.fnt");

                                newAlert->setPositionY(80);
                                newAlert->setID("newpos-alert"_spr);
                                this->getParent()->addChild(newAlert);
                            }
                        }
                        tex->release();
                    } else {
                        auto newAlert = TextAlertPopup::create("Please Input a Valid Image URL", 2, .5f, 0, "bigFont.fnt");

                        newAlert->setPositionY(80);
                        newAlert->setID("newpos-alert"_spr);
                        this->getParent()->addChild(newAlert);
                    }
                    img->release();
                });
            }
        );
    }

    void onConfirm(CCObject* sender) {
        if (customBarURL->getString().empty()) {
            auto newAlert = TextAlertPopup::create("Please Input a URL", 2, .5f, 0, "bigFont.fnt");

            newAlert->setPositionY(80);
            newAlert->setID("newpos-alert"_spr);
            this->getParent()->addChild(newAlert);
        } else {
            fetchImage(customBarURL->getString());
        }
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