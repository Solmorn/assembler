#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "stack.h"

#ifdef _DEBUG
static void FillCanary(StackInfo* stk, stack_type* calloc_ptr, size_t capacity);
static void FillPoison(StackInfo* stk);
#endif

static stack_type* AllocateStack(StackInfo* stk, size_t capacity_got) {

    #ifdef CANARY_ON
    capacity_got += 2;
    stack_type* realloc_ptr = (stack_type*)realloc(stk->data_canary_p.data_canary1, capacity_got * sizeof(stack_type));
    #else
    stack_type* realloc_ptr = (stack_type*)realloc(stk->data                      , capacity_got * sizeof(stack_type));
    #endif //canary

    if (realloc_ptr == nullptr) return nullptr;

    stk->capacity = capacity_got;

    #ifdef CANARY_ON
    stk->data = realloc_ptr + 1;
    FillCanary(stk, realloc_ptr, capacity_got);
    #else
    stk->data = realloc_ptr;
    #endif //canary

    #ifdef _DEBUG
    FillPoison(stk);
    #endif

    return realloc_ptr;

}

error_code StackCtor(StackInfo* stk, size_t capacity_got, BirthInfo* info_got = nullptr) {

    assert(stk);

    if (capacity_got > MAX_SIZE_VALUE) return CapacityError;
    if (capacity_got < MIN_SIZE_VALUE) capacity_got = MIN_SIZE_VALUE;

    stk->size = 0;

    #ifdef _DEBUG
    stk->info = info_got;
    #endif //debug

    stack_type* calloc_ptr = AllocateStack(stk, capacity_got);
    if (calloc_ptr == nullptr) return AllocationError;

    #ifdef HASH_ON
    stk->hash_value = CalculateDataHash(stk);
    #endif

    ASSERT_OK(stk);

    return Ok;

}

error_code StackPush(StackInfo* stk, stack_type element) {

    ASSERT_OK(stk);

    if (stk->size == stk->capacity) {
        stack_type* realloc_ptr = AllocateStack(stk, stk->capacity * STACK_EXPAND_VALUE);
        if (realloc_ptr == nullptr) return AllocationError;
    }

    stk->data[(stk->size)++] = element;

    #ifdef HASH_ON
    stk->hash_value = CalculateDataHash(stk);
    #endif


    ASSERT_OK(stk);

    return Ok;

}

error_code StackPop(StackInfo* stk, stack_type* element) {

    ASSERT_OK(stk);

    if (stk->size == 0) return PopSizeStackError;

    *element = stk->data[(stk->size) - 1];
    stk->data[--(stk->size)] = POISON;

    #ifdef HASH_ON
    stk->hash_value = CalculateDataHash(stk);
    #endif

    ASSERT_OK(stk);

    return Ok;

}


error_code StkDtor(StackInfo* stk) {

    ASSERT_OK(stk);

    #ifdef CANARY_ON
    free((stk->data_canary_p).data_canary1);
    #else
    free(stk->data);
    #endif //canary

    return Ok;

}

#ifdef _DEBUG //for debug funcs

#ifdef CANARY_ON
static void FillCanary(StackInfo* stk, stack_type* calloc_ptr, size_t capacity) {

    (stk->data_canary_p).data_canary1 = calloc_ptr;
    (stk->data_canary_p).data_canary2 = calloc_ptr + capacity + 1;

    *((stk->data_canary_p).data_canary1) = DATA_CANARY;
    *((stk->data_canary_p).data_canary2) = DATA_CANARY;

}
#endif //canary
error_code StkErr(StackInfo* stk) {

    assert(stk);

    error_code code = 0;

    #ifdef CANARY_ON
    if (stk->stack_canary1 != STACK_CANARY || stk->stack_canary2 != STACK_CANARY) {
                                code |= StackCanaryError;
        stk->errors_bit = code;
        return code;
    }
    #endif

    bool check_data_allowed   = true;
    bool check_poison_allowed = true;

    if (stk->size     > MAX_SIZE_VALUE) {
                                code |= SizeError;
        check_data_allowed   = false;
    }

    if (stk->capacity > MAX_SIZE_VALUE) {
                                code |= CapacityError;
        check_poison_allowed = false;
    }

    if (stk->capacity < stk->size) {
                                code |= SizeMoreThanCapacityError;
        check_poison_allowed = false;
    }

    if (stk->data == nullptr) {
                                code |= NullptrDataError;
        stk->errors_bit = code;
        return code;
    }

    for (size_t index = 0; check_data_allowed && index < stk->size; index++) {
        if ((stk->data)[index] == POISON) {
                                code |= PoisonDataError;
            break;
        }
    }

    for (size_t index = stk->size; check_poison_allowed && index < stk->capacity; index++) {
        if ((stk->data)[index] != POISON) {
                                code |= PoisonFillingError;
            break;
        }
    }

    #ifdef CANARY_ON
    if (*((stk->data_canary_p).data_canary1) != DATA_CANARY || *((stk->data_canary_p).data_canary2) != DATA_CANARY) {
                                code |= DataCanaryError;
    }
    #endif

    #ifdef HASH_ON
    if (check_data_allowed && (stk->hash_value != CalculateDataHash(stk))) {
                                code |= HashError;
    }
    #endif

    stk->errors_bit = code;

    return code;
}

