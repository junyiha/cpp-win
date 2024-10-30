/*****************************************************************//**
 * \file   configuration.hpp
 * \brief  配置文件相关操作
 * 
 * \author anony
 * \date   October 2024
 *********************************************************************/
#pragma once
#include <vector>
#include <fstream>
#include <cassert>
#include <iostream>
#include <QVector>
#include "yaml-cpp/yaml.h"

namespace GP
{
	enum class WorkingScenario
	{
		Top = 1,	// 顶板
		Cant,		// 斜板
		Side		// 侧板 
	};

	enum class PositionType
	{
		Prepare = 1,	// 准备位
		Lift,			// 举升位
		Quit			// 退出位
	};

	struct PositionData
	{
		std::string brief;
		std::vector<double> value;
	};

	using PositionMap = std::map<std::pair<GP::WorkingScenario, GP::PositionType>, GP::PositionData>;

	extern PositionMap Position_Map;

	static const int DOF = 11;

	extern double velLine;		//界面点动限速：平移
	extern double velRotate;	//界面点动限速：旋转

	extern std::size_t CYLINDER_INDEX;			// 推杆关节轴号索引
	extern std::size_t STEER_LEFT_INDEX;		// 左舵轮索引
	extern std::size_t STEER_RIGHT_INDEX;		// 右舵轮索引
	extern std::size_t WHEEL_LEFT_INDEX;		// 左行走轮索引
	extern std::size_t WHEEL_RIGHT_INDEX;		// 右行走轮索引
	extern std::size_t TOOL_LIFTING;			// 工具升降索引

	extern double Lift_Distance_In_Parallel;	// 调平状态下举升的位置
	extern double Max_Deviation_In_Parallel;	// 调平允许最大偏差
	extern double Min_Deviation_In_Parallel;	// 调平允许最小偏差

	extern double Distance_work;				//位置

	extern double Lift_Distance_In_FitBoard;    // 贴合状态下举升的位置
	extern double Max_Deviation_In_FitBoard;	// 贴合允许偏差
	extern double Min_Deviation_In_FitBoard;    // 贴合允许偏差

	extern double Line_Deviation_Threshold;		//边线调整允许偏差

	// 机器人: 准备位
	extern std::vector<double> Home_Position;
	extern QVector<double> Home_Position_QV;

	// 机器人: 举升位
	extern std::vector<double> Prepare_Position;
	extern QVector<double> Prepare_Position_QV;

	// 关节末端运动限速
	extern std::vector<double> End_Vel_Limit;
	extern std::vector<double> End_Vel_Position;

	extern std::string Robot_IP;
	extern std::size_t Robot_Port;

	extern std::string IOA_IP;
	extern std::size_t IOA_Port;
}

namespace Config
{
	class ConfigManager final
	{
	public:
		ConfigManager() = delete;
		ConfigManager(std::string path = "D:/Robot/config.yaml");
		~ConfigManager();

	private:
		/**
		 * @brief 加载参数配置文件.
		 */
		bool LoadConfiguration();

		/**
		 * @brief 解析参数配置文件，初始化全局参数.
		 */
		void ParseConfiguration();

		bool WriteToFile();

	public:
		/**
		 * @brief 重新加载参数配置文件.
		 */
		bool ReloadConfiguration();

		/**
		 * @brief 更新指定参数(浮点数).
		 */
		bool UpdateValue(const std::string key, const double value);

		/**
		 * @brief 更新指定参数(数组).
		 */
		bool UpdateValue(const std::string key, const std::vector<double> value);

		bool UpdateValue(const std::string key, const GP::PositionMap position_map);

	private:
		YAML::Node m_root;
		std::string m_path;
	};

	int ConfigurationMain();
}
