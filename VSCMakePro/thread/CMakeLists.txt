add_executable(sync_compete compete.c)
target_link_libraries(sync_compete pthread)

add_executable(sync_mutex mutex.c)
target_link_libraries(sync_mutex pthread)

add_executable(sync_condition condition.c)
target_link_libraries(sync_condition pthread)

add_executable(sync_rwlock rwlock.c)
target_link_libraries(sync_rwlock pthread)

add_executable(sync_semaphore semaphore.c)
target_link_libraries(sync_semaphore pthread)