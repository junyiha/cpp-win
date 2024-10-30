#include "configuration.hpp"

namespace GP
{
	std::map<std::pair<WorkingScenario, PositionType>, PositionData> Position_Map{};

	double velLine{ 0.0 };
	double velRotate{ 0.0 };

	std::size_t CYLINDER_INDEX{ 0 };
	std::size_t STEER_LEFT_INDEX{ 0 };
	std::size_t STEER_RIGHT_INDEX{ 0 };
	std::size_t WHEEL_LEFT_INDEX{ 0 };
	std::size_t WHEEL_RIGHT_INDEX{ 0 };
	std::size_t TOOL_LIFTING{ 0 };

	double Lift_Distance_In_Parallel{ 0.0 };
	double Max_Deviation_In_Parallel{ 0.0 };
	double Min_Deviation_In_Parallel{ 0.0 };

	double Distance_work{ 0.0 };

	double Lift_Distance_In_FitBoard{ 0.0 };
	double Max_Deviation_In_FitBoard{ 0.0 };
	double Min_Deviation_In_FitBoard{ 0.0 };

	double Line_Deviation_Threshold{ 0.0 };

	std::vector<double> Home_Position(DOF, 0.0);
	QVector<double> Home_Position_QV = QVector<double>(DOF, 0.0);

	std::vector<double> Prepare_Position(DOF, 0.0);
	QVector<double> Prepare_Position_QV(DOF, 0.0);

	std::vector<double> End_Vel_Limit(6, 0.0);
	std::vector<double> End_Vel_Position(6, 0.0);

	std::string Robot_IP;
	std::size_t Robot_Port;

	std::string IOA_IP;
	std::size_t IOA_Port;
}

namespace Config
{
	ConfigManager::ConfigManager(std::string path) : m_path(path)
	{
		assert(LoadConfiguration());
	}

	ConfigManager::~ConfigManager()
	{

	}

	bool ConfigManager::LoadConfiguration()
	{
		bool res{ true };
		try
		{
			m_root = YAML::LoadFile(m_path);
			ParseConfiguration();
		}
		catch (YAML::BadFile)
		{
			std::cerr << "Invalid configuration file path: " << m_path << "\n";
			res = false;
		}
		catch (...)
		{
			std::cerr << "parse config.yaml failed, please check the config.yaml file!!!\n";
			res = false;
		}

		return res;
	}

	void ConfigManager::ParseConfiguration()
	{
		GP::End_Vel_Limit = m_root["end_vel_limit"].as<std::vector<double>>();
		GP::End_Vel_Position = m_root["end_vel_position"].as<std::vector<double>>();

		GP::Robot_IP = m_root["robot_ip"].as<std::string>();
		GP::Robot_Port = m_root["robot_port"].as<std::size_t>();

		GP::IOA_IP = m_root["IOA_ip"].as<std::string>();
		GP::IOA_Port = m_root["IOA_port"].as<std::size_t>();

		GP::Home_Position = m_root["home_point"].as<std::vector<double>>();
		GP::Prepare_Position = m_root["prepare_point"].as<std::vector<double>>();
		for (int i = 0; i < GP::DOF; i++)
		{
			GP::Home_Position_QV[i] = GP::Home_Position.at(i);

			GP::Prepare_Position_QV[i] = GP::Prepare_Position.at(i);
		}

		GP::CYLINDER_INDEX = m_root["CYLINDER_INDEX"].as<std::size_t>();
		GP::STEER_LEFT_INDEX = m_root["STEER_LEFT_INDEX"].as<std::size_t>();
		GP::STEER_RIGHT_INDEX = m_root["STEER_RIGHT_INDEX"].as<std::size_t>();
		GP::WHEEL_LEFT_INDEX = m_root["WHEEL_LEFT_INDEX"].as<std::size_t>();
		GP::WHEEL_RIGHT_INDEX = m_root["WHEEL_RIGHT_INDEX"].as<std::size_t>();
		GP::TOOL_LIFTING = m_root["TOOL_LIFTING"].as<std::size_t>();

		GP::velLine = m_root["velLine"].as<double>();
		GP::velRotate = m_root["velRotate"].as<double>() / 57.3;

		GP::Lift_Distance_In_Parallel = m_root["Lift_Distance_In_Parallel"].as<double>();
		GP::Max_Deviation_In_Parallel = m_root["Max_Deviation_In_Parallel"].as<double>();
		GP::Min_Deviation_In_Parallel = m_root["Min_Deviation_In_Parallel"].as<double>();
		GP::Distance_work = m_root["Distance_work"].as<double>();
		GP::Lift_Distance_In_FitBoard = m_root["Lift_Distance_In_FitBoard"].as<double>();
		GP::Max_Deviation_In_FitBoard = m_root["Max_Deviation_In_FitBoard"].as<double>();
		GP::Min_Deviation_In_FitBoard = m_root["Min_Deviation_In_FitBoard"].as<double>();
		GP::Line_Deviation_Threshold = m_root["Line_Deviation_Threshold"].as<double>();

		std::cerr << "brief: " << m_root["test"]["brief"].as<std::string>() << ", value: " << m_root["test"]["value"].as<int>() << "\n";
		for (auto& it : m_root["position_map"])
		{
			auto work_scenario = static_cast<GP::WorkingScenario>(it[1].as<int>());
			auto position_type = static_cast<GP::PositionType>(it[2].as<int>());

			GP::PositionData position_data;
			position_data.brief = it[0].as<std::string>();
			position_data.value = it[3].as<std::vector<double>>();

			GP::Position_Map[{work_scenario, position_type}] = position_data;
		}
	}

	bool ConfigManager::WriteToFile()
	{
		bool res{ false };

		std::ofstream fout(m_path);
		if (fout)
		{
			fout << m_root;
			fout.close();
			res = true;
		}

		return res;
	}

	bool ConfigManager::ReloadConfiguration()
	{
		return LoadConfiguration();
	}

	bool ConfigManager::UpdateValue(const std::string key, const double value)
	{
		bool res{ false };

		if (m_root[key])
		{
			m_root[key] = value;
			if (WriteToFile())
				res = true;
		}
		
		return res;
	}

	bool ConfigManager::UpdateValue(const std::string key, const std::vector<double> value)
	{
		bool res{ false };

		if (m_root[key])
		{
			m_root[key] = value;
			if (WriteToFile())
				res = true;
		}

		return res;
	}

	int ConfigurationMain()
	{
		std::string path{ "C:/Users/anony/Documents/GitHub/cpp-win/data/config.yaml" };
		ConfigManager config_manager(path);

		std::cerr << "Line Deviation Threshold: " << GP::Line_Deviation_Threshold << "\n"
				  << "Home Position: " << GP::Home_Position[0] << "\n";

		config_manager.UpdateValue("Line_Deviation_Threshold", 6.0);
		config_manager.UpdateValue("home_point", { 2, 2, 3, 4, 5, 6 , 7, 8, 9, 10, 11});

		std::cerr << "Line Deviation Threshold: " << GP::Line_Deviation_Threshold << "\n";

		return 0;
	}
}