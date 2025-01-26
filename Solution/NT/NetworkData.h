#pragma once
#include <atomic>


// datas_の構造体定義
struct NetworkData {
    std::atomic<bool> isRunning{ false }; // 実行状態
    struct Data {
        int A = 0; // 通信するfloat型の変数
        int B = 0;
    };
    Data data;
};