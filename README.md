# PCA9536 Driver for MasterPLC / MasterSCADA 4D

Драйвер для сервера автоматизации СА-02м, работающий в среде MasterPLC (MasterSCADA 4D) и управляющий модулем расширения на базе I2C GPIO-экспандера `PCA9536` по адресу `0x41`.

Репозиторий содержит:

- протокольный модуль `mplc_protocol_ca02m`, который отдает в проект MasterPLC диагностические данные Linux и статусы периферии;
- function block `mplc_fb_ca02m` для управления выходами `PCA9536` и линией питания USB;
- SDK-окружение и linux-сценарии сборки `.so` модулей для загрузки в MasterPLC.

Информация по серверу СА-02м и его Linux-образу: <https://github.com/CYNTRON-git/SA-02m-web-build>

## Что умеет драйвер

Протокольный модуль читает и публикует в MasterPLC:

- температуру CPU;
- загрузку CPU;
- использование RAM;
- использование корневого раздела;
- время работы Linux (`UPTIME`, в полных часах);
- системное время;
- время RTC;
- наличие и состояние USB-модема;
- наличие, размер и заполнение USB-накопителя;
- наличие, размер и заполнение SD-карты.

Выходные каналы и function block управляют:

- `BLUE_LED`;
- `RED_LED`;
- `BUZZER`;
- `DOUT`;
- `USB_POWER`.

## Проверка корректности

При ревизии проекта была проверена логика расчета времени работы.

Исправление, внесенное в этот репозиторий:

- раньше `UPTIME` брался из `/proc/uptime`, переводился в часы и округлялся функцией `round()`;
- это давало завышение, например `1.6` часа превращалось в `2`, хотя два полных часа еще не прошли;
- теперь `UPTIME` считается как количество **полных часов** (`floor(seconds / 3600)`), дополнительно добавлено ограничение по диапазону `int16_t`, чтобы исключить переполнение при длительной работе устройства.

## Структура проекта

```text
API/
├─ README.md
├─ build-driver.sh
├─ include/                      # заголовки SDK MasterPLC / MasterSCADA 4D
├─ lib/                          # заголовки и зависимости SDK
├─ examples/
│  ├─ mplc_protocol_ca02m/       # протокольный модуль диагностики и PCA9536
│  └─ mplc_fb_ca02m/             # function block для управления выходами
└─ platform/
   └─ linux/
      └─ api/
         ├─ Makefile
         └─ makedrv.sh
```

## Основные файлы

- `examples/mplc_protocol_ca02m/simple_test_protocol.cpp` - основная логика протокольного драйвера;
- `examples/mplc_protocol_ca02m/simple_test_protocol.h` - объявление класса протокола;
- `examples/mplc_fb_ca02m/test_fb.cpp` - function block для прямого управления выходами;
- `platform/linux/api/Makefile` - сборка Linux `.so` модулей;
- `platform/linux/api/makedrv.sh` - platform-specific настройки toolchain;
- `build-driver.sh` - верхнеуровневый скрипт сборки, добавленный в этом репозитории.

## Аппаратная модель

Для СА-02м используется I2C GPIO-экспандер `PCA9536`:

- I2C bus: `2`
- I2C address: `0x41`
- регистр направления: `0x03`
- регистр выходов: `0x01`

Логика управления активна по низкому уровню, поэтому в коде используется битовая маска, где включение выхода очищает соответствующий бит.

Соответствие выходов в текущей реализации:

- bit `0` - `RED_LED`
- bit `1` - `DOUT`
- bit `2` - `BUZZER`
- bit `3` - `BLUE_LED`

Дополнительно линия `USB_POWER` управляется через `gpioset 0 268=0/1`.

## Требования для сборки

Для сборки под Linux нужен SDK MasterPLC и кросс-компилятор из вашего окружения.

Минимально требуется:

