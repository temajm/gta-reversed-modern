#include "StdInc.h"

#include "Camera.h"

float& CCamera::m_f3rdPersonCHairMultY = *reinterpret_cast<float*>(0xB6EC10);
float& CCamera::m_f3rdPersonCHairMultX = *reinterpret_cast<float*>(0xB6EC14);
float& CCamera::m_fMouseAccelVertical = *reinterpret_cast<float*>(0xB6EC18);
float& CCamera::m_fMouseAccelHorzntl = *reinterpret_cast<float*>(0xB6EC1C);
bool& CCamera::m_bUseMouse3rdPerson = *reinterpret_cast<bool*>(0xB6EC2E);
bool& CCamera::bDidWeProcessAnyCinemaCam = *reinterpret_cast<bool*>(0xB6EC2D);

CCamera& TheCamera = *reinterpret_cast<CCamera*>(0xB6F028);
bool& gbModelViewer = *reinterpret_cast<bool*>(0xBA6728);
char& gbCineyCamMessageDisplayed = *(char*)0x8CC381; // 2

CCam& CCamera::GetActiveCamera() {
    return TheCamera.m_aCams[TheCamera.m_nActiveCam];
}

void CCamera::InjectHooks() {
    RH_ScopedClass(CCamera);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(GetArrPosForVehicleType, 0x50AF00);
    RH_ScopedInstall(GetPositionAlongSpline, 0x50AF80);
    RH_ScopedInstall(InitialiseCameraForDebugMode, 0x50AF90);
    RH_ScopedInstall(ProcessObbeCinemaCameraPed, 0x50B880);
    RH_ScopedInstall(ProcessWideScreenOn, 0x50B890);
    RH_ScopedInstall(RenderMotionBlur, 0x50B8F0);
    RH_ScopedInstall(SetCameraDirectlyBehindForFollowPed_CamOnAString, 0x50BD40);
    RH_ScopedInstall(SetCameraDirectlyInFrontForFollowPed_CamOnAString, 0x50BD70);
    RH_ScopedInstall(SetCamPositionForFixedMode, 0x50BEC0);
    RH_ScopedInstall(SetFadeColour, 0x50BF00);
    RH_ScopedInstall(SetMotionBlur, 0x50BF40);
    RH_ScopedInstall(SetMotionBlurAlpha, 0x50BF80);
    RH_ScopedInstall(SetNearClipScript, 0x50BF90);
    RH_ScopedInstall(SetNewPlayerWeaponMode, 0x50BFB0);
    RH_ScopedInstall(SetRwCamera, 0x50C100);
    RH_ScopedInstall(SetWideScreenOn, 0x50C140);
    RH_ScopedInstall(SetWideScreenOff, 0x50C150);
    RH_ScopedInstall(StartCooperativeCamMode, 0x50C260);
    RH_ScopedInstall(StopCooperativeCamMode, 0x50C270);
    RH_ScopedInstall(AllowShootingWith2PlayersInCar, 0x50C280);
    RH_ScopedInstall(StoreValuesDuringInterPol, 0x50C290);
    RH_ScopedInstall(ProcessScriptedCommands, 0x516AE0);
    RH_ScopedInstall(FinishCutscene, 0x514950);
    RH_ScopedInstall(LerpFOV, 0x50D280);
    RH_ScopedInstall(UpdateAimingCoors, 0x50CB10);
    RH_ScopedInstall(SetColVarsAimWeapon, 0x50CBF0);
    RH_ScopedInstall(ClearPlayerWeaponMode, 0x50AB10);
    RH_ScopedInstall(DontProcessObbeCinemaCamera, 0x50AB40);
    RH_ScopedInstall(Enable1rstPersonCamCntrlsScript, 0x50AC00);
    RH_ScopedInstall(FindCamFOV, 0x50AD20);
    RH_ScopedInstall(GetFading, 0x50ADE0);
    RH_ScopedInstall(GetFadingDirection, 0x50ADF0);
    RH_ScopedInstall(Get_Just_Switched_Status, 0x50AE10);
    RH_ScopedInstall(GetGameCamPosition, 0x50AE50);

//    RH_ScopedInstall(Constructor, 0x51A450);
//    RH_ScopedInstall(Destructor, 0x50A870);
//    RH_ScopedInstall(InitCameraVehicleTweaks, 0x50A3B0);
//    RH_ScopedInstall(ApplyVehicleCameraTweaks, 0x50A480);
//    RH_ScopedInstall(CamShake, 0x50A9F0);
//    RH_ScopedInstall(GetScreenRect, 0x50AB50);
//    RH_ScopedInstall(Enable1rstPersonWeaponsCamera, 0x50AC10);
//    RH_ScopedInstall(Fade, 0x50AC20);
//    RH_ScopedInstall(Find3rdPersonQuickAimPitch, 0x50AD40);
//    RH_ScopedInstall(GetCutSceneFinishTime, 0x50AD90);
//    RH_ScopedInstall(GetScreenFadeStatus, 0x50AE20);
//    RH_ScopedInstall(GetLookingLRBFirstPerson, 0x50AE60);
//    RH_ScopedInstall(GetLookDirection, 0x50AE90);
//    RH_ScopedInstall(GetLookingForwardFirstPerson, 0x50AED0);
//    RH_ScopedInstall(CopyCameraMatrixToRWCam, 0x50AFA0);
    RH_ScopedInstall(CalculateMirroredMatrix, 0x50B380);
    RH_ScopedInstall(DealWithMirrorBeforeConstructRenderList, 0x50B510);
//    RH_ScopedInstall(ProcessFade, 0x50B5D0);
//    RH_ScopedInstall(ProcessMusicFade, 0x50B6D0);
//    RH_ScopedInstall(Restore, 0x50B930);
//    RH_ScopedInstall(RestoreWithJumpCut, 0x50BAB0);
    RH_ScopedInstall(SetCamCutSceneOffSet, 0x50BD20);
//    RH_ScopedInstall(SetCameraDirectlyBehindForFollowPed_ForAPed_CamOnAString, 0x50BDA0);
//    RH_ScopedInstall(SetCameraDirectlyInFrontForFollowPed_ForAPed_CamOnAString, 0x50BE30);
//    RH_ScopedInstall(Using1stPersonWeaponMode, 0x50BFF0);
    RH_ScopedInstall(SetParametersForScriptInterpolation, 0x50C030);
//    RH_ScopedInstall(SetPercentAlongCutScene, 0x50C070);
//    RH_ScopedInstall(SetZoomValueFollowPedScript, 0x50C160);
//    RH_ScopedInstall(SetZoomValueCamStringScript, 0x50C1B0);
//    RH_ScopedInstall(UpdateTargetEntity, 0x50C360);
//    RH_ScopedInstall(TakeControl, 0x50C7C0);
//    RH_ScopedInstall(TakeControlNoEntity, 0x50C8B0);
//    RH_ScopedInstall(TakeControlAttachToEntity, 0x50C910);
    RH_ScopedInstall(TakeControlWithSpline, 0x50CAE0);
//    RH_ScopedInstall(SetCamCollisionVarDataSet, 0x50CB60);
//    RH_ScopedInstall(SetNearClipBasedOnPedCollision, 0x50CB90);
//    RH_ScopedInstall(SetColVarsPed, 0x50CC50);
//    RH_ScopedInstall(SetColVarsVehicle, 0x50CCA0);
//    RH_ScopedInstall(CameraGenericModeSpecialCases, 0x50CD30);
//    RH_ScopedInstall(CameraPedModeSpecialCases, 0x50CD80);
//    RH_ScopedInstall(CameraPedAimModeSpecialCases, 0x50CDA0);
//    RH_ScopedInstall(CameraVehicleModeSpecialCases, 0x50CDE0);
//    RH_ScopedInstall(IsExtraEntityToIgnore, 0x50CE80);
//    RH_ScopedInstall(ConsiderPedAsDucking, 0x50CEB0);
//    RH_ScopedInstall(ResetDuckingSystem, 0x50CEF0);
//    RH_ScopedInstall(HandleCameraMotionForDucking, 0x50CFA0);
//    RH_ScopedInstall(HandleCameraMotionForDuckingDuringAim, 0x50D090);
//    RH_ScopedInstall(VectorMoveLinear, 0x50D160);
//    RH_ScopedInstall(VectorTrackLinear, 0x50D1D0);
//    RH_ScopedInstall(AddShakeSimple, 0x50D240);
//    RH_ScopedInstall(InitialiseScriptableComponents, 0x50D2D0);
//    RH_ScopedInstall(DrawBordersForWideScreen, 0x514860);
//    RH_ScopedInstall(Find3rdPersonCamTargetVector, 0x514970);
//    RH_ScopedInstall(CalculateGroundHeight, 0x514B80);
//    RH_ScopedInstall(CalculateFrustumPlanes, 0x514D60);
//    RH_ScopedInstall(CalculateDerivedValues, 0x5150E0);
//    RH_ScopedInstall(ImproveNearClip, 0x516B20);
//    RH_ScopedInstall(SetCameraUpForMirror, 0x51A560);
//    RH_ScopedInstall(RestoreCameraAfterMirror, 0x51A5A0);
//    RH_ScopedInstall(ConeCastCollisionResolve, 0x51A5D0);
//    RH_ScopedInstall(TryToStartNewCamMode, 0x51E560);
//    RH_ScopedInstall(CameraColDetAndReact, 0x520190);
//    RH_ScopedInstall(CamControl, 0x527FA0);
//    RH_ScopedInstall(Process, 0x52B730);
//    RH_ScopedInstall(DeleteCutSceneCamDataMemory, 0x5B24A0);
//    RH_ScopedInstall(LoadPathSplines, 0x5B24D0);
//    RH_ScopedInstall(Init, 0x5BC520);

//    RH_ScopedInstall(ProcessVectorTrackLinear, 0x50D350);
//    RH_ScopedInstall(ProcessVectorTrackLinear, 0x516440);
//    RH_ScopedInstall(ProcessVectorMoveLinear, 0x50D430);
//    RH_ScopedInstall(ProcessVectorMoveLinear, 0x5164A0);
//    RH_ScopedInstall(ProcessFOVLerp, 0x50D510);
//    RH_ScopedInstall(ProcessFOVLerp, 0x516500);
//    RH_ScopedInstall(ProcessJiggle, 0x516560);

//    RH_ScopedInstall(CamShakeNoPos, 0x50A970);
}

