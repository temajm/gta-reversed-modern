#include "StdInc.h"

#include "Pickup.h"
#include "Radar.h"

void CPickup::InjectHooks() {
    RH_ScopedClass(CPickup);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(ExtractAmmoFromPickup, 0x454BE0);
    RH_ScopedInstall(FindStringForTextIndex, 0x455540);
    RH_ScopedInstall(FindTextIndexForString, 0x455500);
    RH_ScopedInstall(GetPosn, 0x4549A0);
    RH_ScopedInstall(GetRidOfObjects, 0x454CF0);
    // RH_ScopedInstall(GiveUsAPickUpObject, 0x4567E0);
    RH_ScopedInstall(IsVisible, 0x454C70);
    RH_ScopedInstall(PickUpShouldBeInvisible, 0x454D20);
    RH_ScopedInstall(ProcessGunShot, 0x4588B0);
    RH_ScopedInstall(Remove, 0x4556C0);
    RH_ScopedInstall(SetPosn, 0x454960);
    // RH_ScopedInstall(Update, 0x457410);
}

// 0x454BE0
void CPickup::ExtractAmmoFromPickup(CPlayerPed* player) {
    plugin::CallMethod<0x454BE0, CPickup*, CPlayerPed*>(this, player);
}

// 0x455540
const char* CPickup::FindStringForTextIndex(int32 index) {
    if (index == 1)
        return "PROP_3";
    if (index == 2)
        return "PROP_4";
    return "FESZ_CA";
}

// 0x455500
int32 CPickup::FindTextIndexForString(char* message) {
    if (!message)
        return 0;
    if (!_stricmp("PROP_3", message))
        return 1;
    if (!_stricmp("PROP_4", message))
        return 2;
    return 0;
}

// 0x454BE0
void CPickup::ExtractAmmoFromPickup(CPlayerPed* player) {
    if (m_nPickupType == ePickupType::PICKUP_IN_SHOP)
        return;

    const auto weapon = CPickups::WeaponForModel(player->m_nModelIndex);
    switch (CWeaponInfo::GetWeaponInfo(weapon, eWeaponSkill::STD)->m_nSlot) {
    case 3:
    case 4:
    case 5:
        break;
    default:
        return;
    }

    if (!m_nAmmo && m_nFlags.bEmpty)
        return;

    player->GrantAmmo(weapon, m_nAmmo ? m_nAmmo : CWeaponInfo::GetAmmoForWeaponOnStreet(weapon));
    AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_PICKUP_WEAPON, 0.0, 1.0);

    m_nAmmo = 0;
    m_nFlags.bEmpty = true;
}

// 0x454C70
bool CPickup::IsVisible() {
    return DistanceBetweenPoints2D(GetPosn(), TheCamera.GetPosition()) < 100.0f;
    return DistanceBetweenPoints(TheCamera.GetPosition(), {GetXCoord(), GetYCoord(), 0.0f}) < 100.0f; // Izzotop: maybe not same
}

// 0x454CF0
void CPickup::GetRidOfObjects() {
    if (m_pObject) {
        CWorld::Remove(m_pObject);
        delete m_pObject;
        m_pObject = nullptr;
    }
}

// 0x454D20
bool CPickup::PickUpShouldBeInvisible() {
    if (CCutsceneMgr::ms_running)
        return true;

    if (m_nModelIndex == ModelIndices::MI_PICKUP_KILLFRENZY) {
        if (CTheScripts::IsPlayerOnAMission())
            return true;
        if (CDarkel::FrenzyOnGoing())
            return true;
        if (!CLocalisation::KillFrenzy())
            return true;
    }

    if (m_nPickupType == ePickupType::PICKUP_2P && FindPlayerPed(1))
        return true;

    if (CLocalisation::GermanGame()) {
        if (m_nModelIndex == ModelIndices::MI_PICKUP_2P_KILLFRENZY) {
            if (DistanceBetweenPoints2D(CVector2D(1479.0f, -1658.0f), CVector2D(GetPosn())) < 10.0f)
                return true;
            if (DistanceBetweenPoints2D(CVector2D(2511.0f, 1208.0f), CVector2D(GetPosn())) < 10.0f)
                return true;
        }
    }

    if (TheCamera.m_bWideScreenOn) { // Originally included in the above big `if`
        if (m_nPickupType != ePickupType::PICKUP_ASSET_REVENUE) {
            if (m_nModelIndex != eModelID::MODEL_FIRE_EX) {
                return true;
            }
        }
    }

    if (CGameLogic::IsCoopGameGoingOn() && !CWeapon::CanBeUsedFor2Player(CPickups::WeaponForModel(m_nModelIndex)))
        return true;

    return true;
}

// 0x4556C0
void CPickup::Remove() {
    CRadar::ClearBlipForEntity(BLIP_PICKUP, CPickups::GetUniquePickupIndex(this));
    GetRidOfObjects();
    m_nPickupType = ePickupType::PICKUP_NONE;
    m_nFlags.bDisabled = true;
}

// 0x4567E0
void CPickup::GiveUsAPickUpObject(CObject** pObject, int32 objectPoolSlotIndex) {
    plugin::CallMethod<0x4567E0, CPickup*, CObject**, int32>(this, pObject, objectPoolSlotIndex);
}

// 0x457410
bool CPickup::Update(CPlayerPed* playerPed, CVehicle* vehicle, int32 playerId) {
    return plugin::CallMethodAndReturn<bool, 0x457410, CPickup*, CPlayerPed*, CVehicle*, int32>(this, playerPed, vehicle, playerId);
}

// 0x4588B0
void CPickup::ProcessGunShot(CVector* start, CVector* end) {
    if (!m_pObject)
        return;

    if (CCollision::TestLineSphere({start, end}, {4.0f, m_pObject->GetPosition()})) {
        const auto posn = m_pObject->GetPosition();
        CExplosion::AddExplosion(nullptr, nullptr, EXPLOSION_MINE, posn, 0, true, -1.0f, false);
        Remove();
    }
}

// 0x455500
int32 CPickup::FindTextIndexForString(const char* message) {
    if (!message)
        return 0;

    if (stricmp(message, "PROP_3") == 0)
        return 1;

    if (stricmp(message, "PROP_4") == 0)
        return 2;

    return 0;
}

// 0x455540
const char* CPickup::FindStringForTextIndex(int32 state) {
    switch (state) {
    case 1:
        return "PROP_3";
    case 2:
        return "PROP_4";
    }
    return "FESZ_CA";
}
