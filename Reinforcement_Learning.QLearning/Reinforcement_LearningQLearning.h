#ifndef QLEARNING_H
#define QLEARNING_H

#include <QtWidgets/QMainWindow>
#include "ui_Reinforcement_LearningQLearning.h"
#include "Constants.h"
#include "Environment.h"
#include "QLearningAgent.h"
#include "SARSA.h"
#include <QTimer>
#include <QkeyEvent>
#include "Point.h"
#include <vector>
#include <iostream>
#include <fstream>
//#include <windows.h>
#include <thread>
#include <utility>

using namespace std;

class Reinforcement_LearningQLearning : public QMainWindow
{
	Q_OBJECT

public:
	Reinforcement_LearningQLearning(QWidget *parent = Q_NULLPTR);
	~Reinforcement_LearningQLearning();

	void paintEvent(QPaintEvent* event);
	void keyPressEvent(QKeyEvent* event);
	double play_and_train(const int &episodes, QLearningAgent<pair<Point, Point>, action> &agent);
	void init_environment(const string &path);
	void print_table(QLearningAgent<pair<Point, Point>, action> &agent);

private:
	Ui::Reinforcement_LearningQLearningClass ui;
	QTimer *timer = nullptr;

	Environment environment;
	QLearningAgent<pair<Point, Point>, action> agent;
	SARSA agentSARSA;

	pair<Point, Point> headAndFood;

private slots:
	void loop();
};

#endif // QLEARNING_H