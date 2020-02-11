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
#include <string>
#include "ParameterDialog.h"

using namespace std;

class Reinforcement_LearningQLearning : public QMainWindow
{
	Q_OBJECT

public:
	Reinforcement_LearningQLearning(QWidget *parent = Q_NULLPTR);
	~Reinforcement_LearningQLearning();

	void paintEvent(QPaintEvent* event);
	void keyPressEvent(QKeyEvent* event);


	double play_and_train_qlearning(const int &episodes);
	double play_and_train_SARSA(const int &episodes);

	double play_and_train_qlearning_test(const int &episodes);
	double play_and_train_SARSA_test(const int &episodes);

	bool do_step_qlearning();
	bool do_step_SARSA();


	void init_environment(const string &path);
	void print_table(QLearningAgent<pair<Point, Point>, action> &agent);
	void show_episode(int current_episode, list<int> episodes);
	void draw_best_actions(QLearningAgent<pair<Point, Point>, action> &agent, QPainter &painter);

private:
	Ui::Reinforcement_LearningQLearningClass ui;
	QTimer *timer = nullptr;
	QTimer *timerSARSA = nullptr;

	list<int> episodes_to_show = {0, 300, 500, 1000, 1500, 1900};

	Environment environment;
	QLearningAgent<pair<Point, Point>, action> agent;
	SARSA agentSARSA;

	pair<Point, Point> headAndFood;
	map<action, string> actions = { {action::up, "u"}, 
									{action::right, "r" },
									{action::left, "l" },
									{action::down, "d" } };

	ParameterDialog pardialog = ParameterDialog(this);
private slots:
	void loop();
	void loopSARSA();
};

#endif // QLEARNING_H