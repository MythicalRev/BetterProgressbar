#pragma once

#include <cocos2d.h>
#include <geode/utils/file.hpp>
#include <fstream>

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

    void startGame() {
        PlayLayer::startGame();

        if (this->m_progressBar) {
            this->scheduleOnce(schedule_selector(BPBPlayLayer::setupProgressIndecators), 0.0f);
            changeProgressIdecators();
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

        auto sessionBest = CCSprite::create("sessionBest.png"_spr);
        sessionBest->setPositionX(0.0f);
        sessionBest->setPositionY(8.0f);
        sessionBest->setScale(0.15f);
        sessionBest->setID("bpb-sessionbest");
        sessionBest->setVisible(false);

        progressBar->addChild(sessionBest);

        auto bestPercent = CCSprite::create("best.png"_spr);
        bestPercent->setPositionX((progressBar->getContentSize().width * m_fields->m_highestPercent) / 100.0f);
        bestPercent->setPositionY(8.0f);
        bestPercent->setScale(0.17f);
        bestPercent->setID("bpb-bestpercent");

        progressBar->addChild(bestPercent);
    }

    void changeProgressIdecators() {
        auto progressBar = this->m_progressBar;

        if (progressBar) {
            if (m_fields->m_highestPercent > 1.0f) {
                auto bestPercent = progressBar->getChildByID("bpb-bestpercent");
                if (bestPercent) {
                    bestPercent->setVisible(true);
                    bestPercent->setPositionX((progressBar->getContentSize().width * m_fields->m_highestPercent) / 100.0f);
                }
            }

            if (m_fields->m_sessionBestPercent > 1.0f) {
                auto sessionBest = progressBar->getChildByID("bpb-sessionbest");
                if (sessionBest) {
                    sessionBest->setVisible(true);
                    sessionBest->setPositionX((progressBar->getContentSize().width * m_fields->m_sessionBestPercent) / 100.0f);
                }
            }

            if (m_fields->m_sessionBestPercent == m_fields->m_highestPercent) {
                auto bestPercent = progressBar->getChildByID("bpb-bestpercent");
                auto sessionBest = progressBar->getChildByID("bpb-sessionbest");
                if (sessionBest && bestPercent) {
                    sessionBest->setVisible(false);
                    sessionBest->setPositionX((progressBar->getContentSize().width * m_fields->m_sessionBestPercent) / 100.0f);
                }
            }

            if (m_fields->m_highestPercent < 1.0f or m_fields->m_highestPercent >= 100.0f) {
                auto bestPercent = progressBar->getChildByID("bpb-bestpercent");

                if (bestPercent) {
                    bestPercent->setVisible(false);
                }
            }
        }
    }
};