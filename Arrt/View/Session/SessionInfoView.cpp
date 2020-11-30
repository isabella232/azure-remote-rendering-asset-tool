#include <QEvent>
#include <QStackedLayout>
#include <QStylePainter>
#include <QVBoxLayout>
#include <View/ArrtStyle.h>
#include <View/Session/RunningSessionView.h>
#include <View/Session/SessionCreationView.h>
#include <View/Session/SessionInfoButton.h>
#include <View/Session/SessionInfoView.h>
#include <ViewModel/Session/RunningSessionModel.h>
#include <ViewModel/Session/SessionPanelModel.h>

Q_DECLARE_METATYPE(SessionModel::Size);


// frame for the session information panel. It draws a rounded rectangle

class PanelFrame : public QWidget
{
public:
    virtual void paintEvent(QPaintEvent* e) override
    {
        QStylePainter p(this);
        p.setRenderHint(QPainter::RenderHint::Antialiasing);
        p.translate(0.5, 0.5);

        QRect r = rect();

        p.setPen(ArrtStyle::s_buttonEnabledColors.m_border);
        p.setBrush(ArrtStyle::s_buttonEnabledColors.m_background);
        p.drawRoundedRect(r.adjusted(1, 1, -1, -1), 8.0, 8.0);

        QWidget::paintEvent(e);
    }
};


SessionPanelView::SessionPanelView(SessionPanelModel* model, QWidget* parent)
    : QWidget(parent)
    , m_model(model)
    , m_button(new SessionInfoButton(model, this))
    , m_creationView(new SessionCreationView(m_model->getSessionCreationModel(), this))
    , m_runningView(new RunningSessionView(m_model->getRunningSessionModel(), this))
{
    m_panel = new PanelFrame();

    auto* l = new QVBoxLayout(m_panel);
    l->setContentsMargins(0, 0, 0, 0);
    l->addWidget(m_button);
    m_button->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

    QWidget* panel = new QWidget(this);
    panel->setContentsMargins(0, 0, 0, 0);
    auto* stackedLayout = new QStackedLayout(panel);
    stackedLayout->setContentsMargins(0, 0, 0, 0);
    l->addWidget(panel, 1);

    stackedLayout->addWidget(m_runningView);
    stackedLayout->addWidget(m_creationView);

    auto onSessionChanged = [this, stackedLayout]() {
        const bool running = m_model->isRunning();
        bool runningChanged = (m_isRunning != running);
        if (runningChanged)
        {
            m_isRunning = running;
        }
        stackedLayout->setCurrentWidget(m_isRunning ? (QWidget*)m_runningView : m_creationView);

        const bool isModal = m_model->isModal();
        if (isModal != m_isModal || runningChanged)
        {
            m_isModal = isModal;
            m_button->setChecked(isModal);
            updateUi();
            reLayout();
        }
    };
    connect(m_model, &SessionPanelModel::sessionChanged, this, onSessionChanged);
    onSessionChanged();
    m_button->setChecked(m_isModal);
    updateUi();
    reLayout();

    auto onToggled = [this, panel]() { panel->setVisible(m_button->isChecked()); updateUi(); };
    connect(m_button, &FlatButton::toggled, this, onToggled);
    onToggled();

    if (auto* p = parentWidget())
    {
        p->installEventFilter(this);
    }

    auto* mainHLayout = new QHBoxLayout(this);
    mainHLayout->setContentsMargins(0, 0, 0, 0);

    auto* mainVLayout = new QVBoxLayout();
    mainVLayout->setContentsMargins(0, 0, 0, 0);

    mainVLayout->addWidget(m_panel);
    mainVLayout->addStretch(1);
    mainHLayout->addStretch(1);
    mainHLayout->addLayout(mainVLayout);

    reLayout();
}

bool SessionPanelView::eventFilter(QObject* watched, QEvent* e)
{
    switch (e->type())
    {
        case QEvent::Resize:
        {
            reLayout();
            break;
        }
        default:
        {
        }
    }
    return QWidget::eventFilter(watched, e);
}

bool SessionPanelView::event(QEvent* e)
{
    switch (e->type())
    {
        case QEvent::ParentAboutToChange:
        {
            if (auto* p = parentWidget())
            {
                p->removeEventFilter(this);
            }
            break;
        }
        case QEvent::ParentChange:
        {
            if (auto* p = parentWidget())
            {
                p->installEventFilter(this);
                reLayout();
            }
            break;
        }
        default:
        {
            break;
        }
    }
    return QWidget::event(e);
}

void SessionPanelView::reLayout()
{
    if (auto* p = parentWidget())
    {
        if (m_isModal)
        {
            resize(p->size());
        }
        move({p->width() - width(), 0});
    }
}

void SessionPanelView::updateUi()
{
    if (!m_isModal)
    {
        resize(m_panel->minimumSizeHint());
    }
}

void SessionPanelView::paintEvent(QPaintEvent* e)
{
    if (m_isModal)
    {
        QStylePainter sp(this);
        sp.fillRect(rect(), QColor(0, 0, 0, 80));
        QWidget::paintEvent(e);
    }
}

void SessionPanelView::resizeEvent(QResizeEvent* e)
{
    if (!m_isModal)
    {
        QWidget::resizeEvent(e);
        reLayout();
    }
}