// 0x5BC520
void CCamera::Init() {
    plugin::CallMethod<0x5BC520, CCamera*>(this);
}

// 0x51A450
CCamera::CCamera() {
    plugin::CallMethod<0x51A450, CCamera*>(this);
}

CCamera* CCamera::Constructor() {
    this->CCamera::CCamera();
    return this;
}

// 0x50A870
CCamera::~CCamera() {
    plugin::CallMethod<0x50A870, CCamera*>(this);
}

CCamera* CCamera::Destructor() {
    this->CCamera::~CCamera();
    return this;
}

// 0x50A3B0
void CCamera::InitCameraVehicleTweaks() {
    plugin::CallMethod<0x50A3B0, CCamera*>(this);
}

// 0x50A480
void CCamera::ApplyVehicleCameraTweaks(CVehicle* vehicle) {
    plugin::CallMethod<0x50A480, CCamera*, CVehicle*>(this, vehicle);
}

// 0x50A9F0
void CCamera::CamShake(float arg2, float x, float y, float z) {
    plugin::CallMethod<0x50A9F0, CCamera*, float, float, float, float>(this, arg2, x, y, z);
}

// 0x50AB10
void CCamera::ClearPlayerWeaponMode() {
    m_PlayerWeaponMode.m_nMode = 0;
    m_PlayerWeaponMode.m_nMaxZoom = 1;
    m_PlayerWeaponMode.m_nMinZoom = -1;
    m_PlayerWeaponMode.m_fDuration = 0.0f;
}

