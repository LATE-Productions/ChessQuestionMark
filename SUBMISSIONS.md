## SUBMISSIONS

# Project title

- Provide project title: ChessQuestionmark

# Team members

- Provide name of team members: LATE - Productions / Anton.B / Theo.J / Lukas.K

# Repository URL

- Provide repository URL: https://github.com/LATE-Productions/ChessQuestionMark

# Final tag

- Provide final tag for project: final-submission

# Assessed commit hash

- Provide hashcode for git commit on sumbissions: 

# Main interaction

- Provide the main interaction: The main interaction within our project is to move and rotate markers within the camera view, this is made by our integrated pipeline that goes through a series of checks and conditions to provide with correct and valid markers.

# Equipment and software

- Provide equipment and or software used: For equipment to run the program you need a computer with linux or windows OS, a camera is needed aswell. As for software, you need VsCode with VCPKG/OPENCV4 and Godot version 4.5 to run program propperly!

# Limitations

- Provide known limitations with the project: Our physical projections within godot have some slight issues with rotation when directed perfectly towards the camera, this is because rotation calculation euler or quaternions can see negative numbers and positive numbers as equal rotations, meaning the slight changes between less than 0 and more than 0 can result in jittering flips of said object. Some issues with validating marker at steep angles and far away, this can be the cause of rectification or recognition.

- Other known limitations is that within godot, we cannot produce multiple objects with the same marker indexes, this means only one object will be instansiated at the time per marker. This is not something we wanted, but the pipeline works with many of the same marker, altough our marker manager does not.

# Tests and Evidence

- What tests we have: We have a mix of tests, in datastorage there are tests specifically targetet towards the datastorage output and function. These tests are self made and bruteforced. We also have a tests folder containing doctests. Considering we were introduced to testing framework within the last 2 weeks of the course, we implemented a testing .build file to test and use doctest fully. Doctest is a decision we took instead of gtest, because we noticed that the compile time was significantly faster. Therefore we implemented doctest, which we have used seperatly for 2 different components/tasks. One was made for the pipeline builder and the other for marker rectification.

- What evidence we have: During the entire process of our project, the requirement for concrete evidence and saved files/docs/images etc has been unclear. We have instead used a small amount of evidence with pictures, and instead focused on a different .build file for, debugging, testing and running our project in godot. This means, during the process we have been using our own marker_pipeline.exe .build file to run debugg with different imshow windows. These different windows showcase different states and stages within the pipeline to debugg and tests different issues and unintended behavior with our pipeline. Therefore we have not save that much evidence during the process, but instead been working with debugging thoroughly to ensure our pipeline is what we want. 

# Date and time

- Provide date and time of presentation: 2026-11-06 / 13:00
