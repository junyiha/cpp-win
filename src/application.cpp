#include "application.hpp"
#include "ui_NewMainWindow.h"
#include "ui_SceneSelectionWindow.h"

namespace APP
{
	MainWindow::MainWindow(QWidget* parent)
		:QWidget(parent)
	{
		m_index = 0;
		createUi();
	}

	MainWindow::~MainWindow()
	{

	}

	void MainWindow::createUi()
	{
		QVBoxLayout* layout = new QVBoxLayout();    //添加一个竖向自动布局
		p_button = new QPushButton(this);
		p_button->setText(QString::fromLocal8Bit("点我"));    //QString::fromLocal8Bit("中文"),否则会乱码
		connect(p_button, SIGNAL(released()), this, SLOT(slotButtonDown())); //信号与槽

		layout->addWidget(p_button);    //添加一个按钮进入布局管理器
		this->setLayout(layout);    //将布局管理器设置到当前控件
	}

	void MainWindow::slotButtonDown()
	{
		QString str = QString::fromLocal8Bit("点我 %1").arg(m_index++);//arg括号里面的参数会依次替换前面的 %1
		p_button->setText(str);
	}

	/// <summary>
	/// //////////////////////////////////////////////////////////////////////////////////////////////////////
	/// </summary>
	/// <param name="parent"></param>

	NewMainWindow::NewMainWindow(QWidget* parent)
		:QWidget(parent), ui(new Ui::NewMainWindow)
	{
		ui->setupUi(this);
		m_log = spdlog::get("logger");
		ConnectSlotFunction();
	}

	NewMainWindow::~NewMainWindow()
	{
		delete ui;
	}

	void NewMainWindow::ConnectSlotFunction()
	{
		connect(ui->pushButton, &QPushButton::clicked, this, &NewMainWindow::ButtonDown, Qt::UniqueConnection);
		connect(ui->closeButton, &QPushButton::clicked, this, &NewMainWindow::CloseButton, Qt::UniqueConnection);
		connect(ui->btn_config, &QPushButton::clicked, this, &NewMainWindow::ConfigButtonClicked, Qt::UniqueConnection);
		connect(ui->btn_argument, &QPushButton::clicked, this, &NewMainWindow::ArgumentButtonClicked, Qt::UniqueConnection);
		connect(ui->btn_document, &QPushButton::clicked, this, &NewMainWindow::DocumentButtonClicked, Qt::UniqueConnection);
		connect(ui->btn_message_alert, &QPushButton::clicked, this, &NewMainWindow::MessageAlertButtonClicked, Qt::UniqueConnection);
		connect(ui->btn_send_data_x, &QPushButton::clicked, this, &NewMainWindow::SendDataButtonClicked, Qt::UniqueConnection);
	}

	void NewMainWindow::ConfigButtonClicked()
	{
		ui->stackedWidget->setCurrentIndex(0);
	}

	void NewMainWindow::ArgumentButtonClicked()
	{
		ui->stackedWidget->setCurrentIndex(1);
	}

	void NewMainWindow::DocumentButtonClicked()
	{
		ui->stackedWidget->setCurrentIndex(2);
		LoadHelpFile();
	}

	void NewMainWindow::MessageAlertButtonClicked()
	{
		QLabel* label_ptr = new QLabel("this is a message alert...", this);
		label_ptr->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
		label_ptr->setStyleSheet("background-color: green; color: white; padding: 10px; border-radius: 5px;");
		label_ptr->setAlignment(Qt::AlignHCenter);
		label_ptr->setFixedSize(200, 50);

		QPoint global_pos = mapToGlobal(QPoint(0, 0));
		label_ptr->move(global_pos.x() + width() / 2 - label_ptr->width() / 2, global_pos.y() + 50);
		label_ptr->show();

		// 设置定时器 3 秒后关闭窗口
		QTimer::singleShot(3000, label_ptr, &QLabel::deleteLater);
	}

