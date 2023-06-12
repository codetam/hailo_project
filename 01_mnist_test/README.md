# Mnist test

These are the files used when running the **Hailo Software Suite** with a new model.

The **MNIST handwritten digit classification** problem is a standard dataset used in computer vision and deep learning. I will now indicate the steps taken when working with the Hailo:

## Model building
*mnist_model.ipynb*

The chosen model has been built with **3 Dense layers** with the aid of Tensorflow. The mnist training and validation datasets have been imported from *tensorflow.keras.datasets*. The model only has an accuracy of 0.97, since the focus on the project is simply running a model on the Hailo Acceleration Module and optimizing it further would have brought me beyond the scope of the project.

## Parsing
*parsing.ipynb*

After the model has been prepared in its original format (a *SavedModel* file), it can be converted into **Hailo-compatible representation** files. The translation API receives the user’s model and generates an internal Hailo representation format (**HAR** compressed file, which includes *HN* and *NPZ* files). The HN model is a textual JSON output file and it saves the different layers used, while the weights are returned as a NumPy NPZ file.

## Optimizing
*optimization.ipynb*

After building the HAR representation, the next step is to convert the parameters from **float32** to **int8**, so that inference can be ran on a microcontroller. To convert the parameters, the model emulation has been run in native mode on a small set of images (the **calibration dataset** is made of the first 2000 validation images) and collect activation statistics. Based on these statistics, the calibration module will generate a **new network configuration** for the 8-bit representation. This includes int8 weights and biases, scaling configuration, and HW configuration.

## Compiling
*compilation.ipynb*

Now the model can be compiled into a HW compatible binary format with the extension **HEF**. The **Dataflow Compiler Tool** allocates hardware resources to reach the highest possible fps within reasonable allocation difficulty. Then the microcode is compiled and the HEF is generated. This whole step is performed internally, so from the user’s perspective the compilation is done by calling a single API.

## Deployment
*inference/inference_mnist.ipynb*

After the model is compiled, it can be used to run inference on the target device. The **HailoRT library** provides access to the device in order to load and run the model. This library is accessible from both C/C++ and Python APIs, but for the scope of the project only the Python API has been analysed. It also includes command line tools, such as the *HEF-parser*. In case the device is connected to the host through PCIe, the HailoRT library uses Hailo’s **PCIe driver** to communicate with the device.