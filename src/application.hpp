/*****************************************************************//**
 * \file   application.hpp
 * \brief  
 * 
 * \author anony
 * \date   October 2024
 *********************************************************************/
#pragma once

#include "base_headers.hpp"
#include <QtWidgets>
#include <QPixmap>

namespace Ui
{
	class NewMainWindow;
}

namespace APP
{
	class MainWindow : public QWidget
	{
		Q_OBJECT
	public:
		MainWindow(QWidget* parent = 0);
		~MainWindow();

	protected:
		void createUi();

	protected slots:
		void slotButtonDown();

	protected:
		QPushButton* p_button;
		int m_index;
	};


	class NewMainWindow : public QWidget
	{
		Q_OBJECT
	public:
		NewMainWindow(QWidget* parent = nullptr);
		~NewMainWindow();

	private:
		void ConnectSlotFunction();
		void PutImage();

	private slots:
		void ButtonDown();
		void CloseButton();

	private:
		Ui::NewMainWindow *ui;
		int m_index{ 0 };
	};

	int RunAPP(int argc, char* argv[]);

	void LoadImageInQt();
}