// unused
// 0x50AB40
void CCamera::DontProcessObbeCinemaCamera() {
    bDidWeProcessAnyCinemaCam = false;
}

// unused
// 0x50AC00
void CCamera::Enable1rstPersonCamCntrlsScript() {
    m_bEnable1rstPersonCamCntrlsScript = true;
}

// 0x50AC20
void CCamera::Fade(float fadeDuration, eFadeFlag fadeInOutFlag) {
    plugin::CallMethod<0x50AC20, CCamera*, float, eFadeFlag>(this, fadeDuration, fadeInOutFlag);
}

// 0x50AD20
float CCamera::FindCamFOV() {
    return m_aCams[m_nActiveCam].m_fFOV;
}

// 0x50AD40
float CCamera::Find3rdPersonQuickAimPitch() {
    return plugin::CallMethodAndReturn<float, 0x50AD40, CCamera*>(this);
}

// 0x50AD90
uint32 CCamera::GetCutSceneFinishTime() {
    return plugin::CallMethodAndReturn<uint32, 0x50AD90, CCamera*>(this);
}

// 0x50ADE0
bool CCamera::GetFading() {
    return m_bFading;
}

// TODO: eFadingDirection
// 0x50ADF0
int32 CCamera::GetFadingDirection() {
    if (m_bFading)
        return m_nFadeInOutFlag == 1;
    else
        return 2;
}

// 0x50AE10
bool CCamera::Get_Just_Switched_Status() {
    return m_bJust_Switched;
}

// 0x50AE20
int32 CCamera::GetScreenFadeStatus() {
    return plugin::CallMethodAndReturn<int32, 0x50AE20, CCamera*>(this);
}

// 0x50AE50
CVector* CCamera::GetGameCamPosition() {
    return &m_vecGameCamPos;
}

// 0x50AE60
bool CCamera::GetLookingLRBFirstPerson() {
    return plugin::CallMethodAndReturn<bool, 0x50AE60, CCamera*>(this);
}

// 0x50AE90
int32 CCamera::GetLookDirection() {
    return plugin::CallMethodAndReturn<int32, 0x50AE90, CCamera*>(this);
}

// 0x50AED0
bool CCamera::GetLookingForwardFirstPerson() {
    return plugin::CallMethodAndReturn<bool, 0x50AED0, CCamera*>(this);
}

