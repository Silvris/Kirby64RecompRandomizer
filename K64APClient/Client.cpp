#include "Client.h"
#include "lib/APCpp/Archipelago.h"
#include <format>

const char* GAME_NAME = "Kirby 64 - The Crystal Shards";

static std::vector<int64_t> ItemQueue;
static bool ShouldRevertState = false;

std::string GetStrArg(int arg, uint8_t* rdram, recomp_context* ctx) {
    std::string str;
    PTR(char) str_ptr = _arg<0, PTR(char)>(rdram, ctx);

    getStr(rdram, str_ptr, str);
    return str;

}

void AP_HandleReceivedItems(int64_t item, bool notify) {
    ItemQueue.push_back(item);
}

DLLEXPORT void AP_Update(uint8_t* rdram, recomp_context* ctx) {

}


DLLEXPORT void AP_SetCurrentStage(uint8_t* rdram, recomp_context* ctx) {
    std::string current_stage = GetStrArg(0, rdram, ctx);
    AP_SetServerDataRequest* request = new AP_SetServerDataRequest();
    AP_DataStorageOperation operation = AP_DataStorageOperation();
    operation.operation = "Set";
    operation.value = (void*) current_stage.c_str();
    request->key = std::format("k64_current_level_0_{0}", AP_GetPlayerID());
    request->default_value = (void*) std::string("0_S").c_str();
    request->operations.push_back(operation);

    AP_SetServerData(request);
}

DLLEXPORT void InitializeArchipelago(uint8_t* rdram, recomp_context* ctx) {
    std::string address = GetStrArg(0, rdram, ctx);
    std::string playerName = GetStrArg(1, rdram, ctx);
    std::string password = GetStrArg(2, rdram, ctx);

    AP_Init(address.c_str(), GAME_NAME, playerName.c_str(), password.c_str());

    AP_SetItemRecvCallback(AP_HandleReceivedItems);



}

DLLEXPORT void AP_GetNextItem(uint8_t* rdram, recomp_context* ctx) {
    int64_t item = ItemQueue.back();
    int32_t item32 = item & 0xFF;
    ItemQueue.pop_back();
    _return(ctx, item32);
}