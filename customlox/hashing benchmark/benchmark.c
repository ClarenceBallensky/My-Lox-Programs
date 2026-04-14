#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "common.h"
#include "table.h"
#include "object.h"
#include "memory.h"
#include "value.h"

// -----------------------------------------------------
// Timing helper
// -----------------------------------------------------

static double nowSeconds() {
    return (double)clock() / CLOCKS_PER_SEC;
}

// -----------------------------------------------------
// Key helper
// -----------------------------------------------------

static ObjString* makeKey(const char* prefix, int i) {
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%s%d", prefix, i);
    return copyString(buffer, (int)strlen(buffer));
}

// -----------------------------------------------------
// 1. Sequential Insert
// -----------------------------------------------------

static void bench_sequential_insert(int n) {
    Table table;
    initTable(&table);

    for (int i = 0; i < n; i++) {
        ObjString* key = makeKey("key", i);
        tableSet(&table, key, NUMBER_VAL(i));
    }

    freeTable(&table);
}

// -----------------------------------------------------
// 2. Random Insert + Lookup
// -----------------------------------------------------

static void bench_random_mix(int n) {
    Table table;
    initTable(&table);

    ObjString** keys = malloc(sizeof(ObjString*) * n);

    for (int i = 0; i < n; i++) {
        char buffer[32];
        snprintf(buffer, sizeof(buffer), "key%d", rand());

        keys[i] = copyString(buffer, (int)strlen(buffer));
        tableSet(&table, keys[i], NUMBER_VAL(i));
    }

    for (int i = 0; i < n; i++) {
        Value out;
        tableGet(&table, keys[rand() % n], &out);
    }

    free(keys);
    freeTable(&table);
}

// -----------------------------------------------------
// 3. Lookup-heavy (FIXED SIGNATURE)
// -----------------------------------------------------

static void bench_lookup_heavy(int n) {
    int queries = n * 10;

    Table table;
    initTable(&table);

    ObjString** keys = malloc(sizeof(ObjString*) * n);

    for (int i = 0; i < n; i++) {
        keys[i] = makeKey("key", i);
        tableSet(&table, keys[i], NUMBER_VAL(i));
    }

    for (int i = 0; i < queries; i++) {
        Value out;
        tableGet(&table, keys[rand() % n], &out);
    }

    free(keys);
    freeTable(&table);
}

// -----------------------------------------------------
// 4. Deletion-heavy
// -----------------------------------------------------

static void bench_delete_heavy(int n) {
    Table table;
    initTable(&table);

    ObjString** keys = malloc(sizeof(ObjString*) * n);

    for (int i = 0; i < n; i++) {
        keys[i] = makeKey("key", i);
        tableSet(&table, keys[i], NUMBER_VAL(i));
    }

    for (int i = 0; i < n; i++) {
        tableDelete(&table, keys[i]);
    }

    free(keys);
    freeTable(&table);
}

// -----------------------------------------------------
// 5. Collision stress (approximation)
// -----------------------------------------------------

static void bench_collision_test(int n) {
    Table table;
    initTable(&table);

    for (int i = 0; i < n; i++) {
        ObjString* key = makeKey("AAAAA", i);
        tableSet(&table, key, NUMBER_VAL(i));
    }

    freeTable(&table);
}

// -----------------------------------------------------
// 6. Resize stress
// -----------------------------------------------------

static void bench_resize_stress(int n) {
    Table table;
    initTable(&table);

    for (int i = 0; i < n; i++) {
        ObjString* key = makeKey("key", i);
        tableSet(&table, key, NUMBER_VAL(i));

        if (i % 100 == 0) {
            Value out;
            tableGet(&table, key, &out);
        }
    }

    freeTable(&table);
}

// -----------------------------------------------------
// Runner
// -----------------------------------------------------

static void run(const char* name, void (*fn)(int), int n) {
    double start = nowSeconds();
    fn(n);
    double end = nowSeconds();

    printf("%-25s: %f sec (n=%d)\n", name, end - start, n);
}

// -----------------------------------------------------
// Main
// -----------------------------------------------------

int main() {
    srand((unsigned)time(NULL));

    int n = 100000;

    printf("=== clox hash table benchmarks ===\n\n");

    run("sequential insert", bench_sequential_insert, n);
    run("random mix", bench_random_mix, n);
    run("lookup heavy", bench_lookup_heavy, n);
    run("delete heavy", bench_delete_heavy, n);
    run("collision stress", bench_collision_test, n);
    run("resize stress", bench_resize_stress, n);

    printf("\nDone.\n");
    return 0;
}