// 0x50AF00
void CCamera::GetArrPosForVehicleType(eVehicleType type, int32* arrPos) {
    switch (type) {
    case VEHICLE_TYPE_MTRUCK:
        *arrPos = 0;
        break;
    case VEHICLE_TYPE_QUAD:
        *arrPos = 1;
        break;
    case VEHICLE_TYPE_HELI:
        *arrPos = 2;
        break;
    case VEHICLE_TYPE_PLANE:
        *arrPos = 4;
        break;
    case VEHICLE_TYPE_BOAT:
        *arrPos = 3;
        break;
    default:
        break;
    }
}

// 0x50AF80
float CCamera::GetPositionAlongSpline() {
    return m_fPositionAlongSpline;
}

// 0x50AF90
void CCamera::InitialiseCameraForDebugMode() {
    // NOP
}

// 0x50AFA0
void CCamera::CopyCameraMatrixToRWCam(bool bUpdateMatrix) {
    return plugin::CallMethod<0x50AFA0, CCamera*, bool>(this, bUpdateMatrix);
}

// 0x50B380
void CCamera::CalculateMirroredMatrix(CVector posn, float mirrorV, CMatrix *camMatrix, CMatrix* mirrorMatrix) {
    mirrorMatrix->GetPosition() = camMatrix->GetPosition() - posn * 2 * (DotProduct(posn, camMatrix->GetPosition()) - mirrorV);

    const CVector fwd = camMatrix->GetForward() - posn * 2 * DotProduct(posn, camMatrix->GetForward());
    mirrorMatrix->GetForward() = fwd;

    const CVector up = camMatrix->GetUp() - posn * 2 * DotProduct(posn, camMatrix->GetUp());
    mirrorMatrix->GetUp() = up;

    mirrorMatrix->GetRight() = CVector{
        up.y * fwd.z - up.z * fwd.y,
        up.z * fwd.x - up.x * fwd.z,
        up.x * fwd.y - up.y * fwd.x
    };
}

// 0x50B510
void CCamera::DealWithMirrorBeforeConstructRenderList(bool bActiveMirror, CVector mirrorNormal, float mirrorV, CMatrix* matMirror) {
    m_bMirrorActive = bActiveMirror;

    if (!bActiveMirror)
        return;

    if (matMirror)
        m_mMatMirror = *matMirror;
    else
        CalculateMirroredMatrix(mirrorNormal, mirrorV, &m_mCameraMatrix, &m_mMatMirror);

    m_mMatMirrorInverse = Invert(m_mMatMirror);
}

// 0x50B8F0
void CCamera::RenderMotionBlur() {
    if (m_nBlurType) {
//        CMBlur::MotionBlurRender is NOP, 0x71D700
//        CMBlur::MotionBlurRender();
    }
}

// 0x50B930
void CCamera::Restore() {
    return plugin::CallMethod<0x50B930, CCamera*>(this);
}

// 0x50BAB0
void CCamera::RestoreWithJumpCut() {
    return plugin::CallMethod<0x50BAB0, CCamera*>(this);
}

// 0x50BD20
void CCamera::SetCamCutSceneOffSet(const CVector& cutsceneOffset) {
    m_vecCutSceneOffset = cutsceneOffset;
}

// 0x50BD40
void CCamera::SetCameraDirectlyBehindForFollowPed_CamOnAString() {
    m_bCamDirectlyBehind = true;
    CPed* player = FindPlayerPed();
    if (player) {
        m_fPedOrientForBehindOrInFront = CGeneral::GetATanOfXY(player->m_matrix->GetForward().x, player->m_matrix->GetForward().y);
    }
}

// 0x50BD70
void CCamera::SetCameraDirectlyInFrontForFollowPed_CamOnAString() {
    m_bCamDirectlyInFront = true;
    CPed* player = FindPlayerPed();
    if (player) {
        m_fPedOrientForBehindOrInFront = CGeneral::GetATanOfXY(player->m_matrix->GetForward().x, player->m_matrix->GetForward().y);
    }
}

// unused
// 0x50BDA0
void CCamera::SetCameraDirectlyBehindForFollowPed_ForAPed_CamOnAString(CPed* targetPed) {
    return plugin::CallMethod<0x50BDA0, CCamera*, CPed*>(this, targetPed);
}

// 0x50BE30
void CCamera::SetCameraDirectlyInFrontForFollowPed_ForAPed_CamOnAString(CPed* targetPed) {
    return plugin::CallMethod<0x50BE30, CCamera*, CPed*>(this, targetPed);
}

// 0x50BEC0
void CCamera::SetCamPositionForFixedMode(const CVector* fixedModeSource, const CVector* fixedModeUpOffset) {
    m_vecFixedModeSource = *fixedModeSource;
    m_vecFixedModeUpOffSet = *fixedModeUpOffset;
    m_bGarageFixedCamPositionSet = false;
}

