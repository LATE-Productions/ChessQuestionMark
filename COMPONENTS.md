## COMPONENTS FOR PIPELINE

| Header Files | Source Files | Creators |
|--------------|--------------|----------|
| N/A | libs/marker_pipeline/main.cpp | Theo.J / Lukas.K |
| pipeline/Components.hpp | N/A | Theo.J |
| pipeline/Concepts.hpp | N/A | Theo.J |
| Utilities.hpp | N/A | Theo.J |
| RingBuffer.hpp | N/A | Theo.J |
| CameraCapture.hpp | CameraCapture.cpp  | Theo.J |
| CameraCalibration.hpp | CameraCalibration.cpp | Anton.B / Lukas.K |
| Marker.hpp | N/A | Anton.B / Lukas.K / Theo.J |
| ImageRenderer.hpp | ImageRenderer.cpp | Lukas.K |
| MarkerDetection.hpp | MarkerDetection.cpp | Anton.B |
| MarkerRectification.hpp | MarkerRectificatin.cpp | Anton.B / Theo.J |
| MarkerTemplates.hpp | N/A | Theo.J |
| MarkerRecognition.hpp | MarkerRecognition.cpp | Lukas.K |
| MarkerPoseEstimator.hpp | MarkerPoseEstimator.cpp | Anton.B |
| DataStorage.hpp | DataStorage.cpp | Lukas.K |
| DataConversion.hpp | DataConversion.cpp | Anton.B / Lukas.K |


## COMPONENTS FOR PROJECT / GODOT

| Header Files | Source Files | Creators |
|--------------|--------------|----------|
| FrameSource.hpp | FrameSource.cpp | Theo.J |
| VideoSource.hpp | VideoSource.cpp | Theo.J |
| MarkerManager.hpp | MarkerManager.cpp | Anton.B / Theo.J / Lukas.K |

## TESTS FOR PIPELINE WITH DOCTEST

| Header Files | Source Files | Creators |
|--------------|--------------|----------|
| N/A | libs/marker_pipeline/tests/main.cpp | Doctest requirements |
| N/A | libs/marker_pipeline/tests/pipeline_builder.cpp | Theo.J |
| N/A | libs/marker_pipeline/tests/marker_rectification_tests.cpp | Anton.B |

## CMAKE

| CMake Script | Purpose | Creators |
|--------------|---------|----------|
| /cmakelist.txt | rootproject/gdextension | Theo.J |
| /libs/marker_pipeline/cmakelists.txt| static lib pipeline | Theo.J |
| /libs/marker_pipeline/tests/cmakelists.txt | static lib pipeline test | Theo.J |
| /cmake/gdextension.cmake | compile options gdextension dynamic lib/copy build to godot project/creating .gdextension file using cmake template | Theo.J |
| /cmake/godot-dev-configuration.cmake | set up gdextension development using cmake instead of scons | Theo.j |
| /cmake/compile-options | compile commands depending on compiler used | Theo.J |
| /cmake/copy-camera-calibration.cmake | copies camera calibration to godot project, if calibration missing, request developer | Theo.J |
| /libs/marker_pipeline/cmake/vcpkg_config.cmake | configures vcpkg env variables | Theo.J |
| /libs/marker_pipeline/cmake/triplets/x64-linux-pic.cmake | configures vcpkg triplet to allow ffmpeg to be built in a static non local lib | Theo.J |