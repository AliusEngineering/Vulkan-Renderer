# Alius Vulkan VulkanRenderer

## Quick start

This is one of the many modules designated to achieve Alius modularity.
This repository contains all the necessary code for Alius renderer. This is achieved by depending on base repository
which provides pure virtual class to rely on when implementing renderer features.

Below you will find all the necessary guidelines to understand the module's architecture, development flow and
contribution details.

## VulkanRenderer features

The following list is an overall description of available renderer features at a certain moment (to be updated manually)
.

- Render the result of shader pass (no dynamic data passed, baked-in vertices)

## Namespacing

All Alius modules should reside in `AliusModules` namespace for convenience and universalization.

## Submodules

Each Alius module depends on some kind of base repository. The module code should inherit from the base class defined in
the base repository, so it can be used in Alius.

## Code style

This part is completely up to you. I prefer using Clang-Format and chose Mozilla style to use.