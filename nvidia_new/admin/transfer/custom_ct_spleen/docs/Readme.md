# Description
A pre-trained model for volumetric (3D) segmentation of the spleen from CT image. 

# Model Overview
This model is trained using the runnerup [1] awarded pipeline of the "Medical Segmentation Decathlon Challenge 2018" using the AHnet architecture [2] with 32 training images and 9 validation images.

## Data
The training dataset is Task09_Spleen.tar from http://medicaldecathlon.com/.

The data must be converted to 1mm resolution before training:

    tlt-dataconvert -d ${SOURCE_IMAGE_ROOT} -r 1 -s .nii.gz -e .nii.gz -o ${DESTINATION_IMAGE_ROOT}

NOTE: to match up with the default setting, we suggest that ${DESTINATION_IMAGE_ROOT} match DATA_ROOT as defined in environment.json in this MMAR's config folder.

## Training configuration
The training was performed with command train_2gpu.sh, which required 12GB-memory GPUs.

Training Graph Input Shape: dynamic

Actual Model Input: 96 x 96 x 96

## Input and output formats
Input: 1 channel CT image

Output: 2 channels: Label 1: spleen; Label 0: everything else

## Scores
This model achieve the following Dice score on the validation data (our own split from the training dataset):

1. Spleen: ~0.94

# Availability
In order to access this model please apply for general access:

https://developer.nvidia.com/clara

This model is usable only as part of Transfer Learning & Annotation Tools in Clara Train SDK container. You can download the model from NGC registry as described in Getting Started Guide

# Disclaimer
This is an example, not to be used for diagnostic purposes

# References
[1] Xia, Yingda, et al. "3D Semi-Supervised Learning with Uncertainty-Aware Multi-View Co-Training." arXiv preprint arXiv:1811.12506 (2018). https://arxiv.org/abs/1811.12506.

[2] Liu, Siqi, et al. "3d anisotropic hybrid network: Transferring convolutional features from 2d images to 3d anisotropic volumes." International Conference on Medical Image Computing and Computer-Assisted Intervention. Springer, Cham, 2018. https://arxiv.org/abs/1711.08580.