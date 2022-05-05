#include "StdInc.h"

#include "Boat.h"
#include "CarCtrl.h"

CBoat* (&CBoat::apFrameWakeGeneratingBoats)[NUM_WAKE_GEN_BOATS] = *(CBoat*(*)[NUM_WAKE_GEN_BOATS])0xC27994;
float& CBoat::MAX_WAKE_LENGTH = *(float*)0x8D3938;   // 50.0f
float& CBoat::MIN_WAKE_INTERVAL = *(float*)0x8D393C; // 2.0f
float& CBoat::WAKE_LIFETIME = *(float*)0x8D3940;     // 150.0f
float& CBoat::fShapeLength = *(float*)0x8D3944;      // 0.4f
float& CBoat::fShapeTime = *(float*)0x8D3948;        // 0.05f
float& CBoat::fRangeMult = *(float*)0x8D394C;        // 0.6f

int16 (&CBoat::waUnknArr)[4] = *(int16(*)[4])0xC279A4;
int16 (&CBoat::waUnknArr2)[4] = *(int16(*)[4])0xC279AC;

RxObjSpace3DVertex* CBoat::aRenderVertices = (RxObjSpace3DVertex*)0xC278F8;
RxVertexIndex* CBoat::auRenderIndices = (RxVertexIndex*)0xC27988;

void CBoat::InjectHooks() {
    RH_ScopedClass(CBoat);
    RH_ScopedCategory("Vehicle");

    RH_ScopedVirtualInstall(SetModelIndex, 0x6F1140);
    RH_ScopedVirtualInstall(ProcessControl, 0x6F1770);
    RH_ScopedVirtualInstall(Teleport, 0x6F20E0);
    RH_ScopedVirtualInstall(PreRender, 0x6F1180);
    RH_ScopedVirtualInstall(Render, 0x6F0210);
    RH_ScopedVirtualInstall(ProcessControlInputs, 0x6F0A10);
    RH_ScopedVirtualInstall(GetComponentWorldPosition, 0x6F01D0);
    RH_ScopedVirtualInstall(ProcessOpenDoor, 0x6F0190);
    RH_ScopedVirtualInstall(BlowUpCar, 0x6F21B0);
    RH_ScopedInstall(PruneWakeTrail, 0x6F0E20);
    RH_ScopedInstall(AddWakePoint, 0x6F2550);
    RH_ScopedInstall(SetupModelNodes, 0x6F01A0);
    RH_ScopedInstall(DebugCode, 0x6F0D00);
    RH_ScopedInstall(ModifyHandlingValue, 0x6F0DE0);
    RH_ScopedInstall(PrintThrustAndRudderInfo, 0x6F0D90);
    RH_ScopedInstall(FillBoatList, 0x6F2710);
    RH_ScopedInstall(IsSectorAffectedByWake, 0x6F0E80);
    RH_ScopedInstall(IsVertexAffectedByWake, 0x6F0F50);
    RH_ScopedInstall(CheckForSkippingCalculations, 0x6F10C0);
    RH_ScopedGlobalInstall(GetBoatAtomicObjectCB, 0x6F00D0);
}

// 0x6F2940
CBoat::CBoat(int32 modelIndex, eVehicleCreatedBy createdBy) : CVehicle(createdBy) {
    // plugin::CallMethod<0x6F2940, CBoat*, int32, eVehicleCreatedBy>(this, modelIndex, createdBy);
    // return;

    memset(&m_boatFlap, 0, sizeof(m_boatFlap));
    CVehicleModelInfo* mi = CModelInfo::GetModelInfo(modelIndex)->AsVehicleModelInfoPtr();
    m_nVehicleType = VEHICLE_TYPE_BOAT;
    m_nVehicleSubType = VEHICLE_TYPE_BOAT;
    m_vecBoatMoveForce.Set(0.0F, 0.0F, 0.0F);
    m_vecBoatTurnForce.Set(0.0F, 0.0F, 0.0F);
    m_nPadNumber = 0;
    m_fMovingHiRotation = 0.0f;
    m_fPropSpeed = 0.0f;
    m_fPropRotation = 0.0f;
    m_nAttackPlayerTime = CTimer::GetTimeInMS();
    CVehicle::SetModelIndex(modelIndex);
    SetupModelNodes();

    m_pHandlingData = gHandlingDataMgr.GetVehiclePointer(mi->m_nHandlingId);
    m_nHandlingFlagsIntValue = m_pHandlingData->m_nHandlingFlags;
    m_pFlyingHandlingData = gHandlingDataMgr.GetFlyingPointer(mi->m_nHandlingId);
    m_pBoatHandling = gHandlingDataMgr.GetBoatPointer(mi->m_nHandlingId);

    mi->ChooseVehicleColour(m_nPrimaryColor, m_nSecondaryColor, m_nTertiaryColor, m_nQuaternaryColor, 1);

    m_fMass = m_pHandlingData->m_fMass;
    m_fTurnMass = m_pHandlingData->m_fTurnMass * 0.5F;
    m_vecCentreOfMass = m_pHandlingData->m_vecCentreOfMass;
    m_fElasticity = 0.1F;
    m_fBuoyancyConstant = m_pHandlingData->m_fBuoyancyConstant;

    if (m_pHandlingData->m_fDragMult <= 0.01F)
        m_fAirResistance = m_pHandlingData->m_fDragMult;
    else
        m_fAirResistance = m_pHandlingData->m_fDragMult / 1000.0F * 0.5F;

    physicalFlags.bTouchingWater = true;
    physicalFlags.bSubmergedInWater = true;
    m_nBoatFlags.bAnchored = true;
    m_nBoatFlags.bMovingOnWater = true;
    m_nBoatFlags.bOnWater = true;

    m_fSteerAngle = 0.0f;
    m_fGasPedal = 0.0f;
    m_fBreakPedal = 0.0f;
    m_fRawSteerAngle = 0.0f;
    field_63C = 0;
    m_fLastWaterImmersionDepth = 7.0F;
    field_604 = 0;

    m_fAnchoredAngle = -10000.0f;
    m_fBurningTimer = 0.0f;
    m_pWhoDestroyedMe = nullptr;
    m_nNumWaterTrailPoints = 0;
    memset(m_afWakePointLifeTime, 0, sizeof(m_afWakePointLifeTime));

    m_nAmmoInClip = 20;
    m_boatFlap.m_nAxis = AXIS_Y;
    if (m_nModelIndex == MODEL_MARQUIS) {
        m_boatFlap.m_fOpenAngle = PI / 10.0F;
        m_boatFlap.m_fClosedAngle = -PI / 10.0F;
        m_boatFlap.m_nDirn = 4;
    } else {
        m_boatFlap.m_fOpenAngle = TWO_PI / 10.0F;
        m_boatFlap.m_fClosedAngle = -TWO_PI / 10.0F;
        m_boatFlap.m_nDirn = 3;
    }

    m_vehicleAudio.Initialise(this);
    for (auto& fx : m_apPropSplashFx)
        fx = nullptr;
}