// 0x50BF00
void CCamera::SetFadeColour(uint8 red, uint8 green, uint8 blue) {
    m_bFadeTargetIsSplashScreen = false;
    if (red == 2 && green == 2 && blue == 2) {
        m_bFadeTargetIsSplashScreen = true;
    }

    CDraw::FadeGreen = green;
    CDraw::FadeRed = red;
    CDraw::FadeBlue = blue;
}

// TODO: eBlurType
// 0x50BF40
void CCamera::SetMotionBlur(uint8 red, uint8 green, uint8 blue, int32 value, uint32 blurType) {
    m_nBlurRed = red;
    m_nBlurGreen = green;
    m_nBlurBlue = blue;
    m_nBlurType = blurType;
    m_nMotionBlur = value;
}

// 0x50BF80
void CCamera::SetMotionBlurAlpha(int32 alpha) {
    m_nMotionBlurAddAlpha = alpha;
}

// 0x50BF90
void CCamera::SetNearClipScript(float nearClip) {
    m_fNearClipScript = nearClip;
    m_bUseNearClipScript = true;
}

// 0x50BFB0
void CCamera::SetNewPlayerWeaponMode(eCamMode mode, int16 maxZoom, int16 minZoom) {
    m_PlayerWeaponMode.m_nMode     = mode;
    m_PlayerWeaponMode.m_nMaxZoom  = maxZoom;
    m_PlayerWeaponMode.m_nMinZoom  = minZoom;
    m_PlayerWeaponMode.m_fDuration = 0.0f;
}

// 0x50BFF0
bool CCamera::Using1stPersonWeaponMode() {
    return plugin::CallMethodAndReturn<bool, 0x50BFF0, CCamera*>(this);
}

// 0x50C030
void CCamera::SetParametersForScriptInterpolation(float interpolationToStopMoving, float interpolationToCatchUp, uint32 timeForInterpolation) {
    m_nScriptTimeForInterpolation = timeForInterpolation;
    m_bScriptParametersSetForInterPol = true;
    m_fScriptPercentageInterToStopMoving = interpolationToStopMoving * 0.01f;
    m_fScriptPercentageInterToCatchUp = interpolationToCatchUp * 0.01f;
}

// 0x50C070
void CCamera::SetPercentAlongCutScene(float percent) {
    return plugin::CallMethod<0x50C070, CCamera*>(this, percent);
}

// 0x50C100
void CCamera::SetRwCamera(RwCamera* camera) {
    m_pRwCamera = camera;
    m_mViewMatrix.Attach(&camera->viewMatrix, false);
}

// 0x50C140
void CCamera::SetWideScreenOn() {
    TheCamera.m_bWideScreenOn = true;
    TheCamera.m_bWantsToSwitchWidescreenOff = false;
}

// 0x50C150
void CCamera::SetWideScreenOff() {
    m_bWantsToSwitchWidescreenOff = m_bWideScreenOn;
}

// zoomMode : value between 0 - 2
// 0x50C160
void CCamera::SetZoomValueFollowPedScript(int16 zoomMode) {
    return plugin::CallMethod<0x50C160, CCamera*, int16>(this, zoomMode);
}

// zoomMode : 0- ZOOM_ONE , 1- ZOOM_TWO , 2- ZOOM_THREE
// 0x50C1B0
void CCamera::SetZoomValueCamStringScript(int16 zoomMode) {
    return plugin::CallMethod<0x50C1B0, CCamera*, int16>(this, zoomMode);
}

// 0x50C260
void CCamera::StartCooperativeCamMode() {
    m_bCooperativeCamMode = true;
    CGameLogic::n2PlayerPedInFocus = 2;
}

// unused
// 0x50C270
void CCamera::StopCooperativeCamMode() {
    m_bCooperativeCamMode = false;
    CGameLogic::n2PlayerPedInFocus = 2;
}

// 0x50C280
void CCamera::AllowShootingWith2PlayersInCar(bool bAllow) {
    m_bAllowShootingWith2PlayersInCar = bAllow;
}

// 0x50C290
void CCamera::StoreValuesDuringInterPol(CVector* sourceDuringInter, CVector* targetDuringInter, CVector* upDuringInter, float* FOVDuringInter) {
    m_vecSourceDuringInter = *sourceDuringInter;
    m_vecTargetDuringInter = *targetDuringInter;
    m_vecUpDuringInter = *upDuringInter;
    m_fFOVDuringInter = *FOVDuringInter;

    // TODO: variable naming
    float a = sourceDuringInter->x - m_vecTargetDuringInter.x;
    float b = sourceDuringInter->y - m_vecTargetDuringInter.y;
    m_fBetaDuringInterPol = CGeneral::GetATanOfXY(a, b);

    float c = sqrt(b * b + a * a);
    float d = sourceDuringInter->z - m_vecTargetDuringInter.z;
    m_fAlphaDuringInterPol = CGeneral::GetATanOfXY(c, d);
}

// 0x50C360
void CCamera::UpdateTargetEntity() {
    plugin::CallMethod<0x50C360, CCamera*>(this);
}

