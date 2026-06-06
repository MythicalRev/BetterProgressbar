#include <Geode/Geode.hpp>
#include <Geode/loader/SettingV3.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>

#include "includes/custom/CustomBarPopup.hpp"
#include "includes/modifies/BPBPlayLayer.cpp"

using namespace geode::prelude;

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

$on_mod(Loaded) {
    listenForSettingChanges<std::filesystem::path>("customBar", [](std::filesystem::path value) {
        if (Mod::get()->getSettingValue<std::filesystem::path>("customBar") != "Please pick an image file.") {
            auto data = loadImageToByteVector(Mod::get()->getSettingValue<std::filesystem::path>("customBar"));

            Loader::get()->queueInMainThread([data]() {
                auto img = new CCImage();
                if (img->initWithImageData(const_cast<unsigned char*>(data.data()), data.size())) {
                    auto tex = new CCTexture2D();
                    if (tex->initWithImage(img)) {

                        if (tex->getPixelsHigh() <= 1000 && tex->getPixelsWide() <= 1000) {
                            Mod::get()->setSavedValue<bool>("custom_bar", true);
                        } else {
                            geode::Notification::create("Image Too Large! (Max 1000x1000)", geode::NotificationIcon::Error, 5.0f)->show();
                            Mod::get()->setSettingValue<std::filesystem::path>("customBar", "Please pick an image file.");
                        }

                        tex->release();
                        img->release();
                    }
                }
                img->release();
            });
        }
    });
}

#include <Geode/modify/PauseLayer.hpp>
class $modify(BPBPauseLayer, PauseLayer) {
    void customSetup() {
        PauseLayer::customSetup();

        auto spr = CircleButtonSprite::createWithSprite("buttonicon.png"_spr, 1.f, CircleBaseColor::Green, CircleBaseSize::Tiny);

        spr->setScale(1.2f);

        auto openBars = CCMenuItemSpriteExtra::create(
            spr,
            this,
            menu_selector(BPBPauseLayer::onButton)
        );

        auto menu = this->getChildByID("right-button-menu");
        menu->addChild(openBars);
        openBars->setID("openBars"_spr);
        menu->updateLayout();
    }

    void onButton(CCObject*) {
        CustomBarPopup::create()->show();
    }
};