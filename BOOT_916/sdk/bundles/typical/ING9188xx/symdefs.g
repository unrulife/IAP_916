att_dispatch_client_can_send_now = 0x00005875;
att_dispatch_client_request_can_send_now_event = 0x0000587b;
att_dispatch_register_client = 0x00005881;
att_dispatch_register_server = 0x00005895;
att_dispatch_server_can_send_now = 0x000058a9;
att_dispatch_server_request_can_send_now_event = 0x000058af;
att_emit_general_event = 0x00005961;
att_server_can_send_packet_now = 0x00006091;
att_server_deferred_read_response = 0x00006095;
att_server_get_mtu = 0x000060ad;
att_server_indicate = 0x00006125;
att_server_init = 0x000061a9;
att_server_notify = 0x000061e5;
att_server_register_packet_handler = 0x000062fd;
att_server_request_can_send_now_event = 0x00006309;
att_set_db = 0x00006325;
att_set_read_callback = 0x00006339;
att_set_write_callback = 0x00006345;
bd_addr_cmp = 0x000064b5;
bd_addr_copy = 0x000064bb;
bd_addr_to_str = 0x000064c5;
big_endian_read_16 = 0x000064fd;
big_endian_read_32 = 0x00006505;
big_endian_store_16 = 0x00006519;
big_endian_store_32 = 0x00006525;
btstack_config = 0x00006679;
btstack_memory_pool_create = 0x000067b7;
btstack_memory_pool_free = 0x000067e1;
btstack_memory_pool_get = 0x00006841;
btstack_push_user_msg = 0x000068a9;
btstack_push_user_runnable = 0x000068b5;
char_for_nibble = 0x00006b3d;
eTaskConfirmSleepModeStatus = 0x00006e11;
gap_add_dev_to_periodic_list = 0x00007461;
gap_add_whitelist = 0x00007471;
gap_aes_encrypt = 0x0000747d;
gap_clear_white_lists = 0x000074b5;
gap_clr_adv_set = 0x000074c1;
gap_clr_periodic_adv_list = 0x000074cd;
gap_create_connection_cancel = 0x000074d9;
gap_default_periodic_adv_sync_transfer_param = 0x000074e5;
gap_disconnect = 0x000074fd;
gap_disconnect_all = 0x00007529;
gap_ext_create_connection = 0x00007569;
gap_get_connection_parameter_range = 0x00007649;
gap_le_read_channel_map = 0x00007685;
gap_periodic_adv_create_sync = 0x000076f1;
gap_periodic_adv_create_sync_cancel = 0x00007715;
gap_periodic_adv_set_info_transfer = 0x00007721;
gap_periodic_adv_sync_transfer = 0x00007731;
gap_periodic_adv_sync_transfer_param = 0x00007741;
gap_periodic_adv_term_sync = 0x0000775d;
gap_read_antenna_info = 0x000077e5;
gap_read_periodic_adv_list_size = 0x000077f1;
gap_read_phy = 0x000077fd;
gap_read_remote_used_features = 0x00007809;
gap_read_remote_version = 0x00007815;
gap_read_rssi = 0x00007821;
gap_remove_whitelist = 0x0000782d;
gap_rmv_adv_set = 0x000078a9;
gap_rmv_dev_from_periodic_list = 0x000078b5;
gap_rx_test_v2 = 0x000078c5;
gap_rx_test_v3 = 0x000078d5;
gap_set_adv_set_random_addr = 0x00007921;
gap_set_callback_for_next_hci = 0x0000795d;
gap_set_connection_cte_request_enable = 0x0000797d;
gap_set_connection_cte_response_enable = 0x00007999;
gap_set_connection_cte_rx_param = 0x000079a9;
gap_set_connection_cte_tx_param = 0x00007a05;
gap_set_connection_parameter_range = 0x00007a59;
gap_set_connectionless_cte_tx_enable = 0x00007a71;
gap_set_connectionless_cte_tx_param = 0x00007a81;
gap_set_connectionless_iq_sampling_enable = 0x00007ae1;
gap_set_data_length = 0x00007b45;
gap_set_def_phy = 0x00007b5d;
gap_set_ext_adv_data = 0x00007b6d;
gap_set_ext_adv_enable = 0x00007b85;
gap_set_ext_adv_para = 0x00007c01;
gap_set_ext_scan_enable = 0x00007cd9;
gap_set_ext_scan_para = 0x00007cf1;
gap_set_ext_scan_response_data = 0x00007d99;
gap_set_host_channel_classification = 0x00007db1;
gap_set_periodic_adv_data = 0x00007dc1;
gap_set_periodic_adv_enable = 0x00007e31;
gap_set_periodic_adv_para = 0x00007e41;
gap_set_periodic_adv_rx_enable = 0x00007e59;
gap_set_phy = 0x00007e69;
gap_set_random_device_address = 0x00007e85;
gap_start_ccm = 0x00007eb5;
gap_test_end = 0x00007efd;
gap_tx_test_v2 = 0x00007f09;
gap_tx_test_v4 = 0x00007f21;
gap_update_connection_parameters = 0x00007f45;
gap_vendor_tx_continuous_wave = 0x00007f89;
gatt_client_cancel_write = 0x000084b1;
gatt_client_discover_characteristic_descriptors = 0x000084d7;
gatt_client_discover_characteristics_for_handle_range_by_uuid128 = 0x00008517;
gatt_client_discover_characteristics_for_handle_range_by_uuid16 = 0x00008567;
gatt_client_discover_characteristics_for_service = 0x000085b7;
gatt_client_discover_primary_services = 0x000085ed;
gatt_client_discover_primary_services_by_uuid128 = 0x0000861f;
gatt_client_discover_primary_services_by_uuid16 = 0x00008663;
gatt_client_execute_write = 0x0000869f;
gatt_client_find_included_services_for_service = 0x000086c5;
gatt_client_get_mtu = 0x000086f3;
gatt_client_is_ready = 0x00008795;
gatt_client_listen_for_characteristic_value_updates = 0x000087ab;
gatt_client_prepare_write = 0x000087cd;
gatt_client_read_characteristic_descriptor_using_descriptor_handle = 0x00008809;
gatt_client_read_long_characteristic_descriptor_using_descriptor_handle = 0x00008833;
gatt_client_read_long_characteristic_descriptor_using_descriptor_handle_with_offset = 0x00008839;
gatt_client_read_long_value_of_characteristic_using_value_handle = 0x00008867;
gatt_client_read_long_value_of_characteristic_using_value_handle_with_offset = 0x0000886d;
gatt_client_read_multiple_characteristic_values = 0x0000889b;
gatt_client_read_value_of_characteristic_using_value_handle = 0x000088cb;
gatt_client_read_value_of_characteristics_by_uuid128 = 0x000088f9;
gatt_client_read_value_of_characteristics_by_uuid16 = 0x00008945;
gatt_client_register_handler = 0x00008991;
gatt_client_reliable_write_long_value_of_characteristic = 0x0000899d;
gatt_client_signed_write_without_response = 0x00008dcd;
gatt_client_write_characteristic_descriptor_using_descriptor_handle = 0x00008e91;
gatt_client_write_client_characteristic_configuration = 0x00008ecb;
gatt_client_write_long_characteristic_descriptor_using_descriptor_handle = 0x00008f1d;
gatt_client_write_long_characteristic_descriptor_using_descriptor_handle_with_offset = 0x00008f2d;
gatt_client_write_long_value_of_characteristic = 0x00008f69;
gatt_client_write_long_value_of_characteristic_with_offset = 0x00008f79;
gatt_client_write_value_of_characteristic = 0x00008fb5;
gatt_client_write_value_of_characteristic_without_response = 0x00008feb;
hci_add_event_handler = 0x0000a52d;
hci_power_control = 0x0000acad;
hci_register_acl_packet_handler = 0x0000ae61;
kv_commit = 0x0000b5b9;
kv_get = 0x0000b615;
kv_init = 0x0000b621;
kv_init_backend = 0x0000b6a1;
kv_put = 0x0000b6b5;
kv_remove = 0x0000b6c1;
kv_remove_all = 0x0000b6f5;
kv_value_modified = 0x0000b725;
kv_value_modified_of_key = 0x0000b741;
kv_visit = 0x0000b74d;
l2cap_add_event_handler = 0x0000b7dd;
l2cap_can_send_packet_now = 0x0000b7ed;
l2cap_create_le_credit_based_connection_request = 0x0000b9a9;
l2cap_credit_based_send = 0x0000baed;
l2cap_credit_based_send_continue = 0x0000bb19;
l2cap_disconnect = 0x0000bb95;
l2cap_get_le_credit_based_connection_credits = 0x0000bde5;
l2cap_get_peer_mtu_for_local_cid = 0x0000be01;
l2cap_init = 0x0000c1d5;
l2cap_le_send_flow_control_credit = 0x0000c2cb;
l2cap_max_le_mtu = 0x0000c5d5;
l2cap_register_packet_handler = 0x0000c6fd;
l2cap_register_service = 0x0000c709;
l2cap_request_can_send_now_event = 0x0000c819;
l2cap_request_connection_parameter_update = 0x0000c833;
l2cap_send_echo_request = 0x0000cd05;
l2cap_unregister_service = 0x0000cdc5;
le_device_db_add = 0x0000ce1d;
le_device_db_find = 0x0000cef5;
le_device_db_from_key = 0x0000cf21;
le_device_db_iter_cur = 0x0000cf29;
le_device_db_iter_cur_key = 0x0000cf2d;
le_device_db_iter_init = 0x0000cf31;
le_device_db_iter_next = 0x0000cf39;
le_device_db_remove_key = 0x0000cf5f;
ll_aes_encrypt = 0x0000cf8d;
ll_config = 0x0000d009;
ll_free = 0x0000d033;
ll_get_heap_free_size = 0x0000d03d;
ll_hint_on_ce_len = 0x0000d051;
ll_legacy_adv_set_interval = 0x0000d089;
ll_malloc = 0x0000d099;
ll_query_timing_info = 0x0000d1d1;
ll_register_hci_acl_previewer = 0x0000d21d;
ll_scan_set_fixed_channel = 0x0000d281;
ll_set_adv_access_address = 0x0000d499;
ll_set_adv_coded_scheme = 0x0000d4a5;
ll_set_conn_acl_report_latency = 0x0000d4d5;
ll_set_conn_coded_scheme = 0x0000d505;
ll_set_conn_latency = 0x0000d531;
ll_set_conn_tx_power = 0x0000d561;
ll_set_def_antenna = 0x0000d5a9;
ll_set_initiating_coded_scheme = 0x0000d5c5;
ll_set_max_conn_number = 0x0000d5d1;
nibble_for_char = 0x0001e635;
platform_32k_rc_auto_tune = 0x0001e6e1;
platform_32k_rc_tune = 0x0001e75d;
platform_calibrate_32k = 0x0001e771;
platform_config = 0x0001e775;
platform_delete_timer = 0x0001e899;
platform_enable_irq = 0x0001e8a1;
platform_get_current_task = 0x0001e8d9;
platform_get_gen_os_driver = 0x0001e8fd;
platform_get_heap_status = 0x0001e905;
platform_get_task_handle = 0x0001e91d;
platform_get_timer_counter = 0x0001e93d;
platform_get_us_time = 0x0001e941;
platform_get_version = 0x0001e945;
platform_hrng = 0x0001e94d;
platform_install_isr_stack = 0x0001e955;
platform_install_task_stack = 0x0001e961;
platform_patch_rf_init_data = 0x0001e999;
platform_printf = 0x0001e9a5;
platform_raise_assertion = 0x0001e9b9;
platform_rand = 0x0001e9cd;
platform_read_info = 0x0001e9d1;
platform_read_persistent_reg = 0x0001ea01;
platform_reset = 0x0001ea11;
platform_set_abs_timer = 0x0001ea35;
platform_set_evt_callback = 0x0001ea39;
platform_set_evt_callback_table = 0x0001ea4d;
platform_set_irq_callback = 0x0001ea59;
platform_set_irq_callback_table = 0x0001ea75;
platform_set_rf_clk_source = 0x0001ea81;
platform_set_rf_init_data = 0x0001ea8d;
platform_set_rf_power_mapping = 0x0001ea99;
platform_set_timer = 0x0001eaa5;
platform_shutdown = 0x0001eaa9;
platform_switch_app = 0x0001eaad;
platform_trace_raw = 0x0001ead9;
platform_write_persistent_reg = 0x0001eaf1;
printf_hexdump = 0x0001eca5;
pvPortMalloc = 0x0001f799;
pvTaskIncrementMutexHeldCount = 0x0001f881;
pvTimerGetTimerID = 0x0001f899;
pxPortInitialiseStack = 0x0001f8c5;
reverse_128 = 0x0001faa5;
reverse_24 = 0x0001faab;
reverse_256 = 0x0001fab1;
reverse_48 = 0x0001fab7;
reverse_56 = 0x0001fabd;
reverse_64 = 0x0001fac3;
reverse_bd_addr = 0x0001fac9;
reverse_bytes = 0x0001facf;
sm_add_event_handler = 0x0001fd8d;
sm_address_resolution_lookup = 0x0001fee5;
sm_authenticated = 0x00020261;
sm_authorization_decline = 0x0002026f;
sm_authorization_grant = 0x0002028f;
sm_authorization_state = 0x000202af;
sm_bonding_decline = 0x000202c9;
sm_config = 0x00020725;
sm_config_conn = 0x0002073d;
sm_encryption_key_size = 0x000208f3;
sm_just_works_confirm = 0x00020e71;
sm_le_device_key = 0x000211bd;
sm_passkey_input = 0x00021253;
sm_private_random_address_generation_get = 0x0002160d;
sm_private_random_address_generation_get_mode = 0x00021615;
sm_private_random_address_generation_set_mode = 0x00021621;
sm_private_random_address_generation_set_update_period = 0x00021649;
sm_register_oob_data_callback = 0x00021785;
sm_request_pairing = 0x00021791;
sm_send_security_request = 0x00022267;
sm_set_accepted_stk_generation_methods = 0x0002228d;
sm_set_authentication_requirements = 0x00022299;
sm_set_encryption_key_size_range = 0x000222a5;
sscanf_bd_addr = 0x00022601;
sysSetPublicDeviceAddr = 0x000229b5;
uuid128_to_str = 0x00023129;
uuid_add_bluetooth_prefix = 0x00023181;
uuid_has_bluetooth_prefix = 0x000231a1;
uxListRemove = 0x000231bd;
uxQueueMessagesWaiting = 0x000231e5;
uxQueueMessagesWaitingFromISR = 0x0002320d;
uxQueueSpacesAvailable = 0x00023229;
uxTaskGetStackHighWaterMark = 0x00023255;
uxTaskPriorityGet = 0x00023275;
uxTaskPriorityGetFromISR = 0x00023291;
vListInitialise = 0x0002334b;
vListInitialiseItem = 0x00023361;
vListInsert = 0x00023367;
vListInsertEnd = 0x00023397;
vPortEndScheduler = 0x000233b1;
vPortEnterCritical = 0x000233dd;
vPortExitCritical = 0x00023421;
vPortFree = 0x00023455;
vPortSuppressTicksAndSleep = 0x000234e9;
vPortValidateInterruptPriority = 0x00023611;
vQueueDelete = 0x0002366d;
vQueueWaitForMessageRestricted = 0x00023699;
vTaskDelay = 0x000236e1;
vTaskInternalSetTimeOutState = 0x0002372d;
vTaskMissedYield = 0x0002373d;
vTaskPlaceOnEventList = 0x00023749;
vTaskPlaceOnEventListRestricted = 0x00023781;
vTaskPriorityDisinheritAfterTimeout = 0x000237c1;
vTaskPrioritySet = 0x0002386d;
vTaskResume = 0x00023935;
vTaskStartScheduler = 0x000239b9;
vTaskStepTick = 0x00023a49;
vTaskSuspend = 0x00023a79;
vTaskSuspendAll = 0x00023b35;
vTaskSwitchContext = 0x00023b45;
xPortStartScheduler = 0x00023bed;
xQueueAddToSet = 0x00023cb5;
xQueueCreateCountingSemaphore = 0x00023cd9;
xQueueCreateCountingSemaphoreStatic = 0x00023d15;
xQueueCreateMutex = 0x00023d59;
xQueueCreateMutexStatic = 0x00023d6f;
xQueueCreateSet = 0x00023d89;
xQueueGenericCreate = 0x00023d91;
xQueueGenericCreateStatic = 0x00023ddd;
xQueueGenericReset = 0x00023e45;
xQueueGenericSend = 0x00023ed1;
xQueueGenericSendFromISR = 0x0002403d;
xQueueGiveFromISR = 0x000240fd;
xQueueGiveMutexRecursive = 0x000241a1;
xQueueIsQueueEmptyFromISR = 0x000241e1;
xQueueIsQueueFullFromISR = 0x00024205;
xQueuePeek = 0x0002422d;
xQueuePeekFromISR = 0x00024355;
xQueueReceive = 0x000243c1;
xQueueReceiveFromISR = 0x000244ed;
xQueueRemoveFromSet = 0x00024581;
xQueueSelectFromSet = 0x000245a3;
xQueueSelectFromSetFromISR = 0x000245b5;
xQueueSemaphoreTake = 0x000245c9;
xQueueTakeMutexRecursive = 0x00024735;
xTaskCheckForTimeOut = 0x00024779;
xTaskCreate = 0x000247e9;
xTaskCreateStatic = 0x00024845;
xTaskGetCurrentTaskHandle = 0x000248b5;
xTaskGetSchedulerState = 0x000248c1;
xTaskGetTickCount = 0x000248dd;
xTaskGetTickCountFromISR = 0x000248e9;
xTaskIncrementTick = 0x000248f9;
xTaskPriorityDisinherit = 0x000249c5;
xTaskPriorityInherit = 0x00024a59;
xTaskRemoveFromEventList = 0x00024aed;
xTaskResumeAll = 0x00024b6d;
xTaskResumeFromISR = 0x00024c35;
xTimerCreate = 0x00024cc1;
xTimerCreateStatic = 0x00024cf5;
xTimerCreateTimerTask = 0x00024d2d;
xTimerGenericCommand = 0x00024d99;
xTimerGetExpiryTime = 0x00024e09;
xTimerGetTimerDaemonTaskHandle = 0x00024e29;
