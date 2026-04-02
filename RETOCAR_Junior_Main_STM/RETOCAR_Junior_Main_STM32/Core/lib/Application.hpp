/**
 * @file Application.hpp
 * @brief Tầng ứng dụng chính (Application Layer) cho phần mềm Node vệ tinh (Slave).
 * @note Chịu trách nhiệm khởi tạo và điều phối các thành phần phần cứng/phần mềm.
 */
#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Khởi tạo hệ thống phần mềm và phần cứng cấu trúc.
 * @note Hàm này chỉ được hệ điều hành Hardware Abstraction Layer gọi 1 lần (Tại USER CODE BEGIN 2).
 */
void AppMain_Init(void);

/**
 * @brief Vòng lặp liên tục của ứng dụng thời gian thực.
 * @note Hàm này được gắn quyền giám sát liên tiếp (Tại USER CODE BEGIN 3).
 */
void AppMain_Loop(void);

#ifdef __cplusplus
}
#endif

#endif // APPLICATION_HPP
