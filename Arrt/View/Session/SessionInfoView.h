#pragma once
#include <QLineEdit>
#include <QWidget>

class SessionInfoButton;
class SessionPanelModel;
class SessionCreationView;
class RunningSessionView;

// panel used to visualize the status of the session and extend lease time or stop the session.

class SessionPanelView : public QWidget
{
public:
    SessionPanelView(SessionPanelModel* model, QWidget* parent = {});

protected:
    virtual bool event(QEvent* e) override;
    virtual bool eventFilter(QObject* watched, QEvent* e) override;
    virtual void paintEvent(QPaintEvent* e) override;
    virtual void resizeEvent(QResizeEvent* e) override;

private:
    QWidget* m_panel = {};
    SessionPanelModel* const m_model;
    SessionInfoButton* const m_button;

    SessionCreationView* const m_creationView;
    RunningSessionView* const m_runningView;

    void reLayout();
    void updateUi();

    // true when the session panel disables the rest of the Render UI
    bool m_isModal = false;
    bool m_isRunning = false;
};
