/** @file chali_c_api_contract.c @brief 独立 C 宿主的 ABI 协议正反例。 */
#include "abi/chali_abi.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

/**
 * @brief 在两个整数槽上执行 C 宿主加法。
 * @param frame 经调用方核对的参数帧。
 */
static void CHALI_ABI_CALL add(ChaliAbiFrame *frame)
{
    frame->result = frame->slots[0] + frame->slots[1];
    frame->resultIsFloat = 0;
}

/**
 * @brief 运行 C/C++ 目标布局互操作及协议拒绝测试。
 * @return 全部断言通过时为零。
 */
int main(void)
{
    ChaliAbiInfo local = chali_abi_local_info(CHALI_ABI_CAP_SYNC_CALLBACK);
    ChaliAbiInfo changed;
    ChaliAbiRecord records[2] = { { "host::add", add, 2 }, { "host::other", add, 2 } };
    ChaliAbiModuleDescriptor module = { sizeof(ChaliAbiModuleDescriptor), &local, records, 2 };
    char text[128];
    ChaliAbiError error = { text, sizeof(text), 0, 0 };
    int64_t slots[2] = { 20, 22 };
    unsigned char kinds[2] = { ChaliAbiSlotInteger, ChaliAbiSlotInteger };
    ChaliAbiFrame frame = { NULL, slots, kinds, 2, 0, 0 };
    size_t at;
    assert(chali_abi_check(&local, &error) == ChaliAbiOk);
    assert(error.status == 0 && error.required == 0 && text[0] == '\0');
    assert(chali_abi_check_module(&module, &error) == ChaliAbiOk);
    assert(chali_abi_check_frame(&frame, records[0].arity, &error) == ChaliAbiOk);
    records[0].entry(&frame);
    assert(frame.result == 42 && chali_abi_check_result(&frame, &error) == ChaliAbiOk);
    assert(chali_abi_check(NULL, NULL) == ChaliAbiInvalidArgument);
    changed = local;
    changed.structSize--;
    assert(chali_abi_check(&changed, &error) == ChaliAbiIncompatible);
    changed = local;
    changed.version++;
    assert(chali_abi_check(&changed, &error) == ChaliAbiIncompatible);
    changed = local;
    changed.architecture = local.architecture == 2 ? 4 : 2;
    assert(chali_abi_check(&changed, &error) == ChaliAbiIncompatible);
    changed = local;
    changed.platform = local.platform == 1 ? 2 : 1;
    assert(chali_abi_check(&changed, &error) == ChaliAbiIncompatible);
    changed = local;
    changed.pointerBits /= 2;
    assert(chali_abi_check(&changed, &error) == ChaliAbiIncompatible);
    changed = local;
    changed.littleEndian ^= 1;
    assert(chali_abi_check(&changed, &error) == ChaliAbiIncompatible);
    changed = local;
    changed.callingConvention++;
    assert(chali_abi_check(&changed, &error) == ChaliAbiIncompatible);
    changed = local;
    changed.frameAlignment *= 2;
    assert(chali_abi_check(&changed, &error) == ChaliAbiIncompatible);
    changed = local;
    changed.recordSize++;
    assert(chali_abi_check(&changed, &error) == ChaliAbiIncompatible);
    for (at = 0; at < 6; ++at)
    {
        changed = local;
        changed.frameOffsets[at]++;
        assert(chali_abi_check(&changed, NULL) == ChaliAbiIncompatible);
    }
    for (at = 0; at < 3; ++at)
    {
        changed = local;
        changed.recordOffsets[at]++;
        assert(chali_abi_check(&changed, NULL) == ChaliAbiIncompatible);
    }
    changed = local;
    changed.capabilities = UINT64_C(1) << 63;
    assert(chali_abi_check(&changed, &error) == ChaliAbiIncompatible);
    assert(strstr(text, "capability") != NULL);
    {
        char bounded[5] = { 'x', 'x', 'x', 'x', '!' };
        ChaliAbiError small = { bounded, 4, 0, 0 };
        assert(chali_abi_check(NULL, &small) == ChaliAbiInvalidArgument);
        assert(small.required == strlen("missing ABI descriptor") + 1);
        assert(bounded[3] == '\0' && bounded[4] == '!');
        small.data = NULL;
        small.capacity = 0;
        assert(chali_abi_check(NULL, &small) == ChaliAbiInvalidArgument);
        assert(chali_abi_check(&local, &small) == 0 && small.required == 0);
    }
    records[1].name = records[0].name;
    assert(chali_abi_check_module(&module, &error) == ChaliAbiDuplicate);
    records[1].name = "";
    assert(chali_abi_check_module(&module, &error) == ChaliAbiInvalidArgument);
    records[1].name = "host::other";
    records[1].entry = NULL;
    assert(chali_abi_check_module(&module, &error) == ChaliAbiInvalidArgument);
    records[1].entry = add;
    records[1].arity = -2;
    assert(chali_abi_check_module(&module, &error) == ChaliAbiInvalidArgument);
    module.records = NULL;
    assert(chali_abi_check_module(&module, &error) == ChaliAbiInvalidArgument);
    module.count = 0;
    assert(chali_abi_check_module(&module, &error) == ChaliAbiOk);
    assert(chali_abi_check_module(NULL, &error) == ChaliAbiInvalidArgument);
    module.abi = NULL;
    assert(chali_abi_check_module(&module, NULL) == ChaliAbiInvalidArgument);
    assert(chali_abi_check_frame(NULL, 0, &error) == ChaliAbiInvalidArgument);
    assert(chali_abi_check_frame(&frame, 1, &error) == ChaliAbiInvalidArgument);
    frame.count = -1;
    assert(chali_abi_check_frame(&frame, -1, &error) == ChaliAbiInvalidArgument);
    frame.count = INT64_MAX;
    assert(chali_abi_check_frame(&frame, -1, &error) == ChaliAbiInvalidArgument);
    frame.count = 2;
    frame.slots = NULL;
    assert(chali_abi_check_frame(&frame, 2, &error) == ChaliAbiInvalidArgument);
    frame.slots = slots;
    frame.kinds = NULL;
    assert(chali_abi_check_frame(&frame, 2, &error) == ChaliAbiInvalidArgument);
    frame.kinds = kinds;
    kinds[1] = 2;
    assert(chali_abi_check_frame(&frame, 2, &error) == ChaliAbiInvalidArgument);
    kinds[1] = ChaliAbiSlotFloat;
    assert(chali_abi_check_frame(&frame, -1, &error) == ChaliAbiOk);
    frame.resultIsFloat = 2;
    assert(chali_abi_check_result(&frame, &error) == ChaliAbiInvalidArgument);
    frame.resultIsFloat = 1;
    assert(chali_abi_check_result(&frame, &error) == ChaliAbiOk);
    assert(chali_abi_check_result(NULL, &error) == ChaliAbiInvalidArgument);
    frame.count = 0;
    frame.slots = NULL;
    frame.kinds = NULL;
    assert(chali_abi_check_frame(&frame, 0, &error) == ChaliAbiOk);
    puts("C ABI target, module, frame and bounded error contracts passed");
    return 0;
}
