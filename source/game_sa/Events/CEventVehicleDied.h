#include "CEvent.h"

class CVehicle;

class CEventVehicleDied : public CEvent {
public:
    CVehicle* m_vehicle;
    bool      m_IsOccupantLeavingCar;
    char      _pad[3];

public:
    CEventVehicleDied (CVehicle* vehicle);
    ~CEventVehicleDied ();

    eEventType GetEventType() const override { return EVENT_VEHICLE_DIED; }
    int32_t GetEventPriority() const override { return 72; }
    int GetLifeTime() override { return 0; }
    CEvent* Clone() const override;
    bool AffectsPed(CPed* ped) override;

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CEventVehicleDied * Constructor(CVehicle* vehicle);

    CEvent* Clone_Reversed() const;
    bool AffectsPed_Reversed(CPed* ped);
};

VALIDATE_SIZE(CEventVehicleDied, 0x14);
