#include <string.h>
#include "../include/share.h"
#include "../include/SVM_code.h"
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
static BasicType lock_params[] = {AMOUNT_TYPE, ASSET_TYPE, HEX_TYPE};
static BasicType verify_params[] = {BOOLEAN_TYPE};

static SVM_Opcode sha3_ops[] = {OP_SHA3};
static SVM_Opcode sha256_ops[] = {OP_SHA256};
static SVM_Opcode abs_ops[] = {OP_ABS};
static SVM_Opcode min_ops[] = {OP_MIN};
static SVM_Opcode max_ops[] = {OP_MAX};
static SVM_Opcode check_tx_sig_ops[] = {OP_TXSIGHASH, OP_SWAP, OP_CHECKSIG};
static SVM_Opcode check_msg_sig_ops[] = {OP_CHECKSIG};
static SVM_Opcode below_ops[] = {OP_BLOCKHEIGHT, OP_GREATERTHAN};
static SVM_Opcode above_ops[] = {OP_BLOCKHEIGHT, OP_LESSTHAN};
static SVM_Opcode lock_ops[] = {OP_CHECKOUTPUT, OP_VERIFY};
static SVM_Opcode verify_ops[] = {OP_VERIFY};

BuiltinFun builtin_func_list[] = {
    {"sha3", sha3_params, ARRAY_SIZE(sha3_params), HASH_TYPE, sha3_ops, ARRAY_SIZE(sha3_ops)},
    {"sha256", sha256_params, ARRAY_SIZE(sha256_params), HASH_TYPE, sha256_ops, ARRAY_SIZE(sha256_ops)},
    {"abs", abs_params, ARRAY_SIZE(abs_params), INT_TYPE, abs_ops, ARRAY_SIZE(abs_ops)},
    {"min", min_params, ARRAY_SIZE(min_params), INT_TYPE, min_ops, ARRAY_SIZE(min_ops)},
    {"max", max_params, ARRAY_SIZE(max_params), INT_TYPE, max_ops, ARRAY_SIZE(max_ops)},
    {"check_tx_sig", check_tx_sig_params, ARRAY_SIZE(check_tx_sig_params), BOOLEAN_TYPE, check_tx_sig_ops, ARRAY_SIZE(check_tx_sig_ops)},
    {"check_msg_sig", check_msg_sig_params, ARRAY_SIZE(check_msg_sig_params), BOOLEAN_TYPE, check_msg_sig_ops, ARRAY_SIZE(check_msg_sig_ops)},
    {"below", below_params, ARRAY_SIZE(below_params), BOOLEAN_TYPE, below_ops, ARRAY_SIZE(below_ops)},
    {"above", above_params, ARRAY_SIZE(above_params), BOOLEAN_TYPE, above_ops, ARRAY_SIZE(above_ops)},
    {"lock", lock_params, ARRAY_SIZE(lock_params), VOID_TYPE, lock_ops, ARRAY_SIZE(lock_ops)},
    {"verify", verify_params, ARRAY_SIZE(verify_params), VOID_TYPE, verify_ops, ARRAY_SIZE(verify_ops)},
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
