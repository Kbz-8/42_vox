#!/bin/bash

# Update Vulkan headers
rm -rf ThirdParty/vulkan
wget https://github.com/KhronosGroup/Vulkan-Headers/archive/main.zip -O vulkan-headers.zip
unzip -o vulkan-headers.zip -d ThirdParty/
mv ThirdParty/Vulkan-Headers-main/include/vulkan ThirdParty/
rm -rf ThirdParty/Vulkan-Headers-main
rm vulkan-headers.zip
