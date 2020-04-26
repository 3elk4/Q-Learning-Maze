#include "Reinforcement_LearningQLearning.h"
#include <QPainter>

Reinforcement_LearningQLearning::Reinforcement_LearningQLearning(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	this->setWindowTitle("Q-LEARNING");
	this->setFixedSize(this->size());
	this->pardialog.setModal(true);
	ui.statusBar->showMessage(p_states.at(program_state::none));

	auto model = SimpleFeatureModel();
	list<action> actions = { action::up, action::down, action::left, action::right };
	this->approxAgent = AproxQLearning(model, actions);
	this->approxAgent.init_weight_table();
	
	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(loopqlearning()));

	timerSARSA = new QTimer(this);
	connect(timerSARSA, SIGNAL(timeout()), this, SLOT(loopSARSA()));

	timerapproxqlerning = new QTimer(this);
	connect(timerapproxqlerning, SIGNAL(timeout()), this, SLOT(loopapproxqlearning()));
	//timer->start(250);
}

Reinforcement_LearningQLearning::~Reinforcement_LearningQLearning()
{
	if (this->timer != nullptr)
		delete timer;
	if (this->timerSARSA != nullptr)
		delete timerSARSA;
	if (this->timerapproxqlerning != nullptr)
		delete timerapproxqlerning;
}

void Reinforcement_LearningQLearning::init_environment(const string &path) {
	ifstream inFile;
	inFile.open(path);
	if (!inFile) {
		cerr << "Unable to open this file!";
		exit(1);
	}

	string line;
	int x = 0, y = 0, h = 0, w = 0;
	if (inFile >> line) {
		size_t pos = 0;
		string del = "x";
		if ((pos = line.find(del)) != string::npos) {
			w = stoi(line.substr(0, pos));
			line.erase(0, pos + del.length());
			h = stoi(line);
		}

		window_size::get_instance().set_height(h * window_size::get_instance().get_block_height());
		window_size::get_instance().set_width(w * window_size::get_instance().get_block_width());
	}

	Point temp;
	while (inFile >> line) {
		//w = line.size();
		//h++;
		x = 0;
		for (int i = 0; i < line.size(); ++i) {
			char c = line[i];
			temp = Point(x, y);
			if (c == e_sign.wall) {
				this->environment.set_environemnt_element(temp, env_type::wall);
			}
			else if (c == e_sign.path) {
				this->environment.set_environemnt_element(temp, env_type::path);
				//tutaj next states
				/*this->environment.set_next_state(temp, action::up, temp + Point(x, y - window_size::get_instance().get_block_height()));
				this->environment.set_next_state(temp, action::down, temp + Point(x, y + window_size::get_instance().get_block_height()));
				this->environment.set_next_state(temp, action::left, temp + Point(x - window_size::get_instance().get_block_width(), y));
				this->environment.set_next_state(temp, action::right, temp + Point(x + window_size::get_instance().get_block_width(), y));*/
			}
			else if (c == e_sign.head) {
				this->environment.set_environemnt_element(temp, env_type::path);
				//this->environment.set_initial_state(temp);
				this->headAndFood.first = temp;
			}
			else if (c == e_sign.food) {
				this->environment.set_environemnt_element(temp, env_type::path);
				//this->environment.set_end_state(temp);
				this->headAndFood.second = temp;
			}
			x += window_size::get_instance().get_block_width();
		}
		y += window_size::get_instance().get_block_height();
	}
	inFile.close();
}

void Reinforcement_LearningQLearning::keyPressEvent(QKeyEvent* event) {
	switch (event->key()) {
	/*case Qt::Key_Q: {
		pardialog.exec();
		cout << "RESULTS: " << pardialog.get_episodes() << " " << pardialog.get_alpha() << endl;

		double result = play_and_train_qlearning(500);
		cout << "Your result: " << result << endl;
		break;
	}
	case Qt::Key_S: {
		double result = play_and_train_SARSA(500);
		cout << "Your result: " << result << endl;
		break;
	}*/
	//case Qt::Key_F: {
	//	//tutaj obrazek z ustawianiem parametr�w
	//	double result = play_and_train_qlearning(500);
	//	cout << "DONE" << endl;
	//	this->agent.reset_parameters();
	//	//w�acz loopa
	//	this->timer->start(250);
	//	break;
	//}
	//case Qt::Key_T: {
	//	double result = play_and_train_SARSA(500);
	//	cout << "DONE" << endl;
	//	this->agentSARSA.reset_parameters();
	//	//w�acz loopa
	//	this->timerSARSA->start(250);
	//	break;
	//}
	default: {
		return;
	}
	}
} 

