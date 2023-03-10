# Приложение для склейки и просмотра панорам
Этот небольшой инструмент позволяет создавать панорамные изображения из кубических карт, а также просматривать их. Он в первую очередь предназначен для использования с куб. картами, создаваемыми движком x-ray (Сталкер ТЧ, ЗП, ЧН, моды, сборки).

## Установка
Вы можете просто скачать готовые .exe файлы, либо собрать их самому используя cmake. В папку с готовыми исполняемыми файлами нужно скопировать два текстовых файла с шейдерами - **c2p.txt**, **pano.txt**, а также небольшой файл с настройками - **input.txt** (впрочем, в нем всего две строчки, и вы можете создать его сами). 


## Создание панорамного изображения
Если у вас уже имеются готовая кубическая карта (6 изображений), созданные в игре, то вам достаточно просто выделить их и перетащить на **cubemap2panorama.exe**. Изображения при этом переименовывать нельзя - нужно оставить им названия, данные игрой. При отсутствии каких либо ошибок, через пару секунд у вас в папке готовое изображение (по умолчанию его название **output.tga**).
В файле **input.txt** вы увидите всего две строчки. Первая отвечает за высоту выходного изображения. Ширина будет вычислена автоматически, она ровно в 2 раза больше высоты.
Вторая строчка - название выходного изображения и его формат. Поддерживаемые форматы - **bmp**, **png**, **tga**  и **jpg**. Быстрее всего программа работает с форматом **tga**. **png** файл сохраняется гораздо медленнее, но итоговый размер файла тоже меньше.

Порядок изображений в кубической карте соответствует тому, что генерирует движок сталкера: 1 - перед, 2 - зад, 3 - верх, 4 - низ, 5 - лево, 6 - право. Если использовать программу с посторонними скриншотами/фото, то их будет достаточно просто назвать n.tga, где n - номер скриншота/фото согласно списку выше. (или другой поддерживаемый формат, типа n.png)

## Просмотр панорамного изображения
Просто перетяните файл с панорамным изображением на **panoramaviewer.exe**. Управление:
Зажмите любую кнопку мыши, и двигайте мышь чтобы осмотреться.
**F11** - переключить полноэкранный и оконный режим.
**Пробел** - включить/отключить сглаживание текстур.
**ESC** - закрыть приложение.
**Колесо мыши** - изменить угол обзора.
В целом ничем не отличается от любого аналогичного онлайн сервиса.

## Как сделать кубическую карту в игре
Кратко, вам нужно вписать **demo_record 1** в консоль, подлететь в нужное место и нажать **Backspace**. Камера повернется 6 раз, делая 6 скриншотов локации в разных направлениях. Они появятся в той же папке, где и обычные скриншоты. Но, в зависимости от части игры или сборки имеются некоторые нюансы.

В оригинальных ТЧ, ЗП, ЧН угол обзора игрока уменьшен, и придется скачать модифицированный под угол обзора 90 градусов xrGame.dll.

В ЗП (возможно ЧН тоже, не проверял) кроме аналогичной проблемы с углом обзора, добавляется еще одна - функция создания кубической карты полностью отключена в режиме рендера **dx10**, **dx11**. 

Также проблему могут создать эффекты от солнца в экранном пространстве (lensflare и прочие засветы), их придется каким-нибудь способом отключить. 

В некоторых сборках панорама сшивается так, будто между скриншотами есть зазор, данный баг скорее всего не исправляется.

