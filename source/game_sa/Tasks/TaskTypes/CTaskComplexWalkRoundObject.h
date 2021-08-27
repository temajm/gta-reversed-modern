#include "CTaskComplex.h"
#include "CVector.h"

class CPointRoute;

class CTaskComplexWalkRoundObject : public CTaskComplex {
public:
    int32_t m_moveState;
    CVector m_targetPoint;
    CEntity* m_object;
    CPointRoute* m_pointRoute;
    int32_t field_24;
    int32_t field_28;
    int8_t field_2C;
    int8_t field_2D;
    int32_t field_30;

public:
    CTaskComplexWalkRoundObject(int32_t moveState, const CVector& targetPoint, CEntity* object);
    ~CTaskComplexWalkRoundObject();

    CTask* Clone() const override { return new CTaskComplexWalkRoundObject(m_moveState, m_targetPoint, m_object); }
    eTaskType GetId() override { return TASK_COMPLEX_WALK_ROUND_OBJECT; }
    CTask* CreateNextSubTask(CPed* ped) override;
    CTask* CreateFirstSubTask(CPed* ped) override;
    CTask* ControlSubTask(CPed* ped) override;

    CTask* CreateRouteTask(CPed* ped);

private:
    friend void InjectHooksMain();
    static void InjectHooks();

    CTaskComplexWalkRoundObject* Constructor(int32_t moveState, const CVector& targetPoint, CEntity* object);
};

VALIDATE_SIZE(CTaskComplexWalkRoundObject, 0x34);