void Reinforcement_LearningQLearning::paintEvent(QPaintEvent * event)
{
	QPainter paint(this);

	paint.setBrush(Qt::blue);
	auto walls = this->environment.get_environment_elements(env_type::wall);
	for_each(walls.begin(), walls.end(),
		[&paint](Point w) {paint.drawRect(w.get_X(), w.get_Y(), window_size::get_instance().get_block_width(), window_size::get_instance().get_block_height()); });

	paint.setBrush(Qt::green);
	auto food = this->environment.get_environment_elements(env_type::food);
	paint.drawRect(this->headAndFood.second.get_X(), this->headAndFood.second.get_Y(), window_size::get_instance().get_block_width(), window_size::get_instance().get_block_height());

	paint.setBrush(Qt::red);
	paint.drawRect(this->headAndFood.first.get_X(), this->headAndFood.first.get_Y(), window_size::get_instance().get_block_width(), window_size::get_instance().get_block_height());

	draw_best_actions(this->current_agent, paint);
}

void Reinforcement_LearningQLearning::print_table(QLearningAgent<pair<Point, Point>, action> &agent) {

	//TODO: odfiltrowa� do obecnej planszy
	map<pair<Point, Point>, map<action, double>> qtable = agent.get_qtable();
	map<pair<Point, Point>, map<action, double>> filtred;
	for (auto &kv : qtable) {
		if(kv.first.second == this->headAndFood.second) filtred[kv.first] = kv.second;
	}

	printf("|%20s|%8s|%8s|%8s|%8s|\n", "STATE", "up", "right", "down", "left");
	for (auto &kv : filtred) {
		cout << "|" << kv.first.first << ", " << kv.first.second << "|";
		list<action> actions = { action::up, action::right, action::down, action::left };
		for (auto &a : actions) {
			auto s = to_string(kv.second.at(a));
			s.resize(8, ' ');
			cout << s << "|";
		}
		cout << endl;

		/*printf("|%8s, %8s|", kv.first.first, kv.first.second);
		printf("|%10f|", kv.second.at(action::up));
		printf("|%10f|", kv.second.at(action::right));
		printf("|%10f|", kv.second.at(action::down));
		printf("|%10f|\n", kv.second.at(action::left));*/
	}
}

void Reinforcement_LearningQLearning::print_weight_table()
{
	map<feature_names, double> qwtable = approxAgent.get_weight_table();

	printf("|%20s|\n", "FEATURES");
	for (auto &kv : qwtable) {
		auto f = features.at(kv.first);
		f.resize(20, ' ');
		cout << "|" << f << "|";
		cout << kv.second << endl;
	}	
}

void Reinforcement_LearningQLearning::draw_best_actions(QLearningAgent<pair<Point, Point>, action>& agent, QPainter &painter)
{
	painter.setBrush(Qt::blue);
	int offset = window_size::get_instance().get_width() + window_size::get_instance().get_block_width();
	auto walls = this->environment.get_environment_elements(env_type::wall);
	for_each(walls.begin(), walls.end(),
		[&painter](Point w) {painter.drawRect(w.get_X() + window_size::get_instance().get_width() + window_size::get_instance().get_block_width(), w.get_Y(), window_size::get_instance().get_block_width(), window_size::get_instance().get_block_height()); });

	auto states = this->environment.get_all_states();
	list<pair<Point, Point>> filtred;
	for (auto &kv : states) {
		if (kv.second == this->headAndFood.second) filtred.push_back(kv);
	}
	painter.setPen(Qt::darkGreen);
	auto paths = this->environment.get_environment_elements(env_type::path);
	for (auto &p : paths) {
		for (auto &f : filtred) {
			if (p == f.first) {
				auto a = agent.get_best_action(f);
				if (a.has_value()) {
					auto s = this->actions.at(a.value());
					painter.drawText(p.get_X() + offset, p.get_Y(), window_size::get_instance().get_block_width(), window_size::get_instance().get_block_height(), Qt::AlignCenter, s.c_str());
				}	
			}
		}
	}

}





