#pragma once

#include "DebugModule.h"

namespace notsa { 
namespace debugmodules {
class ScriptDebugModule final : public DebugModule {
public:
    void RenderWindow() override;
    void RenderMenuEntry() override;
    void Render3D() override;

private:
    bool m_IsOpen{};
};
}; // namespace debugmodules
}; // namespace notsa

