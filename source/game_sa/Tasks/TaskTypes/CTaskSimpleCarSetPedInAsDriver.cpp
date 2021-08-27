#include "StdInc.h"

CTaskSimpleCarSetPedInAsDriver::CTaskSimpleCarSetPedInAsDriver(CVehicle* pTargetVehicle, CTaskUtilityLineUpPedWithCar* pUtility)
{
    m_bIsFinished = false;
    m_pAnim = nullptr;
    m_pTargetVehicle = pTargetVehicle;
    m_pUtility = pUtility;
    m_bWarpingInToCar = false;
    m_nDoorFlagsToClear = 0;
    m_nNumGettingInToClear = 0;
    if (pTargetVehicle)
        pTargetVehicle->RegisterReference(reinterpret_cast<CEntity**>(&m_pTargetVehicle));
}

CTaskSimpleCarSetPedInAsDriver::~CTaskSimpleCarSetPedInAsDriver()
{
    if (m_pTargetVehicle)
        m_pTargetVehicle->CleanUpOldReference(reinterpret_cast<CEntity**>(&m_pTargetVehicle));
}

CTask* CTaskSimpleCarSetPedInAsDriver::Clone() const
{
    return plugin::CallMethodAndReturn<CTask*, 0x649E00, CTask*>((CTask*)this);
}

bool CTaskSimpleCarSetPedInAsDriver::ProcessPed(CPed* ped)
{
    return plugin::CallMethodAndReturn<bool, 0x64B950, CTask*, CPed*>(this, ped);
}
