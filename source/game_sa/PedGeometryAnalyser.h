#pragma once

#include <array>
#include <span>

#include "Vector.h"
#include "Base.h"

class CVector;
class CEntity;
class CPed;
class CColSphere;
class CPhysical;

class CPointRoute {
public:
    static void* operator new(uint32 size);
    static void operator delete(void* ptr, size_t sz);

    /// Get all active points
    auto GetPoints()       { return m_vecPoints | rng::views::take(m_nNumPoints); }
    auto GetPoints() const { return m_vecPoints | rng::views::take(m_nNumPoints); }

    /// Are there are no points
    bool IsEmpty() const { return m_nNumPoints == 0; }

    /// Is there space for more points
    bool IsFull()  const { return (size_t)m_nNumPoints >= std::size(m_vecPoints); }

    /// Add a point (Doesn't check whenever there's space for it)
    template<typename... T_Points>
    void AddPoints(T_Points&&... point) { ((m_vecPoints[m_nNumPoints++] = point), ...); }

    /// Add a point if we aren't full yet
    void MaybeAddPoint(const CVector& point) {
        if (!IsFull()) {
            AddPoints(point);
        }
    }

    /// Add points if there's space for them
    template<typename... T_Points>
    void MaybeAddPoints(T_Points&&... point) { (MaybeAddPoint(point), ...); }

    /// Reverse the order of points
    void Reverse() { rng::reverse(GetPoints()); }

    /// Clear all points
    void Clear() { m_nNumPoints = 0; }

    // Access active points
    CVector& operator[](size_t idx)       { return GetPoints()[idx]; }
    CVector  operator[](size_t idx) const { return GetPoints()[idx]; }

    // TODO: Make private
    uint32                 m_nNumPoints{};
    std::array<CVector, 8> m_vecPoints; // NOTE: Use `GetPoints` to iterate over only the valid points
};

VALIDATE_SIZE(CPointRoute, 0x64);

class CPedGeometryAnalyser {
public:
    static void InjectHooks();
    
    static void CanPedJumpObstacle(const CPed& ped, const CEntity& entity);
    static void CanPedJumpObstacle(const CPed& ped, const CEntity& entity, const CVector&, const CVector&);

    static bool CanPedTargetPed(CPed& ped, CPed& targetPed, bool checkDirection);
    static bool CanPedTargetPoint(const CPed& ped, const CVector& a2, bool a3);

    static int32 ComputeBuildingHitPoints(const CVector& a1, const CVector& a2);
    
    static void ComputeClearTarget(const CPed& ped, const CVector&, CVector&);

    static bool ComputeClosestSurfacePoint(const CPed& ped, CEntity& entity, CVector& point);
    static bool ComputeClosestSurfacePoint(const CVector& posn, CEntity& entity, CVector& point);
    static bool ComputeClosestSurfacePoint(const CVector& posn, const CVector* corners, CVector& point);

    static void ComputeEntityBoundingBoxCentre(float zPos, CEntity& entity, CVector& center);
    static void ComputeEntityBoundingBoxCentreUncached(float zPos, const CVector* corners, CVector& center);
    static void ComputeEntityBoundingBoxCentreUncachedAll(float zPos, CEntity& entity, CVector& center);
    static void ComputeEntityBoundingBoxCorners(float zPos, CEntity& entity, CVector* corners);
    static void ComputeEntityBoundingBoxCornersUncached(float zPos, CEntity& entity, CVector* corners);
    static void ComputeEntityBoundingBoxPlanes(float zPos, CEntity& entity, CVector(*outPlanes)[4], float* outPlanesDot);
    static void ComputeEntityBoundingBoxPlanesUncached(float zPos, const CVector* corners, CVector(*outPlanes)[4], float* outPlanesDot);
    static void ComputeEntityBoundingBoxPlanesUncachedAll(float zPos, CEntity& entity, CVector (*outPlanes)[4], float* outPlanesDot);
    static void ComputeEntityBoundingBoxSegmentPlanes(float zPos, CEntity& entity, CVector*, float*);
    static CVector* ComputeEntityBoundingBoxSegmentPlanesUncached(const CVector* corners, CVector& center, CVector* a3, float* a4);
    static CVector* ComputeEntityBoundingBoxSegmentPlanesUncachedAll(float zPos, CEntity& entity, CVector* a3, float* a4);
    static void ComputeEntityBoundingSphere(const CPed& ped, CEntity& entity, CColSphere&);

    static int32 ComputeMoveDirToAvoidEntity(const CPed& ped, CEntity& entity, CVector& posn);

    static CVector* ComputeEntityDirs(const CEntity& entity, CVector* posn);
    static int32 ComputeEntityHitSide(const CPed& ped, CEntity& entity);
    static int32 ComputeEntityHitSide(const CVector& point1, const CVector* point2, const float* x);
    static int32 ComputeEntityHitSide(const CVector& point, CEntity& entity);
    static int32 ComputePedHitSide(const CPed& ped, const CPhysical& physical);
    static int32 ComputePedHitSide(const CPed& ped, const CVector& posn);
    static int32 ComputePedShotSide(const CPed& ped, const CVector& posn);

    static int32 ComputeRouteRoundEntityBoundingBox(const CPed& ped, CEntity& entity, const CVector& posn, CPointRoute& pointRoute, int32 a5);
    static int32 ComputeRouteRoundEntityBoundingBox(const CPed& ped, const CVector& a2, CEntity& entity, const CVector& a4, CPointRoute& pointRoute, int32 a6);

    static bool ComputeRouteRoundSphere(const CPed& ped, const CColSphere& sphere, const CVector& a3, const CVector& a4, CVector& a5, CVector& a6);

    static bool GetIsLineOfSightClear(const CPed& ped, const CVector& a2, CEntity& entity, float& a4);
    static bool GetIsLineOfSightClear(const CVector& a1, const CVector& a2, CEntity& a3);
    static CPed* GetNearestPed(const CVector& point);

    static bool IsEntityBlockingTarget(CEntity* entity, const CVector& point, float distance);
    static bool IsInAir(const CPed& ped);
    static bool IsWanderPathClear(const CVector& a1, const CVector& a2, float a3, int32 a4);

    static bool LiesInsideBoundingBox(const CPed& ped, const CVector& posn, CEntity& entity);
};