void StkDump(StackInfo* stk) {

    assert(stk);

    BirthInfo* info_got = stk->info;
    printf("=====INIT_INFO=====\nFILE: %s /-----/ FUCK: %s /-----/ LINE: %d /-----/ NAME: %s\n\n",
                                info_got->file, info_got->func, info_got->line, info_got->name);

    printf("ERROR_CODE: %d\n", stk->errors_bit);
    printf("StkDump(%s[%p]) {\n", info_got->name, &stk);

    #ifdef CANARY_ON
    printf("    stack_canary1 = %x\t%s\n", stk->stack_canary1, ContainsError(stk->errors_bit, StackCanaryError)       ? "(BAD!)" : "");
    #endif //canary

    printf("    size          = %lu\t%s\n", stk->size,      ContainsError(stk->errors_bit, SizeError) ||
                                                            ContainsError(stk->errors_bit, SizeMoreThanCapacityError)  ? "(BAD!)" : "");
    printf("    capacity      = %lu\t%s\n", stk->capacity,  ContainsError(stk->errors_bit, CapacityError)              ? "(BAD!)" : "");
    printf("    poison        = %d\n", POISON);

    #ifdef CANARY_ON
    printf("    data_canary   = %x\n", DATA_CANARY);
    printf("    stack_canary  = %x\n", STACK_CANARY);
    #endif //canary

    #ifdef HASH_ON
    printf("    hash     = %llu\t%s\n", stk->hash_value,ContainsError(stk->errors_bit, HashError)                 ? "(BAD!)" : "");
    #endif
    printf("    data[%p]\t%s", stk->data,              ContainsError(stk->errors_bit, NullptrDataError) ||
                                                       ContainsError(stk->errors_bit, PoisonDataError)  ||
                                                       ContainsError(stk->errors_bit, PoisonFillingError)         ? "(BAD!)" : "");
    if (!(ContainsError(stk->errors_bit, NullptrDataError) || ContainsError(stk->errors_bit, CapacityError))) {

        #ifdef CANARY_ON
        printf(" {\n         [canary] = %x\t%s\n", *((stk->data_canary_p).data_canary1),
                                                       ContainsError(stk->errors_bit, DataCanaryError)            ? "(BAD!)" : "");
        #endif

        for (size_t index = 0; index < stk->capacity; index++) {
            const char* is_poison = "";
            const char* is_filled = "*";
            if (index >= stk->size) is_filled = " ";
            stack_type element = (stk->data)[index];
            if (element == POISON) is_poison = "(poison)";
             printf("        %s [%lu] = %d %s\n", is_filled, index, element, is_poison);
        }
        #ifdef CANARY_ON
        printf("         [canary] = %x\t%s", *((stk->data_canary_p).data_canary2),
                                                      ContainsError(stk->errors_bit, DataCanaryError)             ? "(BAD!)" : "");
        #endif
        printf("\n    }");
    }

    #ifdef CANARY_ON
    printf("\n    stack_canary2 = %x\t%s\n", stk->stack_canary2, ContainsError(stk->errors_bit, StackCanaryError)     ? "(BAD!)" : "");
    #endif
    printf("\n}\n\n");
}

static void FillPoison(StackInfo* stk) {

    for (size_t index = stk->size; index < stk->capacity; index++) {
        (stk->data)[index] = POISON;
    }

}

#ifdef HASH_ON
uint64_t CalculateDataHash(StackInfo* stk) {

    assert(stk);

    uint64_t hash = 5381;

    for (size_t index = 0; index < stk->size; index++) {
        hash = (hash << 5) + hash + (stk->data)[index];
    }

    return hash;
}
#endif


bool ContainsError(error_code code, Stack_Err_t err) {
    return err & code;
}
#endif // for debug func
