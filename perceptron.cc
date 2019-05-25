#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

vector<string> split(const string& str, char delim) {
  // A function for parsing delimited lines of data
  vector<string> tokens;
  size_t prev = 0, pos = 0;
  do {
    pos = str.find(delim, prev);
    if (pos == string::npos) {
      pos = str.length();
    }
    string token = str.substr(prev, pos-prev);
    if (!token.empty()) {
      tokens.push_back(token);
    }
    prev = pos + 1;

  }
  while (pos < str.length() && prev < str.length());

  return tokens;
}

vector<double> load_data(char* filename) {
  ifstream datafile;
  datafile.open(filename);

  vector<double> data;

  if (datafile.is_open()) {
    string line;
    while (getline(datafile, line)) {
      auto data_points = split(line, ' ');
      for (string point : data_points) {
        // stod changes string to double
        data.push_back(stod(point));
      }
    }
    datafile.close();
  }

  else {
    cout << "Could not open file " << filename << endl;
  }

  return data;
}


int main(int argc, char** argv) {

  vector<double> data = load_data(argv[1]);
  cout << "Size=" << data.size() << endl;

  return 0;
}
