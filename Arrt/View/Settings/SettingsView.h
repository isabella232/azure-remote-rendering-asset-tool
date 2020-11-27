#pragma once
#include <QWidget>

class SettingsModel;

// View with Azure Storage account and ARR account settings configurations.

class SettingsView : public QWidget
{
    Q_OBJECT
public:
    SettingsView(SettingsModel* settingsModel, QWidget* parent = nullptr);
    void open();
    void close();

Q_SIGNALS:
    void focusOutside();

private:
    bool m_open = false;
    QMetaObject::Connection m_focusConnection;
};
