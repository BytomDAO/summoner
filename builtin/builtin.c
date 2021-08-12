#include <string.h>
#include "../include/share.h"
#include "../compiler/summoner.h"

static BasicType sha3_params[] = {HEX_TYPE};
static BasicType sha256_params[] = {HEX_TYPE};
static BasicType abs_params[] = {INT_TYPE};
static BasicType min_params[] = {INT_TYPE, INT_TYPE};
static BasicType max_params[] = {INT_TYPE, INT_TYPE};
static BasicType check_tx_sig_params[] = {PUBKEY_TYPE, SIG_TYPE};
static BasicType check_msg_sig_params[] = {PUBKEY_TYPE, HASH_TYPE, SIG_TYPE};
static BasicType below_params[] = {INT_TYPE, BOOLEAN_TYPE};
static BasicType above_params[] = {INT_TYPE, BOOLEAN_TYPE};
static BasicType lock_params[] = {AMOUNT_TYPE, ASSET_TYPE, HEX_TYPE, HEX_TYPE};
static BasicType verify_params[] = {BOOLEAN_TYPE};

BuiltinFun builtin_func_list[] = {
    {"sha3", sha3_params, ARRAY_SIZE(sha3_params), HASH_TYPE, "SHA3"},
    {"sha256", sha256_params, ARRAY_SIZE(sha256_params), HASH_TYPE, "SHA256"},
    {"abs", abs_params, ARRAY_SIZE(abs_params), INT_TYPE, "ABS"},
    {"min", min_params, ARRAY_SIZE(min_params), INT_TYPE, "MIN"},
    {"max", max_params, ARRAY_SIZE(max_params), INT_TYPE, "MAX"},
    {"check_tx_sig", check_tx_sig_params, ARRAY_SIZE(check_tx_sig_params), BOOLEAN_TYPE, "TXSIGHASH SWAP CHECKSIG"},
    {"check_msg_sig", check_msg_sig_params, ARRAY_SIZE(check_msg_sig_params), BOOLEAN_TYPE, "CHECKSIG"},
    {"below", below_params, ARRAY_SIZE(below_params), BOOLEAN_TYPE, "BLOCKHEIGHT GREATERTHAN"},
    {"above", above_params, ARRAY_SIZE(above_params), BOOLEAN_TYPE, "BLOCKHEIGHT LESSTHAN"},
    {"lock", lock_params, ARRAY_SIZE(lock_params), VOID_TYPE, ""},
    {"verify", verify_params, ARRAY_SIZE(verify_params), VOID_TYPE, "VERIFY"},
};

BuiltinFun *search_builtin_function(char *name)
{
    for (int i = 0; i < ARRAY_SIZE(builtin_func_list); i++)
    {
        if (!strcmp(name, builtin_func_list[i].name))
        {
            return &builtin_func_list[i];
        }
    }
    return NULL;
}
