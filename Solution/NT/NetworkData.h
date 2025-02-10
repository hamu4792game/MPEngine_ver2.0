#pragma once
#include <atomic>
#include "Math/MathUtl.h"

// datas_の構造体定義
struct NetworkData {
    std::atomic<bool> isRunning{ false }; // 実行状態
    struct Data {
        int A = 30; // 通信するfloat型の変数
        int B = 30;
        Vector2 posA;
        Vector2 posB;
    };
    Data data;
};