// 0x50C7C0
void CCamera::TakeControl(CEntity* target, eCamMode modeToGoTo, eSwitchType switchType, int32 whoIsInControlOfTheCamera) {
    plugin::CallMethod<0x50C7C0, CCamera*, CEntity*, eCamMode, eSwitchType, int32>(this, target, modeToGoTo, switchType, whoIsInControlOfTheCamera);
}

// 0x50C8B0
void CCamera::TakeControlNoEntity(const CVector* fixedModeVector, eSwitchType switchType, int32 whoIsInControlOfTheCamera) {
    plugin::CallMethod<0x50C8B0, CCamera*, const CVector*, eSwitchType, int32>(this, fixedModeVector, switchType, whoIsInControlOfTheCamera);
}

// 0x50C910
void CCamera::TakeControlAttachToEntity(CEntity* target, CEntity* attached, CVector* attachedCamOffset, CVector* attachedCamLookAt, float attachedCamAngle, eSwitchType switchType, int32 whoIsInControlOfTheCamera) {
    plugin::CallMethod<0x50C910, CCamera*, CEntity*, CEntity*, CVector*, CVector*, float, eSwitchType, int32>(this, target, attached, attachedCamOffset, attachedCamLookAt, attachedCamAngle, switchType, whoIsInControlOfTheCamera);
}

// 0x50CAE0
void CCamera::TakeControlWithSpline(eSwitchType switchType) {
    m_bLookingAtPlayer = false;
    m_bLookingAtVector = false;
    m_bCutsceneFinished = false;
    m_nModeToGoTo = MODE_FLYBY;
    m_nTypeOfSwitch = switchType;
    m_bStartInterScript = true;
}

// 0x50CB10
void CCamera::UpdateAimingCoors(const CVector* aimingTargetCoors) {
    m_vecAimingTargetCoors.x = aimingTargetCoors->x;
    m_vecAimingTargetCoors.y = aimingTargetCoors->y;
    m_vecAimingTargetCoors.z = aimingTargetCoors->z;
}

// unused
// 0x50CB90
void CCamera::SetNearClipBasedOnPedCollision(float arg2) {
    plugin::CallMethod<0x50CB90, CCamera*, float>(this, arg2);
}

// TODO: eAimingType
// 0x50CBF0
void CCamera::SetColVarsAimWeapon(int32 aimingType) {
    if (aimingType <= 3) {
        switch (aimingType) {
        case 0u:
            CCamera::SetCamCollisionVarDataSet(0);
            break;
        case 1u:
            CCamera::SetCamCollisionVarDataSet(1);
            break;
        case 2u:
            CCamera::SetCamCollisionVarDataSet(2);
            break;
        case 3u:
            CCamera::SetCamCollisionVarDataSet(3);
            break;
        default:
            return;
        }
    }
}

// 0x50CC50
void CCamera::SetColVarsPed(ePedType pedType, int32 nCamPedZoom) {
    plugin::Call<0x50CC50, ePedType, int32>(pedType, nCamPedZoom);
}

// 0x50CD30
void CCamera::CameraGenericModeSpecialCases(CPed* targetPed) {
    plugin::CallMethod<0x50CD30, CCamera*, CPed*>(this, targetPed);
}

// unused
// 0x50CD80
void CCamera::CameraPedModeSpecialCases() {
    plugin::CallMethod<0x50CD80, CCamera*>(this);
    //    CCollision::m_bCamCollideWithVehicles = true;
    //    CCollision::m_bCamCollideWithObjects = true;
    //    CCollision::m_bCamCollideWithPeds = true;
}

// 0x50CDA0
void CCamera::CameraPedAimModeSpecialCases(CPed* ped) {
    plugin::CallMethod<0x50CDA0, CCamera*, CPed*>(this, ped);
}

// 0x50CDE0
void CCamera::CameraVehicleModeSpecialCases(CVehicle* vehicle) {
    plugin::CallMethod<0x50CDE0, CCamera*, CVehicle*>(this, vehicle);
}

// 0x50CE80
bool CCamera::IsExtraEntityToIgnore(CEntity* entity) {
    return plugin::CallMethodAndReturn<bool, 0x50CE80, CCamera*, CEntity*>(this, entity);
}

// 0x420C40
bool CCamera::IsSphereVisible(const CVector& origin, float radius, RwMatrix* transformMatrix) {
    return plugin::CallMethodAndReturn<bool, 0x420C40, CCamera*, const CVector&, float, RwMatrix*>(this, origin, radius, transformMatrix);
}

// 0x420D40
bool CCamera::IsSphereVisible(const CVector& origin, float radius) {
    return plugin::CallMethodAndReturn<bool, 0x420D40, CCamera*, const CVector&, float>(this, origin, radius);
}

