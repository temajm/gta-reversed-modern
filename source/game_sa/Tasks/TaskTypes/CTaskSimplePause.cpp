#include "StdInc.h"

void CTaskSimplePause::InjectHooks()
{
    HookInstall(0x48E750, &CTaskSimplePause::Constructor);
    HookInstall(0x48E830, &CTaskSimplePause::ProcessPed_Reversed);
    HookInstall(0x48E810, &CTaskSimplePause::MakeAbortable_Reversed);
}

CTaskSimplePause::CTaskSimplePause(int time)
{
    m_timer.m_nStartTime = 0;
    m_timer.m_nInterval = 0;
    m_timer.m_bStarted = false;
    m_timer.m_bStopped = false;
    m_nTime = time;
}

CTaskSimplePause::~CTaskSimplePause()
{
    // nothing here
}

// 0x48E750
CTaskSimplePause* CTaskSimplePause::Constructor(int time)
{
    this->CTaskSimplePause::CTaskSimplePause(time);
    return this;
}

CTask* CTaskSimplePause::Clone() const
{
    return plugin::CallMethodAndReturn<CTask*, 0x48E780, CTask*>((CTask*)this);
}

bool CTaskSimplePause::MakeAbortable_Reversed(CPed* ped, eAbortPriority priority, const CEvent* event)
{
    m_timer.m_nStartTime = CTimer::m_snTimeInMilliseconds;
    m_timer.m_nInterval = -1;
    m_timer.m_bStarted = true;
    return true;
}

// 0x48E810
bool CTaskSimplePause::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event)
{
    return CTaskSimplePause::MakeAbortable_Reversed(ped, priority, event);
}

bool CTaskSimplePause::ProcessPed_Reversed(CPed* ped)
{
    if (!m_timer.m_bStarted && !m_timer.Start(m_nTime)) {
        return false;
    }
    m_timer.Reset();
    return m_timer.IsOutOfTime();
}

// 0x48E830
bool CTaskSimplePause::ProcessPed(CPed* ped)
{
    return CTaskSimplePause::ProcessPed_Reversed(ped);
}
