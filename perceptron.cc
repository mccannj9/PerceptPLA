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

void reset_stream(ifstream& file) {
  file.clear();
  file.seekg(0, ios::beg);
}

void feed_data(string data_as_string, vector<double>& data) {
// void feed_data(ifstream datafile, vector<double>& data) {
  // string line;
  // getline(datafile, line);
  
  auto tokens = split(data_as_string, ' ');
  vector<double> swap_in;
  for (auto datum: tokens) {
    swap_in.push_back(stod(datum));
  }
  data.swap(swap_in);
}

void feed_data(string data_as_string, vector<int>& data) {
// void feed_data(ifstream datafile, vector<int>& data) {
  // string line;
  // getline(datafile, line);
  
  auto tokens = split(data_as_string, ' ');
  vector<int> swap_in;
  for (auto datum: tokens) {
    swap_in.push_back(stoi(datum));
  }
  data.swap(swap_in);
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
void update_weights(
  vector<double>& weights, const vector<double>& inputs, T scalar, double learning_rate
) {
  for (size_t i = 0; i < inputs.size(); i++) {
    weights[i] += scalar * learning_rate * inputs.at(i);
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

vector<double> initialize_weights(int dim, long seed) {
  
  default_random_engine generator;
  generator.seed(seed);
  uniform_real_distribution<double> distribution(-1.0, 1.0);
  
  vector<double> w;
  w.reserve(dim);
  for (int i = 0; i < dim; i++) {
    w.push_back(distribution(generator));
  }

  return w;
}

vector<double> learn(
  ifstream& data, ifstream& labels, int data_dim, int labels_dim,
  long max_iters=1000, long seed=1, double learning_rate=0.01
) {
  
  vector<double> w = initialize_weights(data_dim, seed);

  vector<double> input_data;
  input_data.reserve(data_dim);

  vector<int> output_labels;
  output_labels.reserve(labels_dim);

  string msg = "Iter";
  short sp {0};
  int iters {0};

  string line_of_data;
  string line_of_labels;

  while(data.good()) {

    getline(data, line_of_data);
    getline(labels, line_of_labels);
    // cout << "got lines" << endl;

    if (data.eof()) {
      data.close();
      break;
    }

    feed_data(line_of_data, input_data);
    feed_data(line_of_labels, output_labels);
    // cout << "Fed the data" << endl;
    // Log(msg, iters, w);
    // Log(msg, iters, input_data);
    // test_scalar_product = scalar_product(input_data, w);
    // cout << "Got scalar product" << test_scalar_product << endl;
    

    sp = check_sign(scalar_product(input_data, w));
    // cout << "Checked the sign" << endl;

    if (sp != output_labels.at(0)) {
      update_weights(w, input_data, -sp, learning_rate);
      iters++;
      Log(msg, iters, w);
      // start reading file from the beginning again
      reset_stream(data);
      reset_stream(labels);
    }

    if (iters > max_iters) {
      break;
    }
  }

  return w;
}


vector<double> learn(
  const vector<double>& data, const vector<short> labels, int dim,
  long max_iters=1000, long seed=1, double learning_rate=0.01
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
        update_weights(w, input, -sp, learning_rate);
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

  // convergence sped up greatly with appropriate learning rate

  if (argc != 6) {
    cout << "Usage: ./perceptron data_path labels_path max_iters seed learning_rate" << endl;
    return 1;
  }

  if (atof(argv[5]) <= 0) {
    cout << "5th arg (learning_rate) MUST be > 0" << endl;
    return 2;
  }

  vector<double> data;
  vector<short> labels;

  int data_dim = get_data_dim(argv[1]);
  int labels_dim = get_data_dim(argv[2]);
  cout << data_dim << "data dim" << endl;
  cout << labels_dim << "labels dim" << endl;
  load_data(argv[1], data);
  load_data(argv[2], labels);

  ifstream input_data;
  ifstream output_labels;
  
  input_data.open(argv[1]);
  output_labels.open(argv[2]);

  // vector<double> w = learn(data, labels, dim, atol(argv[3]), atol(argv[4]), atof(argv[5]));
  vector<double> w = learn(
    input_data, output_labels, data_dim, labels_dim,
    atol(argv[3]), atol(argv[4]), atof(argv[5])
  );

  return 0;
}
