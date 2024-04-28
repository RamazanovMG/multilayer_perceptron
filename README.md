#### Multilayer perceptron is an implementation of a simple neural network using sigmoid activation function. Predicted letter is displayed in the box to the upper right corner.

Functionality:
- load/save image for prediction
- train on any given dataset using train-test split or cross-validation
- adjustable number of layers, size of layers, number of epochs, and number of folds for cross-validation training
- load/save model with calculated weights
- antialiasing


For installation, please execute the command **make install**.
Alternatively, open src/view/CMakeLists.txt file with Qt Creator, ensuring it is configured with version 6.5.1.

To see the accuracy of the model with different parameters, check report.md.


[demo.webm](https://github.com/RamazanovMG/multilayer_perceptron/assets/48622603/bb20279f-6ee5-4e22-873d-4eb76a4a0258)

> trained model demo


[training.webm](https://github.com/RamazanovMG/multilayer_perceptron/assets/48622603/f34167fe-1585-447c-902a-dedc8b02d6af)

> training in real time (success rate displayed in bottom right corner)
