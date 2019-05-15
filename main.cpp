#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <time.h>


class Writer {
public:
	virtual void update(std::string &s) = 0;
};


class Bulk {
	std::queue<std::string> bulk;
public:
	void Add(std::string &str) {
		bulk.emplace(str);
	}
	std::string Retrive() {
		std::string res = "";
		if (!bulk.empty()) {
			res = "bulk: ";
			while (!bulk.empty()) {
				res += bulk.front();
				bulk.pop();
				if (!bulk.empty())
					res += ", ";
			}
		}
		return res;
	}
};


class BulkMechanics {
	size_t N, n_cnt, n_brackets;
	Bulk bulk;
	std::vector<Writer*> subs;
public:
	BulkMechanics(size_t _N, Bulk &_bulk) : N(_N), bulk(_bulk), n_cnt(0), n_brackets(0) { }
	void Parse(std::string &str) {
		if (str == "{") {
			if(n_brackets == 0)
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
		std::string str = bulk.Retrive();
		if (!str.empty()) {
			for (auto s : subs) {
				s->update(str);
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
	void update(std::string &s) override {
		std::cout << s << std::endl;
	}
};

class file_writer : public Writer {
	std::ofstream logfile;
public:
	file_writer(BulkMechanics& mech) {
		mech.Subscribe(this);
		logfile.open("bulk" + std::to_string(time(NULL)) + ".log");
	}
	~file_writer() {
		logfile.close();
	}
	void update(std::string &s) override {
		logfile << s << std::endl;
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
