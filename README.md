# Introduction

This repository contains calibration files for ECCE detector simulation and reconstruction. It is independent from the https://github.com/sPHENIX-Collaboration/calibrations and https://github.com/eic/fun4all_calibrations . The most recent content of this repositry is pulled daily into the CVMFS build distribution and avaialble at `$CALIBRATIONROOT/` after source the ecce env. Every week, the content with archived into the weekly ana.<xyz> persistenty build.

# Local development

To test update locally, please `git clone` this repository and set `$CALIBRATIONROOT` to your local location: 
  
```bash
git clone git@github.com:ECCE-EIC/calibrations.git 
export CALIBRATIONROOT=$PWD/calibrations # or replace with absolute path if at different 
```

Then the [simulation and reconstruction macros](https://github.com/ECCE-EIC/macros) will pick up the local calibration files instead of the CVMFS versions. 
  
# Update to this repository

Please follow the standard [GitHub pull request workflow](https://docs.github.com/en/github/collaborating-with-pull-requests/proposing-changes-to-your-work-with-pull-requests/creating-a-pull-request) to suggest changes to this repository. 

Hint: it is possible to manually pull updates from https://github.com/eic/fun4all_calibrations and merge into this repositry during the local development. 