// 0x6F00F0
CBoat::~CBoat() {
    if (m_pFireParticle) {
        m_pFireParticle->Kill();
        m_pFireParticle = nullptr;
    }

    for (auto& fx : m_apPropSplashFx) {
        if (fx) {
            fx->Kill();
            fx = nullptr;
        }
    }

    m_vehicleAudio.Terminate();
}

inline void CBoat::SetupModelNodes() {
    memset(m_aBoatNodes, 0, sizeof(m_aBoatNodes));
    CClumpModelInfo::FillFrameArray(m_pRwClump, m_aBoatNodes);
}

void CBoat::DebugCode() {
    if (FindPlayerVehicle() != AsVehicle())
        return;

    if (CPad::GetPad(m_nPadNumber)->NewState.Start)
        return;

    auto pad = CPad::GetPad(0);
    if (!pad->NewState.DPadLeft || pad->OldState.DPadLeft)
        return;

    auto uiHandlingId = CModelInfo::GetModelInfo(m_nModelIndex)->AsVehicleModelInfoPtr()->m_nHandlingId;
    m_pHandlingData = &gHandlingDataMgr.m_aVehicleHandling[uiHandlingId];
    SetupModelNodes();
}

// uses debug printing
// 0x6F0D90
void CBoat::PrintThrustAndRudderInfo() {
    char cBuffer[64];
    sprintf(cBuffer, "Thrust %3.2f", m_pHandlingData->m_transmissionData.m_fEngineAcceleration * m_pHandlingData->m_fMass);
    sprintf(cBuffer, "Rudder Angle  %3.2f", m_pHandlingData->m_fSteeringLock);
}

void CBoat::ModifyHandlingValue(const bool& bIncrement) {
    auto fChange = -1.0F;
    if (bIncrement)
        fChange = 1.0F;

    if (field_63C == 4)
        m_pHandlingData->m_fSteeringLock += fChange;
}

void CBoat::PruneWakeTrail() {
    int16 iInd;
    for (iInd = 0; iInd < 32; ++iInd) {
        auto fPointLifeTime = m_afWakePointLifeTime[iInd];
        if (fPointLifeTime <= 0.0F)
            break;

        if (fPointLifeTime <= CTimer::GetTimeStep()) {
            m_afWakePointLifeTime[iInd] = 0.0F;
            break;
        }

        m_afWakePointLifeTime[iInd] -= CTimer::GetTimeStep();
    }

    if (iInd >= 32)
        return;

    m_nNumWaterTrailPoints = iInd;
}

void CBoat::AddWakePoint(CVector posn) {
    auto ucIntensity = static_cast<uint8>(m_vecMoveSpeed.Magnitude() * 100.0F);

    // No wake points existing, early out
    if (m_afWakePointLifeTime[0] < 0.0F) {
        m_anWakePointIntensity[0] = ucIntensity;
        m_avecWakePoints[0].Set(posn.x, posn.y);
        m_afWakePointLifeTime[0] = WAKE_LIFETIME;

        m_nNumWaterTrailPoints = 1;
        return;
    }

    if (DistanceBetweenPoints2D(posn, m_avecWakePoints[0]) <= MIN_WAKE_INTERVAL)
        return;

    int16 uiMaxWakePoints = 31;
    if (m_nStatus != eEntityStatus::STATUS_PLAYER) {
        if (m_nCreatedBy == eVehicleCreatedBy::MISSION_VEHICLE)
            uiMaxWakePoints = 20;
        else
            uiMaxWakePoints = 15;
    }
    int16 uiCurWaterPoints = std::min(m_nNumWaterTrailPoints, uiMaxWakePoints);

    // Shift wake points
    if (m_nNumWaterTrailPoints >= uiMaxWakePoints || uiCurWaterPoints > 0) {
        for (uint32 iInd = uiCurWaterPoints; iInd > 0; --iInd) {
            m_avecWakePoints[iInd] = m_avecWakePoints[iInd - 1];
            m_anWakePointIntensity[iInd] = m_anWakePointIntensity[iInd - 1];
            m_afWakePointLifeTime[iInd] = m_afWakePointLifeTime[iInd - 1];
        }
    }

    m_anWakePointIntensity[0] = ucIntensity;
    m_avecWakePoints[0].Set(posn.x, posn.y);
    m_afWakePointLifeTime[0] = WAKE_LIFETIME;
    if (m_nNumWaterTrailPoints < 32)
        ++m_nNumWaterTrailPoints;
}

bool CBoat::IsSectorAffectedByWake(CVector2D vecPos, float fOffset, CBoat** ppBoats) {
    if (!apFrameWakeGeneratingBoats[0])
        return false;

    bool bWakeFound = false;
    for (auto& boat : apFrameWakeGeneratingBoats) {
        if (!boat)
            continue;

        if (!boat->m_nNumWaterTrailPoints)
            continue;

        for (int32 iTrail = 0; iTrail < boat->m_nNumWaterTrailPoints; ++iTrail) {
            auto fDist = (WAKE_LIFETIME - boat->m_afWakePointLifeTime[iTrail]) * fShapeTime + static_cast<float>(iTrail) * fShapeLength + fOffset;
            if (fabs(boat->m_avecWakePoints[iTrail].x - vecPos.x) >= fDist || fabs(boat->m_avecWakePoints[iTrail].y - vecPos.y) >= fDist)
                continue;

            ppBoats[bWakeFound] = boat;
            bWakeFound = true;
            break;
        }
    }

    return bWakeFound;
}

