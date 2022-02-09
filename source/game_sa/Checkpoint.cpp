#include "StdInc.h"

#include "Checkpoint.h"

void CCheckpoint::InjectHooks() {
    ReversibleHooks::Install("CCheckpoint", "Render", 0x725C00, &CCheckpoint::Render);
}

bool CCheckpoint::UpdateZWithWaterLevel() {
    float waterLevel = 0.0f;
    if (CWaterLevel::GetWaterLevelNoWaves(m_vecPosition, &waterLevel, nullptr, nullptr)) {
        if (waterLevel >= m_vecPosition.z) {
            m_vecPosition.z = waterLevel;
            return true;
        }
    }
    return false;
}

// 0x725C00
void CCheckpoint::Render() {
    switch (m_nType) {
    case eCheckpointType::CHECKPOINT_ENDTUBE:
    case eCheckpointType::CHECKPOINT_TUBE:
    case eCheckpointType::CHECKPOINT_EMPTYTUBE: {

        bool doZCheck = true;
        float waterLevel = 0.0f;
        if (CWaterLevel::GetWaterLevelNoWaves(m_vecPosition, &waterLevel, nullptr, nullptr)) {
            if (waterLevel >= m_vecPosition.z) {
                m_vecPosition.z = waterLevel;
                doZCheck = false;
            }
        }

        switch (m_nType) {
        case eCheckpointType::CHECKPOINT_ENDTUBE: {
            RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)RwTextureGetRaster(gpFinishFlagTex));
            RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)false);
            RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)true);
            RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)true);
            RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS, (void*)rwTEXTUREADDRESSWRAP);

            CVector   screen;
            CVector2D size;
            if (CSprite::CalcScreenCoors(m_vecPosition + CVector(0.0f, 0.0f, 3.0f), screen, size, false, true)) {
                CRGBA color = m_colour;
                color.a = 0xff;
                CSprite::RenderOneXLUSprite(screen, size * (m_fSize / 2.0f), color, 1.0f / screen.z, m_colour.a);
            }

            RwRenderStateSet(rwRENDERSTATETEXTURERASTER, (void*)nullptr);
            RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, (void*)true);
            RwRenderStateSet(rwRENDERSTATEZTESTENABLE, (void*)true);
            RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, (void*)false);
            break;
        }
        }
        
        C3dMarkers::PlaceMarker(m_nIdentifier, e3dMarkerType::MARKER3D_TUBE, m_vecPosition, m_fSize * 1.5f, m_colour.r, m_colour.g, m_colour.b, 128, m_nPulsePeriod, m_fPulseFraction, 1, 0.0f, 0.0f, 0.0f, doZCheck);

        switch (m_nType) {
        case eCheckpointType::CHECKPOINT_TUBE: {
            C3dMarkers::DirectionArrowSet(m_vecPosition, m_fSize * 0.625f, CRGBA(255, 64, 64, 255), -m_vecDirection);
            break;
        }
        }
        break;
    }
    case eCheckpointType::CHECKPOINT_TORUS: {
        if (2 * m_fSize <= FindDistanceToPlayer(m_vecPosition)) {
            C3dMarkers::PlaceMarker(m_nIdentifier, e3dMarkerType::MARKER3D_TORUS, m_vecPosition, m_fSize, m_colour.r, m_colour.g, m_colour.b, m_colour.a, m_nPulsePeriod, m_fPulseFraction, m_nRotateRate, m_vecDirection.x, m_vecDirection.y, m_vecDirection.z, false);
        }
        break;
    }
    case eCheckpointType::CHECKPOINT_TORUS_NOFADE:
    case eCheckpointType::CHECKPOINT_TORUSROT:
    case eCheckpointType::CHECKPOINT_TORUS_UPDOWN:
    case eCheckpointType::CHECKPOINT_TORUS_DOWN: {
        C3dMarkers::PlaceMarker(m_nIdentifier, e3dMarkerType::MARKER3D_TORUS, m_vecPosition, m_fSize, m_colour.r, m_colour.g, m_colour.b, m_colour.a, m_nPulsePeriod, m_fPulseFraction, m_nRotateRate, m_vecDirection.x, m_vecDirection.y, m_vecDirection.z, false);
        break;
    }
    case eCheckpointType::CHECKPOINT_TORUSTHROUGH: {
        // NOTE: Original code did a trick. The jump label `LABEL_17` is basically part of this loop (with i = 0)
        // Pretty smart compiler, huh?
        for (int i = 0; i < 4; i++) {
            C3dMarkers::PlaceMarker(m_nIdentifier, e3dMarkerType::MARKER3D_TORUS, m_vecPosition + m_vecDirection * (float)i * m_multiSize, m_fSize, m_colour.r, m_colour.g, m_colour.b, m_colour.a - 16 * i, m_nPulsePeriod, m_fPulseFraction, m_nRotateRate, m_vecDirection.x, m_vecDirection.y, m_vecDirection.z, false);
        }
        break;
    }
    }
}
