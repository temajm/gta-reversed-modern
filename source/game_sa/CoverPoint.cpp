#include "StdInc.h"

#include "CoverPoint.h"

void CCoverPoint::InjectHooks() {
    ReversibleHooks::Install("CCoverPoint", "CanAccomodateAnotherPed", 0x698E70, &CCoverPoint::CanAccomodateAnotherPed);
    ReversibleHooks::Install("CCoverPoint", "ReserveCoverPointForPed", 0x698EB0, &CCoverPoint::ReserveCoverPointForPed);
    ReversibleHooks::Install("CCoverPoint", "ReleaseCoverPointForPed", 0x698EF0, &CCoverPoint::ReleaseCoverPointForPed);
}

// 0x698E70
bool CCoverPoint::CanAccomodateAnotherPed() {
    if (!m_nMaxPedsInCover)
        return false;

    if (m_nMaxPedsInCover != 2)
        return !m_apCoveredPeds[0];

    return !m_apCoveredPeds[0] || m_apCoveredPeds[1];
}

// 0x698EB0
void CCoverPoint::ReserveCoverPointForPed(CPed* ped) {
    for (int i = 0; i < MAX_COVER_PEDS; i++) {
        if (CPed*& it = m_apCoveredPeds[i]) {
            it = ped;
            ped->RegisterReference(reinterpret_cast<CEntity**>(&it));
            break;
        }
    }
}

// 0x698EF0
void CCoverPoint::ReleaseCoverPointForPed(CPed* toRelease) {
    for (int i = 0; i < MAX_COVER_PEDS; i++) {
        CPed*& it = m_apCoveredPeds[i];
        if (it == toRelease) {
            it->CleanUpOldReference(reinterpret_cast<CEntity**>(&it));
            it = nullptr;
            break;
        }
    }
}