float CBoat::IsVertexAffectedByWake(CVector vecPos, CBoat* boat, int16 wIndex, bool bUnkn) {
    if (bUnkn) {
        waUnknArr[wIndex] = 0;
        waUnknArr2[wIndex] = 8;
    } else if (waUnknArr[wIndex] > 0)
        return 0.0F;

    if (!boat->m_nNumWaterTrailPoints)
        return 0.0F;

    for (int32 iTrail = 0; iTrail < boat->m_nNumWaterTrailPoints; ++iTrail) {
        auto fWakeDistSquared = powf((WAKE_LIFETIME - boat->m_afWakePointLifeTime[iTrail]) * fShapeTime + static_cast<float>(iTrail) * fShapeLength, 2);
        auto fTrailDistSquared = (boat->m_avecWakePoints[iTrail] - vecPos).SquaredMagnitude();
        if (fTrailDistSquared < fWakeDistSquared) {
            waUnknArr2[wIndex] = 0;
            float fContrib = sqrtf(fTrailDistSquared / fWakeDistSquared) * fRangeMult + (WAKE_LIFETIME - boat->m_afWakePointLifeTime[iTrail]) * 1.2F / WAKE_LIFETIME;
            fContrib = std::min(1.0F, fContrib);
            return 1.0F - fContrib;
        }

        auto fDistDiff = fTrailDistSquared - fWakeDistSquared;
        if (fDistDiff > 20.0F) {
            if (waUnknArr2[wIndex] > 3)
                waUnknArr2[wIndex] = 3;
        } else if (fDistDiff > 10.0F) {
            if (waUnknArr2[wIndex] > 2)
                waUnknArr2[wIndex] = 2;
        }
    }

    return 0.0F;
}

void CBoat::CheckForSkippingCalculations() {
    for (size_t ind = 0; ind < 4; ++ind) {
        auto iVal = waUnknArr2[ind];
        if (iVal <= 0 || iVal >= 8) {
            if (waUnknArr[ind] <= 0) {
                waUnknArr2[ind] = 8;
                continue;
            }
            waUnknArr[ind] = iVal - 1;
        } else if (iVal <= waUnknArr[ind] - 1)
            --waUnknArr[ind];

        waUnknArr2[ind] = 8;
    }
}

// 0x6F2710
void CBoat::FillBoatList() {
    for (auto& boat : apFrameWakeGeneratingBoats)
        boat = nullptr;

    auto vecCamPos = CVector2D(TheCamera.GetPosition());
    auto vecCamDir = CVector2D(TheCamera.m_mCameraMatrix.GetForward());
    vecCamDir.Normalise();

    auto iVehNum = GetVehiclePool()->GetSize();
    if (!iVehNum)
        return;

    int32 iCurBoat = 0;

    for (int32 iInd = 0; iInd < iVehNum; ++iInd) {
        auto vehicle = GetVehiclePool()->GetAt(iInd);
        if (!vehicle || !vehicle->IsBoat())
            continue;

        auto boat = vehicle->AsBoat();
        if (!boat->m_nNumWaterTrailPoints)
            continue;

        auto vecBoatPos = CVector2D(boat->GetPosition());
        auto vecBoatCamOffset = vecBoatPos - vecCamPos;
        auto fCamDot = DotProduct2D(vecBoatCamOffset, vecCamDir);
        if (fCamDot > 100.0F || fCamDot < -15.0F)
            continue;

        auto fDistFromCam = DistanceBetweenPoints2D(vecBoatPos, vecCamPos);
        if (fDistFromCam > 80.0F) // Originally squared dist, compared to 6400.0F
            continue;

        // Early out, the list isn't full yet
        if (iCurBoat < 4) {
            apFrameWakeGeneratingBoats[iCurBoat] = boat;
            ++iCurBoat;
            continue;
        }

        // Insert the new boat into list, based on dist from camera
        auto iNewInd = -1;
        auto fMinDist = 999999.99F;
        for (int32 iCheckedInd = 0; iCheckedInd < NUM_WAKE_GEN_BOATS; ++iCheckedInd) {
            auto pCheckedBoat = apFrameWakeGeneratingBoats[iCheckedInd];
            auto vecCheckedPos = CVector2D(pCheckedBoat->GetPosition());
            auto fCheckedDistFromCam = DistanceBetweenPoints2D(vecCheckedPos, vecCamPos); // Originally squared dist
            if (fCheckedDistFromCam < fMinDist) {
                fMinDist = fCheckedDistFromCam;
                iNewInd = iCheckedInd;
            }
        }

        if (iNewInd != -1 && (fDistFromCam < fMinDist || boat->m_nStatus == eEntityStatus::STATUS_PLAYER))
            apFrameWakeGeneratingBoats[iNewInd] = boat;
    }
}

void CBoat::SetModelIndex_Reversed(uint32 index) {
    CVehicle::SetModelIndex(index);
    memset(m_aBoatNodes, 0, sizeof(m_aBoatNodes));
    CClumpModelInfo::FillFrameArray(m_pRwClump, m_aBoatNodes);
}

