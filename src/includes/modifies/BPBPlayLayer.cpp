#include <Geode/Geode.hpp>
#include <Geode/utils/base64.hpp>

using namespace geode::prelude;

#include <Geode/modify/PlayLayer.hpp>
class $modify(BPBPlayLayer, PlayLayer) {
    struct Fields {
        float m_highestPercent = 0.0f;
        float m_sessionBestPercent = 0.0f;
    };

    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        PlayLayer::init(level, useReplay, dontCreateObjects);

        m_fields->m_highestPercent = level->m_normalPercent;

        return true;
    }

    void fetchImage(std::string dataString) {
        geode::ByteVector data = geode::utils::base64::decode(dataString).unwrapOr(geode::ByteVector{});
        
        if (data.empty()) {
            log::error("Base64 decoding failed or string data was corrupted.");
            return;
        }
                
        Loader::get()->queueInMainThread([this, data]() {
            auto img = new CCImage();
            if (img->initWithImageData(const_cast<unsigned char*>(data.data()), data.size())) {
                auto tex = new CCTexture2D();
                if (tex->initWithImage(img)) {
                    if (auto prog = static_cast<CCSprite*>(this->m_progressBar->getChildByID("prog"_spr))) {
                        auto progSprite = static_cast<CCSprite*>(this->m_progressBar->getChildByIndex(0));

                        prog->setTexture(tex);
                        prog->setTextureRect({0, 0, tex->getContentSize().width, tex->getContentSize().height / 1.25f});

                        prog->setColor({255, 255, 255});
                        prog->setAnchorPoint({0, .5f});
                        prog->setPosition(ccp(progSprite->getPositionX(), this->m_progressBar->getContentSize().height / 2));

                        float barWidth = this->m_progressBar->getContentSize().width;
                        float fillWidth = progSprite->getTextureRect().size.width * progSprite->getScaleX();
                        float fillRatio = fillWidth / barWidth;

                        auto texSize = prog->getTexture()->getContentSize();
                        float displayHeight = progSprite->getTextureRect().size.height * progSprite->getScaleY();

                        prog->setScaleX(barWidth / texSize.width);
                        prog->setScaleY((displayHeight / texSize.height) * 0.65f);

                        prog->setTextureRect(CCRect(0, 0, texSize.width * fillRatio, texSize.height));
                        
                        progSprite->setZOrder(-2);
                        prog->setZOrder(-1);
                    }
                }
                tex->release();
            }
            img->release();
        });
    }

    void updateBar(float dt) {
        if (this->m_progressBar) {
            if (Mod::get()->getSavedValue<bool>("custom_bar", false) == true) {
                if (Mod::get()->getSettingValue<std::filesystem::path>("customBar") != "Please pick an image file.") {
                    auto data = loadImageToByteVector(Mod::get()->getSettingValue<std::filesystem::path>("customBar"));
                    std::string dataString = geode::utils::base64::encode(data);

                    if (!dataString.empty()) {
                        fetchImage(dataString);
                    }
                } else {
                    auto dataString = Mod::get()->getSavedValue<std::string>("selected_bar", "");
                    if (!dataString.empty()) {
                        fetchImage(dataString);
                    }
                }
            }
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
            if (Mod::get()->getSettingValue<std::filesystem::path>("customBar") != "Please pick an image file.") {
                auto data = loadImageToByteVector(Mod::get()->getSettingValue<std::filesystem::path>("customBar"));
                std::string dataString = geode::utils::base64::encode(data);

                if (!dataString.empty()) {
                    auto progSprite = static_cast<CCSprite*>(progressBar->getChildByIndex(0));
                    auto newSprite = CCSprite::create();
                    newSprite->setColor({255, 255, 255});
                    newSprite->setAnchorPoint({0, .5f});
                    newSprite->setPosition(ccp(progSprite->getPositionX(), this->m_progressBar->getContentSize().height / 2));
                    newSprite->setScale(progSprite->getScale());
                    newSprite->setZOrder(-1);
                    newSprite->setTextureRect(progSprite->getTextureRect());
                    newSprite->setID("prog"_spr);

                    this->m_progressBar->addChild(newSprite);

                    this->schedule(schedule_selector(BPBPlayLayer::updateBar));
                }
            } else {
                auto dataString = Mod::get()->getSavedValue<std::string>("selected_bar", "");
                if (!dataString.empty()) {
                    auto progSprite = static_cast<CCSprite*>(progressBar->getChildByIndex(0));
                    auto newSprite = CCSprite::create();
                    newSprite->setColor({255, 255, 255});
                    newSprite->setAnchorPoint({0, .5f});
                    newSprite->setPosition(ccp(progSprite->getPositionX(), this->m_progressBar->getContentSize().height / 2));
                    newSprite->setScale(progSprite->getScale());
                    newSprite->setZOrder(-1);
                    newSprite->setTextureRect(progSprite->getTextureRect());
                    newSprite->setID("prog"_spr);

                    this->m_progressBar->addChild(newSprite);

                    fetchImage(dataString);

                    this->schedule(schedule_selector(BPBPlayLayer::updateBar));
                }
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

    geode::ByteVector loadImageToByteVector(const std::filesystem::path& imagePath) {
        std::ifstream file(imagePath, std::ios::binary | std::ios::ate);

        if (!file.is_open()) {
            geode::log::error("Failed to open image file: {}", imagePath.string());
            return {};
        }

        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        geode::ByteVector buffer(size);
        if (file.read(reinterpret_cast<char*>(buffer.data()), size)) {
            return buffer;
        }

        geode::log::error("Failed to read image data from: {}", imagePath.string());
        return {};
    }
};