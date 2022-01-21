#include "StdInc.h"

#include <cstdio>
#include <filesystem>

#include "GenericGameStorage.h"
#include "Messages.h"
#include "C_PcSave.h"
#include "SimpleVariablesSaveStructure.h"
#include "GxtChar.h"

void C_PcSave::InjectHooks() {
    // See note in CGenericGameStorage::InjectHooks as to why all this is unhooked by default

    using namespace ReversibleHooks;
    Install("C_PcSave", "SetSaveDirectory", 0x619040, &C_PcSave::SetSaveDirectory, true);
    Install("C_PcSave", "GenerateGameFilename", 0x6190A0, &C_PcSave::GenerateGameFilename, true);
    Install("C_PcSave", "PopulateSlotInfo", 0x619140, &C_PcSave::PopulateSlotInfo, true);
    Install("C_PcSave", "SaveSlot", 0x619060, &C_PcSave::SaveSlot, true);
    Install("C_PcSave", "DeleteSlot", 0x6190D0, &C_PcSave::DeleteSlot, true);
}

// 0x619040
void C_PcSave::SetSaveDirectory(const char* path) {
    sprintf_s(DefaultPCSaveFileName, "%s\\%s", path, "GTASAsf");
}

// 0x6190A0
void C_PcSave::GenerateGameFilename(int32 slot, char* out) {
    assert(slot < MAX_SAVEGAME_SLOTS);
    sprintf(out, "%s%i%s", DefaultPCSaveFileName, slot + 1, ".b");
}

// 0x619140
void C_PcSave::PopulateSlotInfo(void) {
	s_PcSaveHelper.error = eErrorCode::NONE;

	for (auto i = 0u; i < std::size(CGenericGameStorage::ms_Slots); ++i) {
		CGenericGameStorage::ms_Slots[i] = CGenericGameStorage::eSlotState::EMPTY;
		CGenericGameStorage::ms_SlotFileName[i][0] = 0;
		CGenericGameStorage::ms_SlotSaveDate[i][0] = 0;
	}

	for (auto i = 0u; i < std::size(CGenericGameStorage::ms_Slots); ++i) {
        char path[MAX_PATH]{};
        CSimpleVariablesSaveStructure vars{};

		GenerateGameFilename(i, path);
		auto file = CFileMgr::OpenFile(path, "rb");
		if (file) {
			CFileMgr::Seek(file, strlen(CGenericGameStorage::ms_BlockTagName), SEEK_SET);
			CFileMgr::Read(file, &vars, sizeof(CSimpleVariablesSaveStructure));

            if (std::string_view{ TopLineEmptyFile } != vars.m_szSaveName) {
				memcpy(CGenericGameStorage::ms_SlotFileName[i], vars.m_szSaveName, 48); // TODO: why 48?
				CGenericGameStorage::ms_Slots[i] = CGenericGameStorage::eSlotState::IN_USE;
				CGenericGameStorage::ms_SlotFileName[i][24] = 0;                        // TODO: Why 24?
			}
			CFileMgr::CloseFile(file);
		}

		if (CGenericGameStorage::ms_Slots[i] != CGenericGameStorage::eSlotState::IN_USE) {
			continue;
		}

		if (CGenericGameStorage::CheckDataNotCorrupt(i, path)) {
            const auto& time = vars.m_systemTime;

            char monthGXTKey[64]{};
            sprintf_s(monthGXTKey, "MONTH%d", (uint32)time.wMonth); 
            assert(time.wMonth - 1 < 12); // NOTSA

            char date[128];
            sprintf_s(date,
                "%02d %s %04d %02d:%02d:%02d",
                time.wDay,
                GxtCharToAscii(TheText.Get(monthGXTKey), 0),
                time.wYear,
                time.wHour,
                time.wMinute,
                time.wSecond
			);
			AsciiToGxtChar(date, CGenericGameStorage::ms_SlotSaveDate[i]);
		} else {
			CMessages::InsertNumberInString(TheText.Get("FEC_SLC"), i, -1, -1, -1, -1, -1, CGenericGameStorage::ms_SlotFileName[i]);
		}
	}
}

// 0x619060
uint32 C_PcSave::SaveSlot(int32 slot) {
    CGenericGameStorage::MakeValidSaveName(slot);
    s_PcSaveHelper.error = eErrorCode::NONE;
    CFileMgr::SetDirMyDocuments();
    CGenericGameStorage::DoGameSpecificStuffBeforeSave();
    return CGenericGameStorage::GenericSave() ? 0 : 2;
}

// 0x6190D0
bool C_PcSave::DeleteSlot(int32 slot) {
    assert(slot < MAX_SAVEGAME_SLOTS);

    char path[MAX_PATH]{};
    s_PcSaveHelper.error = eErrorCode::NONE;
	GenerateGameFilename(slot, path);
#ifdef DEFAULT_FUNCTIONS
    DeleteFile(path);
    if (auto f = CFileMgr::OpenFile(path, "rb")) {
        CFileMgr::CloseFile(f);
        return true;
    }
    return false;
#else
    return std::filesystem::remove(path);
#endif
}
