static inline int my_spin_lock (atomic_int *lock) {
    int val = 0;                                                                    //val預設為0
    if (likely(atomic_exchange_explicit(lock, 1, memory_order_acq_rel) == 0))       //lock = 1,並回傳lock舊的值, 同時設定memory order
        return 0;                                                                   //若lock == 0,則結束（代表上鎖成功）
    do {
        do {
            asm("pause");                                                           //使用pause, 避免busy waiting
        } while (*lock != 0);                                                       //直到unlock
        val = 0;                                                                    /* 一起判斷val的值是否等於0, 是的話lock = 1上鎖, 沒成功的繼續執行迴圈等待, 同時設定memory order */
    } while (!atomic_compare_exchange_weak_explicit(lock, &val, 1, memory_order_acq_rel, memory_order_relaxed));
    return 0;                                                                       //lock成功回傳
}

static inline int my_spin_unlock(atomic_int *lock) {
    atomic_store_explicit(lock, 0, memory_order_release);                           //lock = 0解鎖, 並release資訊
    return 0;                                                                       //unlock成功回傳
}
