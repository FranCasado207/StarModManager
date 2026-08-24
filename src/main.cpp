#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "core/config/configmanager.h"
#include "core/mods/modlistmodel.h"
#include "core/mods/modinstaller.h"
#include "core/mods/modlistmanager.h"


int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QGuiApplication::setOrganizationName("Yakureito");
    QGuiApplication::setApplicationName("StarModManager");

    ConfigManager config;
    ModInstaller modInstaller;
    ModListModel modListModel;
    ModListManager modListManager;
    modListModel.setModsPath(config.modsPath());

    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty("configManager", &config);
    engine.rootContext()->setContextProperty("modInstaller", &modInstaller);
    engine.rootContext()->setContextProperty("modListModel", &modListModel);
    engine.rootContext()->setContextProperty("modListManager", &modListManager);

    engine.loadFromModule("com.starmodmanager.app", "Main");

    QObject::connect(&config, &ConfigManager::modsPathChanged, [&]() {
        modListModel.setModsPath(config.modsPath());
    });


    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
