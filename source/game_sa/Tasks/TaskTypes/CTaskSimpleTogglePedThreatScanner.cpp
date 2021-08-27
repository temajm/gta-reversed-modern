#include "StdInc.h"

CTaskSimpleTogglePedThreatScanner::CTaskSimpleTogglePedThreatScanner(bool bScanAllowedScriptPed, bool bScanAllowedInVehicle, bool bScanAllowedScriptedTask)
{
    m_bScanAllowedScriptPed = bScanAllowedScriptPed;
    m_bScanAllowedInVehicle = bScanAllowedInVehicle;
    m_bScanAllowedScriptedTask = bScanAllowedScriptedTask;
}

CTask* CTaskSimpleTogglePedThreatScanner::Clone() const
{
    return plugin::CallMethodAndReturn<CTask*, 0x492D50, CTask*>((CTask*)this);
}

bool CTaskSimpleTogglePedThreatScanner::ProcessPed(CPed* ped)
{
    return plugin::CallMethodAndReturn<bool, 0x6337A0, CTask*, CPed*>(this, ped);
}
