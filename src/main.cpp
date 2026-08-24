#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include "core/config/configmanager.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QGuiApplication::setOrganizationName("Yakureito");
    QGuiApplication::setApplicationName("StarModManager");

    ConfigManager config;

    QQmlApplicationEngine engine;


    engine.loadFromModule("com.starmodmanager.app", "Main");
    engine.rootContext()->setContextProperty("configManager", &config);
    //engine.load(QUrl("qrc:/qt/qml/com/starmodmanager/app/Main.qml"));


    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
