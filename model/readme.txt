Edge Impulse model as Zephyr module

The model has ben tested with:
zephyr v3.7.1
west 1.5.0
Zephyr SDK 0.17.4

Put model-parameters, tflite-model and zephyr folders and CMakeList.txt inside your project.

Add the content of west.yml into your manifest to reference the Edge Impulse SDK version needed for this model.

Add the following to your CMakeList.txt:
list(APPEND ZEPHYR_EXTRA_MODULES ${CMAKE_CURRENT_SOURCE_DIR}/model_folder)

where model_folder is the path to where you copied the folders extracted.