	void NewMainWindow::SendDataButtonClicked()
	{
		double value = ui->doubleSpinBoxForX->value();

		m_log->info("{} value: {}", __LINE__, value);
	}

	void NewMainWindow::PutImage()
	{
		QString image_path = QString::fromLocal8Bit("C:/Users/anony/Desktop/无标题.png");

		if (QFile::exists(image_path))
		{
			QImage image(image_path);
			if (image.isNull())
			{
				std::cerr << "image is null, terminate!!!\n";
			}

			QPixmap pixmap(QString::fromLocal8Bit("C:/Users/anony/Desktop/无标题.png"));
			if (pixmap.isNull())
			{
				std::cerr << "pixmap is null, terminate!!!\n";
				return;
			}
			ui->label->setPixmap(pixmap);
			ui->label->setScaledContents(true);
		}
		else
		{
			std::cerr << "path is invalid!!!\n";
		}
	}

	void NewMainWindow::LoadHelpFile()
	{
		std::string help_file{ "C://Users//anony//Documents//GitHub//junyiha.github.io//_posts//notes//Company//shanghai-tejizhi-robot//碰钉机器人//PDRobotHelp.html" };
		auto current_widget_ptr = ui->stackedWidget->currentWidget();

		QTextBrowser* text_browser = new QTextBrowser;
		text_browser->setSource(QUrl::fromLocalFile(QString::fromLocal8Bit(help_file.c_str())));

		QVBoxLayout* layout = new QVBoxLayout;
		layout->addWidget(text_browser);
		current_widget_ptr->setLayout(layout);
		current_widget_ptr->show();
	}

	void NewMainWindow::ButtonDown()
	{
		ui->pushButton->setText(QString::fromLocal8Bit("show the image"));
		PutImage();
	}

	void NewMainWindow::CloseButton()
	{
		ui->label->clear();
	}

	SceneSelectionWindow::SceneSelectionWindow(QWidget* parent)
		:QWidget(parent), ui(new Ui::SceneSelectionWindow)
	{
		ui->setupUi(this);
		ConnectSlotFunction();
	}

	SceneSelectionWindow::~SceneSelectionWindow()
	{
		delete ui;
	}

	void SceneSelectionWindow::ConnectSlotFunction()
	{
		connect(ui->conform_button, &QPushButton::clicked, this, &SceneSelectionWindow::ConformButtonClicked, Qt::UniqueConnection);
		connect(ui->quit_button, &QPushButton::clicked, this, &SceneSelectionWindow::QuitButtonClicked, Qt::UniqueConnection);
	}

	void SceneSelectionWindow::ConformButtonClicked()
	{
		int index = ui->comboBox->currentIndex();
		auto text = ui->comboBox->currentText().toLocal8Bit().toStdString();
		std::cerr << "index: " << index << ", text: " << text << "\n";

		new_window.show();
		this->close();
	}

	void SceneSelectionWindow::QuitButtonClicked()
	{
		this->close();
	}


	/// <summary>
	/// /////////////////////////////////////////////////////////////////////////////
	/// </summary>

	void LoadImageInQt()
	{
		QString path{ "C:/Users/anony/Desktop/无标题.png" };

		std::cerr << "Current Directory: " << QDir::currentPath().toStdString();

		if (QFile::exists(QString::fromLocal8Bit("C:/Users/anony/Desktop/无标题.png")) == false)
		{
			std::cerr << "file not exist!!!\n";
		}

		QPixmap pixmap;
		pixmap.load(QString::fromLocal8Bit("C:/Users/anony/Desktop/无标题.png"));
		if (pixmap.isNull())
		{
			std::cerr << "pixmap is null\n";
		}
	}

	int RunAPP(int argc, char* argv[])
	{
		QApplication a(argc, argv);

		LoadImageInQt();

		SceneSelectionWindow scene_selection_window;
		scene_selection_window.show();

		return a.exec();
	}
}
