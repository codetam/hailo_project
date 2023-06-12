# Mnist test - Inference

## Deployment
*inference_mnist.ipynb*

After the model is compiled, it can be used to run inference on the target device. The **HailoRT library** provides access to the device in order to load and run the model. This library is accessible from both C/C++ and Python APIs, but for the scope of the project only the Python API has been analysed. It also includes command line tools, such as the *HEF-parser*. In case the device is connected to the host through PCIe, the HailoRT library uses Hailo’s **PCIe driver** to communicate with the device.