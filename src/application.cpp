#include "application.hpp"
#include "ui_NewMainWindow.h"

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

	NewMainWindow::NewMainWindow(QWidget *parent)
		:QWidget(parent), ui(new Ui::NewMainWindow)
	{
		ui->setupUi(this);

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

	void NewMainWindow::ButtonDown()
	{
		ui->pushButton->setText(QString::fromLocal8Bit("show the image"));
		PutImage();
	}

	void NewMainWindow::CloseButton()
	{
		ui->label->clear();
	}

	int RunAPP(int argc, char *argv[])
	{
		QApplication a(argc, argv);

		LoadImageInQt();

		NewMainWindow new_window;
		new_window.show();

		return a.exec();
	}

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
}
