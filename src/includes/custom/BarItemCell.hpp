#pragma once

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

        if (barName == "Default Bar") {
            m_barName = barName;
            m_barUrl  = barUrl;
            m_index   = index;

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
        } else {
            m_barName = barName;
            m_barUrl  = barUrl;
            m_index   = index;

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
                
                if (!img->initWithImageData(
                    const_cast<unsigned char*>(bytes.data()),
                    static_cast<int>(bytes.size())
                )) {
                    img->release();
                    return;
                }

                auto tex = new CCTexture2D();
                if (tex->initWithImage(img)) {
                    auto spr = CCSprite::createWithTexture(tex);
                    spr->setScale(1.164f);
                    spr->setAnchorPoint(ccp(0, 0.5f));
                    spr->setPosition(ccp(8, self->getContentHeight() * 0.5f));
                    spr->setID("bar-preview");

                    if (auto ph = self->getChildByID("img-placeholder")) {
                        ph->removeFromParent();
                    }
                    self->addChild(spr);
                }
                tex->release();
                img->release();
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