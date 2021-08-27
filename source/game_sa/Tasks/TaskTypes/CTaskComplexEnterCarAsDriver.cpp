#include "StdInc.h"

CTaskComplexEnterCarAsDriver::CTaskComplexEnterCarAsDriver(CVehicle* pTargetVehicle)
    : CTaskComplexEnterCar(pTargetVehicle, true, false, false, false)
{
    // empty
}

// 0x643780
CTask* CTaskComplexEnterCarAsDriver::Clone() const
{
    return CTaskComplexEnterCarAsDriver::Clone_Reversed();
}

CTask* CTaskComplexEnterCarAsDriver::Clone_Reversed() const
{
    auto pClonedTask = new CTaskComplexEnterCarAsDriver(m_pTargetVehicle);
    pClonedTask->m_nMoveState = m_nMoveState;
    return pClonedTask;
}