void CBoat::ProcessControl_Reversed() {
    m_vehicleAudio.Service();
    PruneWakeTrail();
    CVehicle::ProcessDelayedExplosion();
    auto fMassCheck = (m_fMass * 0.008F * 100.0F) / 125.0F;
    if (physicalFlags.bDestroyed && fMassCheck < m_fBuoyancyConstant)
        m_fBuoyancyConstant -= ((m_fMass * 0.001F) * 0.008F);

    auto wanted = FindPlayerPed()->m_pPlayerData->m_pWanted;
    if (wanted->m_nWantedLevel > 0 && m_nModelIndex == MODEL_PREDATOR) {
        auto pPlayerVeh = FindPlayerVehicle();
        if (pPlayerVeh && pPlayerVeh->GetVehicleAppearance() == eVehicleAppearance::VEHICLE_APPEARANCE_BOAT) {
            auto iCarMission = m_autoPilot.m_nCarMission;
            if (iCarMission == eCarMission::MISSION_ATTACKPLAYER ||
                (iCarMission >= eCarMission::MISSION_RAMPLAYER_FARAWAY && iCarMission <= eCarMission::MISSION_BLOCKPLAYER_CLOSE)) {

                if (static_cast<uint32>(CTimer::GetTimeInMS()) > m_nAttackPlayerTime)
                    m_nAttackPlayerTime = rand() % 4096 + CTimer::GetTimeInMS() + 4500;
            }
        }
    }

    CVehicle::UpdateClumpAlpha();
    CVehicle::ProcessCarAlarm();

    switch (m_nStatus) {
    case eEntityStatus::STATUS_PLAYER:
        m_nBoatFlags.bAnchored = false;
        m_fAnchoredAngle = -10000.0f;
        if (m_pDriver)
            ProcessControlInputs(m_pDriver->m_nPedType);

        if (m_nModelIndex == MODEL_PREDATOR)
            CVehicle::DoFixedMachineGuns();

        CVehicle::DoDriveByShootings();
        break;
    case eEntityStatus::STATUS_SIMPLE:
        m_nBoatFlags.bAnchored = false;
        m_fAnchoredAngle = -10000.0f;
        CCarAI::UpdateCarAI(this);
        CPhysical::ProcessControl();
        physicalFlags.bSubmergedInWater = true;
        m_nBoatFlags.bOnWater = true;
        m_nBoatFlags.bMovingOnWater = true;
        return;
    case eEntityStatus::STATUS_PHYSICS:
        m_nBoatFlags.bAnchored = false;
        m_fAnchoredAngle = -10000.0f;
        CCarAI::UpdateCarAI(this);
        CCarCtrl::SteerAICarWithPhysics(this);
        break;
    case eEntityStatus::STATUS_ABANDONED:
    case eEntityStatus::STATUS_WRECKED:
        vehicleFlags.bIsHandbrakeOn = false; //?
        physicalFlags.bSubmergedInWater = true;
        m_nBoatFlags.bOnWater = true;
        m_nBoatFlags.bMovingOnWater = true;

        m_fSteerAngle = 0.0F;
        m_fBreakPedal = 0.5F;
        m_fGasPedal = 0.0F;

        auto fDist = (GetPosition() - FindPlayerCentreOfWorld_NoSniperShift(0)).Magnitude();
        if (fDist > 150.0F) {
            m_vecMoveSpeed.Set(0.0F, 0.0F, 0.0F);
            m_vecTurnSpeed.Set(0.0F, 0.0F, 0.0F);
            return;
        }
        break;
    }

    if (m_nStatus == eEntityStatus::STATUS_PLAYER || m_nStatus == eEntityStatus::STATUS_HELI || m_nStatus == eEntityStatus::STATUS_PHYSICS) {
        auto fSTDPropSpeed = 0.0F;
        auto fROCPropSpeed = CPlane::PLANE_ROC_PROP_SPEED;
        if (m_nModelIndex == MODEL_SKIMMER)
            fSTDPropSpeed = CPlane::PLANE_STD_PROP_SPEED;
        else if (m_fGasPedal != 0.0F)
            fROCPropSpeed *= 5.0F;

        if (m_fGasPedal == 0.0F)
            m_fPropSpeed += (fSTDPropSpeed - m_fPropSpeed) * CTimer::GetTimeStep() * fROCPropSpeed;
        else if (m_fGasPedal < 0.0F) {
            fSTDPropSpeed = (CPlane::PLANE_STD_PROP_SPEED - 0.05F) * m_fGasPedal + CPlane::PLANE_STD_PROP_SPEED;
            m_fPropSpeed += (fSTDPropSpeed - m_fPropSpeed) * CTimer::GetTimeStep() * fROCPropSpeed;
        } else {
            fSTDPropSpeed = (CPlane::PLANE_MAX_PROP_SPEED - CPlane::PLANE_STD_PROP_SPEED) * m_fGasPedal + CPlane::PLANE_STD_PROP_SPEED;
            m_fPropSpeed += (fSTDPropSpeed - m_fPropSpeed) * CTimer::GetTimeStep() * fROCPropSpeed;
        }
    } else if (m_fPropSpeed > 0.0F)
        m_fPropSpeed *= 0.95F;

    auto fDamagePower = m_fDamageIntensity * m_pHandlingData->m_fCollisionDamageMultiplier;
    if (fDamagePower > 25.0F && m_nStatus != eEntityStatus::STATUS_WRECKED && m_fHealth >= 250.0F) {
        auto fSavedHealth = m_fHealth;
        if (m_nStatus == eEntityStatus::STATUS_PLAYER && CStats::GetPercentageProgress() >= 100.0F)
            fDamagePower *= 0.5F;

        auto fGivenDamage = fDamagePower;
        if (this == FindPlayerVehicle()->AsBoat()) {
            fGivenDamage -= 25.0F;
            if (vehicleFlags.bTakeLessDamage)
                fGivenDamage /= 6.0F;
            else
                fGivenDamage *= 0.5F;
        } else {
            if (fGivenDamage > 60.0F && m_pDriver)
                m_pDriver->Say(68);

            fGivenDamage -= 25.0F;
            if (vehicleFlags.bTakeLessDamage)
                fGivenDamage /= 12.0F;
            else
                fGivenDamage *= 0.25F;
        }

        m_fHealth -= fGivenDamage;
        if (m_fHealth <= 0.0F && fSavedHealth > 0.0F) {
            m_fHealth = 1.0F;
            m_pWhoDestroyedMe = m_pDamageEntity;
        }
    }

    if (m_fHealth > 460.0F || m_nStatus == eEntityStatus::STATUS_WRECKED) {
        m_fBurningTimer = 0.0F;
        if (m_pFireParticle) {
            m_pFireParticle->Kill();
            m_pFireParticle = nullptr;
        }
    } else {
        auto vecDist = GetPosition() - TheCamera.GetPosition();
        if (fabs(vecDist.x) < 200.0F && fabs(vecDist.y) < 200.0F) {

            CVector vecFirePos(-1.5F, -0.5F, 1.2F);
            if (m_nModelIndex == MODEL_SPEEDER)
                vecFirePos.Set(0.4F, -2.4F, 0.8F);
            else if (m_nModelIndex == MODEL_REEFER)
                vecFirePos.Set(2.0F, -1.0F, 0.5F);

            if (m_fHealth < 250.0F) {
                if (!m_pFireParticle) {
                    if (auto modellingMat = GetModellingMatrix()) {
                        m_pFireParticle = g_fxMan.CreateFxSystem("fire_car", &vecFirePos, modellingMat, false);
                        if (m_pFireParticle) {
                            m_pFireParticle->Play();
                            CEventVehicleOnFire vehOnFireEvent(this);
                            GetEventGlobalGroup()->Add(&vehOnFireEvent, false);
                        }
                    }
                }

                m_fBurningTimer += (CTimer::GetTimeStep() * 20.0F);
                if (m_fBurningTimer > 5000.0F)
                    BlowUpCar(m_pWhoDestroyedMe, false);
            }
        }
    }

    auto bPostCollision = m_fDamageIntensity > 0.0F && m_vecLastCollisionImpactVelocity.z > 0.1F;
    CPhysical::ProcessControl();
    ProcessBoatControl(m_pBoatHandling, &m_fLastWaterImmersionDepth, m_bHasHitWall, bPostCollision);

    if (m_nModelIndex == MODEL_SKIMMER
        && (m_fPropSpeed > CPlane::PLANE_MIN_PROP_SPEED || m_vecMoveSpeed.SquaredMagnitude() > CPlane::PLANE_MIN_PROP_SPEED)) {
        FlyingControl(FLIGHT_MODEL_PLANE, -10000.0f, -10000.0f, -10000.0f, -10000.0f);
    }
    else if (CCheat::IsActive(CHEAT_BOATS_FLY)) {
        FlyingControl(FLIGHT_MODEL_BOAT, -10000.0f, -10000.0f, -10000.0f, -10000.0f);
    }

    if (m_nBoatFlags.bAnchored) {
        m_vecMoveSpeed.x = 0.0F;
        m_vecMoveSpeed.y = 0.0F;

        auto bNoAnchorAngle = m_fAnchoredAngle == -10000.0f;
        if (bNoAnchorAngle)
            m_fAnchoredAngle = GetHeading();
        else {
            CVector vecPos = GetMatrix().GetPosition();
            GetMatrix().RotateZ(m_fAnchoredAngle - GetHeading());
            GetMatrix().SetTranslateOnly(vecPos);
        }
    }
}

