#include <algorithm> //ReLU
#include <cmath>     //raíces cuadradas
#include <iostream>  //salida por terminal
#include <random>    //para inicializar los pesos
#include <vector>    //vectores

/*Filosofia
Todo lo que era tensor nD pasará a ser (n+1)D
Idea: si quiero que la ANN sea escalable por input del usuario, puedo hacer que
sea aquél el que introduzca el tamaño de la primera dimensión del tensor. Por
ende, si en los pesos el usuario exige que weights.size() sea 3, entonces habrá
3 grupos de conexiones, y por lo tanto 4 capas, considerando, pues, 2 capas
ocultas.
*/

struct annFunctions {
  /*=======ACTIVATION FUNCTIONS=======*/
  /*
  double sigmoid(double z) { return 1 / (1 + std::exp(z)); }
  double dsigmoid(double z) { return sigmoid(z) * (1 - sigmoid(z)); }
  */
  double ReLU(double z) {
    double relu = z > 0 ? z : 0;
    return relu;
  }
  double dReLU(double z) {
    double drelu = z > 0 ? 1 : 0;
    return drelu;
  }
};

struct ANN {
  annFunctions annFunction;

  std::vector<std::vector<std::vector<double>>> weights;
  std::vector<std::vector<double>> bias;

  std::vector<std::vector<double>> zCache, sigmaCache;

  double lr;

  ANN(const std::vector<int>
          &hiddenSizes, // hiddensizes = {numero neuronas capa 1, numero
                        // neuronas capa 2, etc}
      const int inputSize, const int outputSize,
      double learningRate)
      :

        weights(hiddenSizes.size() +
                2), //+1 capa de entrada y +1 capa de salida (weights[0] no lo
                    // usaremos pero si no las matemáticas se vuelven más
                    // complicadas)
        bias(hiddenSizes.size() + 2), zCache(hiddenSizes.size() + 2),
        sigmaCache(hiddenSizes.size() + 2),

        lr(learningRate) {
    std::vector<int> layerSizes(weights.size());
    layerSizes[0] = inputSize;
    for (int i = 0; i < hiddenSizes.size(); i++)
      layerSizes[i + 1] = hiddenSizes[i];
    layerSizes.back() = outputSize;

    for (int k = 1; k < layerSizes.size(); k++) {
      weights[k].resize(layerSizes[k - 1], std::vector<double>(layerSizes[k]));
      bias[k].resize(layerSizes[k], 0.0);
      zCache[k].resize(layerSizes[k], 0.0);
      sigmaCache[k].resize(layerSizes[k], 0.0);
    }

    std::cout << "Architecture created: " << inputSize << " - ";
    for (int k = 0; k < hiddenSizes.size(); k++)
      std::cout << hiddenSizes[k] << " - ";
    std::cout << outputSize << "\n";
  }

  void initWeights() {
    std::random_device rd;
    std::mt19937 gen(rd());

    int layerNumber = weights.size() - 1;
    for (int k = 1; k <= layerNumber; k++) {
      int neuralInputs = weights[k].size(),
          neuralOutputs = weights[k][0].size();

      double stddev = std::sqrt(2.0 / neuralInputs);
      std::normal_distribution<double> dist(0.0, stddev);

      for (int i = 0; i < neuralOutputs; i++) {
        for (int j = 0; j < neuralInputs; j++) {
          weights[k][j][i] = dist(gen);
        }
      }
    }

    std::cout << "Weights have been initialized." << std::endl;
  }

  /*=======FORWARD=======*/
  // Forward de cada capa
  void forwardDense(const std::vector<double> &x) {

    int layerNumber = bias.size() - 1;     // contando el cero como capa
    for (int k = 1; k <= layerNumber; k++) // recorremos capas
    {

      int neuralInputs = weights[k].size(),
          neuralOutputs =
              weights[k][0].size(); // # neuronas capa anterior / posterior

      for (int i = 0; i < neuralOutputs;
           i++) // recorremos neuronas capa posterior
      {
        zCache[k][i] = bias[k][i];

        if (k == 1) {
          for (int j = 0; j < neuralInputs;
               j++) // recorremos neuronas capa anterior (caso input layer)
            zCache[k][i] += x[j] * weights[k][j][i];
        } else {
          for (int j = 0; j < neuralInputs;
               j++) // recorremos neuronas capa anterior (caso no-input layer)
            zCache[k][i] += sigmaCache[k - 1][j] * weights[k][j][i];
        }
        if (k == layerNumber) {
          sigmaCache[k][i] = zCache[k][i];
        } else {
          sigmaCache[k][i] = annFunction.ReLU(zCache[k][i]);
        }
      }
    }
  }

  /*=======LOSS FUNCTION=======*/
  double loss(const double yhat, const double y) {
    return (y - yhat) * (y - yhat);
  }
  double lossDerivative(const double yhat, const double y) {
    return 2.0 * (yhat - y);
  }

