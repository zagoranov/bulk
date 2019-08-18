#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <time.h>
#include <ctime>

class Writer {
public:
	virtual void update(std::vector<std::string> &vec, std::time_t firstCommTime) = 0;
};


class Bulk {
	std::queue<std::string> bulk;
public:
	void Add(std::string &str) {
		bulk.emplace(str);
	}
	bool isEmpty() const {
		return bulk.empty();
	}
	void Retrive(std::vector<std::string> &vec) {
		if (!bulk.empty()) {
			while (!bulk.empty()) {
				vec.push_back(bulk.front());
				bulk.pop();
			}
		}
	}
};


class BulkMechanics {
	size_t N, n_cnt, n_brackets;
	Bulk bulk;
	std::vector<Writer*> subs;
	std::time_t commTime;
public:
	BulkMechanics(size_t _N, Bulk &_bulk) : N(_N), bulk(_bulk), n_cnt(0), n_brackets(0) { }
	void Parse(std::string &str) {
		if (str == "{") {
			if (n_brackets == 0)
				Flash();
			++n_brackets;
		}
		else
			if (str == "}") {
				if (n_brackets > 0) {
					if (n_brackets == 1) {
						Flash();
					}
					--n_brackets;
				}
			}
			else {
				if(bulk.isEmpty())
					commTime = std::time(nullptr);
				bulk.Add(str);
				if (n_brackets == 0) {
					++n_cnt;
					if (n_cnt == N) {
						Flash();
					}
				}
			}
	}
	void Flash() {
		std::vector<std::string> vec;
		bulk.Retrive(vec);
		if (vec.size() > 0) {
			for (auto& s : subs) {
				s->update(vec, commTime);
			}
		}
		n_cnt = 0;
	}
	void Subscribe(Writer *writer) {
		subs.push_back(writer);
	}

};

class screen_writer : public Writer {
public:
	screen_writer(BulkMechanics& mech) {
		mech.Subscribe(this);
	}
	void update(std::vector<std::string> &vec, std::time_t firstCommTime) override {
		std::string str_screen = "bulk: ";
		size_t cnt = 0;
		for(const auto& s : vec)	{
			str_screen += s;
			if(++cnt < vec.size())
				str_screen += ", ";
		}
		std::cout << str_screen << std::endl;
	}
};

class file_writer : public Writer {
	std::ofstream logfile;
public:
	file_writer(BulkMechanics& mech) {
		mech.Subscribe(this);
	}
	~file_writer() {
		if(logfile.is_open())
			logfile.close();
	}
	void update(std::vector<std::string> &vec, std::time_t firstCommTime) override {
		logfile.open("bulk" + std::to_string(firstCommTime) + ".log");
		std::string str_file;
		for(const auto& s : vec) {
			str_file += s + "\n";
		}
		logfile << str_file << std::endl;
		logfile.close();
	}
};


int main(int argc, char* argv[])
{
	if (argc < 2) {
		std::cout << "You must provide the N argument\n" << std::endl;
		return 1;
	}
	size_t N = atoi(argv[1]);
	Bulk bulk;
	BulkMechanics mech{ N, bulk };

	screen_writer scr_wr(mech);
	file_writer file_wr(mech);

	std::string line;
	while (std::getline(std::cin, line))
	{
		if (!line.empty())
			mech.Parse(line);
	}
}