void CBoat::Teleport_Reversed(CVector destination, bool resetRotation) {
    CWorld::Remove(this);
    SetPosn(destination);
    if (resetRotation)
        SetOrientation(0.0F, 0.0F, 0.0F);

    m_vecMoveSpeed.Set(0.0F, 0.0F, 0.0F);
    m_vecTurnSpeed.Set(0.0F, 0.0F, 0.0F);
    CWorld::Add(this);
}

void CBoat::PreRender_Reversed() {
    CVehicle::PreRender();

    m_fContactSurfaceBrightness = 0.5F;
    auto fUsedAngle = -m_fSteerAngle;
    SetComponentRotation(m_aBoatNodes[BOAT_RUDDER],         AXIS_Z, fUsedAngle, true);
    SetComponentRotation(m_aBoatNodes[BOAT_REARFLAP_LEFT],  AXIS_Z, fUsedAngle, true);
    SetComponentRotation(m_aBoatNodes[BOAT_REARFLAP_RIGHT], AXIS_Z, fUsedAngle, true);

    auto fPropSpeed = std::min(1.0F, m_fPropSpeed * (32.0F / TWO_PI));
    auto ucTransparency = static_cast<RwUInt8>((1.0F - fPropSpeed) * 255.0F);

    m_fPropRotation += m_fPropSpeed * CTimer::GetTimeStep();
    while (m_fPropRotation > TWO_PI)
        m_fPropRotation -= TWO_PI;

    ProcessBoatNodeRendering(BOAT_STATIC_PROP,   m_fPropRotation * 2,  ucTransparency);
    ProcessBoatNodeRendering(BOAT_STATIC_PROP_2, m_fPropRotation * -2, ucTransparency);

    ucTransparency = (ucTransparency >= 150 ? 0 : 150 - ucTransparency);
    ProcessBoatNodeRendering(BOAT_MOVING_PROP,  -m_fPropRotation, ucTransparency);
    ProcessBoatNodeRendering(BOAT_MOVING_PROP_2, m_fPropRotation, ucTransparency);

    if (m_nModelIndex == MODEL_MARQUIS) {
        auto pFlap = m_aBoatNodes[BOAT_FLAP_LEFT];
        if (pFlap) {
            auto tempMat = CMatrix();
            tempMat.Attach(RwFrameGetMatrix(pFlap), false);
            CVector posCopy = tempMat.GetPosition();
            auto vecTransformed = Multiply3x3(GetMatrix(), posCopy);

            m_boatFlap.Process(this, m_vecBoatMoveForce, m_vecBoatTurnForce, vecTransformed);
            CVector vecAxis;
            if (m_boatFlap.m_nAxis == AXIS_X)
                vecAxis.Set(m_boatFlap.m_fAngle, 0.0F, 0.0F);
            else if (m_boatFlap.m_nAxis == AXIS_Y)
                vecAxis.Set(0.0F, m_boatFlap.m_fAngle, 0.0F);
            else if (m_boatFlap.m_nAxis == AXIS_Z)
                vecAxis.Set(0.0F, 0.0F, m_boatFlap.m_fAngle);

            tempMat.SetRotate(vecAxis.x, vecAxis.y, vecAxis.z);
            tempMat.GetPosition() += posCopy;
            tempMat.UpdateRW();
        }
    }

    if (m_nModelIndex == MODEL_PREDATOR
        || m_nModelIndex == MODEL_REEFER
        || m_nModelIndex == MODEL_TROPIC) {

        auto moving = m_aBoatNodes[BOAT_MOVING];
        if (moving) {
            SetComponentRotation(moving, AXIS_Z, m_fMovingHiRotation, true);
            if (CCheat::IsActive(CHEAT_INVISIBLE_CAR))
            {
                auto firstObj = reinterpret_cast<RpAtomic*>(GetFirstObject(moving));
                RpAtomicRenderMacro(firstObj);
            }
        }
        m_fMovingHiRotation += CTimer::GetTimeStepInSeconds();
    }

    m_vecBoatMoveForce = m_vecMoveSpeed + m_vecFrictionMoveSpeed;
    m_vecBoatTurnForce = m_vecTurnSpeed + m_vecFrictionTurnSpeed;
    auto fSpeed = m_vecMoveSpeed.Magnitude();

    int32 iCounter = 0;
    constexpr eBoatNodes aCheckedNodes[2] = { BOAT_STATIC_PROP, BOAT_STATIC_PROP_2 };
    for (const auto eNode : aCheckedNodes) {
        auto prop = m_aBoatNodes[eNode];
        RwMatrix* splashMat = CEntity::GetModellingMatrix();

        auto splashFx = m_apPropSplashFx[iCounter];
        if (!splashFx && splashMat && prop) {
            RwV3dAssign(RwMatrixGetPos(splashMat), RwMatrixGetPos(RwFrameGetLTM(prop)));
            RwMatrixUpdate(splashMat);
            auto vecPoint = CVector(0.0F, 0.0F, 0.0F);
            splashFx = g_fxMan.CreateFxSystem("boat_prop", &vecPoint, splashMat, false);
            if (splashFx) {
                m_apPropSplashFx[iCounter] = splashFx;
                splashFx->Play();
                splashFx->SetLocalParticles(true);
                splashFx->CopyParentMatrix();
            }
        }

        if (splashFx && splashMat) {
            if (physicalFlags.bSubmergedInWater) {
                if (splashFx->GetPlayStatus() == eFxSystemPlayStatus::FX_STOPPED)
                    splashFx->Play();

                RwV3dAssign(RwMatrixGetPos(splashMat), RwMatrixGetPos(RwFrameGetLTM(prop)));
                RwMatrixUpdate(splashMat);
                splashFx->SetMatrix(splashMat);
                auto fTime = std::min(1.0F, fSpeed * 2.0F);
                splashFx->SetConstTime(true, fTime);
            } else if (splashFx->GetPlayStatus() == eFxSystemPlayStatus::FX_PLAYING) {
                splashFx->Stop();
            }
        }

        ++iCounter;
    }

    auto fWaterDamping = m_vecWaterDamping.Magnitude();
    CVehicle::DoBoatSplashes(fWaterDamping);
}

