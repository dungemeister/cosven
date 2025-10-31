## Description

COSmos Visualization Engine is a program for visualizing space objects and satellite systems. Its primary purpose is to visualize low-orbit satellite communications systems, including the process of transmitting traffic between satellites.

The application rendering planes with satellites as part of a low-orbit satellite constellation.
![DemoGif](assets/demo/demo.gif)

## Необходимые пакеты
```bash
    sudo apt install libgl-dev libglfw3-dev libglew-dev libglm-dev libstb-dev # основные графические библиотеки

    sudo apt install libgtk-3-dev #для nativefiledialog библиотеки
```

## Cmake

Для отрисовки GUI части используется Dear ImGUI библиотека, которая не добавлена в third_party.
```bash
    cd path_to_needed_dir
    git clone --depth=1 git@github.com:ocornut/imgui.git
```
для корректной сборки проекта в cmake следует указать IMGUI_DIRS переменную

```cmake
    set(IMGUI_DIRS path_to_needed_dir) # дефолтное значение ~/opt/imgui
```

## TODO