- Linux, WSL или Git Bash с `bash` и `make`;
- компилятор/кросс-компилятор, соответствующий целевой платформе;
- runtime-библиотеки MasterPLC:
  - `masterplc.so`
  - `mplc_archive.so`
  - `mplcshare.so`
  - `opcua.so`
  - `liblua.so`
  - `mplc_events.so`

По умолчанию `build-driver.sh` ищет эти библиотеки в `/opt/mplc4`, но путь можно переопределить через `--deps-dir` или переменную окружения `MPLC_LIB_SO_DIR`.

## Быстрая сборка

Рекомендуемая команда для СА-02м:

```bash
chmod +x build-driver.sh
./build-driver.sh linux-armv7hf
```

Если библиотеки MasterPLC лежат не в `/opt/mplc4`:

```bash
./build-driver.sh linux-armv7hf --deps-dir /path/to/mplc4
```

Результаты сборки будут собраны в каталог:

```text
dist/linux-armv7hf/
```

Обычно там появляются:

- `mplc_protocol_ca02m.so`
- `mplc_fb_ca02m.so`
- `mplc_protocol_ca02m.map`
- `mplc_fb_ca02m.map`
- `log.txt`

## Поддерживаемые платформы

Скрипт `platform/linux/api/makedrv.sh` поддерживает несколько целей, включая:

- `cyntron SA-02m` - целевая платформа этого драйвера, собирается через цель `linux-armv7hf`
- `linux-x86`
- `linux-x64`
- `linux-armv7hf`
- `linux-armv8`
- другие специализированные цели из SDK

Для просмотра полного списка:

```bash
cd platform/linux/api
bash ./makedrv.sh help
```

## Ручная сборка без обертки

Если нужна сборка старым способом:

```bash
cd platform/linux/api
mkdir -p mplc_lib_so
cp /opt/mplc4/masterplc.so mplc_lib_so/
cp /opt/mplc4/mplcshare.so mplc_lib_so/
cp /opt/mplc4/mplc_archive.so mplc_lib_so/
cp /opt/mplc4/mplc_events.so mplc_lib_so/
cp /opt/mplc4/opcua.so mplc_lib_so/
cp /opt/mplc4/liblua.so mplc_lib_so/
bash ./makedrv.sh linux-armv7hf
```

## Установка на СА-02м

После сборки скопируйте `.so` файлы на устройство в каталог с пользовательскими драйверами MasterPLC.

Типовой порядок:

1. Собрать модуль на Linux/WSL.
2. Передать `mplc_protocol_ca02m.so` и при необходимости `mplc_fb_ca02m.so` на СА-02м по `scp`/WinSCP.
3. Поместить файлы в каталог, из которого MasterPLC загружает внешние модули.
4. Перезапустить runtime MasterPLC или перезагрузить устройство.
5. Проверить, что каналы в проекте обновляются, а выходы `PCA9536` корректно переключаются.

Так как layout установленной среды может отличаться между сборками образа, финальный путь размещения `.so` лучше сверить на конкретном устройстве.

## Диагностика на устройстве

Проверить наличие `PCA9536` на I2C-шине:

```bash
i2cdetect -y 2
```

Настроить все линии как выходы:

```bash
i2cset -y 2 0x41 0x03 0x00
```

Пример записи маски выходов:

```bash
i2cset -y 2 0x41 0x01 0x0e
```

Проверить uptime Linux:

```bash
awk '{print $1}' /proc/uptime
```

Проверить RTC:

```bash
hwclock -r -f /dev/rtc0
```

## Замечания по проекту

- В репозитории присутствует SDK-дерево `include/` и `lib/`, необходимое для сборки модулей.
- `mplc_fb_ca02m` использует счетчик циклов `USB_Counter` для импульса USB reset, поэтому фактическая длительность зависит от периоду вызова `Execute()` в runtime.
- Если на конкретной сборке СА-02м внешний RTC зарегистрирован не как `/dev/rtc0`, строку чтения `RTC_TIME` нужно адаптировать под реальное устройство.

## Лицензирование

Условия использования исходников SDK и runtime-библиотек MasterPLC определяются лицензией правообладателя SDK.
