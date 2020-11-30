#include <Model/Configuration.h>
#include <QDebug>
#include <ViewModel/Session/SessionCreationModel.h>

SessionCreationModel::SessionCreationModel(ArrSessionManager* sessionManager, Configuration* configuration, QObject* parent)
    : SessionModel(sessionManager, configuration, parent)
{
    connect(m_sessionManager, &ArrSessionManager::onEnabledChanged, this, [this]() {
        Q_EMIT onEnabledChanged();
    });
}

SessionModel::Size SessionCreationModel::getSize() const
{
    return static_cast<SessionModel::Size>(m_sessionManager->getSessionCreationParams().Size);
}

void SessionCreationModel::setSize(Size size)
{
    auto params = m_sessionManager->getSessionCreationParams();
    params.Size = RR::RenderingSessionVmSize(size);
    m_sessionManager->setSessionCreationParams(params);
}

SessionModel::Time SessionCreationModel::getLeaseTime() const
{
    auto params = m_sessionManager->getSessionCreationParams();
    return {params.MaxLease.hour, params.MaxLease.minute};
}

void SessionCreationModel::setLeaseTime(const Time& leaseTime)
{
    auto params = m_sessionManager->getSessionCreationParams();
    params.MaxLease = {(int)leaseTime.getHours(), (int)leaseTime.getMinutes(), 0};
    m_sessionManager->setSessionCreationParams(params);
}

bool SessionCreationModel::isAutomaticallyExtended() const
{
    uint minutesToAdd;
    bool extendAutomatically;
    m_sessionManager->getExtensionTime(minutesToAdd, extendAutomatically);
    return extendAutomatically;
}

void SessionCreationModel::setAutomaticallyExtended(bool autoExtension)
{
    m_sessionManager->setExtensionTime(getExtensionTime().m_totalMinutes, autoExtension);
}

SessionModel::Time SessionCreationModel::getExtensionTime() const
{
    uint minutesToAdd;
    bool extendAutomatically;
    m_sessionManager->getExtensionTime(minutesToAdd, extendAutomatically);
    return minutesToAdd;
}

void SessionCreationModel::setExtensionTime(Time extensionTime)
{
    m_sessionManager->setExtensionTime(extensionTime.m_totalMinutes, isAutomaticallyExtended());
}

// try to start a session
bool SessionCreationModel::start()
{
    if (!isRunning())
    {
        const bool succeeded = m_sessionManager->startSession();
        changed();
        if (!succeeded)
        {
            qWarning() << tr("Session didn't successfully start");
        }
        return succeeded;
    }
    else
    {
        // already running. Cannot start again
        return false;
    }
}

bool SessionCreationModel::isEnabled() const
{
    return m_sessionManager->isEnabled();
}
