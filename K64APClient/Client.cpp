#include <filesystem>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <algorithm>

#include "lib/APCpp/Archipelago.h"
#include "Client.h"
#include <thread>

void glueGetLine(std::ifstream& in, std::string& outString)
{
    char c = in.get();

    while (c != '\r' && c != '\n' && c != '\0' && c != -1)
    {
        outString += c;
        c = in.get();
    }

    c = in.peek();

    while (c == '\r' || c == '\n')
    {
        in.get();
        c = in.peek();
    }
}

AP_State* state;
std::u8string room_seed_name;

int levelRemap[28] = {
    0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 0, 1, 2, 3, 4, 5
};

int stageRemap[28] = {
    0, 1, 2, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 0, 1, 2, 3, 4, 4, 4, 4, 3
};

u32 hasItem(u64 itemId)
{
    u32 count = 0;
    u32 items_size = (u32)AP_GetReceivedItemsSize(state);
    for (u32 i = 0; i < items_size; ++i)
    {
        if (AP_GetReceivedItem(state, i) == itemId)
        {
            count += 1;
        }
    }
    return count;
}

int64_t last_location_sent;

void syncLocation(int64_t location_id)
{
    if (location_id == 0)
    {
        return;
    }

    if (location_id == last_location_sent)
    {
        while (!AP_GetLocationIsChecked(state, location_id));
    }
}

void getStr(uint8_t* rdram, PTR(char) ptr, std::string& outString) {
    char c = MEM_B(0, (gpr)ptr);
    u32 i = 0;
    while (c != 0) {
        outString += c;
        i += 1;
        c = MEM_B(i, (gpr)ptr);
    }
}

void getU8Str(uint8_t* rdram, PTR(char) ptr, std::u8string& outString) {
    char8_t c = MEM_B(0, (gpr)ptr);
    u32 i = 0;
    while (c != 0) {
        outString += c;
        i += 1;
        c = MEM_B(i, (gpr)ptr);
    }
}

void setStr(uint8_t* rdram, PTR(char) ptr, const char* inString) {
    char c = -1;
    u32 i = 0;
    while (c != 0) {
        c = inString[i];
        MEM_B(i, (gpr)ptr) = c;
        i += 1;
    }
}

void setU8Str(uint8_t* rdram, PTR(u8) ptr, const char8_t* inString) {
    char8_t c = -1;
    u32 i = 0;
    while (c != 0) {
        c = inString[i];
        MEM_B(i, (gpr)ptr) = c;
        i += 1;
    }
}

template <typename TP>
std::time_t time_point_to_time_t(TP tp)
{
    // Approximation that uses two now calls to avoid relying on C++20 clock_cast. 
    auto sctp = time_point_cast<std::chrono::system_clock::duration>(tp - TP::clock::now()
        + std::chrono::system_clock::now());
    return std::chrono::system_clock::to_time_t(sctp);
}

std::string format_file_time(std::filesystem::file_time_type time) {
    std::time_t time_c = time_point_to_time_t(time);
    std::tm time_tm;
    bool success;

#if _WIN32
    success = localtime_s(&time_tm, &time_c) == 0;
#else
    success = localtime_r(&time_c, &time_tm) != nullptr;
#endif

    if (!success) {
        return "ERR";
    }

    std::stringstream sstream{};

    sstream << std::put_time(&time_tm, "%b %e %y %I:%M:%S %p");

    return sstream.str();
}

bool rando_init_common() {
    AP_SetDeathLinkSupported(state, true);

    AP_Start(state);

    while (!AP_IsConnected(state))
    {
        if (AP_GetConnectionStatus(state) == AP_ConnectionStatus::ConnectionRefused || AP_GetConnectionStatus(state) == AP_ConnectionStatus::NotFound)
        {
            AP_Stop(state);
            return false;
        }
    }

    AP_QueueLocationScoutsAll(state);
    AP_SendQueuedLocationScouts(state, 0);

    return true;
}