inline void CBoat::ProcessBoatNodeRendering(eBoatNodes eNode, float fRotation, RwUInt8 ucAlpha) {
    auto frame = m_aBoatNodes[eNode];
    if (!frame)
        return;

    SetComponentRotation(frame, AXIS_Y, fRotation, true);
    RpAtomic* atomic;
    RwFrameForAllObjects(frame, GetCurrentAtomicObjectCB, &atomic);
    if (atomic)
        CVehicle::SetComponentAtomicAlpha(atomic, ucAlpha);
}

void CBoat::Render_Reversed() {
    m_nTimeTillWeNeedThisCar = CTimer::GetTimeInMS() + 3000;
    if (CCheat::IsActive(CHEAT_INVISIBLE_CAR))
        return;

    CVehicle::Render();

    if (m_nModelIndex == MODEL_SKIMMER)
        return;

    // Code below is used to draw the "no water" zones, so the inside of the boat that is under water surface, doesn't have water visible in it

    RwRGBA rwColor = CRGBA(255, 255, 255, 255).ToRwRGBA();
    RxObjSpace3DVertexSetPreLitColor(&aRenderVertices[0], &rwColor);
    RxObjSpace3DVertexSetPreLitColor(&aRenderVertices[1], &rwColor);
    RxObjSpace3DVertexSetPreLitColor(&aRenderVertices[2], &rwColor);
    RxObjSpace3DVertexSetPreLitColor(&aRenderVertices[3], &rwColor);

    auRenderIndices[0] = 0;
    auRenderIndices[1] = 2;
    auRenderIndices[2] = 1;
    auRenderIndices[3] = 1;
    auRenderIndices[4] = 2;
    auRenderIndices[5] = 3;

    RxObjSpace3DVertexSetU(&aRenderVertices[0], 0.0F);
    RxObjSpace3DVertexSetV(&aRenderVertices[0], 0.0F);
    RxObjSpace3DVertexSetU(&aRenderVertices[1], 1.0F);
    RxObjSpace3DVertexSetV(&aRenderVertices[1], 0.0F);
    RxObjSpace3DVertexSetU(&aRenderVertices[2], 0.0F);
    RxObjSpace3DVertexSetV(&aRenderVertices[2], 1.0F);
    RxObjSpace3DVertexSetU(&aRenderVertices[3], 1.0F);
    RxObjSpace3DVertexSetV(&aRenderVertices[3], 1.0F);

    switch (m_nModelIndex) {
    case MODEL_PREDATOR:
        RwIm3DVertexSetPos(&aRenderVertices[0], -1.45F,  1.90F, 0.96F);
        RwIm3DVertexSetPos(&aRenderVertices[1],  1.45F,  1.90F, 0.96F);
        RwIm3DVertexSetPos(&aRenderVertices[2], -1.45F, -3.75F, 0.96F);
        RwIm3DVertexSetPos(&aRenderVertices[3],  1.45F, -3.75F, 0.96F);
        break;
    case MODEL_SQUALO:
        RwIm3DVertexSetPos(&aRenderVertices[0], -1.222F,  2.004F, 1.409F);
        RwIm3DVertexSetPos(&aRenderVertices[1],  1.222F,  2.004F, 1.409F);
        RwIm3DVertexSetPos(&aRenderVertices[2], -1.240F, -1.367F, 0.846F);
        RwIm3DVertexSetPos(&aRenderVertices[3],  1.240F, -1.367F, 0.846F);
        break;
    case MODEL_SPEEDER:
        RwIm3DVertexSetPos(&aRenderVertices[0], -1.15F,  3.61F, 1.03F);
        RwIm3DVertexSetPos(&aRenderVertices[1],  1.15F,  3.61F, 1.03F);
        RwIm3DVertexSetPos(&aRenderVertices[2], -1.15F, -0.06F, 1.03F);
        RwIm3DVertexSetPos(&aRenderVertices[3],  1.15F, -0.06F, 1.03F);
        break;
    case MODEL_REEFER:
        RwIm3DVertexSetPos(&aRenderVertices[0], -1.90F,  2.83F, 1.00F);
        RwIm3DVertexSetPos(&aRenderVertices[1],  1.90F,  2.83F, 1.00F);
        RwIm3DVertexSetPos(&aRenderVertices[2], -1.66F, -4.48F, 0.83F);
        RwIm3DVertexSetPos(&aRenderVertices[3],  1.66F, -4.48F, 0.83F);
        break;
    case MODEL_TROPIC:
        RwIm3DVertexSetPos(&aRenderVertices[0], -1.886F, -2.347F, 0.787F);
        RwIm3DVertexSetPos(&aRenderVertices[1], 1.886F, -2.347F, 0.787F);
        RwIm3DVertexSetPos(&aRenderVertices[2], -1.886F, -4.670F, 0.842F);
        RwIm3DVertexSetPos(&aRenderVertices[3], 1.886F, -4.670F, 0.842F);
        break;
    case MODEL_COASTG:
        RwIm3DVertexSetPos(&aRenderVertices[0], -0.663F, 3.565F, 0.382F);
        RwIm3DVertexSetPos(&aRenderVertices[1],  0.663F, 3.565F, 0.382F);
        RwIm3DVertexSetPos(&aRenderVertices[2], -1.087F, 0.831F, 0.381F);
        RwIm3DVertexSetPos(&aRenderVertices[3],  1.087F, 0.831F, 0.381F);
        break;
    case MODEL_DINGHY:
        RwIm3DVertexSetPos(&aRenderVertices[0], -0.797F,  1.641F, 0.573F);
        RwIm3DVertexSetPos(&aRenderVertices[1],  0.797F,  1.641F, 0.573F);
        RwIm3DVertexSetPos(&aRenderVertices[2], -0.865F, -1.444F, 0.509F);
        RwIm3DVertexSetPos(&aRenderVertices[3],  0.865F, -1.444F, 0.509F);
        break;
    case MODEL_MARQUIS:
        RwIm3DVertexSetPos(&aRenderVertices[0], -1.246F, -1.373F, 0.787F);
        RwIm3DVertexSetPos(&aRenderVertices[1],  1.246F, -1.373F, 0.787F);
        RwIm3DVertexSetPos(&aRenderVertices[2], -1.023F, -5.322F, 0.787F);
        RwIm3DVertexSetPos(&aRenderVertices[3],  1.023F, -5.322F, 0.787F);
        break;
    case MODEL_LAUNCH:
        RwIm3DVertexSetPos(&aRenderVertices[0], -1.0F,  2.5F, 0.3F);
        RwIm3DVertexSetPos(&aRenderVertices[1],  1.0F,  2.5F, 0.3F);
        RwIm3DVertexSetPos(&aRenderVertices[2], -1.0F, -5.4F, 0.3F);
        RwIm3DVertexSetPos(&aRenderVertices[3],  1.0F, -5.4F, 0.3F);
        break;
    default:
        return;
    }

    RwRenderStateSet(rwRENDERSTATETEXTURERASTER,    RWRSTATE(CWaterLevel::waterclear256Raster));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE,RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEFOGENABLE,        RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,         RWRSTATE(rwBLENDZERO));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,        RWRSTATE(rwBLENDONE));

    RwMatrix tempMat;
    GetMatrix().UpdateRwMatrix(&tempMat);
    if (RwIm3DTransform(aRenderVertices, uiNumVertices, &tempMat, rwMATRIXTYPENORMAL)) {
        RwIm3DRenderIndexedPrimitive(RwPrimitiveType::rwPRIMTYPETRILIST, auRenderIndices, uiNumIndices);
        RwIm3DEnd();
    }

    // Second tri list for Coastguard
    if (m_nModelIndex == MODEL_COASTG) {
        RwIm3DVertexSetPos(&aRenderVertices[0], -1.087F,  0.831F, 0.381F);
        RwIm3DVertexSetPos(&aRenderVertices[1],  1.087F,  0.831F, 0.381F);
        RwIm3DVertexSetPos(&aRenderVertices[2], -1.097F, -2.977F, 0.381F);
        RwIm3DVertexSetPos(&aRenderVertices[3],  1.097F, -2.977F, 0.381F);

        GetMatrix().UpdateRwMatrix(&tempMat);
        if (RwIm3DTransform(aRenderVertices, uiNumVertices, &tempMat, rwMATRIXTYPENORMAL)) {
            RwIm3DRenderIndexedPrimitive(RwPrimitiveType::rwPRIMTYPETRILIST, auRenderIndices, uiNumIndices);
            RwIm3DEnd();
        }
    }

    RwRenderStateSet(rwRENDERSTATEFOGENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,  RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND, RWRSTATE(rwBLENDINVSRCALPHA));
}

