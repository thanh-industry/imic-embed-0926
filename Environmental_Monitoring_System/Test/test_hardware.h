#ifndef TEST_HARDWARE_H
#define TEST_HARDWARE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Khởi tạo toàn bộ 5 ngoại vi và kiểm tra trạng thái phần cứng ban đầu
 */
void test_hardware_init(void);

/**
 * @brief Hàm chạy chu kỳ kiểm thử (gọi trong while(1) của main)
 */
void test_hardware_run(void);

#ifdef __cplusplus
}
#endif

#endif /* TEST_HARDWARE_H */
