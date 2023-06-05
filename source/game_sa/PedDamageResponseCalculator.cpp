#include "StdInc.h"

#include "PedDamageResponseCalculator.h"

float& CPedDamageResponseCalculator::ms_damageFactor = *(float*)0x8A6260; // 5555.5498

void CPedDamageResponseCalculator::InjectHooks() {
    RH_ScopedClass(CPedDamageResponseCalculator);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Constructor, 0x4AD3F0);
    RH_ScopedInstall(AccountForPedDamageStats, 0x4AD430, { .reversed = false });
    RH_ScopedInstall(AccountForPedArmour, 0x4AD550, { .reversed = false });
    RH_ScopedInstall(ComputeWillForceDeath, 0x4AD610, { .reversed = false });
    RH_ScopedInstall(ComputeWillKillPed, 0x4B3210, { .reversed = false });
    RH_ScopedInstall(IsBleedingWeapon, 0x4B5C2A, { .reversed = false });
    RH_ScopedInstall(ComputeDamageResponse, 0x4B5AC0, { .reversed = false });
}

// 0x4AD3F0
CPedDamageResponseCalculator::CPedDamageResponseCalculator(const CEntity* entity, float fDamage, eWeaponType weaponType, ePedPieceTypes bodyPart, bool bSpeak) {
    m_pDamager      = entity;
    m_fDamageFactor = fDamage;
    m_bodyPart      = bodyPart;
    m_weaponType    = weaponType;
    m_bSpeak        = bSpeak;
}

CPedDamageResponseCalculator* CPedDamageResponseCalculator::Constructor(CEntity* entity, float fDamage, eWeaponType weaponType, ePedPieceTypes bodyPart, bool bSpeak) {
    this->CPedDamageResponseCalculator::CPedDamageResponseCalculator(entity, fDamage, weaponType, bodyPart, bSpeak);
    return this;
}

/*!
 *
 * @param ped
 * @param response
 * @addr 0x4AD430
 */
void CPedDamageResponseCalculator::AccountForPedDamageStats(CPed* ped, CPedDamageResponse& response) {
    plugin::CallMethod<0x4AD430, CPedDamageResponseCalculator*, CPed*, CPedDamageResponse&>(this, ped, response);
}

/*!
 *
 * @param ped
 * @param response
 * @addr 0x4AD550
 */
void CPedDamageResponseCalculator::AccountForPedArmour(CPed* ped, CPedDamageResponse& response) {
    plugin::CallMethod<0x4AD550, CPedDamageResponseCalculator*, CPed*, CPedDamageResponse&>(this, ped, response);
    return; // todo: not tested

    if (ped->m_fArmour == 0.0f)
        return;

    if (m_weaponType == WEAPON_DROWNING || m_weaponType == WEAPON_FALL)
        return;

    if (FindPlayerPed() == ped)
        CWorld::Players[CWorld::PlayerInFocus].m_nLastTimeArmourLost = CTimer::GetTimeInMS();

    if (m_fDamageFactor > ped->m_fArmour) {
        m_fDamageFactor = m_fDamageFactor - ped->m_fArmour;
        response.m_fDamageArmor = ped->m_fArmour;
        ped->m_fArmour = 0.0f;
    } else {
        response.m_fDamageArmor = m_fDamageFactor;
        ped->m_fArmour = ped->m_fArmour - m_fDamageFactor;
        m_fDamageFactor = 0.0f;
    }
}

/*!
 *
 * @param ped
 * @addr Added in Android
 */
void CPedDamageResponseCalculator::AdjustPedDamage(CPed* ped) {

}

/*!
 *
 * @param ped
 * @param response
 * @return
 * @addr 0x4AD610
 */
bool CPedDamageResponseCalculator::ComputeWillForceDeath(CPed* ped, CPedDamageResponse& response) {
    return plugin::CallMethodAndReturn<bool, 0x4AD610, CPedDamageResponseCalculator*, CPed*, CPedDamageResponse&>(this, ped, response);
}

/*!
 *
 * @param ped
 * @param response
 * @param bSpeak
 * @addr 0x4B3210
 */
void CPedDamageResponseCalculator::ComputeWillKillPed(CPed* ped, CPedDamageResponse& response, bool bSpeak) {
    plugin::CallMethod<0x4B3210, CPedDamageResponseCalculator*, CPed*, CPedDamageResponse&, bool>(this, ped, response, bSpeak);
}

/*!
 *
 * @param ped
 * @return
 * @addr 0x4B5C2A inlined
 */
bool CPedDamageResponseCalculator::IsBleedingWeapon(CPed* ped) const {
    if (ped->IsPlayer())
        return false;

    if (m_weaponType == WEAPON_KNIFE || m_weaponType == WEAPON_KATANA || m_weaponType == WEAPON_CHAINSAW)
        return true;

    return false;
}

/*!
 *
 * @param ped
 * @param response
 * @param bSpeak
 * @addr 0x4B5AC0
 */
void CPedDamageResponseCalculator::ComputeDamageResponse(CPed* ped, CPedDamageResponse& response, bool bSpeak) {
    plugin::CallMethod<0x4B5AC0, CPedDamageResponseCalculator*, CPed*, CPedDamageResponse&, bool>(this, ped, response, bSpeak);
}

