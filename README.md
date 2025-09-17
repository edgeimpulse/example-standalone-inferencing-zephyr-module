# Edge Impulse Example: standalone inferencing (Zephyr module)

This runs an exported impulse on most Zephyr development boards using Edge Impulse SDK Zephyr module and the Zephyr model deployment.

This project differs from [example-standalone-inferencing-zephyr](https://github.com/edgeimpulse/example-standalone-inferencing-zephyr) because it uses the [Edge Impulse SDK Zepyhr module](https://github.com/edgeimpulse/edge-impulse-sdk-zephyr) and the Zephyr library deployment for the model instead of copying the C++ library export.

## Import the Edge Impulse SDK
There are different ways to import the [Edge Impulse SDK Zepyhr module](https://github.com/edgeimpulse/edge-impulse-sdk-zephyr) to be used with this project:
1. Update the `west.yml` of your Zephyr repo adding the lines below for the SDK then call `west update` to downlaod the SDK into your Zepyr repo.

Here's the lines to add the EI-SDK:
```
    - name: edge-impulse-sdk-zephyr
      path: modules/edge-impulse-sdk-zephyr
      revision: ${SDK_VERSION}
      url: https://github.com/edgeimpulse/edge-impulse-sdk-zephyr
```

2. Use this project as a manifest repository, running from this project folder:
```
west init --local .
cd ..
west update
```
to pull or update the modules needed.

Check the [Zephyr module documentation](https://docs.zephyrproject.org/latest/develop/modules.html) for best practice.

## Update model
Go to the deployment page of your project and choose the Zephyr library option and extract the .zip in the parent folder of this project.

Then update the sample you want to test in [main.cpp](./src/main.cpp) :
```
static const float features[] = {
    // copy raw features here (for example from the 'Live classification' page)
    // see https://docs.edgeimpulse.com/docs/running-your-impulse-locally-zephyr
};
```

The extracted model should be placed here [model](./model/) see [CMakeLists.txt](./CMakeLists.txt)
```
list(APPEND ZEPHYR_EXTRA_MODULES ${CMAKE_CURRENT_SOURCE_DIR}/model)
```

## Build and flash
Build the project running
```
west build -p
```

Then flash it
```
west flash
```

You can specify the board you want to test by modifying the `.west/config` or by calling `west build -b <your board> -p`