// unused
// 0x50CEB0
bool CCamera::ConsiderPedAsDucking(CPed* ped) {
    return plugin::CallMethodAndReturn<bool, 0x50CEB0, CCamera*, CPed*>(this, ped);
}

// 0x50CEF0
void CCamera::ResetDuckingSystem(CPed* ped) {
    plugin::CallMethod<0x50CEF0, CCamera*, CPed*>(this, ped);
}

// arg5 always used as false
// 0x50CFA0
void CCamera::HandleCameraMotionForDucking(CPed* ped, CVector* source, CVector* targPosn, bool arg5) {
    plugin::CallMethod<0x50CFA0, CCamera*, CPed*, CVector*, CVector*, bool>(this, ped, source, targPosn, arg5);
}

// arg5 always used as false
// 0x50D090
void CCamera::HandleCameraMotionForDuckingDuringAim(CPed* ped, CVector* source, CVector* targPosn, bool arg5) {
    plugin::CallMethod<0x50D090, CCamera*, CPed*, CVector*, CVector*, bool>(this, ped, source, targPosn, arg5);
}

// 0x50D160
void CCamera::VectorMoveLinear(CVector* moveLinearPosnEnd, CVector* moveLinearPosnStart, float duration, bool bMoveLinearWithEase) {
    plugin::CallMethod<0x50D160, CCamera*, CVector*, CVector*, float, bool>(this, moveLinearPosnEnd, moveLinearPosnStart, duration, bMoveLinearWithEase);
}

// 0x50D1D0
void CCamera::VectorTrackLinear(CVector* trackLinearStartPoint, CVector* trackLinearEndPoint, float duration, bool bEase) {
    plugin::CallMethod<0x50D1D0, CCamera*, CVector*, CVector*, float, bool>(this, trackLinearStartPoint, trackLinearEndPoint, duration, bEase);
}

// 0x50D240
void CCamera::AddShakeSimple(float duration, int32 type, float intensity) {
    plugin::CallMethod<0x50D240, CCamera*, float, int32, float>(this, duration, type, intensity);
}

// 0x50D280
void CCamera::LerpFOV(float zoomInFactor, float zoomOutFactor, float timeLimit, bool bEase) {
    m_fStartZoomTime = float(CTimer::GetTimeInMS());
    m_fEndZoomTime = float(CTimer::GetTimeInMS()) + timeLimit;

    m_nZoomMode = bEase; // TODO: Rename
    m_fZoomInFactor = zoomInFactor;
    m_fZoomOutFactor = zoomOutFactor;
}

// 0x50D2D0
void CCamera::InitialiseScriptableComponents() {
    plugin::CallMethod<0x50D2D0, CCamera*>(this);
}

// 0x50B5D0
void CCamera::ProcessFade() {
    plugin::CallMethod<0x50B5D0, CCamera*>(this);
}

// 0x50B6D0
void CCamera::ProcessMusicFade() {
    plugin::CallMethod<0x50B6D0, CCamera*>(this);
}

// unused, empty
// 0x50B880
void CCamera::ProcessObbeCinemaCameraPed() {
    // NOP
}

// 0x50B890
void CCamera::ProcessWideScreenOn() {
    if (m_bWantsToSwitchWidescreenOff) {
        m_bWantsToSwitchWidescreenOff = false;
        m_bWideScreenOn = false;
        m_fScreenReductionPercentage = 0.0f;
        m_fFOV_Wide_Screen = 0.0f;
        m_fWideScreenReductionAmount = 0.0f;
    } else {
        this->m_fWideScreenReductionAmount = 1.0f;
        this->m_fScreenReductionPercentage = 30.0f;
        this->m_fFOV_Wide_Screen = m_aCams[m_nActiveCam].m_fFOV * 0.3f;
    }
}

// 0x516440
void CCamera::ProcessVectorTrackLinear() {
    plugin::CallMethod<0x516440, CCamera*>(this);
}

// 0x50D350
void CCamera::ProcessVectorTrackLinear(float ratio) {
    plugin::CallMethod<0x50D350, CCamera*, float>(this, ratio);
}

// 0x50D430
void CCamera::ProcessVectorMoveLinear(float ratio) {
    plugin::CallMethod<0x50D430, CCamera*, float>(this, ratio);
}

// 0x516500
void CCamera::ProcessFOVLerp() {
    plugin::CallMethod<0x516500, CCamera*>(this);
}

// 0x50D510
void CCamera::ProcessFOVLerp(float ratio) {
    plugin::CallMethod<0x50D510, CCamera*, float>(this, ratio);
}

// 0x5164A0
void CCamera::ProcessVectorMoveLinear() {
    plugin::CallMethod<0x5164A0, CCamera*>(this);
}

// unused
// 0x51A6F0
void CCamera::ProcessShake() {
    plugin::CallMethod<0x51A6F0, CCamera*>(this);
}

