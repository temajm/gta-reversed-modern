#include "StdInc.h"

CTaskSimpleCarSetPedInAsPassenger::CTaskSimpleCarSetPedInAsPassenger(CVehicle* pTargetVehicle, int nTargetDoor, CTaskUtilityLineUpPedWithCar* pUtility)
{
    m_iTargetDoor = nTargetDoor;
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

CTaskSimpleCarSetPedInAsPassenger::~CTaskSimpleCarSetPedInAsPassenger()
{
    if (m_pTargetVehicle)
        m_pTargetVehicle->CleanUpOldReference(reinterpret_cast<CEntity**>(&m_pTargetVehicle));
}

CTask* CTaskSimpleCarSetPedInAsPassenger::Clone() const
{
    return plugin::CallMethodAndReturn<CTask*, 0x649D90, CTask*>((CTask*)this);
}

bool CTaskSimpleCarSetPedInAsPassenger::ProcessPed(CPed* ped)
{
    return plugin::CallMethodAndReturn<bool, 0x64B5D0, CTask*, CPed*>(this, ped);
}
