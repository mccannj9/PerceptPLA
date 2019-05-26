#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <random>

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

int get_data_dim(char* filename) {
  ifstream datafile;
  datafile.open(filename);

  if (datafile.is_open()) {
    string line;
    getline(datafile, line);
    auto data_points = split(line, ' ');
    datafile.close();
    return data_points.size();
  }

  else {
    return 0;
  }
}

template<typename T>
double scalar_product(vector<T> x, vector<T> y) {
  double result;

  for (size_t i = 0; i < x.size(); i++) {
    result += x.at(i) * y.at(i);
  }
  return result;
}

template <typename T>
int check_sign(T val) {
  return (T(0) < val) - (val < T(0));
}

template<typename T>
vector<T> peel(vector<T>& data, int dim, int placeholder) {
  vector<T> peeled;
  for (int i = 0; i < dim; i++) {
    peeled.push_back(data.at(i+placeholder));
  }
  return peeled;
}

vector<double> learn(vector<double> data, vector<short> labels, int dim, long seed) {
  bool converged {false};
  default_random_engine generator;
  generator.seed(seed);
  uniform_real_distribution<double> distribution(-1.0, 1.0);
  vector<double> w;

  for (int i = 0; i < dim; i++) {
    w.push_back(distribution(generator));
  }

  vector<double> input;
  int label_counter {0};

  while (!converged) {
    converged = true;
    label_counter = 0;
    for (size_t i = 0; i < data.size(); i+=dim) {
      input = peel(data, dim, i);
      auto p = scalar_product(w, input);
      for (auto x: input) {
        cout << x << ' ';
      }
      cout << labels.at(label_counter) << ' ' << check_sign(p) << endl;
      label_counter++;
    }
  }

  return w;
}

int main(int argc, char** argv) {

  vector<double> data;
  data = load_data(argv[1], data);
  vector<short> labels;
  labels = load_data(argv[2], labels);
  cout << "Data Size=" << data.size() << endl;
  cout << "Labels Size=" << labels.size() << endl;

  int dim = get_data_dim(argv[1]);
  vector<double> w = learn(data, labels, dim, atol(argv[3]));

  return 0;
}