// shakeIntensity not used
// 0x516560
RwV3d* CCamera::ProcessShake(float shakeIntensity) {
    return plugin::CallMethodAndReturn<RwV3d*, 0x516560, CCamera*, float>(this, shakeIntensity);
}

// unused
// 0x516AE0
void CCamera::ProcessScriptedCommands() {
    TheCamera.ProcessVectorMoveLinear();
    TheCamera.ProcessVectorTrackLinear();
    TheCamera.ProcessFOVLerp();
}

// 0x52B730
void CCamera::Process() {
    plugin::CallMethod<0x52B730, CCamera*>(this);
}

// 0x514860
void CCamera::DrawBordersForWideScreen() {
    plugin::CallMethod<0x514860, CCamera*>(this);
}

// 0x514950
void CCamera::FinishCutscene() {
    TheCamera.SetPercentAlongCutScene(100.0f);
    m_fPositionAlongSpline = 1.0f;
    m_bCutsceneFinished = true;
}

// 0x514970
void CCamera::Find3rdPersonCamTargetVector(float range, CVector source, CVector* pCamera, CVector* pPoint) {
    plugin::CallMethod<0x514970, CCamera*, float, CVector, CVector*, CVector*>(this, range, source, pCamera, pPoint);
}

// 0x514B80
float CCamera::CalculateGroundHeight(eGroundHeightType type) {
    return plugin::CallMethodAndReturn<float, 0x514B80, CCamera*, eGroundHeightType>(this, type);
}

// 0x514D60
void CCamera::CalculateFrustumPlanes(bool bForMirror) {
    plugin::CallMethod<0x514D60, CCamera*, bool>(this, bForMirror);
}

// 0x5150E0
void CCamera::CalculateDerivedValues(bool bForMirror, bool bOriented) {
    return plugin::CallMethod<0x5150E0, CCamera*, bool, bool>(this, bForMirror, bOriented);
}

// 0x516B20
void CCamera::ImproveNearClip(CVehicle* vehicle, CPed* ped, CVector* source, CVector* targPosn) {
    return plugin::CallMethod<0x516B20, CCamera*, CVehicle*, CPed*, CVector*, CVector*>(this, vehicle, ped, source, targPosn);
}

static CMatrix& preMirrorMat = *(CMatrix*)0xB6FE40;

// 0x51A560
void CCamera::SetCameraUpForMirror() {
    preMirrorMat = m_mCameraMatrix;
    m_mCameraMatrix = m_mMatMirror;
    CopyCameraMatrixToRWCam(true);
    CalculateDerivedValues(true, false);
}

// 0x51A5A0
void CCamera::RestoreCameraAfterMirror() {
    SetMatrix(preMirrorMat);
    CopyCameraMatrixToRWCam(true);
    CalculateDerivedValues(false, false);
}

// 0x51A5D0
bool CCamera::ConeCastCollisionResolve(CVector* source, CVector* center, CVector* pVecOut, float radius, float arg5, float* pFloatOut) {
    return plugin::CallMethodAndReturn<bool, 0x51A5D0, CCamera*, CVector*, CVector*, CVector*, float, float, float*>(this, source, center, pVecOut, radius, arg5, pFloatOut);
}

// 0x51E560
bool CCamera::TryToStartNewCamMode(int32 camSequence) {
    return plugin::CallMethodAndReturn<bool, 0x51E560, CCamera*, int32>(this, camSequence);
}

// 0x520190
void CCamera::CameraColDetAndReact(CVector* source, CVector* target) {
    plugin::CallMethod<0x520190, CCamera*, CVector*, CVector*>(this, source, target);
}

// 0x527FA0
void CCamera::CamControl() {
    plugin::CallMethod<0x527FA0, CCamera*>(this);
}

// 0x5B24A0
void CCamera::DeleteCutSceneCamDataMemory() {
    plugin::CallMethod<0x5B24A0, CCamera*>(this);
}

// 0x5B24D0
void CCamera::LoadPathSplines(FILE* file) {
    plugin::CallMethod<0x5B24D0, CCamera*>(this);
}

// 0x50AB50
void CCamera::GetScreenRect(CRect* rect) {
    return plugin::Call<0x50AB50>();
}

// unused
// 0x50AC10
void CCamera::Enable1rstPersonWeaponsCamera() {
    return plugin::Call<0x50AC10>();
}

// 0x50CB60
void CCamera::SetCamCollisionVarDataSet(int32 index) {
    plugin::Call<0x50CB60, int32>(index);
}

// 0x50CCA0
void CCamera::SetColVarsVehicle(eVehicleType vehicleType, int32 camVehicleZoom) {
    plugin::Call<0x50CCA0, eVehicleType, int32>(vehicleType, camVehicleZoom);
}

// 0x50A970
void CamShakeNoPos(CCamera* camera, float strength) {
    ((void(__cdecl*)(CCamera*, float))0x50A970)(camera, strength);
}
