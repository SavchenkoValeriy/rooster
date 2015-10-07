# Rooster (Rooster Static Analyzer) #

## Как собрать C++ код ##

### Исходный код Clang и LLVM ###

```
mkdir Rooster && cd Rooster
export ROOSTER_HOME=$PWD
git clone http://llvm.org/git/llvm.git
cd llvm/tools
git clone http://llvm.org/git/clang.git
cd clang/tools
git clone http://llvm.org/git/clang-tools-extra.git extra
cd extra
```
### Клонирование кода Rooster ###
```
git clone git@bitbucket.org:ValeriySavchenko/rooster.git

```
или
```
git clone https://ValeriySavchenko@bitbucket.org/ValeriySavchenko/rooster.git
```
(для использования первого варианта, необходимо добавить ssh ключи к вашей учетной записи - [how-to](https://confluence.atlassian.com/bitbucket/set-up-ssh-for-git-728138079.html))

### Добавление Rooster в процесс сборки LLVM+Clang ###

Откройте файл Rooster/llvm/tools/clang/tools/extra/CMakeLists.txt
Добавьте в него строку:
```
add_subdirectory(rooster)
```

### Установка зависимостей сборки
```
sudo apt-get install gcc g++ python libtool m4 autoconf automake libtool zlib1g-dev ninja cmake
```
Подробнее можно прочитать [здесь](http://llvm.org/docs/GettingStarted.html#software)

Установка указана для Linux систем, на других системах установите аналогичные пакеты.

### Сборка
Для работы рекомендуется создавать отдельные директории для объектных файлов и для сборок, причем держать отдельные поддиректории для разных видов сборки (Release, Debug)

```
cd $ROOSTER_HOME
mkdir build
mkdir build/Release
mkdir build/Debug
```
#### Release сборка
```
cd $ROOSTER_HOME/build/Release
cmake ../../llvm/ -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=../../install/Release -G Ninja
ninja -j5
ninja install
```
```CMAKE_BUILD_TYPE``` - переменная для типа сборки

```CMAKE_INSTALL_PREFIX``` - пременная для пути установки Clang и остальных инструментов

О других переменных CMake для сборки можно прочитать [здесь](http://llvm.org/docs/CMake.html).

```-G Ninja``` - говорит CMake о том, что целевая система сборки - Ninja

```ninja -j5``` - сборка в 5 потоков (рекомендуется собирать проекты с числом потоков = число ядер + 1)

```ninja install``` - установка собранных файлов и библиотек в директорию для установки

#### Debug сборка
```
cd $ROOSTER_HOME/build/Debug
cmake ../../llvm/ -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=../../install/Debug -G Ninja
ninja -j5
ninja install
```

Получившиеся инструменты будут находиться в ```$ROOSTER_HOME/install/Release/bin``` и ```$ROOSTER_HOME/install/Debug/bin```

Собирать LLVM+Clang *приятнее* c помощью Clang, для этого перед выполнением команды CMake можно выполнить следующее (при условии того, что один раз сборка уже была осуществлена)
```
export СС=$ROOSTER_HOME/install/Release/bin/clang
export СXX=$ROOSTER_HOME/install/Release/bin/clang++
```

### Запуск на примере

Среди исполняемых файлов будет также и ```rooster```. Для того, чтобы запустить его на тестовом файле test.cpp выполните
```
$ROOSTER_HOME/install/bin/rooster test.cpp --
```
После -- указываются опции специфичные для компилятора, например, стандарт C++11:
```
$ROOSTER_HOME/install/bin/rooster test.cpp -- -std=c++11
```