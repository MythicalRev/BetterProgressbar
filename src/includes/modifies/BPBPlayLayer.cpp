#include <Geode/Geode.hpp>

using namespace geode::prelude;

#include <Geode/modify/PlayLayer.hpp>
class $modify(BPBPlayLayer, PlayLayer) {
    struct Fields {
        float m_highestPercent = 0.0f;
        float m_sessionBestPercent = 0.0f;

        TaskHolder<web::WebResponse> m_listener;
    };

    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        PlayLayer::init(level, useReplay, dontCreateObjects);

        m_fields->m_highestPercent = level->m_normalPercent;

        return true;
    }

    void fetchImage(const std::string& url) {
        auto req = web::WebRequest();

        m_fields->m_listener.spawn(
            req.get(url), 
            [this](web::WebResponse res) {
                if (!res.ok()) return;

                auto data = res.data();
                
                Loader::get()->queueInMainThread([this, data]() {
                    auto img = new CCImage();
                    if (img->initWithImageData(const_cast<unsigned char*>(data.data()), data.size())) {
                        auto tex = new CCTexture2D();
                        if (tex->initWithImage(img)) {
                            if (auto prog = static_cast<CCSprite*>(this->m_progressBar->getChildByID("prog"_spr))) {
                                prog->setTexture(tex);
                            }
                        }
                        tex->release();
                    }
                    img->release();
                });
            }
        );
    }

    void updateBar(float dt) {
        if (this->m_progressBar) {
            auto progSprite = static_cast<CCSprite*>(this->m_progressBar->getChildByIndex(0));
            auto newSprite = static_cast<CCSprite*>(this->m_progressBar->getChildByID("prog"_spr));

            newSprite->setColor({255, 255, 255});
            newSprite->setAnchorPoint({0, 0});
            newSprite->setPosition(progSprite->getPosition());

            float barWidth = this->m_progressBar->getContentSize().width;
            float fillWidth = progSprite->getTextureRect().size.width * progSprite->getScaleX();
            float fillRatio = fillWidth / barWidth;

            auto texSize = newSprite->getTexture()->getContentSize();
            float displayHeight = progSprite->getTextureRect().size.height * progSprite->getScaleY();

            newSprite->setScaleX(barWidth / texSize.width);
            newSprite->setScaleY(displayHeight / texSize.height);

            newSprite->setTextureRect(CCRect(0, 0, texSize.width * fillRatio, texSize.height));

            progSprite->setVisible(false);
        }
    }
    void startGame() {
        PlayLayer::startGame();

        if (this->m_progressBar) {
            this->scheduleOnce(schedule_selector(BPBPlayLayer::setupProgressIndecators), 0.0f);
        }
    }

    void destroyPlayer(PlayerObject* player, GameObject* object) {
        PlayLayer::destroyPlayer(player, object);

        auto deathPercent = this->getCurrentPercent();

        if (deathPercent > 0.1f) {
            if (deathPercent > m_fields->m_highestPercent) {
                m_fields->m_highestPercent = deathPercent;
            }

            if (deathPercent > m_fields->m_sessionBestPercent) {
                m_fields->m_sessionBestPercent = deathPercent;
            }

            changeProgressIdecators();
        }
    }

    void setupProgressIndecators(float) {
        auto progressBar = this->m_progressBar;

        if (Mod::get()->getSavedValue<bool>("custom_bar", false) == true) {
            auto url = Mod::get()->getSavedValue<std::string>("selected_bar", "");
            if (!url.empty()) {
                auto progSprite = static_cast<CCSprite*>(progressBar->getChildByIndex(0));
                auto newSprite = CCSprite::create();
                newSprite->setColor({255, 255, 255});
                newSprite->setAnchorPoint({0, 0});
                newSprite->setPosition(progSprite->getPosition());
                newSprite->setScale(progSprite->getScale());
                newSprite->setZOrder(-1);
                newSprite->setTextureRect(progSprite->getTextureRect());
                newSprite->setID("prog"_spr);

                this->m_progressBar->addChild(newSprite);

                fetchImage(url);

                this->schedule(schedule_selector(BPBPlayLayer::updateBar));
            }
        }


        auto sessionBest = CCSprite::create("sessionBest.png"_spr);
        sessionBest->setPositionX(0.0f);
        sessionBest->setPositionY(8.0f);
        sessionBest->setScale(0.15f);
        sessionBest->setID("sessionbest"_spr);
        sessionBest->setVisible(false);

        progressBar->addChild(sessionBest);

        auto bestPercent = CCSprite::create("best.png"_spr);
        bestPercent->setPositionX((progressBar->getContentSize().width * m_fields->m_highestPercent) / 100.0f);
        bestPercent->setPositionY(8.0f);
        bestPercent->setScale(0.17f);
        bestPercent->setID("bestpercent"_spr);

        progressBar->addChild(bestPercent);

        changeProgressIdecators();
    }

    void changeProgressIdecators() {
        auto progressBar = this->m_progressBar;

        if (progressBar) {
            if (m_fields->m_highestPercent > 1.0f) {
                auto bestPercent = progressBar->getChildByID("bestpercent"_spr);
                if (bestPercent) {
                    bestPercent->setVisible(true);
                    bestPercent->setPositionX((progressBar->getContentSize().width * m_fields->m_highestPercent) / 100.0f);
                }
            } else {
                auto bestPercent = progressBar->getChildByID("bestpercent"_spr);
                if (bestPercent) {
                    bestPercent->setVisible(false);
                }  
            }

            if (m_fields->m_sessionBestPercent > 1.0f) {
                auto sessionBest = progressBar->getChildByID("sessionbest"_spr);
                if (sessionBest) {
                    sessionBest->setVisible(true);
                    sessionBest->setPositionX((progressBar->getContentSize().width * m_fields->m_sessionBestPercent) / 100.0f);
                }
            }

            if (m_fields->m_sessionBestPercent == m_fields->m_highestPercent) {
                auto bestPercent = progressBar->getChildByID("bestpercent"_spr);
                auto sessionBest = progressBar->getChildByID("sessionbest"_spr);
                if (sessionBest && bestPercent) {
                    sessionBest->setVisible(false);
                    sessionBest->setPositionX((progressBar->getContentSize().width * m_fields->m_sessionBestPercent) / 100.0f);
                }
            }

            if (m_fields->m_highestPercent < 1.0f or m_fields->m_highestPercent >= 100.0f) {
                auto bestPercent = progressBar->getChildByID("bestpercent"_spr);

                if (bestPercent) {
                    bestPercent->setVisible(false);
                }
            }
        }
    }
};