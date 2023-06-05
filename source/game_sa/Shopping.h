#pragma once

#include "TheZones.h"

static inline std::array<float, NUM_LEVELS>& gPriceMultipliers = *(std::array<float, NUM_LEVELS>*)0x8A6204;

// incomplete, todo: move
class CMultiBuilding {};

enum ePriceSection : int32 {
    PRICE_SECTION_NONE = 0,
    PRICE_SECTION_CAR_MODS = 1,
    PRICE_SECTION_CAR_PAINTJOBS = 2, // Pay 'n' Spray paintjobs
    PRICE_SECTION_FURNITURE = 3,
    PRICE_SECTION_CLOTHES = 4,
    PRICE_SECTION_HAIRCUTS = 5,
    PRICE_SECTION_TATTOOS = 6,
    PRICE_SECTION_GIFTS = 7,
    PRICE_SECTION_FOOD = 8,
    PRICE_SECTION_WEAPONS = 9,
    PRICE_SECTION_PROPERTY = 10
};

class CShopping {
    struct PriceModifier {
        uint32 key;
        uint32 price;
    };
    VALIDATE_SIZE(PriceModifier, 0x8);

    struct ItemPrice {
        uint32 key;
        uint32 price;

        union {
            struct {
                uint32 ammo;
            } weapon;
            struct {
                uint32 modelKey;
                uint32 type; // a/k/a textureKey
            } clothes;
            struct {
                uint32 type1;
                uint32 texKey;
            } tattoos;
            struct {
                int32 extra1;
                int32 extra2;
            } general;
        };

        char nameTag[8];
    };
    VALIDATE_SIZE(ItemPrice, 0x18);

    struct StatModifiers {
        struct {
            int8 statIndex;
            int8 change;
        } modifiers[2];
    };
    VALIDATE_SIZE(StatModifiers, 0x4);

    static constexpr auto MAX_PRICE_MODIFIERS = 20u;
    static constexpr auto NUM_ITEMS = 560u; // ?
    static constexpr auto NUM_BUYABLE_ITEMS = 300u; // ?
    static constexpr auto NUM_SECTION_NAMES = 11u;

    inline static std::array<const char*, NUM_SECTION_NAMES>& ms_sectionNames = *(std::array<const char*, NUM_SECTION_NAMES>*)0x8A61D8;

    inline static std::array<bool, NUM_ITEMS>& ms_bHasBought = *(std::array<bool, NUM_ITEMS>*)0xA972A0;
    inline static std::array<StatModifiers, NUM_ITEMS>& ms_statModifiers = *(std::array<StatModifiers, NUM_ITEMS>*)0xA974D0;
    inline static std::array<PriceModifier, MAX_PRICE_MODIFIERS>& ms_priceModifiers = *(std::array<PriceModifier, MAX_PRICE_MODIFIERS>*)0xA98650;
    inline static std::array<ItemPrice, NUM_BUYABLE_ITEMS>& ms_prices = *(std::array<ItemPrice, NUM_BUYABLE_ITEMS>*)0xA986F0;
    inline static uint32& ms_numBuyableItems = *(uint32*)0xA9A310;
    inline static std::array<uint32, NUM_ITEMS>& ms_keys = *(std::array<uint32, NUM_ITEMS>*)0xA97D90;
    inline static std::array<uint32, NUM_BUYABLE_ITEMS>& ms_shopContents = *(std::array<uint32, NUM_BUYABLE_ITEMS>*)0xA9A318;
    inline static ePriceSection& ms_priceSectionLoaded = *(ePriceSection*)0xA9A7C8;
    inline static int32& ms_numPrices = *(int32*)0xA9A7CC;
    inline static int32& ms_numPriceModifiers = *(int32*)0xA9A7D0;
    inline static char (&ms_shopLoaded)[24] = *(char (*)[24])0xA9A7D8;
    inline static int32& ms_numItemsInShop = *(int32*)0xA9A7F0;

public:
    static void InjectHooks();

    static void Init();
    static void ShutdownForRestart();
    static void Buy(uint32 key, int32 extraInfo);
    static int32 FindItem(uint32 itemKey);
    static bool FindSection(FILESTREAM file, const char* sectionName);
    static bool FindSectionInSection(FILESTREAM file, const char* parentSection, const char* childSection);
    static int32 GetExtraInfo(uint32 itemKey, int32 index);
    static int32 GetItemIndex(uint32 itemKey);
    static uint32 GetKey(const char* modelName, ePriceSection section);
    static const char* GetNameTag(uint32 itemKey);
    static const char* GetNextSection(FILESTREAM file);
    static int32 GetPrice(uint32 itemId);
    static ePriceSection GetPriceSectionFromName(const char* name);
    static void SetPlayerHasBought(uint32 itemKey);
    static bool HasPlayerBought(uint32 itemKey);
    static void IncrementStat(uint32 statIndex, int32 change);
    static void IncrementStat2(eStats stat, int32 change);
    static void LoadPrices(const char* sectionName);
    static void LoadShop(const char* sectionName);
    static void LoadStats();
    static void RemoveLoadedPrices();
    static void RemoveLoadedShop();
    static void AddPriceModifier(const char* name, const char* section, int32 price);
    static void AddPriceModifier(uint32 key, int32 price);
    static void RemovePriceModifier(const char* name, const char* section);
    static void RemovePriceModifier(uint32 key);
    static void StoreClothesState();
    static void StoreVehicleMods();
    static void RestoreClothesState();
    static void RestoreVehicleMods();
    static void SetCurrentProperty(CMultiBuilding* property);
    static void UpdateStats(size_t index, bool increment);
    static void Load();
    static void Save();

    // NOTSA
    static int32 GetPriceMultipliedByLevel(int32 price, eLevelName level = NUM_LEVELS) {
        level = (level == NUM_LEVELS) ? CTheZones::m_CurrLevel : level;
        return static_cast<int32>(static_cast<float>(price) * gPriceMultipliers[level]);
    }
};

static constexpr auto NUM_COMPONENTS = 20u; // ?

static inline char& gClothesHaveBeenStored = *(char*)0xA97298;
static inline CPedClothesDesc& gStoredClothesState = *(CPedClothesDesc*)0xA9A810;
static inline std::array<eDamageState, NUM_COMPONENTS>& gComponentDamageState = *(std::array<eDamageState, 20u>*)0xA97258;
static inline std::array<int16, CVehicle::NUM_VEHICLE_UPGRADES>& gStoredVehicleMods = *(std::array<int16, 15u>*)0xA97274;
static inline CMultiBuilding* gpCurrentProperty = (CMultiBuilding*)0xA97294;