extern "C"
{
    DLLEXPORT u32 recomp_api_version = 1;

    DLLEXPORT void rando_init(uint8_t* rdram, recomp_context* ctx)
    {
        PTR(char) arg = _arg<0, PTR(char)>(rdram, ctx);

        std::string game_name = "";

        getStr(rdram, arg, game_name);

        std::string address;
        std::string playerName;
        std::string password;

        PTR(char) address_ptr = _arg<1, PTR(char)>(rdram, ctx);
        PTR(char) player_name_ptr = _arg<2, PTR(char)>(rdram, ctx);
        PTR(char) password_ptr = _arg<3, PTR(char)>(rdram, ctx);

        getStr(rdram, address_ptr, address);
        getStr(rdram, player_name_ptr, playerName);
        getStr(rdram, password_ptr, password);

        state = AP_New();
        AP_Init(state, address.c_str(), game_name.c_str(), playerName.c_str(), password.c_str());

        std::thread init_thread(rando_init_common);

        init_thread.detach();
    }

    DLLEXPORT void rando_get_slot_data(uint8_t* rdram, recomp_context* ctx) {
        // we set them here because it's easy lmao
        // no reason to do it in mod
        SlotDataValue player_levels = AP_GetSlotDataValue(state, "player_levels");
        for (const auto& index : player_levels.values) {
            int i = stoi(index.first) - 1;
            SlotDataValue level = index.second;
            if (!(level.type == SlotDataType::Array)) {
                continue; // we can't handle this one
            }
            for (const auto& stage : level.values) {
                int k = stoi(stage.first);
                if (stage.second.type != SlotDataType::Int) continue;
                s32 lvl = stage.second._int & 0xFFF;
                if (lvl & 0x200) {
                    lvl = (lvl & 0xFF) + 22;
                }
                else {
                    lvl--;
                }
                MEM_W(0xFFFFFFFF801274C0 + ((k + (i << 3)) << 2), 0) = levelRemap[lvl];
                MEM_W(0xFFFFFFFF80127580 + ((k + (i << 3)) << 2), 0) = stageRemap[lvl];

            }
        }
        SlotDataValue boss_requirements = AP_GetSlotDataValue(state, "boss_requirements");
        for (const auto& kvp : boss_requirements.values) {
            int i = stoi(kvp.first);
            SlotDataValue num = kvp.second;
            if (num.type != SlotDataType::Int) continue;
            MEM_B(0xFFFFFFFF801274A8 + i, 0) = (u8)num._int;
        }

    }

    DLLEXPORT void rando_get_seed_name(uint8_t* rdram, recomp_context* ctx) {
        PTR(char) seed_name_out = _arg<0, PTR(char)>(rdram, ctx);
        u32 seed_name_out_len = _arg<1, u32>(rdram, ctx);

        u32 seed_name_size = static_cast<u32>(room_seed_name.size() + 1);

        AP_RoomInfo roomInfo{};
        AP_GetRoomInfo(state, &roomInfo);
        room_seed_name = std::u8string{ reinterpret_cast<const char8_t*>(roomInfo.seed_name.data()), roomInfo.seed_name.size() };

        if (seed_name_out_len == 0) {
            // Write nothing if the output length is 0.
        }
        else if (room_seed_name.size() + 1 >= seed_name_out_len) {
            setU8Str(rdram, seed_name_out, room_seed_name.substr(0, seed_name_out_len - 1).c_str());
        }
        else {
            setU8Str(rdram, seed_name_out, room_seed_name.c_str());
        }

        _return<u32>(ctx, seed_name_size);
    }

    DLLEXPORT void rando_is_connected(uint8_t* rdram, recomp_context* ctx) {
        _return<bool>(ctx, state != nullptr && AP_IsInit(state) && AP_IsConnected(state));
    }

    DLLEXPORT void rando_connect_failed(uint8_t* rdram, recomp_context* ctx) {
        _return<bool>(ctx, state != nullptr && AP_IsInit(state) && !AP_IsConnected(state) && (AP_GetConnectionStatus(state) == AP_ConnectionStatus::ConnectionRefused || AP_GetConnectionStatus(state) == AP_ConnectionStatus::NotFound));
    }

    DLLEXPORT void rando_get_slotdata_u32(uint8_t* rdram, recomp_context* ctx)
    {
        PTR(char) ptr = _arg<0, PTR(char)>(rdram, ctx);

        std::string key = "";
        getStr(rdram, ptr, key);
        u32 value = (u32)(AP_GetSlotDataInt(state, key.c_str()) & 0xFFFFFFFF);

        _return(ctx, value);
    }

    DLLEXPORT void rando_get_slotdata_string(uint8_t* rdram, recomp_context* ctx)
    {
        PTR(char) ptr = _arg<0, PTR(char)>(rdram, ctx);
        PTR(char) ret_ptr = _arg<1, PTR(char)>(rdram, ctx);

        std::string key = "";
        getStr(rdram, ptr, key);
        const char* value = AP_GetSlotDataString(state, key.c_str());

        setStr(rdram, ret_ptr, value);
    }

    DLLEXPORT void rando_get_datastorage_u32_sync(uint8_t* rdram, recomp_context* ctx)
    {
        PTR(char) ptr = _arg<0, PTR(char)>(rdram, ctx);

        std::string key = "";
        getStr(rdram, ptr, key);
        char* value_char_ptr = AP_GetDataStorageSync(state, key.c_str());

        u32 value = 0;

        if (strncmp(value_char_ptr, "null", 4) != 0)
        {
            value = std::stoi(value_char_ptr);
        }

        _return(ctx, value);
    }

    DLLEXPORT void rando_get_datastorage_string_sync(uint8_t* rdram, recomp_context* ctx)
    {
        PTR(char) ptr = _arg<0, PTR(char)>(rdram, ctx);
        PTR(char) ret_ptr = _arg<1, PTR(char)>(rdram, ctx);

        std::string key = "";
        getStr(rdram, ptr, key);
        char* value = AP_GetDataStorageSync(state, key.c_str());

        setStr(rdram, ret_ptr, value);
    }

    DLLEXPORT void rando_set_datastorage_u32_sync(uint8_t* rdram, recomp_context* ctx)
    {
        PTR(char) ptr = _arg<0, PTR(char)>(rdram, ctx);
        u32 value = _arg<1, u32>(rdram, ctx);
        std::string key = "";
        getStr(rdram, ptr, key);

        try
        {
            AP_SetDataStorageSync(state, key.c_str(), (char*)std::to_string(value).c_str());
        }

        catch (std::exception e)
        {
            fprintf(stderr, "error setting datastorage u32\n");
            fprintf(stderr, e.what());
        }
    }

    DLLEXPORT void rando_set_datastorage_u32_async(uint8_t* rdram, recomp_context* ctx)
    {
        PTR(char) ptr = _arg<0, PTR(char)>(rdram, ctx);
        u32 value = _arg<1, u32>(rdram, ctx);
        std::string key = "";
        getStr(rdram, ptr, key);

        try
        {
            AP_SetDataStorageAsync(state, key.c_str(), (char*)std::to_string(value).c_str());
        }

        catch (std::exception e)
        {
            fprintf(stderr, "error setting datastorage u32\n");
            fprintf(stderr, e.what());
        }
    }

    DLLEXPORT void rando_set_datastorage_string_sync(uint8_t* rdram, recomp_context* ctx)
    {
        PTR(char) ptr = _arg<0, PTR(char)>(rdram, ctx);
        PTR(char) value_ptr = _arg<1, PTR(char)>(rdram, ctx);

        std::string key = "";
        getStr(rdram, ptr, key);

        std::string value = "";
        getStr(rdram, value_ptr, value);


        try
        {
            AP_SetDataStorageSync(state, key.c_str(), (char*)value.c_str());
        }

        catch (std::exception e)
        {
            fprintf(stderr, "error setting datastorage u32\n");
            fprintf(stderr, e.what());
        }
    }

    DLLEXPORT void rando_set_datastorage_string_async(uint8_t* rdram, recomp_context* ctx)
    {
        PTR(char) ptr = _arg<0, PTR(char)>(rdram, ctx);
        PTR(char) value_ptr = _arg<1, PTR(char)>(rdram, ctx);

        std::string key = "";
        getStr(rdram, ptr, key);

        std::string value = "";
        getStr(rdram, value_ptr, value);


        try
        {
            AP_SetDataStorageAsync(state, key.c_str(), (char*)value.c_str());
        }

        catch (std::exception e)
        {
            fprintf(stderr, "error setting datastorage u32\n");
            fprintf(stderr, e.what());
        }
    }

    DLLEXPORT void rando_get_death_link_pending(uint8_t* rdram, recomp_context* ctx)
    {
        _return(ctx, AP_DeathLinkPending(state));
    }

    DLLEXPORT void rando_reset_death_link_pending(uint8_t* rdram, recomp_context* ctx)
    {
        AP_DeathLinkClear(state);
    }

    DLLEXPORT void rando_get_death_link_enabled(uint8_t* rdram, recomp_context* ctx)
    {
        _return(ctx, AP_GetSlotDataInt(state, "death_link") == 1);
    }

    DLLEXPORT void rando_send_death_link(uint8_t* rdram, recomp_context* ctx)
    {
        AP_DeathLinkSend(state);
    }

    DLLEXPORT void rando_get_location_type(uint8_t* rdram, recomp_context* ctx)
    {
        u32 arg = _arg<0, u32>(rdram, ctx);
        int64_t location = arg;
        _return(ctx, (int)AP_GetLocationItemType(state, location));
    }

    DLLEXPORT void rando_get_own_team_id(uint8_t* rdram, recomp_context* ctx)
    {
        _return(ctx, ((u32)AP_GetTeamID(state)));
    }

    DLLEXPORT void rando_get_own_slot_id(uint8_t* rdram, recomp_context* ctx)
    {
        _return(ctx, ((u32)AP_GetPlayerID(state)));
    }

    DLLEXPORT void rando_get_location_item_player(uint8_t* rdram, recomp_context* ctx)
    {
        u32 location_id_arg = _arg<0, u32>(rdram, ctx);
        PTR(char) str_ptr = _arg<1, PTR(char)>(rdram, ctx);

        int64_t location_id = (int64_t)location_id_arg;

        setStr(rdram, str_ptr, AP_GetLocationItemPlayer(state, location_id));
    }

    DLLEXPORT void rando_get_location_item_name(uint8_t* rdram, recomp_context* ctx)
    {
        u32 location_id_arg = _arg<0, u32>(rdram, ctx);
        PTR(char) str_ptr = _arg<1, PTR(char)>(rdram, ctx);

        int64_t location_id = (int64_t)location_id_arg;

        setStr(rdram, str_ptr, AP_GetLocationItemName(state, location_id));
    }

    DLLEXPORT void rando_get_items_size(uint8_t* rdram, recomp_context* ctx)
    {
        _return(ctx, ((u32)AP_GetReceivedItemsSize(state)));
    }

    DLLEXPORT void rando_get_item(uint8_t* rdram, recomp_context* ctx)
    {
        u32 items_i = _arg<0, u32>(rdram, ctx);
        _return(ctx, ((u32)AP_GetReceivedItem(state, items_i)));
    }

    DLLEXPORT void rando_get_sending_player(uint8_t* rdram, recomp_context* ctx)
    {
        u32 items_i = _arg<0, u32>(rdram, ctx);
        _return(ctx, ((u32)AP_GetSendingPlayer(state, items_i) & 0xFFFFFFFF));
    }

    DLLEXPORT void rando_get_item_name_from_id(uint8_t* rdram, recomp_context* ctx)
    {
        u32 arg = _arg<0, u32>(rdram, ctx);
        PTR(char) str_ptr = _arg<1, PTR(char)>(rdram, ctx);

        int64_t item_id = (int64_t)arg;

        setStr(rdram, str_ptr, AP_GetItemNameFromID(state, item_id));
    }

    DLLEXPORT void rando_get_sending_player_name(uint8_t* rdram, recomp_context* ctx)
    {
        u32 items_i = _arg<0, u32>(rdram, ctx);
        PTR(char) str_ptr = _arg<1, PTR(char)>(rdram, ctx);

        int64_t sending_player = AP_GetSendingPlayer(state, items_i);

        setStr(rdram, str_ptr, AP_GetPlayerFromSlot(state, sending_player));
    }

    DLLEXPORT void rando_has_item(uint8_t* rdram, recomp_context* ctx)
    {
        u32 arg = _arg<0, u32>(rdram, ctx);
        int64_t item_id = (int64_t)arg;
        syncLocation(last_location_sent);
        _return(ctx, hasItem(item_id));
    }

    DLLEXPORT void rando_has_item_async(uint8_t* rdram, recomp_context* ctx)
    {
        u32 arg = _arg<0, u32>(rdram, ctx);
        int64_t item_id = (int64_t)arg;
        _return(ctx, hasItem(item_id));
    }

    DLLEXPORT void rando_broadcast_location_hint(uint8_t* rdram, recomp_context* ctx)
    {
        u32 arg = _arg<0, u32>(rdram, ctx);
        int64_t location_id = (int64_t)arg;
        AP_QueueLocationScout(state, location_id);
        AP_SendQueuedLocationScouts(state, 2);
    }

    DLLEXPORT void rando_send_location(uint8_t* rdram, recomp_context* ctx)
    {
        u32 arg = _arg<0, u32>(rdram, ctx);
        int64_t location_id = (int64_t)arg;
        if (AP_LocationExists(state, location_id))
        {
            last_location_sent = location_id;
            if (!AP_GetLocationIsChecked(state, location_id))
            {
                AP_SendItem(state, location_id);
            }
        }
    }

    DLLEXPORT void rando_location_is_checked(uint8_t* rdram, recomp_context* ctx)
    {
        u32 arg = _arg<0, u32>(rdram, ctx);
        int64_t location_id = (int64_t)arg;
        syncLocation(location_id);
        _return(ctx, AP_GetLocationIsChecked(state, location_id));
    }

    DLLEXPORT void rando_location_is_checked_async(uint8_t* rdram, recomp_context* ctx)
    {
        u32 arg = _arg<0, u32>(rdram, ctx);
        int64_t location_id = (int64_t)arg;
        _return(ctx, AP_GetLocationIsChecked(state, location_id));
    }

    DLLEXPORT void rando_get_last_location_sent(uint8_t* rdram, recomp_context* ctx)
    {
        _return(ctx, (u32)(last_location_sent & 0xFFFFFF));
    }

    DLLEXPORT void rando_complete_goal(uint8_t* rdram, recomp_context* ctx)
    {
        AP_StoryComplete(state);
    }
}