  /*=======BACKPROP=======*/
  // DELTA
  std::vector<std::vector<double>> backpropDense(const double dL,
                                                 std::vector<double> &dI) {
    /*
    Gradiente de las demás capas. Su tamaño depende de las neuronas de cada
    capa, que se mantiene abstraído. Por ende, hacemos resize mediante zCache,
    en el cual sí están dichos tamaños
    */
    int layerNumber = weights.size() - 1;

    std::vector<std::vector<double>> delta(weights.size());
    for (int layerIndex = 1; layerIndex <= layerNumber; layerIndex++)
      delta[layerIndex].resize(zCache[layerIndex].size());

    for (int k = layerNumber; k > 0; k--) {
      if (k == layerNumber) {
        int neuralOutputs = weights[k][0].size();
        for (int i = 0; i < neuralOutputs; i++)
          delta[k][i] = dL;
      } else {
        int neuralInputs = weights[k + 1].size(),
            neuralOutputs = weights[k + 1][0].size();
        for (int j = 0; j < neuralInputs; j++) { // recorremos la capa anterior
          double s = 0.0;
          for (int i = 0; i < neuralOutputs;
               i++) { // recorremos la capa posterior
            s += weights[k + 1][j][i] * delta[k + 1][i];
          }
          delta[k][j] = s * annFunction.dReLU(zCache[k][j]);
        }
      }
    }

    return delta;
  }

  void parameterUpdate(const std::vector<double> &x,
                       const std::vector<std::vector<double>> &delta) {

    std::vector<std::vector<std::vector<double>>> dW(weights.size());
    for (int k = 1; k < weights.size(); k++) {
      dW[k].resize(weights[k].size());
      for (int j = 0; j < weights[k].size(); j++)
        dW[k][j].resize(weights[k][j].size(), 0.0);
    }

    std::vector<std::vector<double>> db(bias.size());
    for (int k = 1; k < bias.size(); k++)
      db[k].resize(bias[k].size(), 0.0);

    int layerNumber = weights.size() - 1;

    for (int k = 1; k <= layerNumber; k++) {
      int neuralInputs = weights[k].size(),
          neuralOutputs = weights[k][0].size();
      for (int i = 0; i < neuralOutputs; i++) {
        db[k][i] = delta[k][i];

        if (k == 1) {
          for (int j = 0; j < neuralInputs; j++) {
            dW[k][j][i] = delta[k][i] * x[j];
          }
        } else {
          for (int j = 0; j < neuralInputs; j++) {
            dW[k][j][i] = delta[k][i] * sigmaCache[k - 1][j];
          }
        }
      }
    }

    for (int k = 1; k <= layerNumber; k++) {
      int neuralInputs = weights[k].size(),
          neuralOutputs = weights[k][0].size();

      for (int i = 0; i < neuralOutputs; i++) {
        bias[k][i] -= lr * db[k][i];

        for (int j = 0; j < neuralInputs; j++)
          weights[k][j][i] -= lr * dW[k][j][i];
      }
    }
  }

  void train(const std::vector<std::vector<double>> &X,
             const std::vector<int> &Y, int epochs) {
    std::cout << "Training will initialize." << std::endl;

    //===Training Loop===
    double bestLoss = 5.0;
    double delta = 2e-2;
    for (int e = 0; e < epochs; e++) {
      double L = 0.0;
      for (int sample = 0; sample < X.size(); sample++) {
        //===FORWARD===
        forwardDense(X[sample]);
        double Yhat = sigmaCache.back()[0];

        //===LOSS FUNCTION===
        L += loss(Yhat, Y[sample]);
        double dL = lossDerivative(Yhat, Y[sample]);

        //===BACKWARD===
        std::vector<double> dI;
        std::vector<std::vector<double>> delta = backpropDense(dL, dI);
        parameterUpdate(X[sample], delta);
      }
      /*
      if(L < bestLoss - delta)
      {
          bestLoss = L;
      }
      else{
          lr *= 0.9;
      }
          */

      if (e % 5000 == 0 || e == epochs - 1) {
        std::cout << "Mean Loss value = " << L / static_cast<double>(Y.size())
                  << " (epoch " << e << ")" << std::endl;
      }
    }

    std::cout << "Training has finished." << std::endl;
  }

  void validation(const std::vector<double> &x) {
    forwardDense(x);
    double Yhat = sigmaCache.back()[0];

    if (Yhat <= 0.5) {
      Yhat = 0.0;
    } else {
      Yhat = 1.0;
    }

    std::cout << "Input: (" << x[0] << ", " << x[1] << ")" << std::endl;
    std::cout << "Output: " << Yhat << std::endl;
  }
};

int main() {
  /*======DATASET=======*/
  // si el primer valor del dataset es 0, la primera propragación será
  // completamente nula por otra parte, si los inputs son demasiado grandes,
  // explosive gradient => por eso no en grados
  std::vector<std::vector<double>> X = {
      {0, 1},     {0, 0},     {1, 0},     {1, 1},    {0.2, 0.5},
      {0.2, 0.2}, {0.5, 0.5}, {0.5, 0.7}, {0.7, 0.7}}; // 2 neuronas de entrada
  std::vector<int> Y = {1, 0, 1, 0, 1, 0, 0, 1, 0};    // 1 neurona a la salida

  std::vector<int> hiddenSizes = {10};
  double learningRate = 0.005;
  ANN ann(hiddenSizes, X[0].size(), 1, learningRate);

  ann.initWeights();
  ann.train(X, Y, 30000);

  ann.validation({0.2, 0.5});

  return 0;
}
