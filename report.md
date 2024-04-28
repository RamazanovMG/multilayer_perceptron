## Study on the Dependence of Learning Speed and Quality on Perceptron Inputs


| Parameters (hidden layers, learning rate, size of layers, epochs)                  | Training time (s) | Final success rate (%) | Notes |
|-------------------|---------|----------|--|
| 2; 0.01; 32; 7 | 42.4  | 72.9 | The model lacks neurons for quality training |
| 4; 0.01; 128; 7 | 204.8  | 82.5 | Heavy perceptron falls into a local minimum due to low learning rate  |
| 2; 0.1; 128; 3 | 68.5  | 83.6 | Perfectly balanced, as all things should be |
| 2; 0.1; 128; 7 | 161.2  | 85.6 | Same as above with additional epochs, training continues successfully |
| 2; 1; 64; 7 | 77.5  | 57 | Neuron isn’t improving due to too high learning rate, model jumps over local minimum  |
| 4; 0.1; 128; 7 | 204.9  | 86.3 | optimal learning rate, high quality due to long training time|
