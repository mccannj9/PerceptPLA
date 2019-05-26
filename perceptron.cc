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

template<typename T>
vector<T> load_data(char* filename, vector<T>& data) {
  ifstream datafile;
  datafile.open(filename);

  // vector<T> data;

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

  vector<double> data;
  data = load_data(argv[1], data);
  vector<short> labels;
  labels = load_data(argv[2], labels);
  cout << "Data Size=" << data.size() << endl;
  cout << "Labels Size=" << labels.size() << endl;

  // for (size_t i = 0; i < data.size(); i++) {
  //   cout << data.at(i) << endl;
  // }

  return 0;
}
