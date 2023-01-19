Проект: проигрыватель игр Instead адаптированный для работы со скринридерами

Для сборки используется Visual Studio 2015+

Проект **заморожен**

Логика работы с репозиториями. Секция конфигурационного файла описывает источники для данных по играм:

```ini
[Repos]
repo1=https://www.instead-games.ru/xml.php
repo2=https://www.instead-games.ru/xml2.php
[Rss]
rss1=https://www.instead-games.ru/rss.php
rss2=https://www.instead-games.ru/rss.php?approved=0
```

Можно добавить до 10-и репозиториев. Для примера выше добавляется основной репозиторий инстеда и песочница. Можно добавить собственные XML в аналогичном формате и использовать префикс `file://`:

```ini
[Repos]
repo1=game_list.xml
[Rss]
rss1=rss_my.xml
```
