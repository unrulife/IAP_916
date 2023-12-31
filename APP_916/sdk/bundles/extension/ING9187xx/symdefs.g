att_dispatch_client_can_send_now = 0x00005751;
att_dispatch_client_request_can_send_now_event = 0x00005757;
att_dispatch_register_client = 0x0000575d;
att_dispatch_register_server = 0x00005771;
att_dispatch_server_can_send_now = 0x00005785;
att_dispatch_server_request_can_send_now_event = 0x0000578b;
att_emit_general_event = 0x0000583d;
att_server_can_send_packet_now = 0x00005f6d;
att_server_deferred_read_response = 0x00005f71;
att_server_get_mtu = 0x00005f89;
att_server_indicate = 0x00006001;
att_server_init = 0x00006085;
att_server_notify = 0x000060c1;
att_server_register_packet_handler = 0x000061d9;
att_server_request_can_send_now_event = 0x000061e5;
att_set_db = 0x00006201;
att_set_read_callback = 0x00006215;
att_set_write_callback = 0x00006221;
bd_addr_cmp = 0x00006391;
bd_addr_copy = 0x00006397;
bd_addr_to_str = 0x000063a1;
big_endian_read_16 = 0x000063d9;
big_endian_read_32 = 0x000063e1;
big_endian_store_16 = 0x000063f5;
big_endian_store_32 = 0x00006401;
btstack_config = 0x00006555;
btstack_memory_pool_create = 0x00006693;
btstack_memory_pool_free = 0x000066bd;
btstack_memory_pool_get = 0x0000671d;
btstack_push_user_msg = 0x00006785;
btstack_push_user_runnable = 0x00006791;
btstack_reset = 0x0000679d;
char_for_nibble = 0x00006a79;
eTaskConfirmSleepModeStatus = 0x00006d61;
gap_add_dev_to_periodic_list = 0x0000735d;
gap_add_whitelist = 0x0000736d;
gap_aes_encrypt = 0x00007379;
gap_clear_white_lists = 0x000073b1;
gap_clr_adv_set = 0x000073bd;
gap_clr_periodic_adv_list = 0x000073c9;
gap_create_connection_cancel = 0x000073d5;
gap_disconnect = 0x000073e1;
gap_disconnect_all = 0x0000740d;
gap_ext_create_connection = 0x0000744d;
gap_get_connection_parameter_range = 0x00007525;
gap_le_read_channel_map = 0x00007561;
gap_periodic_adv_create_sync = 0x000075cd;
gap_periodic_adv_create_sync_cancel = 0x000075f1;
gap_periodic_adv_term_sync = 0x000075fd;
gap_read_periodic_adv_list_size = 0x00007685;
gap_read_phy = 0x00007691;
gap_read_remote_used_features = 0x0000769d;
gap_read_remote_version = 0x000076a9;
gap_read_rssi = 0x000076b5;
gap_remove_whitelist = 0x000076c1;
gap_rmv_adv_set = 0x0000773d;
gap_rmv_dev_from_periodic_list = 0x00007749;
gap_rx_test_v2 = 0x00007759;
gap_set_adv_set_random_addr = 0x00007791;
gap_set_callback_for_next_hci = 0x000077cd;
gap_set_connection_parameter_range = 0x000077ed;
gap_set_data_length = 0x00007805;
gap_set_def_phy = 0x0000781d;
gap_set_ext_adv_data = 0x0000782d;
gap_set_ext_adv_enable = 0x00007845;
gap_set_ext_adv_para = 0x000078b5;
gap_set_ext_scan_enable = 0x0000798d;
gap_set_ext_scan_para = 0x000079a5;
gap_set_ext_scan_response_data = 0x00007a45;
gap_set_host_channel_classification = 0x00007a5d;
gap_set_periodic_adv_data = 0x00007a6d;
gap_set_periodic_adv_enable = 0x00007add;
gap_set_periodic_adv_para = 0x00007aed;
gap_set_phy = 0x00007b05;
gap_set_random_device_address = 0x00007b21;
gap_start_ccm = 0x00007b51;
gap_test_end = 0x00007b99;
gap_tx_test_v2 = 0x00007ba5;
gap_tx_test_v4 = 0x00007bbd;
gap_update_connection_parameters = 0x00007be1;
gap_vendor_tx_continuous_wave = 0x00007c25;
gatt_client_cancel_write = 0x0000814d;
gatt_client_discover_characteristic_descriptors = 0x00008173;
gatt_client_discover_characteristics_for_handle_range_by_uuid128 = 0x000081b3;
gatt_client_discover_characteristics_for_handle_range_by_uuid16 = 0x00008203;
gatt_client_discover_characteristics_for_service = 0x00008253;
gatt_client_discover_primary_services = 0x00008289;
gatt_client_discover_primary_services_by_uuid128 = 0x000082bb;
gatt_client_discover_primary_services_by_uuid16 = 0x000082ff;
gatt_client_execute_write = 0x0000833b;
gatt_client_find_included_services_for_service = 0x00008361;
gatt_client_get_mtu = 0x0000838f;
gatt_client_is_ready = 0x00008431;
gatt_client_listen_for_characteristic_value_updates = 0x00008447;
gatt_client_prepare_write = 0x00008469;
gatt_client_read_characteristic_descriptor_using_descriptor_handle = 0x000084a5;
gatt_client_read_long_characteristic_descriptor_using_descriptor_handle = 0x000084cf;
gatt_client_read_long_characteristic_descriptor_using_descriptor_handle_with_offset = 0x000084d5;
gatt_client_read_long_value_of_characteristic_using_value_handle = 0x00008503;
gatt_client_read_long_value_of_characteristic_using_value_handle_with_offset = 0x00008509;
gatt_client_read_multiple_characteristic_values = 0x00008537;
gatt_client_read_value_of_characteristic_using_value_handle = 0x00008567;
gatt_client_read_value_of_characteristics_by_uuid128 = 0x00008595;
gatt_client_read_value_of_characteristics_by_uuid16 = 0x000085e1;
gatt_client_register_handler = 0x0000862d;
gatt_client_reliable_write_long_value_of_characteristic = 0x00008639;
gatt_client_signed_write_without_response = 0x00008a69;
gatt_client_write_characteristic_descriptor_using_descriptor_handle = 0x00008b2d;
gatt_client_write_client_characteristic_configuration = 0x00008b67;
gatt_client_write_long_characteristic_descriptor_using_descriptor_handle = 0x00008bb9;
gatt_client_write_long_characteristic_descriptor_using_descriptor_handle_with_offset = 0x00008bc9;
gatt_client_write_long_value_of_characteristic = 0x00008c05;
gatt_client_write_long_value_of_characteristic_with_offset = 0x00008c15;
gatt_client_write_value_of_characteristic = 0x00008c51;
gatt_client_write_value_of_characteristic_without_response = 0x00008c87;
hci_add_event_handler = 0x0000a1ad;
hci_power_control = 0x0000a94d;
hci_register_acl_packet_handler = 0x0000ab01;
kv_commit = 0x0000b275;
kv_get = 0x0000b2d1;
kv_init = 0x0000b2dd;
kv_init_backend = 0x0000b35d;
kv_put = 0x0000b371;
kv_remove = 0x0000b37d;
kv_remove_all = 0x0000b3b1;
kv_value_modified = 0x0000b3e1;
kv_value_modified_of_key = 0x0000b3fd;
kv_visit = 0x0000b409;
l2cap_add_event_handler = 0x0000b499;
l2cap_can_send_packet_now = 0x0000b4a9;
l2cap_create_le_credit_based_connection_request = 0x0000b665;
l2cap_credit_based_send = 0x0000b7a9;
l2cap_credit_based_send_continue = 0x0000b7d5;
l2cap_disconnect = 0x0000b851;
l2cap_get_le_credit_based_connection_credits = 0x0000baa1;
l2cap_get_peer_mtu_for_local_cid = 0x0000babd;
l2cap_init = 0x0000be91;
l2cap_le_send_flow_control_credit = 0x0000bf87;
l2cap_max_le_mtu = 0x0000c291;
l2cap_register_packet_handler = 0x0000c3b9;
l2cap_register_service = 0x0000c3c5;
l2cap_request_can_send_now_event = 0x0000c4d5;
l2cap_request_connection_parameter_update = 0x0000c4ef;
l2cap_send_echo_request = 0x0000c9c9;
l2cap_unregister_service = 0x0000ca89;
le_device_db_add = 0x0000cae1;
le_device_db_find = 0x0000cbb9;
le_device_db_from_key = 0x0000cbe5;
le_device_db_iter_cur = 0x0000cbed;
le_device_db_iter_cur_key = 0x0000cbf1;
le_device_db_iter_init = 0x0000cbf5;
le_device_db_iter_next = 0x0000cbfd;
le_device_db_remove_key = 0x0000cc23;
ll_ackable_packet_alloc = 0x0000cc4f;
ll_ackable_packet_get_status = 0x0000cd81;
ll_ackable_packet_run = 0x0000cdf1;
ll_ackable_packet_set_tx_data = 0x0000ce8d;
ll_aes_encrypt = 0x0000cea9;
ll_channel_monitor_alloc = 0x0000cf25;
ll_channel_monitor_check_each_pdu = 0x0000cfa7;
ll_channel_monitor_run = 0x0000d00d;
ll_config = 0x0000d0c1;
ll_free = 0x0000d0f7;
ll_get_heap_free_size = 0x0000d101;
ll_hint_on_ce_len = 0x0000d115;
ll_legacy_adv_set_interval = 0x0000d14d;
ll_lock_frequency = 0x0000d15d;
ll_malloc = 0x0000d1c1;
ll_query_timing_info = 0x0000d2f9;
ll_raw_packet_alloc = 0x0000d345;
ll_raw_packet_free = 0x0000d419;
ll_raw_packet_get_bare_rx_data = 0x0000d451;
ll_raw_packet_get_rx_data = 0x0000d517;
ll_raw_packet_recv = 0x0000d5b9;
ll_raw_packet_send = 0x0000d675;
ll_raw_packet_set_bare_data = 0x0000d75d;
ll_raw_packet_set_bare_mode = 0x0000d79b;
ll_raw_packet_set_param = 0x0000d8a1;
ll_raw_packet_set_tx_data = 0x0000d8ff;
ll_register_hci_acl_previewer = 0x0000d965;
ll_scan_set_fixed_channel = 0x0000d9c9;
ll_set_adv_access_address = 0x0000dbe1;
ll_set_adv_coded_scheme = 0x0000dbed;
ll_set_conn_acl_report_latency = 0x0000dc1d;
ll_set_conn_coded_scheme = 0x0000dc4d;
ll_set_conn_interval_unit = 0x0000dc79;
ll_set_conn_latency = 0x0000dc85;
ll_set_conn_tx_power = 0x0000dcb5;
ll_set_def_antenna = 0x0000dcfd;
ll_set_initiating_coded_scheme = 0x0000dd19;
ll_set_max_conn_number = 0x0000dd25;
ll_unlock_frequency = 0x0000ddb9;
nibble_for_char = 0x0001e089;
platform_32k_rc_auto_tune = 0x0001e135;
platform_32k_rc_tune = 0x0001e1b1;
platform_calibrate_32k = 0x0001e1c5;
platform_config = 0x0001e1c9;
platform_delete_timer = 0x0001e2ed;
platform_enable_irq = 0x0001e2f5;
platform_get_current_task = 0x0001e32d;
platform_get_gen_os_driver = 0x0001e351;
platform_get_heap_status = 0x0001e359;
platform_get_link_layer_interf = 0x0001e371;
platform_get_task_handle = 0x0001e379;
platform_get_timer_counter = 0x0001e399;
platform_get_us_time = 0x0001e39d;
platform_get_version = 0x0001e3a1;
platform_hrng = 0x0001e3a9;
platform_install_isr_stack = 0x0001e3b1;
platform_install_task_stack = 0x0001e3bd;
platform_patch_rf_init_data = 0x0001e3f5;
platform_printf = 0x0001e401;
platform_raise_assertion = 0x0001e415;
platform_rand = 0x0001e429;
platform_read_info = 0x0001e42d;
platform_read_persistent_reg = 0x0001e45d;
platform_reset = 0x0001e46d;
platform_set_abs_timer = 0x0001e491;
platform_set_evt_callback = 0x0001e495;
platform_set_evt_callback_table = 0x0001e4a9;
platform_set_irq_callback = 0x0001e4b5;
platform_set_irq_callback_table = 0x0001e4d1;
platform_set_rf_clk_source = 0x0001e4dd;
platform_set_rf_init_data = 0x0001e4e9;
platform_set_rf_power_mapping = 0x0001e4f5;
platform_set_timer = 0x0001e501;
platform_shutdown = 0x0001e505;
platform_switch_app = 0x0001e509;
platform_trace_raw = 0x0001e535;
platform_write_persistent_reg = 0x0001e54d;
printf_hexdump = 0x0001e701;
pvPortMalloc = 0x0001f1f5;
pvTaskIncrementMutexHeldCount = 0x0001f2dd;
pvTimerGetTimerID = 0x0001f2f5;
pxPortInitialiseStack = 0x0001f321;
reverse_128 = 0x0001f4c9;
reverse_24 = 0x0001f4cf;
reverse_256 = 0x0001f4d5;
reverse_48 = 0x0001f4db;
reverse_56 = 0x0001f4e1;
reverse_64 = 0x0001f4e7;
reverse_bd_addr = 0x0001f4ed;
reverse_bytes = 0x0001f4f3;
sm_add_event_handler = 0x0001f6c1;
sm_address_resolution_lookup = 0x0001f819;
sm_authenticated = 0x0001fb95;
sm_authorization_decline = 0x0001fba3;
sm_authorization_grant = 0x0001fbc3;
sm_authorization_state = 0x0001fbe3;
sm_bonding_decline = 0x0001fbfd;
sm_config = 0x00020059;
sm_config_conn = 0x00020071;
sm_encryption_key_size = 0x00020227;
sm_just_works_confirm = 0x000207ad;
sm_le_device_key = 0x00020af9;
sm_passkey_input = 0x00020b8f;
sm_private_random_address_generation_get = 0x00020f49;
sm_private_random_address_generation_get_mode = 0x00020f51;
sm_private_random_address_generation_set_mode = 0x00020f5d;
sm_private_random_address_generation_set_update_period = 0x00020f85;
sm_register_external_ltk_callback = 0x000210c1;
sm_register_oob_data_callback = 0x000210cd;
sm_request_pairing = 0x000210d9;
sm_send_security_request = 0x00021baf;
sm_set_accepted_stk_generation_methods = 0x00021bd5;
sm_set_authentication_requirements = 0x00021be1;
sm_set_encryption_key_size_range = 0x00021bed;
sscanf_bd_addr = 0x00021fbd;
sysSetPublicDeviceAddr = 0x00022371;
uuid128_to_str = 0x0002297d;
uuid_add_bluetooth_prefix = 0x000229d5;
uuid_has_bluetooth_prefix = 0x000229f5;
uxListRemove = 0x00022a11;
uxQueueMessagesWaiting = 0x00022a39;
uxQueueMessagesWaitingFromISR = 0x00022a61;
uxQueueSpacesAvailable = 0x00022a7d;
uxTaskGetStackHighWaterMark = 0x00022aa9;
uxTaskPriorityGet = 0x00022ac9;
uxTaskPriorityGetFromISR = 0x00022ae5;
vListInitialise = 0x00022b9f;
vListInitialiseItem = 0x00022bb5;
vListInsert = 0x00022bbb;
vListInsertEnd = 0x00022beb;
vPortEndScheduler = 0x00022c05;
vPortEnterCritical = 0x00022c31;
vPortExitCritical = 0x00022c75;
vPortFree = 0x00022ca9;
vPortSuppressTicksAndSleep = 0x00022d3d;
vPortValidateInterruptPriority = 0x00022e65;
vQueueDelete = 0x00022ec1;
vQueueWaitForMessageRestricted = 0x00022eed;
vTaskDelay = 0x00022f35;
vTaskInternalSetTimeOutState = 0x00022f81;
vTaskMissedYield = 0x00022f91;
vTaskPlaceOnEventList = 0x00022f9d;
vTaskPlaceOnEventListRestricted = 0x00022fd5;
vTaskPriorityDisinheritAfterTimeout = 0x00023015;
vTaskPrioritySet = 0x000230c1;
vTaskResume = 0x00023189;
vTaskStartScheduler = 0x0002320d;
vTaskStepTick = 0x0002329d;
vTaskSuspend = 0x000232cd;
vTaskSuspendAll = 0x00023389;
vTaskSwitchContext = 0x00023399;
xPortStartScheduler = 0x00023441;
xQueueAddToSet = 0x00023509;
xQueueCreateCountingSemaphore = 0x0002352d;
xQueueCreateCountingSemaphoreStatic = 0x00023569;
xQueueCreateMutex = 0x000235ad;
xQueueCreateMutexStatic = 0x000235c3;
xQueueCreateSet = 0x000235dd;
xQueueGenericCreate = 0x000235e5;
xQueueGenericCreateStatic = 0x00023631;
xQueueGenericReset = 0x00023699;
xQueueGenericSend = 0x00023725;
xQueueGenericSendFromISR = 0x00023891;
xQueueGiveFromISR = 0x00023951;
xQueueGiveMutexRecursive = 0x000239f5;
xQueueIsQueueEmptyFromISR = 0x00023a35;
xQueueIsQueueFullFromISR = 0x00023a59;
xQueuePeek = 0x00023a81;
xQueuePeekFromISR = 0x00023ba9;
xQueueReceive = 0x00023c15;
xQueueReceiveFromISR = 0x00023d41;
xQueueRemoveFromSet = 0x00023dd5;
xQueueSelectFromSet = 0x00023df7;
xQueueSelectFromSetFromISR = 0x00023e09;
xQueueSemaphoreTake = 0x00023e1d;
xQueueTakeMutexRecursive = 0x00023f89;
xTaskCheckForTimeOut = 0x00023fcd;
xTaskCreate = 0x0002403d;
xTaskCreateStatic = 0x00024099;
xTaskGetCurrentTaskHandle = 0x00024109;
xTaskGetSchedulerState = 0x00024115;
xTaskGetTickCount = 0x00024131;
xTaskGetTickCountFromISR = 0x0002413d;
xTaskIncrementTick = 0x0002414d;
xTaskPriorityDisinherit = 0x00024219;
xTaskPriorityInherit = 0x000242ad;
xTaskRemoveFromEventList = 0x00024341;
xTaskResumeAll = 0x000243c1;
xTaskResumeFromISR = 0x00024489;
xTimerCreate = 0x00024515;
xTimerCreateStatic = 0x00024549;
xTimerCreateTimerTask = 0x00024581;
xTimerGenericCommand = 0x000245ed;
xTimerGetExpiryTime = 0x0002465d;
xTimerGetTimerDaemonTaskHandle = 0x0002467d;
