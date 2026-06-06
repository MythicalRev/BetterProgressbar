#pragma once

#include "Geode/cocos/layers_scenes_transitions_nodes/CCLayer.h"
#include "Geode/cocos/touch_dispatcher/CCTouchDelegateProtocol.h"
#include "Geode/cocos/touch_dispatcher/CCTouchDispatcher.h"
#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>

using namespace geode::prelude;

class BarItemCell : public CCLayerColor {
public:
    std::function<void()> onUse;

private:
    std::string m_barName;
    std::string m_barUrl;
    int m_index;
    TaskHolder<web::WebResponse> m_listener;

public:
    bool init(const std::string& barName, const std::string& barUrl, int index) {
        if (!CCLayerColor::init()) return false;

        m_barName = barName;
        m_barUrl  = barUrl;
        m_index   = index;

        if (barName == "Default Bar") {
            this->setOpacity((index % 2 == 0) ? 100 : 50);
            this->setContentSize(ccp(313, 30));
            this->setAnchorPoint(ccp(0, 1));

            auto placeholder = CCLabelBMFont::create("Default Progress Bar", "bigFont.fnt");
            placeholder->setScale(0.45f);
            placeholder->setAnchorPoint(ccp(0, 0.5f));
            placeholder->setPosition(ccp(8, this->getContentHeight() * 0.5f));
            placeholder->setID("text");
            this->addChild(placeholder);

            auto useBtnSpr = ButtonSprite::create("Use", "goldFont.fnt", "GJ_button_01.png");
            useBtnSpr->setScale(0.6f);
            auto useBtn = CCMenuItemSpriteExtra::create(
                useBtnSpr, this, menu_selector(BarItemCell::onUseBtn)
            );

            auto menu = CCMenu::createWithItem(useBtn);
            menu->setAnchorPoint(ccp(1, 0.5f));
            menu->ignoreAnchorPointForPosition(false);
            menu->setPosition(ccp(
                this->getContentWidth() - 8,
                this->getContentHeight() * 0.5f
            ));
            menu->setContentSize(CCSize(60, 40));
            RowLayout::create()->apply(menu);
            menu->updateLayout();
            this->addChild(menu);

            CCTouchDispatcher::get()->addTargetedDelegate(menu, CCTouchDispatcher::get()->getTargetPrio() - 5, true);
        } else {
            this->setOpacity((index % 2 == 0) ? 100 : 50);
            this->setContentSize(ccp(313, 30));
            this->setAnchorPoint(ccp(0, 1));

            auto placeholder = CCScale9Sprite::create("square02_small.png");
            placeholder->setContentSize(ccp(206, 14));
            placeholder->setAnchorPoint(ccp(0, 0.5f));
            placeholder->setPosition(ccp(8, this->getContentHeight() * 0.5f));
            placeholder->setOpacity(80);
            placeholder->setID("img-placeholder");
            this->addChild(placeholder);

            auto useBtnSpr = ButtonSprite::create("Use", "goldFont.fnt", "GJ_button_01.png");
            useBtnSpr->setScale(0.6f);
            auto useBtn = CCMenuItemSpriteExtra::create(
                useBtnSpr, this, menu_selector(BarItemCell::onUseBtn)
            );

            auto menu = CCMenu::createWithItem(useBtn);
            menu->setAnchorPoint(ccp(1, 0.5f));
            menu->ignoreAnchorPointForPosition(false);
            menu->setPosition(ccp(
                this->getContentWidth() - 8,
                this->getContentHeight() * 0.5f
            ));
            menu->setContentSize(CCSize(60, 40));
            RowLayout::create()->apply(menu);
            menu->updateLayout();
            this->addChild(menu);
            
            CCTouchDispatcher::get()->addTargetedDelegate(menu, CCTouchDispatcher::get()->getTargetPrio() - 5, true);

            fetchImage();
        }

        return true;
    }

    void fetchImage() {
        auto url = m_barUrl;
        auto self = Ref<BarItemCell>(this);

        m_listener.spawn(
            web::WebRequest().get(url),
            [self](web::WebResponse res) {
                if (!res.ok()) return;

                auto bytes = res.data();
                auto img = new CCImage();
                if (!img->initWithImageData(const_cast<unsigned char*>(bytes.data()), static_cast<int>(bytes.size()))) {
                    img->release();
                    return;
                }

                auto tex = new CCTexture2D();
                bool success = tex->initWithImage(img);
                img->release();

                if (success) {
                    auto spr = CCSprite::createWithTexture(tex);
                    
                    float targetWidth = 206.f;
                    float targetHeight = 14.f;
                    spr->setScaleX(targetWidth / spr->getContentSize().width);
                    spr->setScaleY(targetHeight / spr->getContentSize().height);
                    
                    spr->setAnchorPoint(ccp(0, 0.5f));
                    spr->setPosition(ccp(8, self->getContentHeight() * 0.5f));
                    spr->setID("bar-preview");

                    if (auto ph = self->getChildByID("img-placeholder")) {
                        ph->removeFromParent();
                    }
                    self->addChild(spr);
                }
                tex->release();
            }
        );
    }

    void onUseBtn(CCObject*) {
        if (onUse) onUse();
    }

    static BarItemCell* create(const std::string& barName, const std::string& barUrl, int index) {
        auto pRet = new BarItemCell();
        if (pRet && pRet->init(barName, barUrl, index)) {
            pRet->autorelease();
            return pRet;
        }
        CC_SAFE_DELETE(pRet);
        return nullptr;
    }
};