#pragma once

#include "CEventVehicleDamage.h"

class CEventVehicleDamageWeapon : public CEventVehicleDamage {
public:
    CEventVehicleDamageWeapon(CVehicle* vehicle, CEntity* attacker, eWeaponType weaponType);
    ~CEventVehicleDamageWeapon();

    eEventType GetEventType() const override { return EVENT_VEHICLE_DAMAGE_WEAPON; }
    CEventEditableResponse* CloneEditable() const override;

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CEventVehicleDamageWeapon* Constructor(CVehicle* vehicle, CEntity* attacker, eWeaponType weaponType);

    CEventEditableResponse* CloneEditable_Reversed() const;
};

VALIDATE_SIZE(CEventVehicleDamageWeapon, 0x20);
