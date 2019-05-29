#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <random>
#include <cmath>

using namespace std;

vector<string> split(const string& str, const char delim) {
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
void load_data(const char* filename, vector<T>& data) {
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
}

int get_data_dim(const char* filename) {
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
double scalar_product(vector<T>& x, vector<T>& y) {
  double result {0.0};

  for (size_t i = 0; i < x.size(); i++) {
    result += x.at(i) * y.at(i);
  }
  return result;
}

template<typename T>
vector<T> vector_addition(vector<T>& x, vector<T>& y) {
  vector<double> result;

  for (size_t i = 0; i < x.size(); i++) {
    result.push_back(x.at(i) + y.at(i));
  }

  return result;
}

template<typename T, typename U>
vector<T> scalar_multiplication(vector<T>& x, U a) {
  vector<double> result;

  for (size_t i = 0; i < x.size(); i++) {
    result.push_back(x.at(i) * a);
  }

  return result;
}

template<typename T>
void vector_norm(vector<T>& x) {
  double length {0.0};

  for (size_t i = 0; i < x.size(); i++) {
    length += pow(x.at(i), 2);
  }

  length = sqrt(length);

  for (size_t i = 0; i < x.size(); i++) {
    x[i] /= length;
  }
}

template<typename T>
void update_weights(vector<double>& weights, const vector<double>& inputs, T scalar) {
  for (size_t i = 0; i < inputs.size(); i++) {
    weights[i] += scalar * inputs.at(i);
  }
}

template <typename T>
short check_sign(T val) {
  return (T(0) < val) - (val < T(0));
}

template<typename T>
vector<T> peel(const vector<T>& data, int dim, int placeholder) {
  vector<T> peeled;
  for (int i = 0; i < dim; i++) {
    peeled.push_back(data.at(i+placeholder));
  }
  return peeled;
}

void Log(const string& msg, const int iters, const vector<double> weights) {
  cout << "[" << msg << ": " << iters << "] ";
  for (auto x: weights) {
    cout << x << ' ';
  }
  cout << endl;
}

vector<double> learn(
  const vector<double>& data, const vector<short>& labels,
  int dim, long max_iters=1000, long seed=1
) {

  default_random_engine generator;
  generator.seed(seed);
  uniform_real_distribution<double> distribution(-1.0, 1.0);

  // Randomly initialize the weight vector -> (-1.0, 1.0)
  vector<double> w;
  w.reserve(dim);
  for (int i = 0; i < dim; i++) {
    w.push_back(distribution(generator));
  }

  // data preparation for algorithm
  bool converged {false};
  int iters {0};

  vector<double> input;
  input.reserve(dim);

  int label_counter {0};
  string msg = "Iter";
  short sp {0};

  while(!converged) {
    converged = true;
    label_counter = 0;
    Log(msg, iters, w);
    for (size_t i = 0; i < data.size(); i+=dim) {
      input = peel(data, dim, i);
      sp = check_sign(scalar_product(input, w));

      if (sp != labels.at(label_counter)) {
        // label = -sp
        update_weights(w, input, -sp);
        converged = false;
        break;
      }
      label_counter++;
    }
    iters++;
    if (iters > max_iters) {
      converged = true;
    }
  }
  vector_norm(w);
  msg = "Final";
  Log(msg, iters-1, w);
  return w;
}

int main(int argc, char** argv) {

  if (argc != 5) {
    cout << "Usage: ./perceptron data_path labels_path max_iters seed" << endl;
    return 1;
  }

  vector<double> data;
  vector<short> labels;

  int dim = get_data_dim(argv[1]);
  load_data(argv[1], data);
  load_data(argv[2], labels);

  vector<double> w = learn(data, labels, dim, atol(argv[3]), atol(argv[4]));

  return 0;
}
