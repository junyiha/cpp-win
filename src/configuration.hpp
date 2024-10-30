/*****************************************************************//**
 * \file   configuration.hpp
 * \brief  �����ļ���ز���
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
		Top = 1,	// ����
		Cant,		// б��
		Side		// ��� 
	};

	enum class PositionType
	{
		Prepare = 1,	// ׼��λ
		Lift,			// ����λ
		Quit			// �˳�λ
	};

	struct PositionData
	{
		std::string brief;
		std::vector<double> value;
	};

	using PositionMap = std::map<std::pair<GP::WorkingScenario, GP::PositionType>, GP::PositionData>;

	extern PositionMap Position_Map;

	static const int DOF = 11;

	extern double velLine;		//����㶯���٣�ƽ��
	extern double velRotate;	//����㶯���٣���ת

	extern std::size_t CYLINDER_INDEX;			// �Ƹ˹ؽ��������
	extern std::size_t STEER_LEFT_INDEX;		// ���������
	extern std::size_t STEER_RIGHT_INDEX;		// �Ҷ�������
	extern std::size_t WHEEL_LEFT_INDEX;		// ������������
	extern std::size_t WHEEL_RIGHT_INDEX;		// ������������
	extern std::size_t TOOL_LIFTING;			// ������������

	extern double Lift_Distance_In_Parallel;	// ��ƽ״̬�¾�����λ��
	extern double Max_Deviation_In_Parallel;	// ��ƽ�������ƫ��
	extern double Min_Deviation_In_Parallel;	// ��ƽ������Сƫ��

	extern double Distance_work;				//λ��

	extern double Lift_Distance_In_FitBoard;    // ����״̬�¾�����λ��
	extern double Max_Deviation_In_FitBoard;	// ��������ƫ��
	extern double Min_Deviation_In_FitBoard;    // ��������ƫ��

	extern double Line_Deviation_Threshold;		//���ߵ�������ƫ��

	// ������: ׼��λ
	extern std::vector<double> Home_Position;
	extern QVector<double> Home_Position_QV;

	// ������: ����λ
	extern std::vector<double> Prepare_Position;
	extern QVector<double> Prepare_Position_QV;

	// �ؽ�ĩ���˶�����
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
		 * @brief ���ز��������ļ�.
		 */
		bool LoadConfiguration();

		/**
		 * @brief �������������ļ�����ʼ��ȫ�ֲ���.
		 */
		void ParseConfiguration();

		bool WriteToFile();

	public:
		/**
		 * @brief ���¼��ز��������ļ�.
		 */
		bool ReloadConfiguration();

		/**
		 * @brief ����ָ������(������).
		 */
		bool UpdateValue(const std::string key, const double value);

		/**
		 * @brief ����ָ������(����).
		 */
		bool UpdateValue(const std::string key, const std::vector<double> value);

		bool UpdateValue(const std::string key, const GP::PositionMap position_map);

	private:
		YAML::Node m_root;
		std::string m_path;
	};

	int ConfigurationMain();
}
