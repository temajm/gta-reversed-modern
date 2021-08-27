#include "CEvent.h"
#include "CVector.h"

enum eKnockOffType : uint8_t {
    KNOCK_OFF_TYPE_NONE = 0,
    KNOCK_OFF_TYPE_NONE_1 = 55, // same as KNOCK_OFF_TYPE_NONE = 0
    KNOCK_OFF_TYPE_FALL = 53,
    KNOCK_OFF_TYPE_SKIDBACK_FALLR = 54,
    KNOCK_OFF_TYPE_SKIDBACKFRONT = 49,
};

class CVehicle;

class CEventKnockOffBike : public CEvent {
public:
    CVector m_moveSpeed;
    CVector m_collisionImpactVelocity;
    float m_damageIntensity;
    float field_28;
    uint32_t m_time;
    CPed* m_ped;
    uint8_t m_knockOffDirection;
    uint8_t m_isVictimDriver : 1;
    uint8_t m_forceKnockOff : 1; // If set, then it ignores CPed::CantBeKnockedOffBike flag
    uint8_t m_knockOffType;
    uint8_t m_exitDoor;
    CVehicle* m_vehicle;

public:
    CEventKnockOffBike();
    // CEventKnockOffBike(const CEventKnockOffBike& event);
    // TODO fix signature CEventKnockOffBike(CVehicle*, CVector const&, CVector const&, float, float, eWeaponType, signed char, int, CPed*, bool, bool)
    CEventKnockOffBike(CVehicle* vehicle, const CVector& moveSpeed, const CVector& collisionImpactVelocity, float damageIntensity, float a6, uint8_t knockOffType, uint8_t knockOutDirection, int32_t time, CPed* ped, bool isVictimDriver, bool forceKnockOff);
    ~CEventKnockOffBike();

    eEventType GetEventType() const override { return EVENT_KNOCK_OFF_BIKE; }
    float GetLocalSoundLevel() override { return 60.0f; }
    int32_t GetEventPriority() const override { return 70; }
    int32_t GetLifeTime() override { return 0; }
    CEventKnockOffBike* Clone() const override { return new CEventKnockOffBike(m_vehicle, m_moveSpeed, m_collisionImpactVelocity, m_damageIntensity, field_28, m_knockOffType, m_knockOffDirection, m_time, m_ped, m_isVictimDriver, m_forceKnockOff); }
    bool AffectsPed(CPed* ped) override;
    bool IsCriminalEvent() override { return true; }
    void ReportCriminalEvent(CPed* ped) override;

    void operator=(const CEventKnockOffBike& right) { From(right); }
    void From(const CEventKnockOffBike& right);
    void SetPedOutCar(CPed* ped);
    int32_t CalcForcesAndAnims(CPed* ped);
    bool SetPedSafePosition(CPed* ped);

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CEventKnockOffBike* Constructor();
    CEventKnockOffBike* Constructor(CVehicle* vehicle, const CVector& moveSpeed, const CVector& collisionImpactVelocity, float damageIntensity, float a6, uint8_t knockOffType, uint8_t knockOutDirection, int32_t time, CPed* ped, bool isVictimDriver, bool forceKnockOff);

    bool AffectsPed_Reversed(CPed* ped);
    void ReportCriminalEvent_Reversed(CPed* ped);
};

VALIDATE_SIZE(CEventKnockOffBike, 0x3C);
