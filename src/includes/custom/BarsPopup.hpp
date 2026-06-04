#pragma once

#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>
#include "BarItemCell.hpp"

using namespace geode::prelude;

class BarsPopup : public Popup {
protected:
    ScrollLayer* scrollLayer = nullptr;

    bool init() {
        if (!Popup::init(350, 260))
            return false;

        this->setTitle("Select a Bar");

        auto scrollLayerBG = CCLayerColor::create({0, 0, 0, 60}, 313, 210);
        scrollLayerBG->ignoreAnchorPointForPosition(false);
        scrollLayerBG->setAnchorPoint(ccp(0.5f, 0.5f));

        scrollLayer = ScrollLayer::create(ccp(313, 210));
        scrollLayer->ignoreAnchorPointForPosition(false);
        scrollLayer->setAnchorPoint(ccp(0.5f, 0.5f));

        auto scrollBar = Scrollbar::create(scrollLayer);

        auto border = ListBorders::create();
        border->setContentSize(ccp(313, 210));
        border->ignoreAnchorPointForPosition(false);
        border->setAnchorPoint(ccp(0.5f, 0.5f));

        m_mainLayer->addChildAtPosition(scrollLayerBG, Anchor::Center, ccp(0, -10));
        m_mainLayer->addChildAtPosition(scrollLayer, Anchor::Center, ccp(0, -10));
        m_mainLayer->addChildAtPosition(border, Anchor::Center, ccp(0, -10));
        m_mainLayer->addChildAtPosition(scrollBar, Anchor::Center, ccp(162.5f, -10));

        auto layout = ColumnLayout::create();
        layout->setAxisReverse(true);
        layout->setGap(0);
        layout->setAxisAlignment(AxisAlignment::Start);
        scrollLayer->m_contentLayer->setLayout(layout);

        auto loadingLabel = CCLabelBMFont::create("Loading...", "bigFont.fnt");
        loadingLabel->setScale(0.5f);
        loadingLabel->setID("loading-label");
        m_mainLayer->addChildAtPosition(loadingLabel, Anchor::Center, ccp(0, -10));

        fetchBars();
        return true;
    }

    void fetchBars() {
        auto self = Ref<BarsPopup>(this);

        std::thread([self]() {
            auto res = web::WebRequest()
                .header("User-Agent", "BPB-Mod/1.0")
                .getSync("https://api.github.com/repos/MythicalRev/BPB-Bars/contents/");

            if (!res.ok()) {
                Loader::get()->queueInMainThread([self]() {
                    self->showError("Failed to fetch bars.");
                });
                return;
            }

            auto jsonStr = res.string().unwrapOr("[]");

            Loader::get()->queueInMainThread([self, jsonStr]() {
                if (auto lbl = self->m_mainLayer->getChildByID("loading-label"))
                    lbl->removeFromParent();
                self->parseAndPopulate(jsonStr);
            });
        }).detach();
    }

    void parseAndPopulate(const std::string& jsonStr) {
        auto jsonResult = matjson::parse(jsonStr);
        if (!jsonResult.isOk()) {
            showError("Failed to parse response.");
            return;
        }

        auto json = jsonResult.unwrap();
        if (!json.isArray()) {
            showError("Unexpected response.");
            return;
        }

        std::vector<std::pair<std::string, std::string>> bars;

        for (auto& item : json.asArray().unwrap()) {
            auto name = item["name"].asString().unwrapOr("");
            auto type = item["type"].asString().unwrapOr("");
            auto url  = item["download_url"].asString().unwrapOr("");

            if (type != "file" || url.empty()) continue;

            std::string lower = name;
            std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

            if (lower.size() >= 4 && lower.substr(lower.size() - 4) == ".png")
                bars.push_back(std::make_pair(name, url));
            else if (lower.size() >= 4 && lower.substr(lower.size() - 4) == ".jpg")
                bars.push_back(std::make_pair(name, url));
            else if (lower.size() >= 5 && lower.substr(lower.size() - 5) == ".jpeg")
                bars.push_back(std::make_pair(name, url));
        }

        if (bars.empty()) { showError("No bar images found."); return; }

        scrollLayer->m_contentLayer->removeAllChildren();
        scrollLayer->m_contentLayer->setContentSize(CCSize(313, 30 * (bars.size() + 1)));

        auto defaultCell = BarItemCell::create("Default Bar", "", -1);
        defaultCell->setContentSize(ccp(313, 30));
        defaultCell->ignoreAnchorPointForPosition(false);
        defaultCell->setAnchorPoint(ccp(0, 1));

        defaultCell->onUse = [this]() {
            Mod::get()->setSavedValue<std::string>("selected_bar", "");
            Mod::get()->setSavedValue<bool>("custom_bar", false);
            this->onClose(nullptr);
        };
        
        scrollLayer->m_contentLayer->addChild(defaultCell);

        int index = 0;
        for (auto& [name, url] : bars) {
            auto cell = BarItemCell::create(name, url, index);
            cell->setContentSize(ccp(313, 30));
            cell->ignoreAnchorPointForPosition(false);
            cell->setAnchorPoint(ccp(0, 1));
            cell->onUse = [this, name, url]() { onBarSelected(name, url); };
            scrollLayer->m_contentLayer->addChild(cell);
            ++index;
        }

        scrollLayer->m_contentLayer->updateLayout();
        scrollLayer->moveToTop();
    }

    void showError(const std::string& msg) {
        if (auto lbl = m_mainLayer->getChildByID("loading-label"))
            lbl->removeFromParent();
        auto errLabel = CCLabelBMFont::create(msg.c_str(), "bigFont.fnt");
        errLabel->setScale(0.45f);
        errLabel->setColor({255, 80, 80});
        m_mainLayer->addChildAtPosition(errLabel, Anchor::Center, ccp(0, -10));
    }

    void onBarSelected(const std::string& name, const std::string& url) {
        Mod::get()->setSavedValue<std::string>("selected_bar", url);
        Mod::get()->setSavedValue<bool>("custom_bar", true);
        this->onClose(nullptr);
    }

public:
    static BarsPopup* create() {
        auto pRet = new BarsPopup();
        if (pRet && pRet->init()) {
            pRet->autorelease();
            return pRet;
        }
        CC_SAFE_DELETE(pRet);
        return nullptr;
    }
};