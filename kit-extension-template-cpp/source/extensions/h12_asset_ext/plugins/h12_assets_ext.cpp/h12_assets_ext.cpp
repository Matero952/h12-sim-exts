#define CARB_EXPORTS
#include <carb/PluginUtils.h>
#include <omni/ext/IExt.h>
#include <omni/kit/IApp.h>
#include <carb/eventdispatcher/IEventDispatcher.h>

#include <cstdlib>
#include <iostream>
// #include <stdexcept>
#include <fstream>

const struct carb::PluginImplDesc pluginImplDesc = { "h12_assets_ext.cpp.plugin",
                                                     "The extension to load assets for h12-sim", "Matero952",
                                                     carb::PluginHotReload::eEnabled, "dev" };
CARB_PLUGIN_IMPL_DEPS(carb::eventdispatcher::IEventDispatcher)
// const std::string h12AssetsEnvVar = "H12_ASSETS_PATH";
// const char* pH12AssestsEnvVar = (h12AssetsEnvVar.c_str());
// bool errorWasThrown = false;
// const char* pH12AssestsEnvVar = &h12AssetsEnvVar;
namespace h12_assets_ext {
namespace cpp {
    class H12AssetsExtension : public omni::ext::IExt {
        public:
            void onStartup(const char* extId) override {
                if (carb::eventdispatcher::IEventDispatcher* eventDispatcher =
                    carb::getCachedInterface<carb::eventdispatcher::IEventDispatcher>()) {
                        //gets the event dispatcher from the carbonite framework
                        m_updateEventsSubscription = eventDispatcher->observeEvent(
                            carb::RStringKey("H12 Assets Extension"), carb::eventdispatcher::kDefaultOrder,
                            omni::kit::kGlobalEventPostUpdate,
                            [this](const carb::eventdispatcher::Event& e) {onUpdate(); });
                }
                std::cout << "1111111";
                bootstrapAssets();
                std::cout << "222222";
            }
            
            void bootstrapAssets() {
                std::cout << "44444";
                if (!std::getenv("H12_ASSETS_PATH")) {
                    std::cout << "H12_ASSETS_PATH IS A NULLPTR";
                }
                else {
                    std::cout << "H12_ASSETS_PATH IS NOT A NULLPTR";
                }
                // try {
                //     std::string foo = std::string(std::getenv("H12_ASSETS_PATH"));
                // }
                // catch (std::logic_error& e) {
                //     std::cout << e.what();
                //     printf("H12_ASSETS_PATH IS NOT SET");
                // }
                
                // const char* foo = std::getenv("H12_ASSETS_PATH");
                // std::cout << foo;
                // const char* readme = "/README.md";
                // std::cout << foo;
                // std::cout << readme;
                // std::string h12AssetsPathString = foo;
                std::cout << "333333";
                // if (foo == "") {
                //     std::cout << "'H12_ASSETS_PATH' is not set. Please clone the github repository and set the H12_ASSETS_PATH to its location.";
                //     // throw std::runtime_error("'H12_ASSETS_PATH' is not set. Please clone the github repository and set the H12_ASSETS_PATH to its location.");
                // }
                // else {
                //     std::ifstream fooFile(foo + readme);
                //     std::cout << "44444444";
                //     if (!fooFile.is_open()) {
                //         std::cout << ((typeid(std::getenv("H12_ASSETS_PATH")).name()));
                //         // printf(typeid)
                //         // printf((std::getenv("H12_ASSETS_PATH")) + std::string("is not the correct path."));
                //         // throw std::runtime_error(std::getenv("H12_ASSETS_PATH") + std::string("is not the correct path."));
                //     }
                // }
            }

            // bool checkAssetsExist(const char* h12AssetsRepoPath) {
            //     if (std::getenv(h12AssetsRepoPath)) {
            //         //if the env variable is set, check directory structure
            //         std::string stringH12AssetsRepoPath(h12AssetsRepoPath);

            //         std::ifstream inputFile(stringH12AssetsRepoPath + "/README.md");
            //         if (inputFile.is_open()) {
            //             return true;
            //         }
            //     }
            //     return false;
            // }

            void onShutdown() override {
                std::cout << "H12 Assets Extension shutting down. \n";
                m_updateEventsSubscription.reset();
            }

            void onUpdate() {
                // if (m_updateCounter % 1000 == 0) {
                //     std::cout << "Hi from the H12 Assets Extensi";
                //     // if (errorWasThrown) {
                //     //     printf("ERROR WAS THROWN");
                //     // }

                //     // printf("Hi from the H12 Assets Extenson! %d updates counted. \n", m_updateCounter);
                // }
            }
        
        private:
            carb::eventdispatcher::ObserverGuard m_updateEventsSubscription;
            int m_updateCounter = 0;
    };
}
}

CARB_PLUGIN_IMPL(pluginImplDesc, h12_assets_ext::cpp::H12AssetsExtension)



void fillInterface(h12_assets_ext::cpp::H12AssetsExtension& iface)
{
}