void CBoat::ProcessControlInputs_Reversed(uint8 ucPadNum) {
    m_nPadNumber = ucPadNum;
    if (ucPadNum > 3)
        m_nPadNumber = 3;

    auto pad = CPad::GetPad(ucPadNum);
    float fBrakePower = (static_cast<float>(pad->GetBrake()) * (1.0F / 255.0F) - m_fBreakPedal) * 0.1F + m_fBreakPedal;
    fBrakePower = clamp(fBrakePower, 0.0F, 1.0F);
    m_fBreakPedal = fBrakePower;

    auto fGasPower = fBrakePower * -0.3F;
    if (fBrakePower < 0.05F) {
        m_fBreakPedal = 0.0F;
        fGasPower = static_cast<float>(pad->GetAccelerate()) * (1.0F / 255.0F);
    }
    m_fGasPedal = fGasPower;

    // Mouse steering
    if (CCamera::m_bUseMouse3rdPerson && CVehicle::m_bEnableMouseSteering) {
        auto bChangedInput = CVehicle::m_nLastControlInput != eControllerType::CONTROLLER_MOUSE || pad->GetSteeringLeftRight();
        if (CPad::NewMouseControllerState.X == 0.0F && bChangedInput) { // No longer using mouse controls
            m_fRawSteerAngle += (static_cast<float>(-pad->GetSteeringLeftRight()) * (1.0F / 128.0F) - m_fRawSteerAngle) * 0.2F * CTimer::GetTimeStep();
            CVehicle::m_nLastControlInput = eControllerType::CONTROLLER_KEYBOARD1;
        } else if (m_fRawSteerAngle != 0.0F || m_fRawSteerAngle != 0.0F) {
            CVehicle::m_nLastControlInput = eControllerType::CONTROLLER_MOUSE;
            if (!pad->NewState.m_bVehicleMouseLook)
                m_fRawSteerAngle += CPad::NewMouseControllerState.X * -0.0035F;

            if (fabs(m_fRawSteerAngle) < 0.5 || pad->NewState.m_bVehicleMouseLook)
                m_fRawSteerAngle *= pow(0.985F, CTimer::GetTimeStep());
        }
    } else {
        m_fRawSteerAngle += (static_cast<float>(-pad->GetSteeringLeftRight()) * (1.0F / 128.0F) - m_fRawSteerAngle) * 0.2F * CTimer::GetTimeStep();
        CVehicle::m_nLastControlInput = eControllerType::CONTROLLER_KEYBOARD1;
    }

    m_fRawSteerAngle = clamp(m_fRawSteerAngle, -1.0F, 1.0F);
    auto fSignedPow = m_fRawSteerAngle * fabs(m_fRawSteerAngle);
    m_fSteerAngle = DegreesToRadians(m_pHandlingData->m_fSteeringLock * fSignedPow);
}

