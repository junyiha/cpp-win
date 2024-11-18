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
	class SceneSelectionWindow;
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
		void ConfigButtonClicked();
		void ArgumentButtonClicked();
		void DocumentButtonClicked();

	private:
		Ui::NewMainWindow *ui;
		int m_index{ 0 };
	};

	class SceneSelectionWindow : public QWidget
	{
		Q_OBJECT
	public:
		SceneSelectionWindow(QWidget* parent = nullptr);
		~SceneSelectionWindow();

	private:
		void ConnectSlotFunction();

	private slots:
		void ConformButtonClicked();
		void QuitButtonClicked();

	private:
		Ui::SceneSelectionWindow* ui;
		NewMainWindow new_window;
	};

	int RunAPP(int argc, char* argv[]);

	void LoadImageInQt();
}