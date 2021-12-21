int __pthread_spin_lock (pthread_spinlock_t *lock) {
    int val = 0;                                                    //val預設為0
    if (__glibc_likely (atomic_exchange_acquire (lock, 1) == 0))    //lock = 1,並回傳lock舊的值
        return 0;                                                   //若lock == 0,則結束（代表上鎖成功）
    do {                                                            //上鎖不成功則會進入do_while迴圈
        do {
            /* TODO Back-off.  */                                   //隨機等待一段時間, 避免有人同時執行成功
            atomic_spin_nop ();                                     //類似於pause,避免busy waiting
            val = atomic_load_relaxed (lock);                       //讀取lock的值到val
        } while (val != 0);                                         //如果val == 0, 代表目前為unlock, 用輕量的判斷先檢查, 避免負擔太重
    } while (!atomic_compare_exchange_weak_acquire (lock, &val, 1));//一起判斷val的值是否等於0, 是的話lock = 1上鎖, 沒成功的繼續執行迴圈等待
    return 0;                                                       //lock成功回傳
}

int __pthread_spin_unlock (pthread_spinlock_t *lock) {
    atomic_store_release (lock, 0);                                 //lock = 0解鎖, 並release資訊
        return 0;                                                   //unlock成功回傳
}