void CBoat::GetComponentWorldPosition_Reversed(int32 componentId, CVector& outPos) {
    outPos = RwFrameGetLTM(m_aBoatNodes[componentId])->pos;
}

void CBoat::ProcessOpenDoor_Reversed(CPed* ped, uint32 doorComponentId, uint32 animGroup, uint32 animId, float fTime) {
    // NOP
}

void CBoat::BlowUpCar_Reversed(CEntity* damager, uint8 bHideExplosion) {
    if (!vehicleFlags.bCanBeDamaged)
        return;

    physicalFlags.bDestroyed = true;
    m_nStatus = eEntityStatus::STATUS_WRECKED;
    CVisibilityPlugins::SetClumpForAllAtomicsFlag(m_pRwClump, eAtomicComponentFlag::ATOMIC_IS_BLOWN_UP);
    m_vecMoveSpeed.z += 0.13F;
    m_fHealth = 0.0F;
    m_wBombTimer = 0;

    const auto& vecPos = GetPosition();
    TheCamera.CamShake(0.4F, vecPos.x, vecPos.y, vecPos.z);
    CVehicle::KillPedsInVehicle();
    m_nOverrideLights = eVehicleOverrideLightsState::NO_CAR_LIGHT_OVERRIDE;
    vehicleFlags.bEngineOn = false;
    vehicleFlags.bLightsOn = false;
    CVehicle::ChangeLawEnforcerState(false);
    CExplosion::AddExplosion(this, damager, eExplosionType::EXPLOSION_BOAT, vecPos, 0, 1, -1.0F, bHideExplosion);
    CDarkel::RegisterCarBlownUpByPlayer(this, 0);

    auto movingComponent = m_aBoatNodes[BOAT_MOVING];
    if (!movingComponent)
        return;

    RpAtomic* movingCompAtomic;
    RwFrameForAllObjects(movingComponent, GetBoatAtomicObjectCB, &movingCompAtomic);
    if (!movingCompAtomic)
        return;

    auto obj = new CObject();
    obj->SetModelIndexNoCreate(379); // todo: MODEL_TEMPCOL_WHEEL1 ?
    obj->RefModelInfo(m_nModelIndex);

    auto movingCompMatrix = RwFrameGetLTM(movingComponent);
    auto newRwFrame = RwFrameCreate();
    auto movingCompAtomicClone = RpAtomicClone(movingCompAtomic);
    memcpy(RwFrameGetMatrix(newRwFrame), movingCompMatrix, sizeof(RwMatrix));
    RpAtomicSetFrame(movingCompAtomicClone, newRwFrame);
    CVisibilityPlugins::SetAtomicRenderCallback(movingCompAtomicClone, nullptr);
    obj->AttachToRwObject((RwObject*)movingCompAtomicClone, true);

    ++CObject::nNoTempObjects;
    obj->m_bDontStream = true;
    obj->m_fMass = 10.0F;
    obj->m_fTurnMass = 25.0F;
    obj->m_fAirResistance = 0.99F;
    obj->m_fElasticity = 0.1F;
    obj->m_fBuoyancyConstant = 8.0F / 75.0F;
    obj->m_nObjectType = eObjectType::OBJECT_TEMPORARY;
    obj->SetIsStatic(false);
    obj->objectFlags.bIsPickup = false;
    obj->m_nRemovalTime = CTimer::GetTimeInMS() + 20000;

    obj->m_vecMoveSpeed = m_vecMoveSpeed;
    if (GetUp().z <= 0.0F)
        obj->m_vecMoveSpeed.z = 0.0F;
    else
        obj->m_vecMoveSpeed.z = 0.3F;

    obj->m_vecTurnSpeed.Set(0.5F, m_vecTurnSpeed.y * 2.0F, m_vecTurnSpeed.z * 2.0F);

    auto vecOffset = obj->GetPosition() - vecPos;
    vecOffset.Normalise();
    if (GetUp().z > 0.0F)
        vecOffset += GetUp();

    auto vecObjPos = obj->GetPosition() + vecOffset;
    obj->SetPosn(vecObjPos);

    CWorld::Add(obj);
    RwFrameForAllObjects(movingComponent, GetBoatAtomicObjectCB, &movingCompAtomic);
    if (movingCompAtomic)
        RpAtomicSetFlags(movingCompAtomic, 0x0);
}

RwObject* GetBoatAtomicObjectCB(RwObject* object, void* data) {
    if (RpAtomicGetFlags(object) & rpATOMICRENDER)
        *static_cast<RpAtomic**>(data) = reinterpret_cast<RpAtomic*>(object);

    return object;
}

void CBoat::SetModelIndex(uint32 index) { return CBoat::SetModelIndex_Reversed(index); }
void CBoat::ProcessControl() { return CBoat::ProcessControl_Reversed(); }
void CBoat::Teleport(CVector destination, bool resetRotation) { return CBoat::Teleport_Reversed(destination, resetRotation); }
void CBoat::PreRender() { return CBoat::PreRender_Reversed(); }
void CBoat::Render() { return CBoat::Render_Reversed(); }
void CBoat::ProcessControlInputs(uint8 playerNum) { return CBoat::ProcessControlInputs_Reversed(playerNum); }
void CBoat::GetComponentWorldPosition(int32 componentId, CVector& outPos) { return CBoat::GetComponentWorldPosition_Reversed(componentId, outPos); }
void CBoat::ProcessOpenDoor(CPed* ped, uint32 doorComponentId, uint32 animGroup, uint32 animId, float fTime) { return CBoat::ProcessOpenDoor_Reversed(ped, doorComponentId, animGroup, animId, fTime); }
void CBoat::BlowUpCar(CEntity* damager, uint8 bHideExplosion) { return CBoat::BlowUpCar_Reversed(damager, bHideExplosion); }
