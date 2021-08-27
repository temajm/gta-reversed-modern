#include "CEvent.h"

class CEventDeath : public CEvent {
    bool     m_bDrowning;
    char     _pad[3];
    uint32_t m_deathTimeInMs;

public:
    CEventDeath(bool bDrowning);
    CEventDeath(bool bDrowning, unsigned int deathTimeInMs);
    ~CEventDeath();

    eEventType GetEventType() const override { return EVENT_DEATH; }
    int32_t GetEventPriority() const override { return 73; }
    int GetLifeTime() override { return 0; }
    CEvent* Clone() const override;
    bool AffectsPed(CPed* ped) override { return 1; }

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CEventDeath* Constructor(bool bDrowning);

    CEvent* Clone_Reversed() const;
};

VALIDATE_SIZE(CEventDeath, 0x14);
